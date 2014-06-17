/***************************************************************************************************************:')

libCLITCP.c

TCP client.
Sort of "class" CLITCP.
A typical use should be as follows :

Init()
Send(), Recv()...
Stop()

If the client is disconnected from the server while using it, it will try to reconnect automatically.
Should be used with libSRVTCP.c/h.

Fabrice Le Bars

2006-2007

To do :
To try to match better our needs, another version has been made, but it has not been fully tested yet.

***************************************************************************************************************:)*/

// Prevent Visual Studio Intellisense from defining _WIN32 when we use it to edit Linux
// code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__

#include "OldTCPCliUtils0.h"

/*
Thread trying to connect or reconnect a server in a loop.

void* lpParam : (IN,OUT) parameter of the thread. Should be a pointer to the data structure representing the client

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
THREAD_PROC_RETURN_VALUE ConnectThrProc(void* lpParam)	{

	int EndThread = 0;
	CLITCP* pCLITCP = (CLITCP*)lpParam;
	int connected = 0;

	do
	{
		memcpy_ts(&connected, &pCLITCP->connected, sizeof(int), &pCLITCP->CSconnected); // Thread-safe copy
		if (connected != 1)	{ // The client is not connected to the server, trying to connect or reconnect
			DestroySocketTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf);
			if (
				(CreateSocketTCP(&pCLITCP->ConnectSocket, pCLITCP->address, pCLITCP->port, &pCLITCP->addrinf) == EXIT_SUCCESS)&&
//				(SetSockOptTCP(pCLITCP->ConnectSocket, 1, 10000) == EXIT_SUCCESS)&& // Setting timeouts on the client socket
				(ConnectTCP(pCLITCP->ConnectSocket, &pCLITCP->addrinf) == EXIT_SUCCESS)//&&
//				(SetSockOptTCP(pCLITCP->ConnectSocket, 1, 10000) == EXIT_SUCCESS) // Setting timeouts on the client socket
				)	{
					fprintf(stdout, "Connection successful\n");
					connected = 1;
					memcpy_ts(&pCLITCP->connected, &connected, sizeof(int), &pCLITCP->CSconnected); // Thread-safe copy
			}
			else fprintf(stdout, "Unable to connect\n");
		}
		mSleep(1000);
		memcpy_ts(&EndThread, &pCLITCP->EndThread, sizeof(int), &pCLITCP->CSEndThread); // Thread-safe copy
	} while(!EndThread);

	return 0; 
} 

/*
Initializes the TCP client.

CLITCP* pCLITCP : (IN,OUT) pointer to the data structure representing the client
char* address : (IN) server address to connect
char* port : (IN) server port to connect

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int InitCLITCP(CLITCP* pCLITCP, char* address, char* port)	{

//	int EndThread = 1;

	if((pCLITCP == NULL) || (pCLITCP->initialized == 1))	{
		return EXIT_FAILURE;
	}

	pCLITCP->address = (char*)calloc(strlen(address)+1, sizeof(char));
	sprintf(pCLITCP->address, "%s", address);
	pCLITCP->port = (char*)calloc(strlen(port)+1, sizeof(char));
	sprintf(pCLITCP->port, "%s", port);
	pCLITCP->ConnectSocket = INVALID_SOCKET;
	pCLITCP->addrinf = NULL;
	pCLITCP->EndThread = 0;
	pCLITCP->connected = 0;

	if (
		(InitNet() == EXIT_SUCCESS)&&
		(CreateSocketTCP(&pCLITCP->ConnectSocket, pCLITCP->address, pCLITCP->port, &pCLITCP->addrinf) == EXIT_SUCCESS)&&
//		(SetSockOptTCP(pCLITCP->ConnectSocket, 1, 10000) == EXIT_SUCCESS)&& // Setting timeouts on the client socket
		(ConnectTCP(pCLITCP->ConnectSocket, &pCLITCP->addrinf) == EXIT_SUCCESS)//&&
//		(SetSockOptTCP(pCLITCP->ConnectSocket, 1, 10000) == EXIT_SUCCESS) // Setting timeouts on the client socket
		)	{
			fprintf(stdout, "Connection successful\n");
			pCLITCP->connected = 1;
	}
	else fprintf(stdout, "Unable to connect\n");

	if (InitCriticalSection(&pCLITCP->CSEndThread) != EXIT_SUCCESS)	{
		DestroySocketTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf);
		free(pCLITCP->address);pCLITCP->address = NULL;
		free(pCLITCP->port);pCLITCP->port = NULL;
		return EXIT_FAILURE;
	}
	if (InitCriticalSection(&pCLITCP->CSconnected) != EXIT_SUCCESS)	{
		DeleteCriticalSection(&pCLITCP->CSEndThread);
		DestroySocketTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf);
		free(pCLITCP->address);pCLITCP->address = NULL;
		free(pCLITCP->port);pCLITCP->port = NULL;
		return EXIT_FAILURE;
	}

	if (CreateDefaultThread(ConnectThrProc, pCLITCP, &pCLITCP->ThrId) != EXIT_SUCCESS)	{
		DeleteCriticalSection(&pCLITCP->CSEndThread);
		DeleteCriticalSection(&pCLITCP->CSconnected);
		DestroySocketTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf);
		free(pCLITCP->address);pCLITCP->address = NULL;
		free(pCLITCP->port);pCLITCP->port = NULL;
		return EXIT_FAILURE;
	}
/*	if (SetThreadDefaultPriority(pCLITCP->ThrId, THREAD_PRIORITY_NORMAL) != EXIT_SUCCESS)	{
		EndThread = 1;
		memcpy_ts((char*)&pCLITCP->EndThread, (char*)&EndThread, sizeof(int), &pCLITCP->CSEndThread); // Thread-safe copy
		WaitForThread(pCLITCP->ThrId);
		DeleteCriticalSection(&pCLITCP->CSEndThread);
		DeleteCriticalSection(&pCLITCP->CSconnected);
		DestroySocketTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf);
		free(pCLITCP->address);pCLITCP->address = NULL;
		free(pCLITCP->port);pCLITCP->port = NULL;
		return EXIT_FAILURE;
	}
*/
	pCLITCP->initialized = 1;

	return EXIT_SUCCESS;
}

