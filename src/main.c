#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rs232.h>
#include <rsGama.h>
#include <dataTypes.h>
#include <pthread.h>
#include <sys/time.h>
#include <showMessage.h>

int main(int argc, char * argv[])
{
	int rc;
	char v[] = "Olá a todos, este é o projeto rsgama cujo o "
				"objetivo principal consiste na criação de um protocolo para rs232"
				"Vale observar que este é um projeto desenvolvido para a disciplina"
				"de rede da UnB-Gama";
	
	rc = configurePort(NULL);
	if(rc < 0)
	{
		printError(rc);
		return rc;
	}
	
	puts("Dormi...");
	sleep(2);
	
	puts("Enviei...");
	rc = rsgSend(v, sizeof(v));
	if(rc < 0)
	{
		printf("Erro: %d \n", rc);
		printError(rc);
		return rc;
	}

	/*	
	sleep(1);
	rc = rsgReceive(&msg, &size);
	if(rc < 0)
	{
		printf("Erro: %d \n", rc);
		printError(rc);
		return rc;
	}
	printf("MENSAGEM RECEBIDA: %s \n", (char *)msg);
	*/	
	closePort();
		
	return 0;
}
