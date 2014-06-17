#include "libCLITCP.h"


/*
Thread trying to reconnect to the server in a loop (if necessary).

void* lpParam : (IN,OUT) parameter of the thread. Should be a pointer to the data structure representing the client

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
#ifdef _WIN32
DWORD WINAPI ConnectThrProc(LPVOID lpParam)	{
#else
void* ConnectThrProc(void* lpParam)	{
#endif

	int EndThread = 0;
	CLITCP* pCLITCP = (CLITCP*)lpParam;
	int connected = 0;


	do
	{
		// Checks whether the client is currently connected or not
		memcpy_ts(&connected, &pCLITCP->connected, sizeof(int), &pCLITCP->CSconnected); // Thread-safe copy
		if (connected != 1)	{ // The client is not connected to the server, trying to connect or reconnect
			ShutdownTCP(&pCLITCP->ConnectSocket, SD_BOTH);
			StopTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf);
			if (
				(InitTCP(&pCLITCP->ConnectSocket, pCLITCP->address, pCLITCP->port, &pCLITCP->addrinf) == EXIT_SUCCESS)&&
				(SetSockOptTCP(pCLITCP->ConnectSocket, 1, 10000) == EXIT_SUCCESS)&& // Setting timeouts on the client socket
				(ConnectTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf) == EXIT_SUCCESS)
				)	
			{
					connected = 1;
					memcpy_ts(&pCLITCP->connected, &connected, sizeof(int), &pCLITCP->CSconnected); // Thread-safe copy
					fprintf(stdout, "Connection successful\n");
			}
			else	{
				fprintf(stdout, "Unable to connect\n");
			}
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

	if ((pCLITCP == NULL) || (pCLITCP->initialized == 1))	{
		return EXIT_FAILURE;
	}

	pCLITCP->address = (char*)calloc(strlen(address)+1, sizeof(char));
	sprintf(pCLITCP->address, "%s", address);
	pCLITCP->port = (char*)calloc(strlen(port)+1, sizeof(char));
	sprintf(pCLITCP->port, "%s", port);
	pCLITCP->ConnectSocket = INVALID_SOCKET;
	pCLITCP->addrinf = NULL;
	pCLITCP->EndThread = 1; // The thread is not running
	pCLITCP->connected = 0; // The client is not currently connected

	if (InitTCP(&pCLITCP->ConnectSocket, pCLITCP->address, pCLITCP->port, &pCLITCP->addrinf) != EXIT_SUCCESS)	{
		free(pCLITCP->address);pCLITCP->address = NULL;
		free(pCLITCP->port);pCLITCP->port = NULL;
		return EXIT_FAILURE;
	}

	if (SetSockOptTCP(pCLITCP->ConnectSocket, 1, 10000) != EXIT_SUCCESS)	{ // Setting timeouts on the client socket
		StopTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf);
		free(pCLITCP->address);pCLITCP->address = NULL;
		free(pCLITCP->port);pCLITCP->port = NULL;
		return EXIT_FAILURE;
	}

	if (InitCriticalSection(&pCLITCP->CSEndThread) != EXIT_SUCCESS)	{
		StopTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf);
		free(pCLITCP->address);pCLITCP->address = NULL;
		free(pCLITCP->port);pCLITCP->port = NULL;
		return EXIT_FAILURE;
	}

	if (InitCriticalSection(&pCLITCP->CSconnected) != EXIT_SUCCESS)	{
		DelCriticalSection(&pCLITCP->CSEndThread);
		StopTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf);
		free(pCLITCP->address);pCLITCP->address = NULL;
		free(pCLITCP->port);pCLITCP->port = NULL;
		return EXIT_FAILURE;
	}

	pCLITCP->initialized = 1;

	return EXIT_SUCCESS;
}

/*
Connects to the server until it succeeds. This function should not block more than 10 s.
If the client can not connect immediately, a thread keeps trying to connect to the server until it succeeds
or StopCLITCP() is called. A return value of EXIT_SUCCESS does not
necessary mean that the client is connected. Use GetStatusCLITCP() to verify it.

CLITCP* pCLITCP : (IN,OUT) pointer to the data structure representing the client

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int ConnectCLITCP(CLITCP* pCLITCP)	{

	int EndThread = 0;


	if ((pCLITCP == NULL) || (pCLITCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	memcpy_ts(&EndThread, &pCLITCP->EndThread, sizeof(int), &pCLITCP->CSEndThread); // Thread-safe copy
	if (EndThread == 0)	{ // A thread is already running and only one thread should be running at a time
		return EXIT_FAILURE; // This function should not have been called
	}

	if (ConnectTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf) == EXIT_SUCCESS)	{
		pCLITCP->connected = 1; // Notifies that the client is currently connected
		fprintf(stdout, "Connection successful\n");
	}
	else	{
		pCLITCP->connected = 0; // Notifies that the client is currently not connected
		fprintf(stdout, "Unable to connect\n");
	}

	pCLITCP->EndThread = 0; // Notifies that a thread should be running

	if (CreateDefaultThread(ConnectThrProc, pCLITCP, &pCLITCP->ThrId) != EXIT_SUCCESS)	{
		pCLITCP->EndThread = 1; // Notifies that no thread is running as its creation failed
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Retries to connect to the server until it succeeds. 
Should be called after a successful call to ConnectCLITCP(), in case
of a SendCLITCP() or RecvCLITCP() failure (if the failure was unexpected).
Whatever it happens, this function returns immediately, but a thread 
keeps trying to reconnect to the server until it succeeds
or StopCLITCP() is called. A return value of EXIT_SUCCESS does not
necessary mean that the client is connected. Use GetStatusCLITCP() to verify it.

CLITCP* pCLITCP : (IN,OUT) pointer to the data structure representing the client

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int ReconnectCLITCP(CLITCP* pCLITCP)	{

	int EndThread = 0;


	if ((pCLITCP == NULL) || (pCLITCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	memcpy_ts(&EndThread, &pCLITCP->EndThread, sizeof(int), &pCLITCP->CSEndThread); // Thread-safe copy
	if (EndThread == 1)	{ // No thread is currently running, there was probably no successful call to ConnectCLITCP() before
		return EXIT_FAILURE;
	}

	EnterCriticalSection(&pCLITCP->CSconnected);
	if (pCLITCP->connected == 1)	{ // The client was supposed to be connected before
		pCLITCP->connected = 0; // Notifies the thread to try to reconnect the server
		fprintf(stdout, "Connection lost\n");
	}
	LeaveCriticalSection(&pCLITCP->CSconnected);

	return EXIT_SUCCESS;
}

/*
Returns whether the client is currently connected to a server.
If it is connected, *pConnected = 1, 0 otherwise.

CLITCP* pCLITCP : (IN,OUT) pointer to the data structure representing the client
int* pConnected : (OUT) pointer to a variable containing the answer

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int GetStatusCLITCP(CLITCP* pCLITCP, int* pConnected)	{

	if ((pCLITCP == NULL) || (pCLITCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	EnterCriticalSection(&pCLITCP->CSconnected);
	*pConnected = pCLITCP->connected;
	LeaveCriticalSection(&pCLITCP->CSconnected);

	return EXIT_SUCCESS;
}

/*
Sends data to the server connected to the client. Retries automatically if all the bytes were not sent.
Fails when a timeout occurs.

CLITCP* pCLITCP : (IN,OUT) pointer to the data structure representing the client
char *sendbuf : (IN) data to send
int sendbuflen : (IN) number of bytes to send

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int SendCLITCP(CLITCP* pCLITCP, char* sendbuf, int sendbuflen)	{

	int connected = 0;


	if ((pCLITCP == NULL) || (pCLITCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	EnterCriticalSection(&pCLITCP->CSconnected);
	connected = pCLITCP->connected;
	LeaveCriticalSection(&pCLITCP->CSconnected);

	if (connected != 1)	{
		return EXIT_FAILURE;
	}

	return SendTCP(pCLITCP->ConnectSocket, sendbuf, sendbuflen);
}

/*
Receives data from the server connected to the client. Retries automatically if all the bytes were not received.
Fails when a timeout occurs.

CLITCP* pCLITCP : (IN,OUT) pointer to the data structure representing the client
char *recvbuf : (OUT) buffer which will contain the data received
int recvbuflen : (IN) number of bytes to receive

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int RecvCLITCP(CLITCP* pCLITCP, char* recvbuf, int recvbuflen)	{

	int connected = 0;


	if ((pCLITCP == NULL) || (pCLITCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	EnterCriticalSection(&pCLITCP->CSconnected);
	connected = pCLITCP->connected;
	LeaveCriticalSection(&pCLITCP->CSconnected);

	if (connected != 1)	{
		return EXIT_FAILURE;
	}

	return RecvTCP(pCLITCP->ConnectSocket, recvbuf, recvbuflen);
}

/*
Stops the TCP client.

CLITCP* pCLITCP : (IN,OUT) pointer to the data structure representing the client

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int StopCLITCP(CLITCP* pCLITCP)	{

	int errflag = 0;
	int EndThread = 1;


	if ((pCLITCP == NULL) || (pCLITCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	pCLITCP->initialized = 0;

	memcpy_ts(&EndThread, &pCLITCP->EndThread, sizeof(int), &pCLITCP->CSEndThread); // Thread-safe copy
	if (EndThread == 0)	{ // A thread is currently running, so it should be stopped
		EndThread = 1;
		memcpy_ts((char*)&pCLITCP->EndThread, (char*)&EndThread, sizeof(int), &pCLITCP->CSEndThread); // Thread-safe copy
		// Wait until the thread has terminated.
		if (WaitForThread(pCLITCP->ThrId) != EXIT_SUCCESS)	{
			errflag = 1;
		}
	}

	DelCriticalSection(&pCLITCP->CSconnected);
	DelCriticalSection(&pCLITCP->CSEndThread);

	ShutdownTCP(&pCLITCP->ConnectSocket, SD_BOTH);

	if (StopTCP(&pCLITCP->ConnectSocket, &pCLITCP->addrinf) != EXIT_SUCCESS)	{
		errflag = 1;
	}

	free(pCLITCP->address);pCLITCP->address = NULL;
	free(pCLITCP->port);pCLITCP->port = NULL;

	if (errflag)	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


