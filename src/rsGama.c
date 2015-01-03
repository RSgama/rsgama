/**
 *@file rsGama.c
 *@brief This file keeps the implementation of protocol RSGAMA.
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <dataTypes.h>
#include <rsGama.h>
#include <rs232.h>
#include <showMessage.h>
#include <pduQueue.h>

/*
 *Keeps the current STATUS visible for all.
 */
static rsgStatus STATUS = DISCONNECTED;

/*
 *Size previously negotiated.
 */
static char sizeNeg = DEFAULT_SIZE_NEG;

static int readPDU(rsgPDU ** _pduReceive)
{
	int attempts = 0, rc = 0;
	
	if(!_pduReceive)
	{
		return ERROR_PARAM;
	}
	
	do
	{
		rc = readPort(_pduReceive);
		if(!rc)
		{
			printPDU(*_pduReceive);
			return SUCCESFULL;
		}
		
		if(rc == TIMEOUT_EXCEEDED)
		{
			attempts++;
			continue;
		}
		else
		{
			return rc;
		}
	}while(attempts < NUMBER_OF_ATTEMPTS);

	if(attempts == NUMBER_OF_ATTEMPTS)
	{
		return ERROR_EXCEEDED_ATTEMPTS;
	}
	
	return ERROR_EXCEEDED_ATTEMPTS;
}

static rsgPDU fillDefaultRsgPDU(const rsgControlType _type)
{
	rsgPDU defaultFillPDU;
	
	/*Default configuration for PDU.*/
	defaultFillPDU._FLAG0 = FLAG;
	defaultFillPDU._PROTOCOL = RS232;
	defaultFillPDU._TF = CONTROL;
	defaultFillPDU._TYPE = _type;
	//retPDU._CHECKSUM = checksum(&retPDU);
	defaultFillPDU._FLAG1 = FLAG;
	
	return defaultFillPDU;
}

int checksum(const rsgPDU * _pduTarget)
{
	/*Sum the total and auxiliar i.*/
	unsigned int sum = 0, i = 0;
	
	/*Sum the components which has lass then 20 bits.*/
	sum += _pduTarget->_PROTOCOL + _pduTarget->_TF + _pduTarget->_SIZE;
	
	/*Sum with PF and window or type*/
	if(_pduTarget->_TF == DATA)
	{
		sum += _pduTarget->_PF + _pduTarget->_window;
	}
	else
	{
		sum += _pduTarget->_TYPE;
	}
		
	/*Makes the sum of data.*/
	if(_pduTarget->_TF == DATA)
	{
		/*Sum 8 bits by 8 bits*/
		for(i = 0; i < _pduTarget->_SIZE; i++)
		{
			sum += *(((char *)_pduTarget->_DATA) + i);
		}
	}
	/*Resets the first 12 bits of the sum.*/
	return sum & ((1 << 20) - 1);
}

int rsgConnect()
{ 
	rsgPDU connection, * readTmp = NULL; 
	int rc = 0, rejections = 0;

	readTmp = (rsgPDU *)calloc(1, sizeof(rsgPDU));
	if(!readTmp)
	{
		return ERROR_ALLOCATION;
	}

	/*Frame of connection default.*/
	connection = fillDefaultRsgPDU(NEGOTIATE);
	
RESEND:
	rc = writePort(&connection);
	if(rc < 0)
	{
		return rc;
	}

	rc = readPDU(&readTmp);
	if(rc < 0)
	{
		return rc;
	}

	/*Verify if is ready to receive.*/
	switch(readTmp->_TYPE)
	{
		case RECEIVE_READY:
			connection._TYPE = CONNECT;
			rc = writePort(&connection);
			if(rc < 0)
			{
				return rc;	
			}
			STATUS = CONNECTED;
			break;
		case SELECTIVE_REJECT:
		case REJECT:
			if(++rejections >= NUMBER_OF_REJECTIONS)
			{
				return ERROR_REJECTIONS;
			}
			goto RESEND;
		case RECEIVE_NOT_READY:
			return ERROR_CANNOT_CONNECT;
		
		default:
			return rc;
	}

	/*Receive the new value negotiate.*/
	sizeNeg = readTmp->_SIZE;
	connection._TYPE = CONNECT;
	connection._SIZE = sizeNeg;

	free(readTmp);

	STATUS = CONNECTED;
	
	rc = writePort(&connection);
	if(rc < 0)
	{
		return rc;
	}

	return SUCCESFULL;
}

int rsgDisconnect()
{
	rsgPDU disconnection = fillDefaultRsgPDU(DISCONNECT);
	
	/*Write on port and out*/
	if(writePort(&disconnection) < 0)
	{
		return ERROR_WRITE_PORT;
	}
	
	/*Change the state of protocol to disconnected.*/
	STATUS = DISCONNECTED;

	return SUCCESFULL;
}

static int assignmentConstantPDU(rsgPDU * _pdu)
{
	_pdu->_FLAG0 = FLAG;
	_pdu->_PROTOCOL = RS232;
	_pdu->_TF = DATA;
	_pdu->_PF = 0;
	_pdu->_SIZE = sizeNeg;
	_pdu->_DATA = malloc(sizeNeg);
	if(!_pdu->_DATA)
	{
		return ERROR_ALLOCATION;
	}
	_pdu->_FLAG1 = FLAG;

	return SUCCESFULL;
}

