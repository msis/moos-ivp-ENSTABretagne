
// Prevent Visual Studio Intellisense from defining _WIN32 when we use it to edit Linux
// code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__

#include "OldTCPSrvUtils1.h"

#ifdef _MSC_VER
// Disable some Visual Studio warnings that happen in some Winsock macros
// related to fd_set.
#pragma warning(disable : 4127) 
#endif // _MSC_VER

/*
Thread containing the main loop of the server (mode REQSRVTCP).

void* lpParam : (IN,OUT) parameter of the thread

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
#ifdef _WIN32
DWORD WINAPI REQSRVTCPThrProc(LPVOID lpParam)	{
#else
void* REQSRVTCPThrProc(void* lpParam)	{
#endif

	int EndThread = 0;
	struct THRPARAMS	{
		SRVTCP* pSRVTCP; 
		int (*OnLoop)(SRVTCP*, SOCKET, void*);
		void* OnLoopParams;
		int (*OnNewCli)(SRVTCP*, SOCKET, void*);
		void* OnNewCliParams;
		int (*OnCliReq)(SRVTCP*, SOCKET, void*);
		void* OnCliReqParams;
		int TooManyConnectionsAction;
	};
	struct THRPARAMS* pParams = (struct THRPARAMS*)lpParam;
	int NbConnections = 0;

	// Copy of the thread parameters in local variables
	SRVTCP* pSRVTCP = pParams->pSRVTCP;
	int (*OnLoop)(SRVTCP*, SOCKET, void*) = pParams->OnLoop;
	void* OnLoopParams = pParams->OnLoopParams;
	int (*OnNewCli)(SRVTCP*, SOCKET, void*) = pParams->OnNewCli;
	void* OnNewCliParams = pParams->OnNewCliParams;
	int (*OnCliReq)(SRVTCP*, SOCKET, void*) = pParams->OnCliReq;
	void* OnCliReqParams = pParams->OnCliReqParams;
	int TooManyConnectionsAction = pParams->TooManyConnectionsAction;

	fd_set tmpset = {0}; // fd_set modified by select()
	fd_set tmpset1 = {0};
	fd_set tmpset2 = {0};
	SOCKET_DATA* sd = NULL;
	SOCKET_DATA* sd_tmp = NULL;
	struct timeval timeout = {SELECT_SEC_TIMEOUT_SRVTCP, SELECT_USEC_TIMEOUT_SRVTCP}; // Timeout for select()
	SOCKET ClientSocket = INVALID_SOCKET; // New client socket to add to the fd_set and its associated fd_list
	int iResult = SOCKET_ERROR;
	SOCKET_DATA* sd2 = NULL;
	SOCKET_DATA* sd2_tmp = NULL;
	long OldestTime = GetTickCount();


	// Main loop of the server.
	// First, does user-defined actions for every client sockets (for example, sending some data).
	// Then, waits for data to read from the client sockets and does user-defined actions to
	// handle this data.
	// There is an exception when there is available data on the server socket. It is a client that 
	// is requesting a connection. User-defined actions can be done in case of a successful new connection.
	do
	{
		if (OnLoop != NULL)	{ // Fo every client socket, does user-defined actions
			sd = pSRVTCP->sock_list.first->next; // Excludes the server socket which is the first
			while (sd)	{
				// Does user-defined actions
				if (OnLoop(pSRVTCP, sd->sock, OnLoopParams) != EXIT_SUCCESS)	{ // On error, disconnects the client
					ShutdownTCP(&sd->sock, SD_BOTH);
					DisconnectTCP(&sd->sock);
					sd_tmp = sd->next;
					FD_REMOVE(sd, &pSRVTCP->sock_list, &pSRVTCP->sock_set);
					sd = sd_tmp;
					// Update the number of clients currently connected
					NbConnections = pSRVTCP->sock_list.fd_count - 1;
					memcpy_ts(&pSRVTCP->NbConnections, &NbConnections, sizeof(int), &pSRVTCP->CSNbConnections); // Thread-safe copy
					continue; // Without continue we would miss the next element
				}
				sd = sd->next;
			}
		}

		// Copies the fd_set in a temporary variable that may be modified by select()
		// to indicate which sockets are readable
		memcpy(&tmpset, &pSRVTCP->sock_set, sizeof(pSRVTCP->sock_set));

		// Timeout for select()
		timeout.tv_sec = SELECT_SEC_TIMEOUT_SRVTCP; 
		timeout.tv_usec = SELECT_USEC_TIMEOUT_SRVTCP; 

		// Waits for the readability of a socket in the fd_set, with a timeout of 100 ms
		iResult = select(pSRVTCP->sock_list.max_socket+1, &tmpset, NULL, NULL, &timeout);

		if (iResult == SOCKET_ERROR)	{
			// Checks the validity of every socket and disconnects those which are bad
			sd = pSRVTCP->sock_list.first->next; // Excludes the server socket which is the first
			while (sd)	{ 
				// Initialize 2 temporary fd_set which contain only one socket
				FD_ZERO(&tmpset1);FD_ZERO(&tmpset2); 
				FD_SET(sd->sock, &tmpset1);FD_SET(sd->sock, &tmpset2);
				timeout.tv_sec = 0; 
				timeout.tv_usec = 0; 
				if (select((int)sd->sock+1, &tmpset1, &tmpset2, NULL, &timeout) == SOCKET_ERROR)	{ // On error, disconnects the client
					ShutdownTCP(&sd->sock, SD_BOTH);
					DisconnectTCP(&sd->sock);
					sd_tmp = sd->next;
					FD_REMOVE(sd, &pSRVTCP->sock_list, &pSRVTCP->sock_set);
					sd = sd_tmp;
					FD_CLR(sd->sock, &tmpset1);FD_CLR(sd->sock, &tmpset2);
					// Update the number of clients currently connected
					NbConnections = pSRVTCP->sock_list.fd_count - 1;
					memcpy_ts(&pSRVTCP->NbConnections, &NbConnections, sizeof(int), &pSRVTCP->CSNbConnections); // Thread-safe copy
					continue; // Without continue we would miss the next element
				}
				FD_CLR(sd->sock, &tmpset1);FD_CLR(sd->sock, &tmpset2);
				sd = sd->next;
			}
			continue;
		}

		if (iResult == 0)	{ // The timeout on select() occured
			continue;
		}

		// Checks every socket of the fd_list associated with the fd_set
		sd = pSRVTCP->sock_list.first; 
		while(sd)	{

			if (FD_ISSET(sd->sock, &tmpset))	{ // This socket is in tmp_set (modified by select()), so it is readable
				if (sd->sock == pSRVTCP->ListenSocket)	{ // New incoming connection (the server socket should be the first in the fd_list)
					if (
						(AcceptTCP(pSRVTCP->ListenSocket, &ClientSocket) == EXIT_SUCCESS)&&
						(SetSockOptTCP(ClientSocket, 1, 10000) == EXIT_SUCCESS) // Setting timeouts for the client socket
						)	
					{
						// Adds it to the fd_set and its associated fd_list
						if (FD_ADD(ClientSocket, &pSRVTCP->sock_list, &pSRVTCP->sock_set) != EXIT_SUCCESS)	{
							if (TooManyConnectionsAction == REMOVE_UNUSED)	{ // Removes an unused socket (the one that was not used for the longest time)
								sd2 = pSRVTCP->sock_list.first->next; // Excludes the server socket from the search
								OldestTime = sd2->LastUsedTime;
								// Looks for the oldest time
								while (sd2)	{
									OldestTime = min(sd2->LastUsedTime, OldestTime);
									sd2 = sd2->next;
								}
								sd2 = pSRVTCP->sock_list.first->next; // Excludes the server socket from the search
								// Removes the socket corresponding to the oldest time
								while (sd2)	{
									if (sd2->LastUsedTime == OldestTime)	{
										ShutdownTCP(&sd2->sock, SD_BOTH);
										DisconnectTCP(&sd2->sock);
										sd2_tmp = sd2->next;
										FD_REMOVE(sd2, &pSRVTCP->sock_list, &pSRVTCP->sock_set);
										sd2 = sd2_tmp;
										// Update the number of clients currently connected
										NbConnections = pSRVTCP->sock_list.fd_count - 1;
										memcpy_ts(&pSRVTCP->NbConnections, &NbConnections, sizeof(int), &pSRVTCP->CSNbConnections); // Thread-safe copy
										break;
									}
									else	{
										sd2 = sd2->next;
									}
								}
								// Retries to add the client socket
								if (FD_ADD(ClientSocket, &pSRVTCP->sock_list, &pSRVTCP->sock_set) != EXIT_SUCCESS)	{
									// Disconnects immediately the client as it can not be handled
									ShutdownTCP(&ClientSocket, SD_BOTH);
									DisconnectTCP(&ClientSocket);
								}
								else	{ // The new client socket was successfully added
									// Update the number of clients currently connected
									NbConnections = pSRVTCP->sock_list.fd_count - 1;
									memcpy_ts(&pSRVTCP->NbConnections, &NbConnections, sizeof(int), &pSRVTCP->CSNbConnections); // Thread-safe copy
									if (OnNewCli != NULL)	{ // For the newly accepted client socket, does user-defined actions
										if (OnNewCli(pSRVTCP, ClientSocket, OnNewCliParams) != EXIT_SUCCESS)	{ // On error, disconnects the new client
											// Looks for the SOCKET_DATA corresponding to the new client socket
											sd2 = pSRVTCP->sock_list.first->next; // Excludes the server socket from the search
											while (sd2)	{
												if (sd2->sock == ClientSocket)	{
													ShutdownTCP(&sd2->sock, SD_BOTH);
													DisconnectTCP(&sd2->sock);
													sd2_tmp = sd2->next;
													FD_REMOVE(sd2, &pSRVTCP->sock_list, &pSRVTCP->sock_set);
													sd2 = sd2_tmp;
													// Update the number of clients currently connected
													NbConnections = pSRVTCP->sock_list.fd_count - 1;
													memcpy_ts(&pSRVTCP->NbConnections, &NbConnections, sizeof(int), &pSRVTCP->CSNbConnections); // Thread-safe copy
													break;
												}
												else	{
													sd2 = sd2->next;
												}
											}
										}
									}
								}
							}
							else	{
								// Disconnects immediately the client
								ShutdownTCP(&ClientSocket, SD_BOTH);
								DisconnectTCP(&ClientSocket);
							}
						}
						else	{ // The new client socket was successfully added
							// Update the number of clients currently connected
							NbConnections = pSRVTCP->sock_list.fd_count - 1;
							memcpy_ts(&pSRVTCP->NbConnections, &NbConnections, sizeof(int), &pSRVTCP->CSNbConnections); // Thread-safe copy
							if (OnNewCli != NULL)	{ // For the newly accepted client socket, does user-defined actions
								if (OnNewCli(pSRVTCP, ClientSocket, OnNewCliParams) != EXIT_SUCCESS)	{ // On error, disconnects the new client
									// Looks for the SOCKET_DATA corresponding to the new client socket
									sd2 = pSRVTCP->sock_list.first->next; // Excludes the server socket from the search
									while (sd2)	{
										if (sd2->sock == ClientSocket)	{
											ShutdownTCP(&sd2->sock, SD_BOTH);
											DisconnectTCP(&sd2->sock);
											sd2_tmp = sd2->next;
											FD_REMOVE(sd2, &pSRVTCP->sock_list, &pSRVTCP->sock_set);
											sd2 = sd2_tmp;
											// Update the number of clients currently connected
											NbConnections = pSRVTCP->sock_list.fd_count - 1;
											memcpy_ts(&pSRVTCP->NbConnections, &NbConnections, sizeof(int), &pSRVTCP->CSNbConnections); // Thread-safe copy
											break;
										}
										else	{
											sd2 = sd2->next;
										}
									}
								}
							}
						}
					}
				}
				else { // If we are here, the socket is a client and was already accepted before
					sd->LastUsedTime = GetTickCount();
					if (OnCliReq != NULL)	{ // Fo the current client socket, does user-defined actions
						if (OnCliReq(pSRVTCP, sd->sock, OnCliReqParams) != EXIT_SUCCESS)	{ // On error, disconnects the client
							ShutdownTCP(&sd->sock, SD_BOTH);
							DisconnectTCP(&sd->sock);
							sd_tmp = sd->next;
							FD_REMOVE(sd, &pSRVTCP->sock_list, &pSRVTCP->sock_set);
							sd = sd_tmp;
							// Update the number of clients currently connected
							NbConnections = pSRVTCP->sock_list.fd_count - 1;
							memcpy_ts(&pSRVTCP->NbConnections, &NbConnections, sizeof(int), &pSRVTCP->CSNbConnections); // Thread-safe copy
							continue; // Without continue we would miss the next element
						}
					}
				}
			}

			sd = sd->next;
		}
		mSleep(100);
		memcpy_ts(&EndThread, &pSRVTCP->EndThread, sizeof(int), &pSRVTCP->CSEndThread); // Thread-safe copy
	} while (!EndThread);

	return 0; 
} 

/*
Client Thread (mode STREAMSRVTCP).

void* lpParam : (IN,OUT) parameter of the thread

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
#ifdef _WIN32
DWORD WINAPI STREAMSRVTCPLoopCliThrProc(LPVOID lpParam)	{
#else
void* STREAMSRVTCPLoopCliThrProc(void* lpParam)	{
#endif

	int EndThread = 0;
	struct CLITHRPARAMS	{
		SRVTCP* pSRVTCP; 
		SOCKET ClientSocket;
		int (*OnLoop)(SRVTCP*, SOCKET, void*);
		void* OnLoopParams;
		int (*OnNewCli)(SRVTCP*, SOCKET, void*);
		void* OnNewCliParams;
	};
	struct CLITHRPARAMS* pParams = (struct CLITHRPARAMS*)lpParam;

	// Copy of the thread parameters in local variables
	SRVTCP* pSRVTCP = pParams->pSRVTCP;
	SOCKET ClientSocket = pParams->ClientSocket;
	int (*OnLoop)(SRVTCP*, SOCKET, void*) = pParams->OnLoop;
	void* OnLoopParams = pParams->OnLoopParams;
	int (*OnNewCli)(SRVTCP*, SOCKET, void*) = pParams->OnNewCli;
	void* OnNewCliParams = pParams->OnNewCliParams;


	// Updates the number of client connections
	EnterCriticalSection(&pSRVTCP->CSNbConnections);
	pSRVTCP->NbConnections++;
	LeaveCriticalSection(&pSRVTCP->CSNbConnections);

	// For the newly accepted client socket, does user-defined actions
	if (OnNewCli(pSRVTCP, ClientSocket, OnNewCliParams) != EXIT_SUCCESS)	{ // On error, disconnects the client

		ShutdownTCP(&ClientSocket, SD_BOTH);
		DisconnectTCP(&ClientSocket);
		// Updates the number of client connections
		EnterCriticalSection(&pSRVTCP->CSNbConnections);
		pSRVTCP->NbConnections--;
		LeaveCriticalSection(&pSRVTCP->CSNbConnections);
		return 0; 
	}

	// Main loop
	do
	{
		// Does user-defined actions
		if (OnLoop(pSRVTCP, ClientSocket, OnLoopParams) != EXIT_SUCCESS)	{ // On error, disconnects the client

			ShutdownTCP(&ClientSocket, SD_BOTH);
			DisconnectTCP(&ClientSocket);
			// Updates the number of client connections
			EnterCriticalSection(&pSRVTCP->CSNbConnections);
			pSRVTCP->NbConnections--;
			LeaveCriticalSection(&pSRVTCP->CSNbConnections);
			return 0; 
		}

		mSleep(100);
		memcpy_ts(&EndThread, &pSRVTCP->EndThread, sizeof(int), &pSRVTCP->CSEndThread); // Thread-safe copy
	} while (!EndThread);

	ShutdownTCP(&ClientSocket, SD_BOTH);
	DisconnectTCP(&ClientSocket);
	// Updates the number of client connections
	EnterCriticalSection(&pSRVTCP->CSNbConnections);
	pSRVTCP->NbConnections--;
	LeaveCriticalSection(&pSRVTCP->CSNbConnections);

	return 0; 
} 

/*
Thread containing the main loop of the server (mode STREAMSRVTCP).

void* lpParam : (IN,OUT) parameter of the thread

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
#ifdef _WIN32
DWORD WINAPI STREAMSRVTCPThrProc(LPVOID lpParam)	{
#else
void* STREAMSRVTCPThrProc(void* lpParam)	{
#endif

	int EndThread = 0;
	struct THRPARAMS	{
		SRVTCP* pSRVTCP; 
		int (*OnLoop)(SRVTCP*, SOCKET, void*);
		void* OnLoopParams;
		int (*OnNewCli)(SRVTCP*, SOCKET, void*);
		void* OnNewCliParams;
		int (*OnCliReq)(SRVTCP*, SOCKET, void*);
		void* OnCliReqParams;
		int TooManyConnectionsAction;
	};
	struct THRPARAMS* pParams = (struct THRPARAMS*)lpParam;
	int NbConnections = 0;

	// Copy of the thread parameters in local variables
	SRVTCP* pSRVTCP = pParams->pSRVTCP;
	int (*OnLoop)(SRVTCP*, SOCKET, void*) = pParams->OnLoop;
	void* OnLoopParams = pParams->OnLoopParams;
	int (*OnNewCli)(SRVTCP*, SOCKET, void*) = pParams->OnNewCli;
	void* OnNewCliParams = pParams->OnNewCliParams;

	SOCKET ClientSocket = INVALID_SOCKET; // New client socket
	THREAD_IDENTIFIER ThrId = 0; // Client thread ID
	struct CLITHRPARAMS	{
		SRVTCP* pSRVTCP; 
		SOCKET ClientSocket;
		int (*OnLoop)(SRVTCP*, SOCKET, void*);
		void* OnLoopParams;
		int (*OnNewCli)(SRVTCP*, SOCKET, void*);
		void* OnNewCliParams;
	};
	struct CLITHRPARAMS params;


	// Main loop of the server.
	do
	{
		if (
			(AcceptTCP(pSRVTCP->ListenSocket, &ClientSocket) == EXIT_SUCCESS)&& // New incoming connection
			(SetSockOptTCP(ClientSocket, 1, 10000) == EXIT_SUCCESS) // Setting timeouts for the client socket
			)	
		{
			// Client thread parameters
			params.pSRVTCP = pSRVTCP;
			params.ClientSocket = ClientSocket;
			params.OnLoop = OnLoop;
			params.OnLoopParams = OnLoopParams;
			params.OnNewCli = OnNewCli;
			params.OnNewCliParams = OnNewCliParams;

			// Creates a thread for the new client
			if (
				(CreateDefaultThread(STREAMSRVTCPLoopCliThrProc, &params, &ThrId) != EXIT_SUCCESS)||
				(DetachThread(ThrId) != EXIT_SUCCESS)
				)	
			{ 
				// Disconnect the client as it can not be handled
				ShutdownTCP(&ClientSocket, SD_BOTH);
				DisconnectTCP(&ClientSocket);
			}
		}

		mSleep(100);
		memcpy_ts(&EndThread, &pSRVTCP->EndThread, sizeof(int), &pSRVTCP->CSEndThread); // Thread-safe copy
	} while (!EndThread);

	// Waits for the client threads to terminate
	do
	{
		mSleep(1000);
		memcpy_ts(&NbConnections, &pSRVTCP->NbConnections, sizeof(int), &pSRVTCP->CSNbConnections); // Thread-safe copy
	} while (NbConnections > 0);

	return 0; 
} 

/*
Initializes the TCP server. This server will only be able to handle one client at a time.

SRVTCP* pSRVTCP : (IN,OUT) pointer to the data structure representing the server
char* address : (IN) server address
char* port : (IN) server port

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int InitSRVTCP(SRVTCP* pSRVTCP, char* address, char* port)	{

	if ((pSRVTCP == NULL) || (pSRVTCP->initialized == 1))	{
		return EXIT_FAILURE;
	}

	pSRVTCP->address = (char*)calloc(strlen(address)+1, sizeof(char));
	sprintf(pSRVTCP->address, "%s", address);
	pSRVTCP->port = (char*)calloc(strlen(port)+1, sizeof(char));
	sprintf(pSRVTCP->port, "%s", port);
	pSRVTCP->DontBlock = 0;
	pSRVTCP->EndThread = 1; // The thread is not running
	pSRVTCP->NbConnections = 0; // No clients are currently connected
	pSRVTCP->ListenSocket = INVALID_SOCKET;
	pSRVTCP->addrinf = NULL;
	pSRVTCP->sock_list;
	pSRVTCP->sock_set;

	if (InitNet() != EXIT_SUCCESS)	{
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}

	if (CreateSocketTCP(&pSRVTCP->ListenSocket, pSRVTCP->address, pSRVTCP->port, &pSRVTCP->addrinf) != EXIT_SUCCESS)	{
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}

	if (SetSockOptTCP(pSRVTCP->ListenSocket, 1, 10000) != EXIT_SUCCESS)	{ // Setting timeouts on the server socket
		DestroySocketTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf);
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}

	if (BindListenTCP(pSRVTCP->ListenSocket, &pSRVTCP->addrinf) != EXIT_SUCCESS)	{
		DestroySocketTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf);
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}

	if (InitCriticalSection(&pSRVTCP->CSEndThread) != EXIT_SUCCESS)	{
		DestroySocketTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf);
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}

	if (InitCriticalSection(&pSRVTCP->CSNbConnections) != EXIT_SUCCESS)	{
		DeleteCriticalSection(&pSRVTCP->CSEndThread);
		DestroySocketTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf);
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}

	if (FD_INIT(pSRVTCP->ListenSocket, &pSRVTCP->sock_list, &pSRVTCP->sock_set) != EXIT_SUCCESS)	{
		DeleteCriticalSection(&pSRVTCP->CSNbConnections);
		DeleteCriticalSection(&pSRVTCP->CSEndThread);
		DestroySocketTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf);
		free(pSRVTCP->address);pSRVTCP->address = NULL;
		free(pSRVTCP->port);pSRVTCP->port = NULL;
		return EXIT_FAILURE;
	}

	pSRVTCP->initialized = 1;

	return EXIT_SUCCESS;
}

/*
Main loop of the server. Waits for incoming connections. When a new client is successfully connected to
the server, the user-defined function OnNewCli() is called. When a client sends data to the server, the
user-defined function OnCliReq() is called to handle the client request (only in REQSRVTCP mode). At each loop of the 
server, the user-defined function OnLoop() is called for every connected client. Every client should be disconnected
if a SendSRVTCP() or RecvSRVTCP() fails in one of the user-defined functions by returning EXIT_FAILURE.
TooManyConnectionsAction should be DENY_CONNECTION if new incoming connections should be denied if there is too many
clients currently connected, or REMOVE_UNUSED if new incoming connections should replace oldest unused connections 
(only in REQSRVTCP mode).
If DontBlock is set, this function return immediately, leaving a thread running. This thread should be stopped by
calling StopLoopSRVTCP(). Otherwise, this function should block until StopLoopSRVTCP() is successfully called.
The REQSRVTCP mode is designed for servers that wait for requests from the clients while the STREAMSRVTCP mode
is designed for servers that continuously sends data to the clients without waiting for requests.


SRVTCP* pSRVTCP : (IN,OUT) pointer to the data structure representing the server
int (*OnLoop)(SRVTCP*, SOCKET, void*) : (IN) function called at every server loop, for each connected socket
void* OnLoopParams : (IN) parameter passed to every call to OnLoopParams()
int (*OnNewCli)(SRVTCP*, SOCKET, void*) : (IN) function called when a new client socket is successfully connected
void* OnNewCliParams : (IN) parameter passed to every call to OnNewCliParams()
int (*OnCliReq)(SRVTCP*, SOCKET, void*) : (IN) function called when a client socket sends data to the server (only in REQSRVTCP mode)
void* OnCliReqParams : (IN) parameter passed to every call to OnCliReq() (only in REQSRVTCP mode)
int TooManyConnectionsAction : (IN) DENY_CONNECTION or REMOVE_UNUSED (only in REQSRVTCP mode)
int DontBlock : (IN) 1 or 0
int mode : (IN) REQSRVTCP or STREAMSRVTCP

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
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
				   )	
{
	struct THRPARAMS	{
		SRVTCP* pSRVTCP; 
		int (*OnLoop)(SRVTCP*, SOCKET, void*);
		void* OnLoopParams;
		int (*OnNewCli)(SRVTCP*, SOCKET, void*);
		void* OnNewCliParams;
		int (*OnCliReq)(SRVTCP*, SOCKET, void*);
		void* OnCliReqParams;
		int TooManyConnectionsAction;
	};
	struct THRPARAMS params;
	int EndThread = 0;


	if ((pSRVTCP == NULL) || (pSRVTCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	if ((OnLoop == NULL) && (OnNewCli == NULL) && (OnCliReq == NULL))	{
		return EXIT_FAILURE;
	}

	memcpy_ts(&EndThread, &pSRVTCP->EndThread, sizeof(int), &pSRVTCP->CSEndThread); // Thread-safe copy
	if (EndThread == 0)	{ // A thread is already running and only one thread should be running at a time
		return EXIT_FAILURE; // This function should not have been called
	}
	pSRVTCP->EndThread = 0; // Notifies that a thread should be running

	// Thread parameters
	params.pSRVTCP = pSRVTCP;
	params.OnLoop = OnLoop;
	params.OnLoopParams = OnLoopParams;
	params.OnNewCli = OnNewCli;
	params.OnNewCliParams = OnNewCliParams;
	params.OnCliReq = OnCliReq;
	params.OnCliReqParams = OnCliReqParams;
	params.TooManyConnectionsAction = TooManyConnectionsAction;

	// If DontBlock is not set, this function will block until StopLoopSRVTCP() is successfully called.
	// Otherwise, it returns immediately after the creation of the thread
	pSRVTCP->DontBlock = DontBlock;

	if (mode == REQSRVTCP)	{
		if (CreateDefaultThread(REQSRVTCPThrProc, &params, &pSRVTCP->ThrId) != EXIT_SUCCESS)	{
			pSRVTCP->EndThread = 1; // Notifies that no thread is running as its creation failed
			return EXIT_FAILURE;
		}

		if (!pSRVTCP->DontBlock)	{ // Should return after a successful call to StopLoopSRVTCP()
			if (WaitForThread(pSRVTCP->ThrId) != EXIT_SUCCESS)	{ 
				return EXIT_FAILURE;
			}
		}
	}
	else	{
		if (CreateDefaultThread(STREAMSRVTCPThrProc, &params, &pSRVTCP->ThrId) != EXIT_SUCCESS)	{
			pSRVTCP->EndThread = 1; // Notifies that no thread is running as its creation failed
			return EXIT_FAILURE;
		}

		if (!pSRVTCP->DontBlock)	{ // Should return after a successful call to StopLoopSRVTCP()
			if (WaitForThread(pSRVTCP->ThrId) != EXIT_SUCCESS)	{ 
				return EXIT_FAILURE;
			}
		}
	}

	return EXIT_SUCCESS;
}

/*
Stops the main loop of the TCP server. 
Can be called in the user-defined functions passed in parameters of
MainLoopSRVTCP() if DontBlock is not set.

SRVTCP* pSRVTCP : (IN,OUT) pointer to the data structure representing the server

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int StopLoopSRVTCP(SRVTCP* pSRVTCP)	{

	int EndThread = 0;


	if ((pSRVTCP == NULL) || (pSRVTCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	memcpy_ts(&EndThread, &pSRVTCP->EndThread, sizeof(int), &pSRVTCP->CSEndThread); // Thread-safe copy
	if (EndThread == 0)	{ // A thread is currently running
		EndThread = 1;
		memcpy_ts((char*)&pSRVTCP->EndThread, (char*)&EndThread, sizeof(int), &pSRVTCP->CSEndThread); // Thread-safe copy
		if (pSRVTCP->DontBlock)	{
			if (WaitForThread(pSRVTCP->ThrId) != EXIT_SUCCESS)	{
				return EXIT_FAILURE;
			}
		}
	}
	else	{ // No thread was running, this function should not have been called
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Returns the number of clients currently connected to the server in *pNbConnections.

SRVTCP* pSRVTCP : (IN,OUT) pointer to the data structure representing the server
int* pNbConnections : (OUT) pointer to a variable containing the answer

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int GetStatusSRVTCP(SRVTCP* pSRVTCP, int* pNbConnections)	{

	if ((pSRVTCP == NULL) || (pSRVTCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	EnterCriticalSection(&pSRVTCP->CSNbConnections);
	*pNbConnections = pSRVTCP->NbConnections;
	LeaveCriticalSection(&pSRVTCP->CSNbConnections);

	return EXIT_SUCCESS;
}

/*
Sends data to a client connected to the server. Retries automatically if all the bytes were not sent.
Fails when a timeout occurs.

SRVTCP* pSRVTCP : (IN,OUT) pointer to the data structure representing the server
char *sendbuf : (IN) data to send
int sendbuflen : (IN) number of bytes to send

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int SendSRVTCP(SRVTCP* pSRVTCP, SOCKET ClientSocket, char* sendbuf, int sendbuflen)	{

	int NbConnections = 0;


	if ((pSRVTCP == NULL) || (pSRVTCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	EnterCriticalSection(&pSRVTCP->CSNbConnections);
	NbConnections = pSRVTCP->NbConnections;
	LeaveCriticalSection(&pSRVTCP->CSNbConnections);

	if (NbConnections <= 0)	{
		return EXIT_FAILURE;
	}

	return SendTCP(ClientSocket, sendbuf, sendbuflen);
}

/*
Receives data from a client connected to the server. Retries automatically if all the bytes were not received.
Fails when a timeout occurs.

SRVTCP* pSRVTCP : (IN,OUT) pointer to the data structure representing the server
char *recvbuf : (OUT) buffer which will contain the data received
int recvbuflen : (IN) number of bytes to receive

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int RecvSRVTCP(SRVTCP* pSRVTCP, SOCKET ClientSocket, char* recvbuf, int recvbuflen)	{

	int NbConnections = 0;

	if ((pSRVTCP == NULL) || (pSRVTCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	EnterCriticalSection(&pSRVTCP->CSNbConnections);
	NbConnections = pSRVTCP->NbConnections;
	LeaveCriticalSection(&pSRVTCP->CSNbConnections);

	if (NbConnections <= 0)	{
		return EXIT_FAILURE;
	}

	return RecvTCP(ClientSocket, recvbuf, recvbuflen);
}

/*
Stops the TCP server. Should never be called in the user-defined functions 
passed in parameters of MainLoopSRVTCP(). Should not be used to stop the thread
created by MainLoopSRVTCP(), use StopLoopSRVTCP() before.

SRVTCP* pSRVTCP : (IN,OUT) pointer to the data structure representing the server

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int StopSRVTCP(SRVTCP* pSRVTCP)	{

	int errflag = 0;
	int EndThread = 0;
	SOCKET_DATA* sd = NULL;
	SOCKET_DATA* sd_tmp = NULL;


	if ((pSRVTCP == NULL) || (pSRVTCP->initialized != 1))	{
		return EXIT_FAILURE;
	}

	pSRVTCP->initialized = 0;

	memcpy_ts(&EndThread, &pSRVTCP->EndThread, sizeof(int), &pSRVTCP->CSEndThread); // Thread-safe copy
	if (EndThread == 0)	{ // A thread is currently running, so it should be stopped
		EndThread = 1;
		memcpy_ts((char*)&pSRVTCP->EndThread, (char*)&EndThread, sizeof(int), &pSRVTCP->CSEndThread); // Thread-safe copy
		// Wait until the thread has terminated.
		if (WaitForThread(pSRVTCP->ThrId) != EXIT_SUCCESS)	{
			errflag = 1;
		}
	}

	DeleteCriticalSection(&pSRVTCP->CSNbConnections);
	DeleteCriticalSection(&pSRVTCP->CSEndThread);

	// Close and remove every client socket of the fd_list and the fd_set
	sd = pSRVTCP->sock_list.first->next; // The server socket is excluded
	while (sd)	{
		ShutdownTCP(&sd->sock, SD_BOTH);
		DisconnectTCP(&sd->sock);
		sd_tmp = sd->next;
		FD_REMOVE(sd, &pSRVTCP->sock_list, &pSRVTCP->sock_set);
		sd = sd_tmp;
	}

	// Removes the server socket from the fd_list and the fd_set
	FD_REMOVE(pSRVTCP->sock_list.first, &pSRVTCP->sock_list, &pSRVTCP->sock_set);

	ShutdownTCP(&pSRVTCP->ListenSocket, SD_BOTH);

	if (DestroySocketTCP(&pSRVTCP->ListenSocket, &pSRVTCP->addrinf) != EXIT_SUCCESS)	{
		errflag = 1;
	}

	free(pSRVTCP->address);pSRVTCP->address = NULL;
	free(pSRVTCP->port);pSRVTCP->port = NULL;

	if (errflag)	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#ifdef _MSC_VER
// Restore the Visual Studio warnings previously disabled for Winsock macros
// related to fd_set.
#pragma warning(default : 4127) 
#endif // _MSC_VER
