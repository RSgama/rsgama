/**
 *@file rs232.c
 * */

#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <termios.h> 
#include <fcntl.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/time.h>

#include <rs232.h>
#include <rsGama.h>
#include <dataTypes.h>
#include <showMessage.h>

#define INCREMENT_POINTER(_byte) _byte++
#define COMPRESS_FLAG0(_byte, _pduFlag) *_byte = _pduFlag->_FLAG0; INCREMENT_POINTER(_byte)
#define COMPRESS_FLAG1(_byte, _pduFlag) *_byte = _pduFlag->_FLAG1
/*Have 2 bits for rotate to front.*/
#define COMPRESS_PROTOCOL(_bits, _pduProtocol) *_bits = ((unsigned char)_pduProtocol->_PROTOCOL) << 6
#define COMPRESS_FRAMETYPE(_bits, _pduTypeFrame) *_bits |= (((unsigned char)_pduTypeFrame->_TF) << 5)
#define COMPRESS_TYPE(_bits, _pduType) *_bits |= ((unsigned char)_pduType->_TYPE) << 2
#define COMPRESS_POLLFILL(_bits, _pduPF) *_bits |= ((_pduPF->_PF + 1) << 3)
#define COMPRESS_WINDOW(_bits, _pduWindow) *_bits |= (_pduWindow->_window << 2);
/*Two bits more significant of SIZE (SIZE has 6 bits)*/
#define COMPRESS_PDUSIZE(_bits, _pduSize) *_bits |= _pduSize->_SIZE >> 4; INCREMENT_POINTER(_bits)
#define COMPRESS_SIZECHECK(_bits, _pduSizeCheck) *_bits  = _pduSizeCheck->_SIZE << 4

#define EXTRACT_FLAG0(_pduFlag, _byte) _pduFlag->_FLAG0 = *_byte; INCREMENT_POINTER(_byte)
#define EXTRACT_PROTOCOL(_pduProtocol, _bits) _pduProtocol->_PROTOCOL = (*_bits >> 6)
#define EXTRACT_TYPEFRAME(_pduFrame, _bits) _pduFrame->_TF = (*_bits >> 5) & 0b1
#define EXTRACT_TYPE(_pduType, _bits) _pduType->_TYPE = (*_bits >> 2) & 0b111
#define EXTRACT_POLLFILL(_pduPollFill, _bits) _pduPollFill->_PF = ((*_bits >> 3) - 1) & 0b1
#define EXTRACT_WINDOW(_pduWindow, _bits) _pduWindow->_window = (*_bits  >> 2) & 0b1
#define EXTRACT_SIZE(_pduSize, _bits) _pduSize->_SIZE   = *_bits & 0b11; \
	pdu->_SIZE <<= 4; INCREMENT_POINTER(_bits)
#define EXTRACT_FLAG1(_pduFlag, _byte) _pduFlag->_FLAG1 = *_byte;

/* Data structure that keeps port configuration. This struture is static to maintain 
 * their isolation in this file. Therefore only internal function can manipulate 
 * this global variable.
 **/
static SerialPort portConfig;

 /*Convert the struct rsgPDU to frame of bytes.
  * Attention: Who's call this function is responsible for deallocation of the frame.
  * Note: To fill the field of frame and the P/F field is used the logic that both 
  * are on.
  **/
