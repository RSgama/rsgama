/**
 *@file rsGama.h
 *@brief This file keeps the definition of some function used by RSGAMA.
 **/

#ifndef _RS_GAMA_H_
#define _RS_GAMA_H_

#include <dataTypes.h>

/**
 *Busca por pacotes na camada de rede para transmitir sobre o canal.
 * 
 * @param[out] pk Fill the field of pk with one reference to package.
 * @return Upon succesfull completition, fromNetworkLayer() shall return a non-negative 
 * integer. Otherwise, the function shall return one negative number that correspond
 * an internal code error.
 * @brief Search packages in the network layer.
 **/
int fromNetworkLayer(packet * _pk);

/**
 * @param[out] _fm Fill the value of fm with one reference of a frame.
 * @return Upon succesfull completition, fromPhysicalLayer() shall return a non-negative
 * integer. Otherwise, the function shall return one negative number that correspond 
 * an internal code error.
 * @brief Receive a packages from physical layer.
 **/
int fromPhysicalLayer(rsgPDU * _fm);

/**
 * @param[in] _seq Number of sequence.
 * @return Upon succesfull completition, startTimer() shall return a non-negative 
 * integer. Otherwise, the function shall return one negative number that correspond 
 * an internal code error.
 * @brief Receive the reference frame and begin the count of time. This enable the 
 * timeout.
 **/
int startTimer(winSeq _seq);

/**
 * @param[in] _seq Number of sequence.
 * @return Upon succesfull completition, stopTimer() shall return a non-negative 
 * integer. Otherwise, the function shall return one negative number that correspond 
 * an internal code error.
 * @brief For the time disable the timeout.
 **/
int stopTimer(winSeq _seq);

/**
 * @return Upon succesfull completition, enableNetworkLayer() shall return a non-negative 
 * integer. Otherwise, the function shall return one negative number that correspond 
 * an internal code error.
 * @brief Allows that network layer cause an event that allow read from network layer.
 **/
int enableNetworkLayer();

/**
 * @return Upon succesfull completition, disableNetworkLayer() shall return a non-negative 
 * integer. Otherwise, the function shall return one negative number that correspond 
 * an internal code error.
 * @brief Forbidding that network layer cause an event of read for layer.
 **/
int disableNetworkLayer();

/**
* @return Upon succesfull completition, rsgConnect() shall return a non-negative 
 * integer. Otherwise, the function shall return one negative number that correspond 
 * an internal code error.
  @brief Establish the connection with host connected in RS-232.
*/
extern int rsgConnect();

/**
* @return Upon succesfull completition, rsgDisconnect() shall return a non-negative 
 * integer. Otherwise, the function shall return one negative number that correspond 
 * an internal code error.
 * @brief Begin the disconnection.
*/
extern int rsgDisconnect();

/**
* @param _msg Message that will be sent via serial port.
* @param _msgSize The size of messsage.
* @return Upon succesfull completition, rsgSend() shall return a non-negative 
* integer. Otherwise, the function shall return one negative number that correspond 
* an internal code error.
* @brief Send Message.
*/
extern int rsgSend(const void * _msg, int _msgSize);

/**
* @param[in] _msg Receive a reference that will be filled.
* @param * _size Size of message.
* @return Upon succesfull completition, rsgReceive() shall return a non-negative 
* integer. Otherwise, the function shall return one negative number that correspond 
* an internal code error.
* @brief Receive a message and a passes.
*/
extern int rsgReceive(void ** _msg, int * _size);

/**
 * @param[in] Reference to PDU.
 * @return Upon succesfull completition, checksum() shall return a non-negative 
* integer. Otherwise, the function shall return one negative number that correspond 
* an internal code error.
 * @brief Calculate the checksum of PDU.
 **/
extern int checksum(const rsgPDU * p);

#endif
