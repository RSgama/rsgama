/**
 *@file pduQueue.h
 *@brief Necessary definition for creation of queues, whose the main objective is 
 * storing PDUs.
 **/

#ifndef _PDU_QUEUE_H_
#define _PDU_QUEUE_H_

#include <dataTypes.h>

/**
 *@struct _node
 * @brief Specific node for PDU queue.
 **/
typedef struct _node
{
	rsgPDU pdu;				/**< Data kept by node.*/
	struct _node * next;	/**< Next of queue.*/
}Node;

/**
 *@struct _pduQueue
 * @brief Data structure used by queue.
 **/
typedef struct _pduQueue
{
	Node * head;	/**< First element of queue.*/
	Node * tail;	/**< Last element of queue.*/
}pduQueue;

/**
* @param queue Expected a valid reference for a queue..
* @param pdu PDU that will be inserted at the end of queue.
* @return Upon successfull completion, push shall return a non-negative integer. 
* Otherwise, the function shall return one negative number that correspond a internal 
* code error.
* @brief Add a new PDU at the end of queue. 
*/
extern int push(pduQueue * _queue, rsgPDU * _pdu);

/**
* @param queue Queue that wish to remove one element.
* @param[out] pdu PDU that was contain in queue.
* @return Upon succesfull completition, pop shall return a non-negative integer. 
* Otherwise, the function shall return one negative number that correspond a internal
* code error.
* @brief Remove some element from queue.
*/
extern int pop(pduQueue * _queue, rsgPDU * _pdu);

#endif