static frame * rsgPDU2Frame(const rsgPDU * _pdu)
{
	frame * pduToBitsFrame = NULL;
	unsigned char * auxHandleBytes = NULL;
	
	if(!_pdu)
	{
		return NULL;
	}
	
	pduToBitsFrame = (frame *)malloc(sizeof(frame));
	if(!pduToBitsFrame)
	{
		return NULL;
	}
	
	pduToBitsFrame->size = DEFAULT_HEAD_SIZE; 
	
	/*If is a data pack, include the size of data.*/
	if(_pdu->_TF == DATA)
	{
		pduToBitsFrame->size += _pdu->_SIZE;
	}
		
	/*Allocate the necessary space for bytes conversion.*/
	pduToBitsFrame->data = calloc(1, pduToBitsFrame->size);
	if(!pduToBitsFrame->data)
	{
		free(pduToBitsFrame);
		return NULL;
	}
	
	/*Just to make easy the handle.*/
	auxHandleBytes = pduToBitsFrame->data; 
	
	/*1ª BYTE*/
	COMPRESS_FLAG0(auxHandleBytes, _pdu);
	
	/*2ª BYTE - PROTOCOL, TF e TYPE|PF,WINDOW*/
	COMPRESS_PROTOCOL(auxHandleBytes, _pdu);
	COMPRESS_FRAMETYPE(auxHandleBytes, _pdu);

	if(_pdu->_TF == CONTROL)
	{
		COMPRESS_TYPE(auxHandleBytes, _pdu);
	}
	else 
	{
		COMPRESS_POLLFILL(auxHandleBytes, _pdu);
		COMPRESS_WINDOW(auxHandleBytes, _pdu);
	}
	
	/*Two bits more significant of SIZE (SIZE has 6 bits)*/
	COMPRESS_PDUSIZE(auxHandleBytes, _pdu);
	
	/*3ª BYTE - 4 bits of SIZE and, the first 4 "checksum"*/
	COMPRESS_SIZECHECK(auxHandleBytes, _pdu);
	
	/*If is bigendian, the address of checksum point to the most significant bytes.*/
	if(is_bigendian())
	{
		*auxHandleBytes |= *((unsigned char *)&_pdu->_CHECKSUM + 1);
		INCREMENT_POINTER(auxHandleBytes);
		
		/*Last 16 bits of checksum.*/ 
		memcpy(auxHandleBytes, (unsigned char *)&_pdu->_CHECKSUM + 2, 16);
		auxHandleBytes += 16;
	}
	/*If isn't bigendian, the address of "checksum" points to less significant byte.*/
	else
	{
		*auxHandleBytes |= *((unsigned char *)&_pdu->_CHECKSUM + 2);
		INCREMENT_POINTER(auxHandleBytes);
		
		/*The last 16 bits of checksum*/
		*auxHandleBytes = *((unsigned char *)&_pdu->_CHECKSUM + 1);
		INCREMENT_POINTER(auxHandleBytes);
		*auxHandleBytes = *(unsigned char *)&_pdu->_CHECKSUM;
		INCREMENT_POINTER(auxHandleBytes);
	}
	
	/*Assign the data if exists.*/
	if(_pdu->_TF == DATA)
	{
		auxHandleBytes  = memcpy(auxHandleBytes, _pdu->_DATA, _pdu->_SIZE);
		if(!auxHandleBytes)
		{
			free(pduToBitsFrame->data);
			free(pduToBitsFrame);
			return NULL;
		}
		else
		{
			auxHandleBytes += _pdu->_SIZE;
		}
	}
	COMPRESS_FLAG1(auxHandleBytes, _pdu);
	
	return pduToBitsFrame;
}

/*Convert one frame of bytes in rsgPDU. 
 * Attention: Who use this function need to remember that deallocate the memory.
 **/
