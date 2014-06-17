/***************************************************************************************************************:')

libSRVTCP.c

TCP server.
Sort of "class" SRVTCP.
A typical use should be as follows :

Init()
Send(), Recv()...
Stop()

If the server is disconnected from the client while using it, it will try to reconnect automatically.
This server supports only one client and should be used with libCLITCP.c/h.

Fabrice Le Bars

2006-2007

Version 0.1.0

To do :
To try to match better our needs, another version has been made, but it has not been fully tested yet.

***************************************************************************************************************:)*/

#include "libSRVTCP.h"


/*
Thread trying to connect or reconnect a client in a loop.

void* lpParam : (IN,OUT) parameter of the thread. Should be a pointer to the data structure representing the server

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
#ifdef _WIN32
DWORD WINAPI ListenThrProc(LPVOID lpParam)	{
#else
void* ListenThrProc(void* lpParam)	{
#endif

	int EndThread = 0;
	SRVTCP* pSRVTCP = (SRVTCP*)lpParam;
	int connected = 0;

	do
	{
		memcpy_ts(&connected, &pSRVTCP->connected, sizeof(int), &pSRVTCP->CSconnected); // Thread-safe copy
		if (connected != 1)	{ // The server is not connected to any client, trying to connect or reconnect
			DisconnectTCP(&pSRVTCP->ClientSocket);
			StopTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf);
			if ( // Because timeouts are set on this socket, it will not block on accept very long
				(InitTCP(&pSRVTCP->ListenSocket, pSRVTCP->address, pSRVTCP->port, &pSRVTCP->addrinf) == EXIT_SUCCESS)&&
//				(SetSockOptTCP(pSRVTCP->ListenSocket, 1, 10000) == EXIT_SUCCESS)&& // Setting timeouts on the server socket
				(BindListenTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf) == EXIT_SUCCESS)&&
				(AcceptTCP(&pSRVTCP->ListenSocket, &pSRVTCP->ClientSocket) == EXIT_SUCCESS)//&&
//				(SetSockOptTCP(pSRVTCP->ClientSocket, 1, 10000) == EXIT_SUCCESS)&& // Setting timeouts for the client socket
//				(SetSockOptTCP(pSRVTCP->ListenSocket, 1, 10000) == EXIT_SUCCESS) // Setting timeouts on the server socket
				)	{
					fprintf(stdout, "Connection successful\n");
					connected = 1;
					memcpy_ts(&pSRVTCP->connected, &connected, sizeof(int), &pSRVTCP->CSconnected); // Thread-safe copy
			}
			else fprintf(stdout, "Unable to connect\n");
		}
		mSleep(1000);
		memcpy_ts(&EndThread, &pSRVTCP->EndThread, sizeof(int), &pSRVTCP->CSEndThread); // Thread-safe copy
	} while(!EndThread);

	return 0; 
} 

/*
Initializes the TCP server. This server will only be able to handle one client at a time.

SRVTCP* pSRVTCP : (IN,OUT) pointer to the data structure representing the server
char* address : (IN) server address
char* port : (IN) server port

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int InitSRVTCP(SRVTCP* pSRVTCP, char* address, char* port)	{

	int EndThread = 1;

	if((pSRVTCP == NULL) || (pSRVTCP->initialized == 1))	{
		return EXIT_FAILURE;
	}

	pSRVTCP->address = (char*)calloc(strlen(address)+1, sizeof(char));
	sprintf(pSRVTCP->address, "%s", address);
	pSRVTCP->port = (char*)calloc(strlen(port)+1, sizeof(char));
	sprintf(pSRVTCP->port, "%s", port);
	pSRVTCP->ClientSocket = INVALID_SOCKET;
	pSRVTCP->ListenSocket = INVALID_SOCKET;
	pSRVTCP->addrinf = NULL;
	pSRVTCP->EndThread = 0;
	pSRVTCP->connected = 0;

	if ( // Because timeouts are set on this socket, it will not block on accept very long
		(InitTCP(&pSRVTCP->ListenSocket, pSRVTCP->address, pSRVTCP->port, &pSRVTCP->addrinf) == EXIT_SUCCESS)&&
//		(SetSockOptTCP(pSRVTCP->ListenSocket, 1, 10000) == EXIT_SUCCESS)&& // Setting timeouts on the server socket
		(BindListenTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf) == EXIT_SUCCESS)&&
		(AcceptTCP(&pSRVTCP->ListenSocket, &pSRVTCP->ClientSocket) == EXIT_SUCCESS)//&&
//		(SetSockOptTCP(pSRVTCP->ClientSocket, 1, 10000) == EXIT_SUCCESS)&& // Setting timeouts for the client socket
//		(SetSockOptTCP(pSRVTCP->ListenSocket, 1, 10000) == EXIT_SUCCESS) // Setting timeouts on the server socket
		)	{
			fprintf(stdout, "Connection successful\n");
			pSRVTCP->connected = 1;
	}
	else fprintf(stdout, "Unable to connect\n");

	if (InitCriticalSection(&pSRVTCP->CSEndThread) != EXIT_SUCCESS)	{
		DisconnectTCP(&pSRVTCP->ClientSocket);
		StopTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf);
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}
	if (InitCriticalSection(&pSRVTCP->CSconnected) != EXIT_SUCCESS)	{
		DelCriticalSection(&pSRVTCP->CSEndThread);
		DisconnectTCP(&pSRVTCP->ClientSocket);
		StopTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf);
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}

	if (CreateDefaultThread(ListenThrProc, pSRVTCP, &pSRVTCP->ThrId) != EXIT_SUCCESS)	{
		DelCriticalSection(&pSRVTCP->CSEndThread);
		DelCriticalSection(&pSRVTCP->CSconnected);
		DisconnectTCP(&pSRVTCP->ClientSocket);
		StopTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf);
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}
	if (SetThreadDefaultPriority(pSRVTCP->ThrId, THREAD_PRIORITY_NORMAL) != EXIT_SUCCESS)	{
		EndThread = 1;
		memcpy_ts((char*)&pSRVTCP->EndThread, (char*)&EndThread, sizeof(int), &pSRVTCP->CSEndThread); // Thread-safe copy
		WaitForThread(pSRVTCP->ThrId);
		DelCriticalSection(&pSRVTCP->CSEndThread);
		DelCriticalSection(&pSRVTCP->CSconnected);
		DisconnectTCP(&pSRVTCP->ClientSocket);
		StopTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf);
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}

	pSRVTCP->initialized = 1;

	return EXIT_SUCCESS;
}

/*
Sends data to the client connected to the server. Retries automatically if all the bytes were not sent.
Returns and tries to reconnect the server until it succeeds or StopSRVTCP is called if there is a
tranfer error.

SRVTCP* pSRVTCP : (IN,OUT) pointer to the data structure representing the server
char *sendbuf : (IN) data to send
int sendbuflen : (IN) number of bytes to send

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int SendSRVTCP(SRVTCP* pSRVTCP, char* sendbuf, int sendbuflen)	{

	if((pSRVTCP == NULL) || (pSRVTCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	if(SendTCP(pSRVTCP->ClientSocket, sendbuf, sendbuflen) != EXIT_SUCCESS)	{
		EnterCriticalSection(&pSRVTCP->CSconnected);
		if(pSRVTCP->connected == 1)	{
			fprintf(stderr, "Connection lost\n");
			pSRVTCP->connected = 0; // Notifies the thread to try to reconnect the client
		}
		LeaveCriticalSection(&pSRVTCP->CSconnected);
		return EXIT_FAILURE;
	}

	EnterCriticalSection(&pSRVTCP->CSconnected);
	if(pSRVTCP->connected != 1)	{
		fprintf(stdout, "Connection successful\n");
		pSRVTCP->connected = 1; // Notifies the thread not to try to reconnect the client
	}
	LeaveCriticalSection(&pSRVTCP->CSconnected);

	return EXIT_SUCCESS;
}

/*
Receives data from the client connected to the server. Retries automatically if all the bytes were not received.
Returns and tries to reconnect the server until it succeeds or StopSRVTCP is called if there is a
tranfer error.

SRVTCP* pSRVTCP : (IN,OUT) pointer to the data structure representing the server
char *recvbuf : (OUT) buffer which will contain the data received
int recvbuflen : (IN) number of bytes to receive

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int RecvSRVTCP(SRVTCP* pSRVTCP, char* recvbuf, int recvbuflen)	{

	if((pSRVTCP == NULL) || (pSRVTCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	if(RecvTCP(pSRVTCP->ClientSocket, recvbuf, recvbuflen) != EXIT_SUCCESS)	{
		EnterCriticalSection(&pSRVTCP->CSconnected);
		if(pSRVTCP->connected == 1)	{
			fprintf(stderr, "Connection lost\n");
			pSRVTCP->connected = 0; // Notifies the thread to try to reconnect the client
		}
		LeaveCriticalSection(&pSRVTCP->CSconnected);
		return EXIT_FAILURE;
	}

	EnterCriticalSection(&pSRVTCP->CSconnected);
	if(pSRVTCP->connected != 1)	{
		fprintf(stdout, "Connection successful\n");
		pSRVTCP->connected = 1; // Notifies the thread not to try to reconnect the client
	}
	LeaveCriticalSection(&pSRVTCP->CSconnected);

	return EXIT_SUCCESS;
}

/*
Stops the TCP server.

SRVTCP* pSRVTCP : (IN,OUT) pointer to the data structure representing the server

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int StopSRVTCP(SRVTCP* pSRVTCP)	{

	int EndThread = 1;

	if((pSRVTCP == NULL) || (pSRVTCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	pSRVTCP->initialized = 0;

	memcpy_ts((char*)&pSRVTCP->EndThread, (char*)&EndThread, sizeof(int), &pSRVTCP->CSEndThread); // Thread-safe copy

	if (WaitForThread(pSRVTCP->ThrId) != EXIT_SUCCESS)	{
		DelCriticalSection(&pSRVTCP->CSEndThread);
		DelCriticalSection(&pSRVTCP->CSconnected);
		DisconnectTCP(&pSRVTCP->ClientSocket);
		StopTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf);
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}

	DelCriticalSection(&pSRVTCP->CSEndThread);
	DelCriticalSection(&pSRVTCP->CSconnected);

	if(DisconnectTCP(&pSRVTCP->ClientSocket) != EXIT_SUCCESS)	{
		StopTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf);
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}

	if(StopTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf) != EXIT_SUCCESS)	{
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}

	free(pSRVTCP->address);pSRVTCP->address = NULL;
	free(pSRVTCP->port);pSRVTCP->port = NULL;

	return EXIT_SUCCESS;
}


