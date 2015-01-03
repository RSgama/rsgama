/**
 *@file showMessage.h
 *@brief Keeps the functions responsable to print data in screen.
 **/

#ifndef _SHOW_MESSAGE_H_
#define _SHOW_MESSAGE_H_

#include <dataTypes.h>
#include <pduQueue.h>

/**
 * @param _messageType Character telling which part of the protocol you want to get the string.
 * @param _id Value correspond to enum.
 * @return Return one string.
 * @brief Return the string corresponding to the name of any of the field of PDU.
 **/
extern char * enum2str(char _messageType, int _id);

/**
 * Print the data in PDU.
 * @brief Receive one reference of PDU and print the value of them in screen.
 * @param _pdu Receive the reference of one PDU for data print.
 **/
extern void printPDU(const rsgPDU * _pdu);

/**
 * @param _cod Code of error. 
 * @brief Receive the code of error and based on them, print an error.
 **/
extern void printError(const int _cod);

/**
 * @param  _queue Queue of PDU
 * @brief Print the node of list.
 **/
extern void printPDUQueue(const pduQueue * _queue);

#endif
