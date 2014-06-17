/***************************************************************************************************************:')

libSRVTCP.h

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

#ifndef _LIBSRVTCP_H
#define _LIBSRVTCP_H


#define _CRT_SECURE_NO_DEPRECATE 1

// Server-client header
#include "../TCP_Utils.h"

// Thread handling header
#include "../Thread_Utils.h"

#include "../Utils.h"


// Reserved data structure representing the TCP server
struct _SRVTCP	{
	char* address;
	char* port;
	SOCKET ClientSocket;
	SOCKET ListenSocket;
	struct addrinfo* addrinf;
	int EndThread;
	int connected;
	CRITICAL_SECTION CSEndThread; 
	CRITICAL_SECTION CSconnected; 
	THREAD_IDENTIFIER ThrId;
	int initialized;
};
typedef struct _SRVTCP SRVTCP;


int InitSRVTCP(SRVTCP* pSRVTCP, char* address, char* port);
int SendSRVTCP(SRVTCP* pSRVTCP, char* sendbuf, int sendbuflen);
int RecvSRVTCP(SRVTCP* pSRVTCP, char* recvbuf, int recvbuflen);
int StopSRVTCP(SRVTCP* pSRVTCP);


#endif // _LIBSRVTCP_H
