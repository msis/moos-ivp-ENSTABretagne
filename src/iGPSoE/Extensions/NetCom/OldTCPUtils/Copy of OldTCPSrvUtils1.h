#ifndef _LIBSRVTCP_H
#define _LIBSRVTCP_H


#define _CRT_SECURE_NO_DEPRECATE 1

// Server-client header
#include "../TCP_Utils.h"
#include "../fd_list.h"

// Thread handling header
#include "../Thread_Utils.h"


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


int InitSRVTCP(SRVTCP* pSRVTCP, char* address, char* port);
int MainLoopSRVTCP(
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
int StopLoopSRVTCP(SRVTCP* pSRVTCP);
int GetStatusSRVTCP(SRVTCP* pSRVTCP, int* pNbConnections);
int SendSRVTCP(SRVTCP* pSRVTCP, SOCKET ClientSocket, char* sendbuf, int sendbuflen);
int RecvSRVTCP(SRVTCP* pSRVTCP, SOCKET ClientSocket, char* recvbuf, int recvbuflen);
int StopSRVTCP(SRVTCP* pSRVTCP);


#endif // _LIBSRVTCP_H
