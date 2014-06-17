/***************************************************************************************************************:')

TCPClientUtils.h

TCP client. It can connect to a SimpleTCPServer or a MultiClientTCPServer.

Fabrice Le Bars

Created : 2009-04-25

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef TCPCLIENTUTILS_H
#define TCPCLIENTUTILS_H

#include "OSNet.h" 


int CreateTCPClient(HTCPCLIENT* phCli, char* address, char* port);
int SendToServer(HTCPCLIENT hCli, char* sendbuf, int sendbuflen);
int RecvFromServer(HTCPCLIENT hCli, char* recvbuf, int recvbuflen);
int DestroyTCPClient(HTCPCLIENT* phCli);


#endif // TCPCLIENTUTILS_H