static rsgPDU * frame2RsgPDU(frame * _bitsFrame)
{
	rsgPDU * pdu = NULL;
	unsigned char * auxHandleBytes = NULL;
	
	if(!_bitsFrame)
	{
		return NULL;
	}
	
	pdu = (rsgPDU *)calloc(1, sizeof(rsgPDU));
	if(!pdu)
	{
		return NULL;
	}
	
	/*Just to make easy the handle.*/
	auxHandleBytes = _bitsFrame->data;
	
	/*1º BYTE*/
	EXTRACT_FLAG0(pdu, auxHandleBytes);
	
	/*2º BYTE*/
	EXTRACT_PROTOCOL(pdu, auxHandleBytes);
	EXTRACT_TYPEFRAME(pdu, auxHandleBytes);
	
	/*Verify if is a PDU of Data or Control*/
	if(pdu->_TF == CONTROL)
	{
		EXTRACT_TYPE(pdu, auxHandleBytes);
	}
	else
	{
		/*If (p->_PF == 0) then is recorded the sequence 01 (p = 0, f = 1). P is set.*/
		/*If (p->_PF == 1) then is recorded the sequence 10 (p = 1, f = 0), F is set.*/
		
		/*PRO, TF, P/F, window and two most significant bits of SIZE go to the second frame.*/
		EXTRACT_POLLFILL(pdu, auxHandleBytes);
		EXTRACT_WINDOW(pdu, auxHandleBytes);
	}
	
	/*Takes the two most significant bits of SIZE.*/
	EXTRACT_SIZE(pdu, auxHandleBytes);
	
	/*3º BYTE last 4 bits of SIZE nd the first 4 bits of checksum.*/
	pdu->_SIZE |= *auxHandleBytes >> 4;
	
	if(is_bigendian())
	{
		*((unsigned char *)&pdu->_CHECKSUM + 1) = *auxHandleBytes & NIBBLE_ON;
		INCREMENT_POINTER(auxHandleBytes);
		*((unsigned char *)&pdu->_CHECKSUM + 2) = *auxHandleBytes;
		INCREMENT_POINTER(auxHandleBytes);
		*((unsigned char *)&pdu->_CHECKSUM + 3) = *auxHandleBytes;
		INCREMENT_POINTER(auxHandleBytes);
	}
	else
	{
		*((unsigned char *)&pdu->_CHECKSUM + 2) = *auxHandleBytes & NIBBLE_ON;
		INCREMENT_POINTER(auxHandleBytes);
		*((unsigned char *)&pdu->_CHECKSUM + 1) = *auxHandleBytes;
		INCREMENT_POINTER(auxHandleBytes);
		*(unsigned char *)&pdu->_CHECKSUM = *auxHandleBytes;
		INCREMENT_POINTER(auxHandleBytes);
	}
	
	if(pdu->_TF == DATA)
	{
		pdu->_DATA = malloc(pdu->_SIZE);
		if(!pdu->_DATA)
		{
			free(pdu);
			return NULL;
		}	
		pdu->_DATA = memcpy(pdu->_DATA, auxHandleBytes, pdu->_SIZE);
		auxHandleBytes += pdu->_SIZE;
	}
	EXTRACT_FLAG1(pdu, auxHandleBytes);
	
	return pdu;
}

int closePort()
{
	if (portConfig.fd > 0)
	{
        close(portConfig.fd);
        return 0;
    }
    
	return ERROR_CLOSE_PORT;
}

/*Open serial port.
 * See for some information about port: http://www.easysw.com/~mike/serial/serial.html
 **/
static int openPort()
{
	int rc = 0;
	
	/*When the port is open, this is configured to notify the UNIX that the program 
	 * does not want to control over terminal for "port" (O_NOCTTY).*/
	portConfig.fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

	if(portConfig.fd < 0)
	{
		rc = closePort();
		return ERROR_OPEN_PORT;
	}
	else
	{
		rc = fcntl(portConfig.fd, F_SETFL, 0);
		if(rc == -1)
		{
			return rc;
		}
	}
	
	return SUCCESFULL;
}
/*Configure serial port.
 *For more information see: http://www.easysw.com/~mike/serial/serial.html
 **/
