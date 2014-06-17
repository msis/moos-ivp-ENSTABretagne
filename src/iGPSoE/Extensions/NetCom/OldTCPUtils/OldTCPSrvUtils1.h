
// Prevent Visual Studio Intellisense from defining _WIN32 when we use it to edit Linux
// code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__

#ifndef OLDTCPSRVUTILS1_H
#define OLDTCPSRVUTILS1_H

#include "OSTCPSock.h"
#include "OSFdList.h"
#include "OSCriticalSection.h"

// Timeout of 100 ms for select()
#define SELECT_SEC_TIMEOUT_SRVTCP 0
#define SELECT_USEC_TIMEOUT_SRVTCP 100000

#define REQSRVTCP 0x00
#define STREAMSRVTCP 0x01

#define DENY_CONNECTION 0x00
#define REMOVE_UNUSED 0x01

// Reserved data structure representing the TCP server
struct _SRVTCP	{
	char* address;
	char* port;
	int DontBlock;
	SOCKET ListenSocket;
	struct addrinfo* addrinf;
	fd_list sock_list;
	fd_set sock_set;
	int EndThread;
	int NbConnections;
	CRITICAL_SECTION CSEndThread; 
	CRITICAL_SECTION CSNbConnections; 
	THREAD_IDENTIFIER ThrId;
	int initialized;
};
typedef struct _SRVTCP SRVTCP;

EXTERN_C int InitSRVTCP(SRVTCP* pSRVTCP, char* address, char* port);
EXTERN_C int MainLoopSRVTCP(
				   SRVTCP* pSRVTCP, 
				   int (*OnLoop)(SRVTCP*, SOCKET, void*), 
				   void* OnLoopParams, 
				   int (*OnNewCli)(SRVTCP*, SOCKET, void*), 
				   void* OnNewCliParams, 
				   int (*OnCliReq)(SRVTCP*, SOCKET, void*), 
				   void* OnCliReqParams, 
				   int TooManyConnectionsAction,
				   int DontBlock, 
				   int mode
				   );
EXTERN_C int StopLoopSRVTCP(SRVTCP* pSRVTCP);
EXTERN_C int GetStatusSRVTCP(SRVTCP* pSRVTCP, int* pNbConnections);
EXTERN_C int SendSRVTCP(SRVTCP* pSRVTCP, SOCKET ClientSocket, char* sendbuf, int sendbuflen);
EXTERN_C int RecvSRVTCP(SRVTCP* pSRVTCP, SOCKET ClientSocket, char* recvbuf, int recvbuflen);
EXTERN_C int StopSRVTCP(SRVTCP* pSRVTCP);

#endif // OLDTCPSRVUTILS1_H
