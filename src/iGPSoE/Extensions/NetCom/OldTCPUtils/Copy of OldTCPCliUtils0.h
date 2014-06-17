/***************************************************************************************************************:')

libCLITCP.h

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

Version 0.1.0

To do :
To try to match better our needs, another version has been made, but it has not been fully tested yet.

***************************************************************************************************************:)*/

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
int SendCLITCP(CLITCP* pCLITCP, char* sendbuf, int sendbuflen);
int RecvCLITCP(CLITCP* pCLITCP, char* recvbuf, int recvbuflen);
int StopCLITCP(CLITCP* pCLITCP);


#endif // _LIBCLITCP_H