static void assemblePDU(rsgPDU * _pdu, const void * _msgSend, const int _numPDUs, 
		const int _current, const int _rest)
{
	/*Define if is the last PDU of message.*/
	if(_current != _numPDUs - 1)
	{
		_pdu->_DATA = memcpy(_pdu->_DATA, 
			((char *)_msgSend) + (sizeNeg * _current), sizeNeg);

	}
	else
	{
		_pdu->_PF = 1;
		_pdu->_SIZE = _rest;
		_pdu->_DATA = memcpy(_pdu->_DATA, 
			((char *)_msgSend) + (sizeNeg * _current), _rest);
	}
		
	/*Define the sequence of window.*/
	_pdu->_window = _current % 2;
		
	/*Calculate the checksum*/
	_pdu->_CHECKSUM = checksum(_pdu);
}

int rsgSend(const void * _messageToSend, int _sizeMessage)
{
	int numPDUs = 0, restPDU, i = 0;
	int rc = 0, rejections = 0, attempts = 0;
	rsgPDU sendPDU, * ackPDU = NULL;
	
	ackPDU = (rsgPDU *) calloc(1, sizeof(rsgPDU));
	if(!ackPDU)
	{
		return ERROR_ALLOCATION;
	}

	/*Assignment of constant value.*/
	rc = assignmentConstantPDU(&sendPDU);
	if(rc == ERROR_ALLOCATION)
	{
		free(ackPDU);
		return ERROR_ALLOCATION;
	}

	/*Calculate the number of PDUs to be assembled.*/
	numPDUs = _sizeMessage / sizeNeg;
	if(numPDUs < 0)
	{
		free(ackPDU);
		return ERROR_CALC_NUM_PDU;
	}

	/*Number of bytes in the case of frames isn't accurate.*/
	restPDU = _sizeMessage % sizeNeg;
	
	/*Increase a PDU in case there are bytes left.*/
	if(restPDU > 0)
	{
		numPDUs++;
	}

		
	for(i = 0; i < numPDUs; i++)
	{
		assemblePDU(&sendPDU, _messageToSend, numPDUs, i, restPDU);
		
		/*Write the bytes*/
		rc = writePort(&sendPDU);
		if(rc < SUCCESFULL)
		{
			free(ackPDU);
			return rc;
		}
				
		rc = readPDU(&ackPDU);
		if(rc < SUCCESFULL)
		{
			free(ackPDU);
			return rc;
		}
			
		switch(ackPDU->_TYPE)
		{
			case REJECT:
			case RECEIVE_NOT_READY:
			case RECEIVE_READY:
				rejections = 0;
				attempts = 0;
				break;
			case SELECTIVE_REJECT:
			/*
			case REJECT:
				i--;
				rejections++;
				if(rejections >= NUMBER_OF_REJECTIONS)
					return ERROR_REJECTIONS;
				break;
			case RECEIVE_NOT_READY:
				attempts++;
				sleep(TIME_TO_GIVE_UP);//Não é a solução perfeita ainda...
				if(attempts >= NUMBER_OF_ATTEMPTS)
					return ERROR_REJECTIONS;
				break;
			*/
			default:
				return rc;
		}
	}
	
	free(sendPDU._DATA);
	free(ackPDU);
		
	return SUCCESFULL;
}

int rsgReceive(void ** _messageReceive, int * _sizeMessage)
{
	rsgPDU * rcvPDU = NULL, ackPDU, tmpPDU;
	int  rc, checkAttempts = 0, i = 0, numPDU = 0;
	pduQueue rcvPDUQueue = {0, 0};
	
	ackPDU = fillDefaultRsgPDU(RECEIVE_READY);
	
	while(1)
	{
		/*Vefiry which event enter in wait
		 * waitForEvent();*/
		
		/*Wait for one PDU from port by the maximum number of attempts.*/
		rc = readPDU(&rcvPDU);
		if(rc < SUCCESFULL)
		{
			return rc;
		}
					
		// Calculate checksum and compare
		
		/*Reset the number of attempts of checksum for each new PDU.*/
		checkAttempts = 0;
		
		rc = push(&rcvPDUQueue, rcvPDU);
		if(rc < SUCCESFULL) 
		{
			return rc;
		}
		
		rc = writePort(&ackPDU);
		if(rc < SUCCESFULL) 
		{
			return rc;
		}
		
		/*Increase the number of PDUs read and valid.*/
		numPDU++;
		if(rcvPDU->_PF)
		{
			free(rcvPDU);
			break;
		}
		free(rcvPDU);
	}

	/*Begin the assemble of message received.*/
	if(numPDU)
	{
		numPDU--; /*Decrement to make easy the handle of last PDU.*/
	}
		
	*_messageReceive = malloc((numPDU * sizeNeg) + rcvPDUQueue.tail->pdu._SIZE);
	if(!*_messageReceive)
	{
		return ERROR_ALLOCATION;
	}
		
	for(i = 0; i < numPDU; i++)
	{	
		rc = pop(&rcvPDUQueue, &tmpPDU);
		if(rc < SUCCESFULL)
		{
			free(_messageReceive);
			return rc;
		}
		
		/*Include the message to be returned.*/
		memcpy(*(char **)_messageReceive + (i*sizeNeg), tmpPDU._DATA, sizeNeg);
	}
	
	// Se o número de PDUs for maior que 0, significa que ainda resta 
	// uma PDU para incluir na mensagem
	
	if(numPDU >= 0)
	{
		rc = pop(&rcvPDUQueue, &tmpPDU);
		if(rc < SUCCESFULL) 
		{
			free(_messageReceive);
			return rc;
		}
		/*Include in message to be returned.*/
		memcpy(*(char **)_messageReceive + (i * sizeNeg), tmpPDU._DATA, tmpPDU._SIZE);
	}

	return SUCCESFULL;
}
