Bugs
1 - O transmissor tem que começar a transmissão primeiro. Não pode demorar para o receptor ser ativado.


FALTA
1 - Caculate Checksum in rsgReceive
		if(rcvPDU->_CHECKSUM != checksum(rcvPDU))
		{
			// Ops, parece que veio algo errado, solicita a mesma PDU
			ackPDU._TYPE = REJECT;
			
			// Manda o ACK de volta
			rc = writePort(&ackPDU);
			if(rc < 0)
				return rc;
			
			if(++checkAttempts >= NUMBER_OF_ATTEMPTS)
				return ERROR_CHECKSUM;	
			
			continue;	
		}
