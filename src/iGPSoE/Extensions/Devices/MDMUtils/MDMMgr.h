/***************************************************************************************************************:')

MDMMgr.h

Tritech Micron data modem handling.

Fabrice Le Bars

Created : 2012-07-01

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef MDMMGR_H
#define MDMMGR_H

#include "OSThread.h"
#include "OSCriticalSection.h"
#include "MDMCfg.h"

/*
Debug macros specific to MDMMgr.
*/
#ifdef _DEBUG_MESSAGES_MDMUTILS
#	define _DEBUG_MESSAGES_MDMMGR
#endif // _DEBUG_MESSAGES_MDMUTILS

#ifdef _DEBUG_WARNINGS_MDMUTILS
#	define _DEBUG_WARNINGS_MDMMGR
#endif // _DEBUG_WARNINGS_MDMUTILS

#ifdef _DEBUG_ERRORS_MDMUTILS
#	define _DEBUG_ERRORS_MDMMGR
#endif // _DEBUG_ERRORS_MDMUTILS

#ifdef _DEBUG_MESSAGES_MDMMGR
#	define PRINT_DEBUG_MESSAGE_MDMMGR(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MDMMGR(params)
#endif // _DEBUG_MESSAGES_MDMMGR

#ifdef _DEBUG_WARNINGS_MDMMGR
#	define PRINT_DEBUG_WARNING_MDMMGR(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MDMMGR(params)
#endif // _DEBUG_WARNINGS_MDMMGR

#ifdef _DEBUG_ERRORS_MDMMGR
#	define PRINT_DEBUG_ERROR_MDMMGR(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MDMMGR(params)
#endif // _DEBUG_ERRORS_MDMMGR

#define DATA_POLLING_MODE_MDM 0x00000001

struct MDMMGR
{
	HMDM hMDM;
	char szDevice[256];
	THREAD_IDENTIFIER ThreadId;
	int timeout;
	int PollingTimeInterval;
	int PollingMode;
	BOOL bRunThread;
	BOOL bConnected;
	CRITICAL_SECTION MDMCS;
	CRITICAL_SECTION dataCS;
	UINT datacounter;
	uint8 data[2];
	BOOL bWaitForFirstData;
};
typedef struct MDMMGR* HMDMMGR;

#define INVALID_HMDMMGR_VALUE NULL

/*
Private function.
Thread function that handles the polling and automatic reconnection of a Tritech Micron data modem.

LPVOID lpParam : (INOUT) Parameter of the thread.

Return : 0.
*/
EXTERN_C THREAD_PROC_RETURN_VALUE MDMMgrThreadProc(void* lpParam);

