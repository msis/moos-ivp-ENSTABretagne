/***************************************************************************************************************:')

MESMgr.h

Tritech Micron echosounder handling.

Fabrice Le Bars

Created : 2011-11-30

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef MESMGR_H
#define MESMGR_H

#include "OSThread.h"
#include "OSCriticalSection.h"
#include "MESCfg.h"

/*
Debug macros specific to MESMgr.
*/
#ifdef _DEBUG_MESSAGES_MESUTILS
#	define _DEBUG_MESSAGES_MESMGR
#endif // _DEBUG_MESSAGES_MESUTILS

#ifdef _DEBUG_WARNINGS_MESUTILS
#	define _DEBUG_WARNINGS_MESMGR
#endif // _DEBUG_WARNINGS_MESUTILS

#ifdef _DEBUG_ERRORS_MESUTILS
#	define _DEBUG_ERRORS_MESMGR
#endif // _DEBUG_ERRORS_MESUTILS

#ifdef _DEBUG_MESSAGES_MESMGR
#	define PRINT_DEBUG_MESSAGE_MESMGR(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MESMGR(params)
#endif // _DEBUG_MESSAGES_MESMGR

#ifdef _DEBUG_WARNINGS_MESMGR
#	define PRINT_DEBUG_WARNING_MESMGR(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MESMGR(params)
#endif // _DEBUG_WARNINGS_MESMGR

#ifdef _DEBUG_ERRORS_MESMGR
#	define PRINT_DEBUG_ERROR_MESMGR(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MESMGR(params)
#endif // _DEBUG_ERRORS_MESMGR

#define DISTANCE_POLLING_MODE_MES 0x00000001

struct MESMGR
{
	HMES hMES;
	char szDevice[256];
	THREAD_IDENTIFIER ThreadId;
	int PollingTimeInterval;
	int PollingMode;
	BOOL bRunThread;
	BOOL bConnected;
	CRITICAL_SECTION MESCS;
	CRITICAL_SECTION distanceCS;
	UINT distancecounter;
	double distance;
	BOOL bWaitForFirstData;
};
typedef struct MESMGR* HMESMGR;

#define INVALID_HMESMGR_VALUE NULL

/*
Private function.
Thread function that handles the polling and automatic reconnection of a 
Tritech Micron echosounder.

LPVOID lpParam : (INOUT) Parameter of the thread.

Return : 0.
*/
EXTERN_C THREAD_PROC_RETURN_VALUE MESMgrThreadProc(void* lpParam);

