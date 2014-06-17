/***************************************************************************************************************:')

SimpleTCPServerUtils.h

Simple TCP server. It supports only one client.

Fabrice Le Bars

Created : 2009-04-25

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef SIMPLETCPSERVERUTILS_H
#define SIMPLETCPSERVERUTILS_H

#include "OSNet.h" 

/*
Structure corresponding to the server.
*/
struct SIMPLETCPSERVER
{
	SOCKET ServerSocket;
	SOCKET ClientSocket;
char[32] address;
char[8] port;
struct addrinfo* addrinf;
};
typedef struct SIMPLETCPSERVER SIMPLETCPSERVER;

typedef SIMPLETCPSERVER* HSIMPLETCPSERVER;


int CreateSimpleTCPServer(HSIMPLETCPSERVER* phSrv, char* address, char* port);
int WaitForClientForSimpleTCPServer(HSIMPLETCPSERVER hSrv);
int SendToClient(HSIMPLETCPSERVER hSrv, char* sendbuf, int sendbuflen);
int RecvFromClient(HSIMPLETCPSERVER hSrv, char* recvbuf, int recvbuflen);
int DisconnectClientFromSimpleTCPServer(HSIMPLETCPSERVER hSrv);
int DestroySimpleTCPServer(HSIMPLETCPSERVER* phSrv);



#endif // SIMPLETCPSERVERUTILS_H
