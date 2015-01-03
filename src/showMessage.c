/**
 *@file showMessage.c
 *@brief Implementation of functions responsable to print datas in screen.
 **/

#include <showMessage.h>
#include <dataTypes.h>
#include <pduQueue.h>

#include <stdio.h>

char * enum2str(char _messageType, int _id)
{
	switch(_messageType)
	{
		case 'p':
			switch(_id)
			{
				case RS232: return "RS232";
				case RS422: return "RS422";
				case RS485: return "RS485";
			}
		case 't':
			switch(_id)
			{
				case DATA:    return "DATA";
				case CONTROL: return "CONTROL";
			}
		case 'c':
			switch(_id)
			{
				case RECEIVE_READY: 	return "RECEIVE_READY";
				case REJECT: 			return "REJECT";
				case RECEIVE_NOT_READY: return "RECEIVE_NOT_READY";
				case SELECTIVE_REJECT:  return "SELECTIVE_REJECT";
				case DISCONNECT:        return "DISCONNECT";
				case CONNECT: 			return "CONNECT";
				case DISCONNECT_OK: 	return "DISCONNECT_OK";
				case NEGOTIATE: 		return "NEGOTIATE";
			}
		case 's':
			switch(_id)
			{
				case NEGOTIATING: 	return "NEGOTIATING";
				case CONNECTED: 	return "CONNECTED";
				case DISCONNECTING: return "DISCONNECTING";
				case DISCONNECTED: 	return "DISCONNECTED";
				case RECEIVE: 		return "RECEIVE";
				case RECEIVING: 	return "RECEIVING";
				case SENDING: 		return "SENDING";
			}
		case 'e':
			switch(_id)
			{
				case NOTHING: 
					return "NADA";
				case TIMEOUT_OK: 
					return "TUDO BEM COM O TIMEOUT";
				case ERROR_CALC_NUM_PDU: 
					return "OCORREU ALGUM ERRO NO CÁLCULO DO NUMERO DE PDUs";
				case ERROR_FRAME2PDU: 
					return "ERRO NA CONVERSÃO DO FRAME PARA PDU";
				case ERROR_PDU2FRAME:
					return "ERRO NA CONVERSÃO DA PDU PARA FRAME";
				case ERROR_CHECKSUM:
					return "ERRO NO CHECKSUM";
				case ERROR_BYTES:
					return "ERRO BYTE";
				case ERROR_PTHREAD_CREATE:
					return "ERRO AO CRIAR A THREAD";
				case ERROR_ALLOCATION:
					return "ERRO DE  ALOCAÇÃO";
				case ERROR_PARAM:
					return "ERRO NO PARÂMETRO";
				case ERROR_POINTER:
					return "ERRO COM O PONTEIRO";
				case ERROR_DESCRIPTOR:
					return "ERRO COM O DESCRITOR";
				case ERROR_PORT_OPERATION:
					return "ERRO AO REALIZAR A OPERAÇÃO SOBRE A PORTA";
				case TIMEOUT_EXCEEDED:
					return "O TIMEOUT ESTOUROU";
				case ERROR_REJECTIONS:
					return "O NÚMERO DE TENTATIVAS DE REENVIO DE QUADRO ESTOUROU";
				case ERROR_CANNOT_CONNECT:
					return "Sr. USUÁRIO INFELIZMENTE NÃO FOI POSSÍVEL REALIZAR A CONEXÃO.";
			}
	}
	return "INVALIDO!";
}

void printPDU(const rsgPDU * _pdu)
{
	printf("Details of PDU[%p]: %d bytes\n", _pdu, _pdu->_TF == DATA ? 6 + _pdu->_SIZE : 6);
	printf("\tp->_FLAG0:    %d\n", _pdu->_FLAG0);
	printf("\tp->_PROTOCOL: %s\n", enum2str('p', _pdu->_PROTOCOL));
	printf("\tp->_TF:       %s\n", enum2str('t', _pdu->_TF));
	
	if(_pdu->_TF == CONTROL)
	{
		printf("\tp->_TYPE:     %s\n", enum2str('c', _pdu->_TYPE));
	}
	else
	{
		printf("\tp->_PF:       %d\n", _pdu->_PF);
		printf("\tp->_window:   %d\n", _pdu->_window);
	}
	
	printf("\tp->_SIZE:     %d\n", _pdu->_SIZE);
	if(_pdu->_TF == DATA)
	{
		printf("\tp->_DATA:     %p, %d bytes\n", _pdu->_DATA, _pdu->_SIZE);
	}
	printf("\tp->_CHECKSUM: 0x%x\n", _pdu->_CHECKSUM);
	printf("\tp->_FLAG1:    %d\n\n", _pdu->_FLAG1);
	
	return;
}

void printError(const int _cod)
{
	perror(enum2str('e', _cod));
	return;
}

void printPDUQueue(const pduQueue * _queue)
{
	Node * tmp;
	
	if(!_queue)
		return;
		
	for(tmp = _queue->head; tmp; tmp = tmp->next)
	{
		printPDU(&tmp->pdu);
	}
	
	return;
}
