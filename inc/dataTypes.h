/**
 *@file dataTypes.h
 *@brief Centralizes definitions, data structure and other common data for all program.
 **/

#ifndef _DATA_TYPES_H_
#define _DATA_TYPES_H_

#include <termios.h>

/**
 * @def NOT_IMPLEMENT
 * @brief Used to indicate that the function not was implemented.
 **/
#define NOT_IMPLEMENT -321

/**
 * @def NUMBER_OF_ATTEMPTS
 * @brief Maximum number of attempts stipulated by protocol.
 **/
#define NUMBER_OF_ATTEMPTS 3

/**
 * @def NUMBER_OF_REJECTIONS
 * @brief Number of attempts doing, before giving up the operation.
 **/
#define NUMBER_OF_REJECTIONS 3

/**
 * @def TIME_TO_GIVE_UP
 * @brief Time that the protocol rsGama wait to send other protocol. case the ack is 
 * equal to RECEIVE_NOT_READY.
 **/
#define TIME_TO_GIVE_UP 1

/**
 * @def MAX_SEQ
 * @brief Define the number of windows.
 **/
#define MAX_SEQ 1

/**
 *@def MAX_PKT
 * @brief Maximum size of package.
 **/
#define MAX_PKT 2048

/**
 *@def DEFAULT_HEAD_SIZE
 * @brief The default size of head.
 **/
#define DEFAULT_HEAD_SIZE 6

/**
 *@def FLAG
 * @brief Default value for head flag.
 **/
#define FLAG 0b01111110

/**
*@def NIBBLE_ON
* @brief Mask of 4 bits in 1
*/
#define NIBBLE_ON 0b1111

/**
 *@def DEFAULT_SIZE_NEG
 * @brief Define the default value for size of frame negociate.
 **/
#define DEFAULT_SIZE_NEG 63

/**
 *@def SUCCESFULL
 * @brief In general, upon successful completion this value is returned.
 **/
#define SUCCESFULL 0

/**
 *@def LIMIT_OF_BYTE_READ
 * @brief Limit of bytes reads from port.
 **/
#define LIMIT_OF_BYTE_READ 71

/**
 *@var endian
 * @brief Global variable used to verify if the memory is bigendian.
 **/
static const int endian = 1;

#ifndef _MACRO_BIGENDIAN_
#define _MACRO_BIGENDIAN_
/**
 *@def is_bigendian
 * @brief This macro return 1 for bigendian and 0 in otherwise.
 **/
#define is_bigendian() ((*(char*)&endian) == 0)
#endif

/**
 * @enum internalReturn
 * @brief Enumeration of some codes for return informations, just for make easy the 
 * control of program.
 **/
typedef enum
{
	NOTHING = 355,
	TIMEOUT_OK = 15,
	ERROR_REJECTIONS = -1,
	ERROR_EXCEEDED_ATTEMPTS = -2,
	ERROR_CALC_NUM_PDU = -3,
	ERROR_FRAME2PDU = -4,
	ERROR_PDU2FRAME = -5,
	ERROR_CHECKSUM = -6,
	ERROR_BYTES = -7,
	ERROR_PTHREAD_CREATE = -8,
	ERROR_ALLOCATION = -9,
	ERROR_PARAM = -10,
	ERROR_POINTER = -11,
	ERROR_DESCRIPTOR = -12,
	ERROR_SELECT = -13,
	ERROR_PORT_OPERATION = -14,
	TIMEOUT_EXCEEDED = -15,
	ERROR_CANNOT_CONNECT = -16,
	ERROR_OPEN_PORT = -17,
	ERROR_CONFIGURE_PORT = -18,
	ERROR_CLOSE_PORT = -19,
	ERROR_WRITE_PORT = -20
}internalReturn;

/**
 *@enum rsgProtocol
 * @brief Enumeration for fild <i>Protocol</i> of header RSGama. This protocol 
 * is support by RSGama.
 **/
typedef enum 
{
	RS232 = 0b00,	/**< 00 - Protocol RS-232.*/
	RS422 = 0b01,	/**< 01 - Protocol RS-422.*/
	RS485 = 0b10	/**< 10 - Protocol RS-485.*/
}rsgProtocol;

/**
*@enum rsgTypeFrame
* @brief Keeps the information reference of type of frame that will be send.
*/
typedef enum
{
	DATA,		/**< 0 - DATA: Frame of data.*/
	CONTROL		/**< 1 - CONTROL: Frame of control.*/
}rsgTypeFrame;

