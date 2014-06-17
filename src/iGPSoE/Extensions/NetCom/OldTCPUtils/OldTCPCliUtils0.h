/***************************************************************************************************************:')

OldTCPCliUtils0.h

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

#ifndef OLDTCPCLIUTILS0_H
#define OLDTCPCLIUTILS0_H

#include "OSTCPSock.h"
#include "OSCriticalSection.h"

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

EXTERN_C int InitCLITCP(CLITCP* pCLITCP, char* address, char* port);
EXTERN_C int SendCLITCP(CLITCP* pCLITCP, char* sendbuf, int sendbuflen);
EXTERN_C int RecvCLITCP(CLITCP* pCLITCP, char* recvbuf, int recvbuflen);
EXTERN_C int StopCLITCP(CLITCP* pCLITCP);

#endif // OLDTCPCLIUTILS0_H
