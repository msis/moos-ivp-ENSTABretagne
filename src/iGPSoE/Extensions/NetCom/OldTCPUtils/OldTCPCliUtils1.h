
// Prevent Visual Studio Intellisense from defining _WIN32 when we use it to edit Linux
// code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__

#ifndef OLDTCPCLIUTILS1_H
#define OLDTCPCLIUTILS1_H

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
EXTERN_C int ConnectCLITCP(CLITCP* pCLITCP);
EXTERN_C int ReconnectCLITCP(CLITCP* pCLITCP);
EXTERN_C int GetStatusCLITCP(CLITCP* pCLITCP, int* pConnected);
EXTERN_C int SendCLITCP(CLITCP* pCLITCP, char* sendbuf, int sendbuflen);
EXTERN_C int RecvCLITCP(CLITCP* pCLITCP, char* recvbuf, int recvbuflen);
EXTERN_C int StopCLITCP(CLITCP* pCLITCP);

#endif // OLDTCPCLIUTILS1_H
