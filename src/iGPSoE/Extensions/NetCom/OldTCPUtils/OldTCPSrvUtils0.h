/***************************************************************************************************************:')

OldTCPSrvUtils0.h

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

To do :
To try to match better our needs, another version has been made, but it has not been fully tested yet.

***************************************************************************************************************:)*/

// Prevent Visual Studio Intellisense from defining _WIN32 when we use it to edit Linux
// code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__

#ifndef OLDTCPSRVUTILS0_H
#define OLDTCPSRVUTILS0_H

#include "OSTCPSock.h"
#include "OSCriticalSection.h"

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

EXTERN_C int InitSRVTCP(SRVTCP* pSRVTCP, char* address, char* port);
EXTERN_C int SendSRVTCP(SRVTCP* pSRVTCP, char* sendbuf, int sendbuflen);
EXTERN_C int RecvSRVTCP(SRVTCP* pSRVTCP, char* recvbuf, int recvbuflen);
EXTERN_C int StopSRVTCP(SRVTCP* pSRVTCP);

#endif // OLDTCPSRVUTILS0_H