/*
Private function.
Open a Tritech Micron echosounder and get data from it.

HMES hMES : (IN) Identifier of the MES.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ConnectToMES(HMESMGR hMESMgr)
{
	// No critical section is needed here because the other functions 
	// should not try to access the device or data while the device is
	// not connected (bConnected = FALSE).

	if (OpenMES(&hMESMgr->hMES, hMESMgr->szDevice) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_MESMGR(("Unable to connect to a Tritech Micron echosounder\n"));
		return EXIT_FAILURE;
	}

	// Get data before setting bConnected to TRUE to be sure that
	// the other functions will get valid data.
	if (hMESMgr->bWaitForFirstData)
	{
		if (hMESMgr->PollingMode & DISTANCE_POLLING_MODE_MES)
		{
			if (GetDistanceMES(hMESMgr->hMES, &hMESMgr->distance) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_MESMGR(("Unable to connect to a Tritech Micron echosounder\n"));
				return EXIT_FAILURE;
			}
		}
	}

	hMESMgr->bConnected = TRUE;
	PRINT_DEBUG_MESSAGE_MESMGR(("MES connected\n"));

	return EXIT_SUCCESS;
}

/*
Initialize a structure that manage the polling and the reconnection of a 
Tritech Micron echosounder.

HMESMGR* phMESMgr : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* szDevice : (IN) Device serial port to open.
int PollingTimeInterval : (IN) Time interval between 2 requests of data (in ms).
int PollingMode : (IN) DISTANCE_POLLING_MODE_MES.
int Priority : (IN) Priority of the thread that manages the polling and the 
reconnection of the device. See SetThreadDefaultPriority().
BOOL bWaitForFirstData : (IN) TRUE to wait for the first entire data in 
InitMESMgr(), FALSE if it does not matter if the first data after the 
connection to the device is invalid during some seconds.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitMESMgr(HMESMGR* phMESMgr, char* szDevice, 
						 int PollingTimeInterval,
						 int PollingMode,
						 int Priority,
						 BOOL bWaitForFirstData)
{
	*phMESMgr = (HMESMGR)calloc(1, sizeof(struct MESMGR));

	if (*phMESMgr == NULL)
	{
		PRINT_DEBUG_ERROR_MESMGR(("InitMESMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	sprintf((*phMESMgr)->szDevice, "%s", szDevice);

	(*phMESMgr)->PollingTimeInterval = PollingTimeInterval;
	(*phMESMgr)->PollingMode = PollingMode;
	(*phMESMgr)->bConnected = FALSE;
	(*phMESMgr)->bRunThread = TRUE;
	(*phMESMgr)->distancecounter = 0;
	(*phMESMgr)->distance = 0;
	(*phMESMgr)->bWaitForFirstData = bWaitForFirstData;

	if (
		(InitCriticalSection(&(*phMESMgr)->MESCS) != EXIT_SUCCESS)||
		(InitCriticalSection(&(*phMESMgr)->distanceCS) != EXIT_SUCCESS)
		)
	{
		PRINT_DEBUG_ERROR_MESMGR(("InitMESMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a critical section. ", 
			szDevice));
		DeleteCriticalSection(&(*phMESMgr)->distanceCS);
		DeleteCriticalSection(&(*phMESMgr)->MESCS);
		free(*phMESMgr);
		return EXIT_FAILURE;
	}

	// First attempt to connect to the device.
	ConnectToMES(*phMESMgr);

	if (CreateDefaultThread(MESMgrThreadProc, (void*)*phMESMgr, &(*phMESMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MESMGR(("InitMESMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a thread. ", 
			szDevice));
		if ((*phMESMgr)->bConnected)
		{
			CloseMES(&(*phMESMgr)->hMES);
		}
		DeleteCriticalSection(&(*phMESMgr)->distanceCS);
		DeleteCriticalSection(&(*phMESMgr)->MESCS);
		free(*phMESMgr);
		return EXIT_FAILURE;
	}

#ifdef ENABLE_PRIORITY_HANDLING
	if (SetThreadDefaultPriority((*phMESMgr)->ThreadId, Priority) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_MESMGR(("InitMESMgr error (%s) : %s"
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
Get the distance from a Tritech Micron echosounder (thread safe).
If PollingMode have the DISTANCE_POLLING_MODE_MES flag set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HMESMGR hMESMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of the device.
double* pDistance : (INOUT) Valid pointer receiving the distance (in m).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetDistanceMESMgr(HMESMGR hMESMgr, double* pDistance)
{
	if (hMESMgr->PollingMode & DISTANCE_POLLING_MODE_MES)
	{
		EnterCriticalSection(&hMESMgr->distanceCS);

		if (!hMESMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MESMGR(("GetDistanceMESMgr error (%s) : %s"
				"(hMESMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMESMgr));

			LeaveCriticalSection(&hMESMgr->distanceCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		*pDistance = hMESMgr->distance;

		LeaveCriticalSection(&hMESMgr->distanceCS);
	}
	else
	{
		EnterCriticalSection(&hMESMgr->MESCS);

		if (!hMESMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MESMGR(("GetDistanceMESMgr error (%s) : %s"
				"(hMESMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMESMgr));

			LeaveCriticalSection(&hMESMgr->MESCS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetDistanceMES(hMESMgr->hMES, pDistance) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_MESMGR(("Connection to a Tritech Micron echosounder lost\n"));
			PRINT_DEBUG_ERROR_MESMGR(("GetDistanceMESMgr error (%s) : %s"
				"(hMESMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hMESMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hMESMgr->bConnected = FALSE; 
			CloseMES(&hMESMgr->hMES);

			LeaveCriticalSection(&hMESMgr->MESCS);
			return EXIT_FAILURE;
		}
		
		LeaveCriticalSection(&hMESMgr->MESCS);
	}

	return EXIT_SUCCESS;
}

/*
Release a structure that manage the polling and the reconnection of a 
Tritech Micron echosounder.

HMESMGR* phMESMgr : (INOUT) Valid pointer to the identifier of the structure  
that manages the polling and the reconnection of the device.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReleaseMESMgr(HMESMGR* phMESMgr)
{
	BOOL bError = FALSE;

	// Indicate to the thread that it should terminate its loop.
	(*phMESMgr)->bRunThread = FALSE;

	if (WaitForThread((*phMESMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MESMGR(("ReleaseMESMgr error (%s) : %s"
			"(*phMESMgr=%#x)\n", 
			strtime_m(), 
			"Error waiting for the thread. ", 
			*phMESMgr));
		bError = TRUE;
	}

	DeleteCriticalSection(&(*phMESMgr)->distanceCS);
	DeleteCriticalSection(&(*phMESMgr)->MESCS);

	free(*phMESMgr);

	*phMESMgr = INVALID_HMESMGR_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // MESMGR_H