/*
Private function.
Open a Tritech Micron data modem and get data from it.

HMDM hMDM : (IN) Identifier of the device.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ConnectToMDM(HMDMMGR hMDMMgr)
{
	// No critical section is needed here because the other functions 
	// should not try to access the device or data while the device is
	// not connected (bConnected = FALSE).

	if (OpenMDM(&hMDMMgr->hMDM, hMDMMgr->szDevice, hMDMMgr->timeout) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_MDMMGR(("Unable to connect to a Tritech Micron data modem\n"));
		return EXIT_FAILURE;
	}

	// Get data before setting bConnected to TRUE to be sure that
	// the other functions will get valid data.
	if (hMDMMgr->bWaitForFirstData)
	{
	}

	hMDMMgr->bConnected = TRUE;
	PRINT_DEBUG_MESSAGE_MDMMGR(("Tritech Micron data modem connected\n"));

	return EXIT_SUCCESS;
}

/*
Initialize a structure that manage the polling and the reconnection of a 
Tritech Micron data modem.

HMDMMGR* phMDMMgr : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* szDevice : (IN) Device serial port to open.
int timeout : (IN) Time to wait to get at least 1 byte in ms (near 1000 ms for example, max is 
MAX_TIMEOUT_RS232PORT).
int PollingTimeInterval : (IN) Time interval between 2 requests of data (in ms).
int PollingMode : (IN) 0.
int Priority : (IN) Priority of the thread that manages the polling and the 
reconnection of the device. See SetThreadDefaultPriority().
BOOL bWaitForFirstData : (IN) TRUE to wait for the first entire data in 
InitMDMMgr(), FALSE if it does not matter if the first data after the 
connection to the device is invalid during some seconds.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitMDMMgr(HMDMMGR* phMDMMgr, char* szDevice, int timeout,
					  int PollingTimeInterval,
					  int PollingMode,
					  int Priority,
					  BOOL bWaitForFirstData)
{
	*phMDMMgr = (HMDMMGR)calloc(1, sizeof(struct MDMMGR));

	if (*phMDMMgr == NULL)
	{
		PRINT_DEBUG_ERROR_MDMMGR(("InitMDMMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	sprintf((*phMDMMgr)->szDevice, "%s", szDevice);

	(*phMDMMgr)->timeout = timeout;
	(*phMDMMgr)->PollingTimeInterval = PollingTimeInterval;
	(*phMDMMgr)->PollingMode = PollingMode;
	(*phMDMMgr)->bConnected = FALSE;
	(*phMDMMgr)->bRunThread = TRUE;
	(*phMDMMgr)->datacounter = 0;
	memset((*phMDMMgr)->data, 0, sizeof((*phMDMMgr)->data));
	(*phMDMMgr)->bWaitForFirstData = bWaitForFirstData;

	if (
		(InitCriticalSection(&(*phMDMMgr)->MDMCS) != EXIT_SUCCESS)||
		(InitCriticalSection(&(*phMDMMgr)->dataCS) != EXIT_SUCCESS)
		)
	{
		PRINT_DEBUG_ERROR_MDMMGR(("InitMDMMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a critical section. ", 
			szDevice));
		DeleteCriticalSection(&(*phMDMMgr)->dataCS);
		DeleteCriticalSection(&(*phMDMMgr)->MDMCS);
		free(*phMDMMgr);
		return EXIT_FAILURE;
	}

	// First attempt to connect to the device.
	ConnectToMDM(*phMDMMgr);

	if (CreateDefaultThread(MDMMgrThreadProc, (void*)*phMDMMgr, &(*phMDMMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MDMMGR(("InitMDMMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a thread. ", 
			szDevice));
		if ((*phMDMMgr)->bConnected)
		{
			CloseMDM(&(*phMDMMgr)->hMDM);
		}
		DeleteCriticalSection(&(*phMDMMgr)->dataCS);
		DeleteCriticalSection(&(*phMDMMgr)->MDMCS);
		free(*phMDMMgr);
		return EXIT_FAILURE;
	}

#ifdef ENABLE_PRIORITY_HANDLING
	if (SetThreadDefaultPriority((*phMDMMgr)->ThreadId, Priority) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_MDMMGR(("InitMDMMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error setting the priority of a thread. ", 
			szDevice));
	}
#else
	UNREFERENCED_PARAMETER(Priority);
#endif // ENABLE_PRIORITY_HANDLING

	return EXIT_SUCCESS;
}

/*
Discard old data from a Tritech Micron data modem (thread safe).
Note that a call to this function can take some time.

HMDMMGR hMDMMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of the device.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int DiscardDataMDMMgr(HMDMMGR hMDMMgr)
{
	EnterCriticalSection(&hMDMMgr->MDMCS);

	if (!hMDMMgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_MDMMGR(("DiscardDataMDMMgr error (%s) : %s"
			"(hMDMMgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hMDMMgr));

		LeaveCriticalSection(&hMDMMgr->MDMCS);
		return EXIT_FAILURE;
	}

	if (DiscardDataMDM(hMDMMgr->hMDM) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_MDMMGR(("Connection to a Tritech Micron data modem lost\n"));
		PRINT_DEBUG_ERROR_MDMMGR(("DiscardDataMDMMgr error (%s) : %s"
			"(hMDMMgr=%#x)\n", 
			strtime_m(), 
			"Error communicating with the device. ", 
			hMDMMgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hMDMMgr->bConnected = FALSE; 
		CloseMDM(&hMDMMgr->hMDM);

		LeaveCriticalSection(&hMDMMgr->MDMCS);
		return EXIT_FAILURE;
	}

	LeaveCriticalSection(&hMDMMgr->MDMCS);

	return EXIT_SUCCESS;
}

/*
Send data with a Tritech Micron data modem (thread safe).
Should be used in combination with RecvDataMDMMgr().

HMDMMGR hMDMMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of the device.
uint8* buf : (IN) Valid pointer to the data to send.
UINT buflen : (IN) Number of bytes to send.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SendDataMDMMgr(HMDMMGR hMDMMgr, uint8* buf, UINT buflen)
{
	EnterCriticalSection(&hMDMMgr->MDMCS);

	if (!hMDMMgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_MDMMGR(("SendDataMDMMgr error (%s) : %s"
			"(hMDMMgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hMDMMgr));

		LeaveCriticalSection(&hMDMMgr->MDMCS);
		return EXIT_FAILURE;
	}

	if (SendDataMDM(hMDMMgr->hMDM, buf, buflen) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_MDMMGR(("Connection to a Tritech Micron data modem lost\n"));
		PRINT_DEBUG_ERROR_MDMMGR(("SendDataMDMMgr error (%s) : %s"
			"(hMDMMgr=%#x)\n", 
			strtime_m(), 
			"Error sending data with the device. ", 
			hMDMMgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hMDMMgr->bConnected = FALSE; 
		CloseMDM(&hMDMMgr->hMDM);

		LeaveCriticalSection(&hMDMMgr->MDMCS);
		return EXIT_FAILURE;
	}

	LeaveCriticalSection(&hMDMMgr->MDMCS);

	return EXIT_SUCCESS;
}

/*
Receive data with a Tritech Micron data modem (thread safe).
Should be used in combination with SendDataMDMMgr().

HMDMMGR hMDMMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of the device.
uint8* buf : (INOUT) Valid pointer that will receive the data received.
UINT buflen : (IN) Number of bytes to receive.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int RecvDataMDMMgr(HMDMMGR hMDMMgr, uint8* buf, UINT buflen)
{
	EnterCriticalSection(&hMDMMgr->MDMCS);

	if (!hMDMMgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_MDMMGR(("RecvDataMDMMgr error (%s) : %s"
			"(hMDMMgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hMDMMgr));

		LeaveCriticalSection(&hMDMMgr->MDMCS);
		return EXIT_FAILURE;
	}

	if (RecvDataMDM(hMDMMgr->hMDM, buf, buflen) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_MDMMGR(("Connection to a Tritech Micron data modem lost\n"));
		PRINT_DEBUG_ERROR_MDMMGR(("RecvDataMDMMgr error (%s) : %s"
			"(hMDMMgr=%#x)\n", 
			strtime_m(), 
			"Error sending data with the device. ", 
			hMDMMgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hMDMMgr->bConnected = FALSE; 
		CloseMDM(&hMDMMgr->hMDM);

		LeaveCriticalSection(&hMDMMgr->MDMCS);
		return EXIT_FAILURE;
	}

	LeaveCriticalSection(&hMDMMgr->MDMCS);

	return EXIT_SUCCESS;
}

/*
Send data with a Tritech Micron data modem (thread safe).
Append automatically a CRC-16 at the end of the data to enable error checking.
Should be used in combination with RecvDataCRC16MDMMgr().

HMDMMGR hMDMMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of the device.
uint8* buf : (IN) Valid pointer to the data to send.
UINT buflen : (IN) Number of bytes to send.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SendDataCRC16MDMMgr(HMDMMGR hMDMMgr, uint8* buf, UINT buflen)
{
	EnterCriticalSection(&hMDMMgr->MDMCS);

	if (!hMDMMgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_MDMMGR(("SendDataCRC16MDMMgr error (%s) : %s"
			"(hMDMMgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hMDMMgr));

		LeaveCriticalSection(&hMDMMgr->MDMCS);
		return EXIT_FAILURE;
	}

	if (SendDataCRC16MDM(hMDMMgr->hMDM, buf, buflen) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_MDMMGR(("Connection to a Tritech Micron data modem lost\n"));
		PRINT_DEBUG_ERROR_MDMMGR(("SendDataCRC16MDMMgr error (%s) : %s"
			"(hMDMMgr=%#x)\n", 
			strtime_m(), 
			"Error sending data with the device. ", 
			hMDMMgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hMDMMgr->bConnected = FALSE; 
		CloseMDM(&hMDMMgr->hMDM);

		LeaveCriticalSection(&hMDMMgr->MDMCS);
		return EXIT_FAILURE;
	}

	LeaveCriticalSection(&hMDMMgr->MDMCS);

	return EXIT_SUCCESS;
}

/*
Receive data with a Tritech Micron data modem (thread safe).
Check the CRC-16 at the end of the data.
Should be used in combination with SendDataCRC16MDMMgr().

HMDMMGR hMDMMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of the device.
uint8* buf : (INOUT) Valid pointer that will receive the data received.
UINT buflen : (IN) Number of bytes to receive.

Return : EXIT_SUCCESS, EXIT_INVALID_DATA if bad CRC-16 or EXIT_FAILURE if there is an error.
*/
inline int RecvDataCRC16MDMMgr(HMDMMGR hMDMMgr, uint8* buf, UINT buflen)
{
	EnterCriticalSection(&hMDMMgr->MDMCS);

	if (!hMDMMgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_MDMMGR(("RecvDataCRC16MDMMgr error (%s) : %s"
			"(hMDMMgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hMDMMgr));

		LeaveCriticalSection(&hMDMMgr->MDMCS);
		return EXIT_FAILURE;
	}

	switch (RecvDataCRC16MDM(hMDMMgr->hMDM, buf, buflen))
	{
	case EXIT_SUCCESS:
		break;
	case EXIT_INVALID_DATA:
		PRINT_DEBUG_ERROR_MDMMGR(("RecvDataCRC16MDMMgr error (%s) : %s"
			"(hMDMMgr=%#x)\n", 
			strtime_m(), 
			"Bad CRC-16. ", 
			hMDMMgr));

		LeaveCriticalSection(&hMDMMgr->MDMCS);
		return EXIT_INVALID_DATA;
		break;
	default:
		PRINT_DEBUG_MESSAGE_MDMMGR(("Connection to a Tritech Micron data modem lost\n"));
		PRINT_DEBUG_ERROR_MDMMGR(("RecvDataCRC16MDMMgr error (%s) : %s"
			"(hMDMMgr=%#x)\n", 
			strtime_m(), 
			"Error receiving data with the device. ", 
			hMDMMgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hMDMMgr->bConnected = FALSE; 
		CloseMDM(&hMDMMgr->hMDM);

		LeaveCriticalSection(&hMDMMgr->MDMCS);
		return EXIT_FAILURE;
		break;
	}

	LeaveCriticalSection(&hMDMMgr->MDMCS);

	return EXIT_SUCCESS;
}

/*
Release a structure that manage the polling and the reconnection of a 
Tritech Micron data modem.

HMDMMGR* phMDMMgr : (INOUT) Valid pointer to the identifier of the structure  
that manages the polling and the reconnection of the device.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReleaseMDMMgr(HMDMMGR* phMDMMgr)
{
	BOOL bError = FALSE;

	// Indicate to the thread that it should terminate its loop.
	(*phMDMMgr)->bRunThread = FALSE;

	if (WaitForThread((*phMDMMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MDMMGR(("ReleaseMDMMgr error (%s) : %s"
			"(*phMDMMgr=%#x)\n", 
			strtime_m(), 
			"Error waiting for the thread. ", 
			*phMDMMgr));
		bError = TRUE;
	}

	DeleteCriticalSection(&(*phMDMMgr)->dataCS);
	DeleteCriticalSection(&(*phMDMMgr)->MDMCS);

	free(*phMDMMgr);

	*phMDMMgr = INVALID_HMDMMGR_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // MDMMGR_H
