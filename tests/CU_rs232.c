#include <stdio.h>
#include <string.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <dataTypes.h>
#include <rs232.h>

char teste[] = "Estou escrevendo na porta";
dataSerial data;

void testCONFIGUREPORT(void)
{
	/*0 - Testa se a função foi implementada.*/
	CU_ASSERT_NOT_EQUAL_FATAL(configurePort(NULL), NOT_IMPLEMENT);
	/*1 - Teste de parâmetro.*/
	CU_ASSERT(configurePort("tty0") == 1);
	/*2 - Fecha a porta.*/
	CU_ASSERT(closePort() == 1);
}

void testWRITEPORT(void)
{
	data.buffer = teste;
	data.sizeBuffer = 19;
	/*0 - Testa se a função foi implementada.*/
	CU_ASSERT_NOT_EQUAL_FATAL(writePort(&data), NOT_IMPLEMENT);
	CU_ASSERT_NOT_EQUAL_FATAL(configurePort("tty0"), NOT_IMPLEMENT);
	/*1 - Teste de parâmetro.*/
	data.sizeBuffer = -19;
	CU_ASSERT(writePort(&data) == -1);
	data.buffer = NULL;
	data.sizeBuffer = 19;
	CU_ASSERT(writePort(&data) == -1);
	data.sizeBuffer = -19;
	CU_ASSERT(writePort(&data) == -1);
	/*2 - Teste de escrita.*/
	data.buffer = teste;
	data.sizeBuffer = strlen(teste);
	CU_ASSERT_EQUAL(writePort(&data), strlen(teste));
	CU_ASSERT_EQUAL(closePort(), 1);
}

void testREADPORT(void)
{
	char tmpBuffer[1024];
	data.buffer = teste;
	data.sizeBuffer = strlen(teste);
	/*0 - Testa se a função foi implementada.*/
	CU_ASSERT_NOT_EQUAL_FATAL(configurePort("tty0"), NOT_IMPLEMENT);
	CU_ASSERT_NOT_EQUAL_FATAL(writePort(&data), NOT_IMPLEMENT);
	data.buffer = tmpBuffer;
	data.sizeBuffer = 1024;
	CU_ASSERT_NOT_EQUAL_FATAL(readPort(&data), NOT_IMPLEMENT);
	/*1 - Teste de parâmetro.*/
	data.buffer = NULL;
	data.sizeBuffer = -10;
	CU_ASSERT(readPort(&data) == -1);
	data.buffer = tmpBuffer;
	data.sizeBuffer = -10;
	CU_ASSERT(readPort(&data) == -1);
	data.buffer = NULL;
	data.sizeBuffer = 1024;
	CU_ASSERT(readPort(&data) == -1);
	/*2 - Teste de leitura.*/
	data.buffer = tmpBuffer;
	data.sizeBuffer = strlen(tmpBuffer);
	CU_ASSERT_EQUAL(readPort(&data), 1);
	CU_ASSERT_EQUAL(closePort(), 1);
}

void testCLOSEPORT(void)
{
	/*0 - Testa se a função foi implementada.*/
	CU_ASSERT_NOT_EQUAL_FATAL(closePort(), NOT_IMPLEMENT);
	CU_ASSERT_EQUAL(closePort(), 1);
}

int main()
{
   CU_pSuite pSuite = NULL;

	/*Inicializa o registro de teste do CUnit.*/
	if(CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/*Adiciona um suite para o registro.*/
	pSuite = CU_add_suite("Suite_de_teste_rs232", NULL, NULL);
	if(NULL == pSuite)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}
	/*Adiciona o teste para o suite
	 * ATENÇÃO: A ordem aqui importa*/
	if((NULL == CU_add_test(pSuite, "Testando configurePorte()", testCONFIGUREPORT))
	 ||(NULL == CU_add_test(pSuite, "Testando writePort()", testWRITEPORT))
	 ||(NULL == CU_add_test(pSuite, "Testando readPort()", testREADPORT))
	 ||(NULL == CU_add_test(pSuite, "Testando closePort()", testCLOSEPORT)))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Executa todos os teste usando a interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