/**
*@enum rsgControlType
* @brief Keeps the code for many types of control frame
*/
typedef enum 
{
	RECEIVE_READY     = 0b000,	/**< Confirmation and indicate that this ready for new frames.*/
	REJECT            = 0b001,	/**< Happens one error in frame.*/
	RECEIVE_NOT_READY = 0b010,	/**< Confirmation and indicate that <b>isn't</b> ready for new frames.*/
	SELECTIVE_REJECT  = 0b011,	/**< Calls the retransmission of frame defective.*/
	DISCONNECT        = 0b100,	/**< Closed the conexion.*/
	CONNECT           = 0b101,	/**< Establishes the connection.*/
	DISCONNECT_OK     = 0b110,	/**< */
	NEGOTIATE         = 0b111	/**<  Negociate the conexion.*/
}rsgControlType;

/**
*@enum rsgStatus
* @brief Keeps differents status in which host can be found. It also aims to inform the 
* type of event.
*/
typedef enum
{
	NEGOTIATING,		/**< State of negociation.*/
	CONNECTED,			/**< State of connected.*/
	DISCONNECTING,		/**< State of desconncting.*/
	DISCONNECTED,		/**< State of desconnected.*/
	RECEIVE,			/**< State of receipt.*/
	RECEIVING,			/**< State of receiving data.*/
	SENDING,			/**< State of sending data.*/
	WAITING_CONFIRM		/**< State of confirmation. */
}rsgStatus;

/**
 *@brief Definition for the sequence of frame.
 **/
typedef unsigned char winSeq;

/**
 *@struct _Packet
 * @brief Responsible to keeps de package.
 **/
typedef struct _Packet
{
	unsigned char data[MAX_PKT];	/**< Vector of packages*/
}packet;

/**
*@struct _rsgPDU
* @brief Data structure responsible for represent the PDU.
*some details about PDU header:
*<pre>
* +---------+----+---+-----+--------+---+----------------------+----------+
* |  FLAG   |PRO |TF |TYPE |  SIZE  | D |     CHECKSUM 20      |   FLAG   |
* +---------+----+---+-----+--------+---+----------------------+----------+
* |01111110 | XX | X | xxx | XXXXXX | - | XXXXXXXXXXXXXXXXXXXX | 01111110 |
* +---------+----+---+-----+--------+---+----------------------+----------+
*</pre>
*<ul>
* <li>Description of field:
* <ol>
*	<li> FLAG: This filds keeps the binary sequence that correspond 01111110 and that 
* 		which is intended to mark the beginning and end of the header
* 	<li> Protocol: Indicates which protocol is being used and therefore changes the 
* 		interpretation of other fields.
*	<li> TF - Type of Frame: Indicates the type of frame that is being sent.
* 	<li> Type: This field works with some aspects of each type of frame.
* 	<li> Size: Specify the maximum size in bytes.
* 	<li> Data: effective data.
* 	<li> Checksum: Used to perform the checksum of all bits of the frame.
* </ol>
*</ul>
*/
typedef struct _rsgPDU
{
	unsigned char 	_FLAG0;		/**< Compound for the sequence: 01111110.*/
	rsgProtocol 	_PROTOCOL;	/**< Indicate the protocol: RS232, RS422 ou RS485*/
	rsgTypeFrame 	_TF;		/**< Indicate the type of frame that being sent.*/
	rsgControlType 	_TYPE;		/**< Deals with aspects related to the type of frame.*/
	unsigned char 	_PF;		/**< The bit P/F mean Poll and final. If is equal 0 
									 there are still frames to receive, otherwise 
									 indicate that the frame is the last.*/
	winSeq		 	_window;	/**< Reference to windos.*/
	unsigned char 	_SIZE;		/**< Maximum size in bytes.*/
	void 		   *_DATA;		/**< Effective data.*/
	unsigned int	_CHECKSUM;	/**< Verify the sum of bits in the frame.*/
	unsigned char 	_FLAG1;		/**< Compound for the sequence: 01111110.*/
}rsgPDU;

/**
 *@struct _SerialPort
 * @brief Structure responsible for maintaining data regarding the serial port.
 **/
typedef struct _SerialPort
{
	struct termios portOption;	/**< Port configuration.*/
	int fd;						/**< Descriptor of port.*/
}SerialPort;

/**
 *@struct frame
 * @brief Data strutucture responsable to transported via rs232.
 **/
typedef struct
{
    void * data;	/**< Compact byte to send.*/
    int size; 		/**< Number of bytes in data.*/
}frame;

#endif
