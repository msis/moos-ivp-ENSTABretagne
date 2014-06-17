#ifndef _LIBCLITCP_H
#define _LIBCLITCP_H


#define _CRT_SECURE_NO_DEPRECATE 1

// Server-client header
#include "../TCP_Utils.h"

// Thread handling header
#include "../Thread_Utils.h"

#include "../Utils.h"


// Reserved data structure representing the TCP client
struct _CLITCP	{
	char* address;
	char* port;
	SOCKET ConnectSocket;
	struct addrinfo* addrinf;
	int EndThread;
	int connected;
	CRITICAL_SECTION CSEndThread; 
	CRITICAL_SECTION CSconnected; 
	THREAD_IDENTIFIER ThrId;
	int initialized;
};
typedef struct _CLITCP CLITCP;


int InitCLITCP(CLITCP* pCLITCP, char* address, char* port);
int ConnectCLITCP(CLITCP* pCLITCP);
int ReconnectCLITCP(CLITCP* pCLITCP);
int GetStatusCLITCP(CLITCP* pCLITCP, int* pConnected);
int SendCLITCP(CLITCP* pCLITCP, char* sendbuf, int sendbuflen);
int RecvCLITCP(CLITCP* pCLITCP, char* recvbuf, int recvbuflen);
int StopCLITCP(CLITCP* pCLITCP);


#endif // _LIBCLITCP_H
