/**
 *@file rs232.h
 *@brief This file keeps some functions related with RS232
 **/

#ifndef _RS232_H_
#define _RS232_H_

#include <stdlib.h>
#include <sys/time.h>

#include <dataTypes.h>

/**
*@def MAX_BUFFER 
*@brief Defines the maximum size that will be used for some operation of read 
* or write.
*/
#define MAX_BUFFER 256

/**
 * @param[in] path Recebe uma string informando em qual terminal deseja trabalhar.
 * @return  Upon succesfull completition, cofigurePort() shall return a non-negative 
 * integer. Otherwise, the function shall return one negative number that correspond 
 * an internal code error.
 * @brief Function whose the objective is realize the primary configurations of port, 
 * because of this the function receives one data structure of SerialPort that be 
 * shall previously configurated.
 **/
extern int configurePort(const char * path);

/**
 * @param[in] pdu Recebe a PDU que será transformada em uma tripa de bytes 
 * e em seguida será enviada.
 * @return Upon succesfull completition, writePort() shall return a non-negative 
 * integer. Otherwise, the function shall return one negative number that correspond
 * an internal code error.
 * @brief Write some data in port.
 * @see rsgPDU
 **/
extern int writePort(const rsgPDU * pdu);

/**
 * @param[out] pdu Receive one reference to one PDU.
 * @return Upon succesfull completition, readPort() shall return a non-negative 
 * integer. Otherwise, the function shall return one negative number that correspond
 * an internal code error.
 * @brief Read some data coming from port.
 * @see rsgPDU
 **/
extern int readPort(rsgPDU ** pdu);

/**
 * @return Upon succesfull completition, closePort() shall return a non-negative integer.
 * Otherwise, the function shall return one negative number that correspond an internal 
 * code error.
 * @brief Close the port.
 **/
extern int closePort();

#endif
