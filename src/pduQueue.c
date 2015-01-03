/**
 *@file pduQueue.c 
 **/

#include <stdio.h>
#include <stdlib.h>

#include <dataTypes.h>
#include <pduQueue.h>

int push(pduQueue * _queueOfPdu, rsgPDU * _pdu)
{
	Node * auxToInsertPdu = NULL;
	
	if(!_queueOfPdu || !_pdu)
	{
		return ERROR_PARAM;
	}
	
	auxToInsertPdu = malloc(sizeof(Node));
	if(!auxToInsertPdu)
	{
		return ERROR_ALLOCATION;
	}
			
	auxToInsertPdu->pdu = *_pdu;
	auxToInsertPdu->next = NULL;
	/*Verify if queue is empty or not, before to insert.*/
	if(!_queueOfPdu->head)
	{
		_queueOfPdu->tail = _queueOfPdu->head = auxToInsertPdu;
	}
	else
	{
		_queueOfPdu->tail->next = auxToInsertPdu;
		_queueOfPdu->tail = auxToInsertPdu;
	}
	
	return SUCCESFULL;		
}

int pop(pduQueue * _queueOfPdu, rsgPDU * _pdu)
{
	Node * auxToRemovePdu = NULL;
	
	if(!_queueOfPdu || !_pdu || !_queueOfPdu->head)
	{
		return ERROR_PARAM;
	}
	/*Changes the reference of from the top of queue.*/
	auxToRemovePdu = _queueOfPdu->head;
	_queueOfPdu->head = _queueOfPdu->head->next;
	
	*_pdu = auxToRemovePdu->pdu;
	
	free(auxToRemovePdu);

	return SUCCESFULL;
}