/*
Sends data to the server connected to the client. Retries automatically if all the bytes were not sent.
Returns and tries to reconnect the server until it succeeds or StopCLITCP is called if there is a
tranfer error.

CLITCP* pCLITCP : (IN,OUT) pointer to the data structure representing the client
char *sendbuf : (IN) data to send
int sendbuflen : (IN) number of bytes to send

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int SendCLITCP(CLITCP* pCLITCP, char* sendbuf, int sendbuflen)	{

	if((pCLITCP == NULL) || (pCLITCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	if(SendTCP(pCLITCP->ConnectSocket, sendbuf, sendbuflen) != EXIT_SUCCESS)	{
		EnterCriticalSection(&pCLITCP->CSconnected);
		if(pCLITCP->connected == 1)	{
			fprintf(stderr, "Connection lost\n");
			pCLITCP->connected = 0; // Notifies the thread to try to reconnect the server
		}
		LeaveCriticalSection(&pCLITCP->CSconnected);
		return EXIT_FAILURE;
	}

	EnterCriticalSection(&pCLITCP->CSconnected);
	if(pCLITCP->connected != 1)	{
		fprintf(stdout, "Connection successful\n");
		pCLITCP->connected = 1; // Notifies the thread not to try to reconnect the server
	}
	LeaveCriticalSection(&pCLITCP->CSconnected);

	return EXIT_SUCCESS;
}

/*
Receives data from the server connected to the client. Retries automatically if all the bytes were not received.
Returns and tries to reconnect the server until it succeeds or StopCLITCP is called if there is a
tranfer error.

CLITCP* pCLITCP : (IN,OUT) pointer to the data structure representing the client
char *recvbuf : (OUT) buffer which will contain the data received
int recvbuflen : (IN) number of bytes to receive

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int RecvCLITCP(CLITCP* pCLITCP, char* recvbuf, int recvbuflen)	{

	if((pCLITCP == NULL) || (pCLITCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	if(RecvTCP(pCLITCP->ConnectSocket, recvbuf, recvbuflen) != EXIT_SUCCESS)	{
		EnterCriticalSection(&pCLITCP->CSconnected);
		if(pCLITCP->connected == 1)	{
			fprintf(stderr, "Connection lost\n");
			pCLITCP->connected = 0; // Notifies the thread to try to reconnect the server
		}
		LeaveCriticalSection(&pCLITCP->CSconnected);
		return EXIT_FAILURE;
	}

	EnterCriticalSection(&pCLITCP->CSconnected);
	if(pCLITCP->connected != 1)	{
		fprintf(stdout, "Connection successful\n");
		pCLITCP->connected = 1; // Notifies the thread not to try to reconnect the server
	}
	LeaveCriticalSection(&pCLITCP->CSconnected);

	return EXIT_SUCCESS;
}

/*
Stops the TCP client.

CLITCP* pCLITCP : (IN,OUT) pointer to the data structure representing the client

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int StopCLITCP(CLITCP* pCLITCP)	{

	int EndThread = 1;

	if((pCLITCP == NULL) || (pCLITCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	pCLITCP->initialized = 0;

	memcpy_ts((char*)&pCLITCP->EndThread, (char*)&EndThread, sizeof(int), &pCLITCP->CSEndThread); // Thread-safe copy

	if (WaitForThread(pCLITCP->ThrId) != EXIT_SUCCESS)	{
		DeleteCriticalSection(&pCLITCP->CSEndThread);
		DeleteCriticalSection(&pCLITCP->CSconnected);
		DestroySocketTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf);
		free(pCLITCP->address);pCLITCP->address = NULL;
		free(pCLITCP->port);pCLITCP->port = NULL;
		return EXIT_FAILURE;
	}

	DeleteCriticalSection(&pCLITCP->CSEndThread);
	DeleteCriticalSection(&pCLITCP->CSconnected);

	if(DestroySocketTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf) != EXIT_SUCCESS)	{
		free(pCLITCP->address);pCLITCP->address = NULL;
		free(pCLITCP->port);pCLITCP->port = NULL;
		return EXIT_FAILURE;
	}

	free(pCLITCP->address);pCLITCP->address = NULL;
	free(pCLITCP->port);pCLITCP->port = NULL;

	return EXIT_SUCCESS;
}
