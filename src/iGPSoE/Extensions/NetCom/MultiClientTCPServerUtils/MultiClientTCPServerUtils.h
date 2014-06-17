/***************************************************************************************************************:')

MultiClientTCPServerUtils.h

TCP server that can support several clients.

Fabrice Le Bars

Created : 2009-04-25

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef MULTICLIENTTCPSERVERUTILS_H
#define MULTICLIENTTCPSERVERUTILS_H

#include "OSNet.h" 


int CreateMultiClientTCPServer(HMULTICLIENTTCPSERVER* phSrv, char* address, char* port);

int MainLoopSRVTCP(
				   SRVTCP* pSRVTCP, 
//				   int (*OnLoop)(SRVTCP*, SOCKET, void*), 
//				   void* OnLoopParams, 
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



//int SendToAllClients(HMULTICLIENTTCPSERVER hSrv, char* sendbuf, int sendbuflen);
int SendToSpecificClient(HMULTICLIENTTCPSERVER hSrv, SOCKET ClientSocket, char* sendbuf, int sendbuflen);
int RecvFromSpecificClient(HMULTICLIENTTCPSERVER hSrv, SOCKET ClientSocket, char* recvbuf, int recvbuflen);
int DisconnectClientFromMultiClientTCPServer(HMULTICLIENTTCPSERVER hSrv, SOCKET ClientSocket);//Necessary?
int DestroyMultiClientTCPServer(HMULTICLIENTTCPSERVER* phSrv);



#endif // MULTICLIENTTCPSERVERUTILS_H