int configurePort(const char * _pathFileConfig)
{
	int rc = 0;
	
	rc = openPort();
    if(rc < SUCCESFULL)
    {
		return -1;
	}

    rc = tcgetattr(portConfig.fd, &portConfig.portOption);
	if(rc < 0)
	{
		return ERROR_CONFIGURE_PORT;
	}

	rc = cfsetispeed(&portConfig.portOption, B9600);
	if(rc < 0)
	{
		return ERROR_CONFIGURE_PORT;
	}
	
	rc = cfsetospeed(&portConfig.portOption, B9600);
	if(rc < 0)
	{
		return ERROR_CONFIGURE_PORT;
	}
	
	portConfig.portOption.c_cflag |= (CLOCAL | CREAD);
	portConfig.portOption.c_cflag &= ~PARENB;
	portConfig.portOption.c_cflag &= ~CSTOPB;
	portConfig.portOption.c_cflag &= ~CSIZE;
	portConfig.portOption.c_cflag |= CS8;
	
	/*Configure entry line.*/
	portConfig.portOption.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	/*Configure the processing of out.*/
	portConfig.portOption.c_oflag &= ~OPOST;
	/*Configure timeout.*/
	portConfig.portOption.c_cc[VMIN] = 0;
	portConfig.portOption.c_cc[VTIME] = 60;

	rc = tcsetattr(portConfig.fd, TCSANOW, &portConfig.portOption);
	if(rc < 0)
	{
		return ERROR_CONFIGURE_PORT;
	}
	
    return SUCCESFULL;
}

int writePort(const rsgPDU * _pduToSend)
{
  	int rc = 0;
  	frame * frameOfBits = NULL;

	if(!_pduToSend)
	{
		return ERROR_PARAM;
	}

	rc = portConfig.fd;
    if(rc < 1)
    {
        return ERROR_DESCRIPTOR;
    }
    
    frameOfBits = rsgPDU2Frame(_pduToSend); 		
	if(!frameOfBits)
	{
		return ERROR_PDU2FRAME;
	}
	
	rc = write(portConfig.fd, frameOfBits->data, frameOfBits->size);
	if (rc < 0)
	{
		return ERROR_PORT_OPERATION;
	}
		
	free(frameOfBits);
	
    return SUCCESFULL;
}

int readPort(rsgPDU ** _pdu)
{	
	unsigned char open = 0, closed = 0, bytesRead = 0;
	unsigned char i = 0, numOfBytesRead = 0, readJunk = 0;
	unsigned char * bufferRawData = NULL, * copyBuf = NULL;
	frame frameOfBits;
		
	if(portConfig.fd < 1)
	{
		return ERROR_DESCRIPTOR;
	}
   	
   	copyBuf = bufferRawData = (unsigned char *)calloc(1, MAX_BUFFER);
   	if(!bufferRawData)
   	{
		return ERROR_ALLOCATION;
	}

    while(!closed)
    {
    	//tcflush(portConfig.fd, TCIFLUSH);
    	//tcflush(portConfig.fd, TCOFLUSH);
		numOfBytesRead = read(portConfig.fd, bufferRawData, MAX_BUFFER);
		if(numOfBytesRead < 0)
		{
			free(bufferRawData);
		    return ERROR_PORT_OPERATION;
		}
		if(!numOfBytesRead)
		{
			free(bufferRawData);
			return TIMEOUT_EXCEEDED;
		}
		/*Verify the bytes reads from port.*/
		for(i = 0; i < numOfBytesRead; i++)
		{
			if(open)
			{
				bytesRead++;
				if(*(bufferRawData + i) == FLAG)
				{
					closed = 1;
					break;
				}
			}
			else if(*(bufferRawData + i) == FLAG)
			{
				open = 1;
				bytesRead++;
			}
		}
		/*Count the total of bytes read until the next flag is found.*/
		if(open)
		{
			bufferRawData += numOfBytesRead;
		}
		/*Verify the limit of bytes was read.*/
		if((readJunk += numOfBytesRead) > LIMIT_OF_BYTE_READ)
		{
			return TIMEOUT_EXCEEDED; 
		}
    }
	/*Copy the value read*/
 	if(bytesRead > (DEFAULT_HEAD_SIZE - 1) && bytesRead < (LIMIT_OF_BYTE_READ - 1) && open && closed)
 	{
		frameOfBits.data = copyBuf;
		*_pdu = frame2RsgPDU(&frameOfBits);
		free(copyBuf);
		
		if(!*_pdu)
		{
			return ERROR_FRAME2PDU;
		}
	}
	else
	{
		free(copyBuf);
		return TIMEOUT_EXCEEDED; 
	}
	
    return SUCCESFULL;
}

