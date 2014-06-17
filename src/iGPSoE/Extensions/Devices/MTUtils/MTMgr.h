/***************************************************************************************************************:')

MTMgr.h

MT inertial measurement unit handling.

Fabrice Le Bars

Created : 2009-06-24

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef MTMGR_H
#define MTMGR_H

#include "OSThread.h"
#include "OSCriticalSection.h"
#include "MTCfg.h"

/*
Debug macros specific to MTMgr.
*/
#ifdef _DEBUG_MESSAGES_MTUTILS
#	define _DEBUG_MESSAGES_MTMGR
#endif // _DEBUG_MESSAGES_MTUTILS

#ifdef _DEBUG_WARNINGS_MTUTILS
#	define _DEBUG_WARNINGS_MTMGR
#endif // _DEBUG_WARNINGS_MTUTILS

#ifdef _DEBUG_ERRORS_MTUTILS
#	define _DEBUG_ERRORS_MTMGR
#endif // _DEBUG_ERRORS_MTUTILS

#ifdef _DEBUG_MESSAGES_MTMGR
#	define PRINT_DEBUG_MESSAGE_MTMGR(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MTMGR(params)
#endif // _DEBUG_MESSAGES_MTMGR

#ifdef _DEBUG_WARNINGS_MTMGR
#	define PRINT_DEBUG_WARNING_MTMGR(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MTMGR(params)
#endif // _DEBUG_WARNINGS_MTMGR

#ifdef _DEBUG_ERRORS_MTMGR
#	define PRINT_DEBUG_ERROR_MTMGR(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MTMGR(params)
#endif // _DEBUG_ERRORS_MTMGR

#define MTDATA_POLLING_MODE_MT 0x00000001

struct MTMGR
{
	HMT hMT;
	char szDevice[256];
	double MagWeightFactor;
	double Gain;
	BOOL bAdaptToMagneticDisturbances;
	THREAD_IDENTIFIER ThreadId;
	int PollingMode;
	BOOL bRunThread;
	BOOL bConnected;
	CRITICAL_SECTION MTCS;
	CRITICAL_SECTION dataCS;
	UINT datacounter;
	MTDATA data;
	BOOL bWaitForFirstData;
};
typedef struct MTMGR* HMTMGR;

#define INVALID_HMTMGR_VALUE NULL

/*
Private function.
Thread function that handles the polling and automatic reconnection of a MT.

LPVOID lpParam : (INOUT) Parameter of the thread.

Return : 0.
*/
EXTERN_C THREAD_PROC_RETURN_VALUE MTMgrThreadProc(void* lpParam);

/*
Private function.
Open a MT and get data from it.

HMT hMT : (IN) Identifier of the MT.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ConnectToMT(HMTMGR hMTMgr)
{
	// No critical section is needed here because the other functions 
	// should not try to access the device or data while the device is
	// not connected (bConnected = FALSE).

	if (OpenMT(&hMTMgr->hMT, hMTMgr->szDevice, 
		hMTMgr->MagWeightFactor, hMTMgr->Gain, 
		hMTMgr->bAdaptToMagneticDisturbances) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_MTMGR(("Unable to connect to a MT\n"));
		return EXIT_FAILURE;
	}

	// Get data before setting bConnected to TRUE to be sure that
	// the other functions will get valid data.
	if (hMTMgr->bWaitForFirstData)
	{
		if (hMTMgr->PollingMode & MTDATA_POLLING_MODE_MT)
		{
			if (ReadDataMT(hMTMgr->hMT, &hMTMgr->data) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_MTMGR(("Unable to connect to a MT\n"));
				return EXIT_FAILURE;
			}
		}
	}

	hMTMgr->bConnected = TRUE;
	PRINT_DEBUG_MESSAGE_MTMGR(("MT connected\n"));

	return EXIT_SUCCESS;
}

/*
Initialize a structure that manage the polling and the reconnection of a MT.
Note that the time interval between 2 requests of data will be of 10 ms.

HMTMGR* phMTMgr : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* szDevice : (IN) MT serial port to open.
double MagWeightFactor : (IN) Magnetometer weighting factor (Rho) (valid range 
is [0.0,10.0[, default 1.0).
double Gain : (IN) Filter gain (valid range is ]0.01,50.0], default 1.0).
BOOL bAdaptToMagneticDisturbances : (IN) Adapt to magnetic disturbances 
(experimental).
int PollingMode : (IN) Either MTDATA_POLLING_MODE_MT or 0 if you do not
want polling.
int Priority : (IN) Priority of the thread that manages the polling and the 
reconnection of the device. See SetThreadDefaultPriority().
BOOL bWaitForFirstData : (IN) TRUE to wait for the first entire data in 
InitMTMgr(), FALSE if it does not matter if the first data after the 
connection to the device is invalid during some seconds.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitMTMgr(HMTMGR* phMTMgr, char* szDevice, 
						double MagWeightFactor, double Gain, 
						BOOL bAdaptToMagneticDisturbances,
						int PollingMode,
						int Priority,
						BOOL bWaitForFirstData)
{
	*phMTMgr = (HMTMGR)calloc(1, sizeof(struct MTMGR));

	if (*phMTMgr == NULL)
	{
		PRINT_DEBUG_ERROR_MTMGR(("InitMTMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	sprintf((*phMTMgr)->szDevice, "%s", szDevice);

	(*phMTMgr)->MagWeightFactor = MagWeightFactor;
	(*phMTMgr)->Gain = Gain;
	(*phMTMgr)->bAdaptToMagneticDisturbances = bAdaptToMagneticDisturbances;
	(*phMTMgr)->PollingMode = PollingMode;
	(*phMTMgr)->bConnected = FALSE;
	(*phMTMgr)->bRunThread = TRUE;
	(*phMTMgr)->datacounter = 0;
	(*phMTMgr)->bWaitForFirstData = bWaitForFirstData;

	if (
		(InitCriticalSection(&(*phMTMgr)->MTCS) != EXIT_SUCCESS)||
		(InitCriticalSection(&(*phMTMgr)->dataCS) != EXIT_SUCCESS)		
		)
	{
		PRINT_DEBUG_ERROR_MTMGR(("InitMTMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a critical section. ", 
			szDevice));
		DeleteCriticalSection(&(*phMTMgr)->dataCS);
		DeleteCriticalSection(&(*phMTMgr)->MTCS);
		free(*phMTMgr);
		return EXIT_FAILURE;
	}

	// First attempt to connect to the device.
	ConnectToMT(*phMTMgr);

	if (CreateDefaultThread(MTMgrThreadProc, (void*)*phMTMgr, &(*phMTMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MTMGR(("InitMTMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a thread. ", 
			szDevice));
		if ((*phMTMgr)->bConnected)
		{
			CloseMT(&(*phMTMgr)->hMT);
		}
		DeleteCriticalSection(&(*phMTMgr)->dataCS);
		DeleteCriticalSection(&(*phMTMgr)->MTCS);
		free(*phMTMgr);
		return EXIT_FAILURE;
	}

#ifdef ENABLE_PRIORITY_HANDLING
	if (SetThreadDefaultPriority((*phMTMgr)->ThreadId, Priority) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_MTMGR(("InitMTMgr error (%s) : %s"
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
Get the data from a MT (thread safe).
If PollingMode have the MTDATA_POLLING_MODE_MT flag set, get the last data 
polled in the last 10 ms, otherwise a direct thread safe request 
is made to the device.

HMTMGR hMTMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a MT.
MTDATA* pData : (INOUT) Valid pointer receiving the data.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetDataMTMgr(HMTMGR hMTMgr, MTDATA* pData)
{
	if (hMTMgr->PollingMode & MTDATA_POLLING_MODE_MT)
	{
		EnterCriticalSection(&hMTMgr->dataCS);

		if (!hMTMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MTMGR(("GetDataMTMgr error (%s) : %s"
				"(hMTMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMTMgr));

			LeaveCriticalSection(&hMTMgr->dataCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		*pData = hMTMgr->data;

		LeaveCriticalSection(&hMTMgr->dataCS);
	}
	else
	{
		EnterCriticalSection(&hMTMgr->MTCS);

		if (!hMTMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MTMGR(("GetDataMTMgr error (%s) : %s"
				"(hMTMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMTMgr));

			LeaveCriticalSection(&hMTMgr->MTCS);
			return EXIT_FAILURE;
		}

		// Flush the serial port of the device because the hardware is 
		// always sending data every 10 ms.
		if (DiscardDataMT(hMTMgr->hMT) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_MTMGR(("Connection to a MT lost\n"));
			PRINT_DEBUG_ERROR_MTMGR(("GetDataMTMgr error (%s) : %s"
				"(hMTMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hMTMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hMTMgr->bConnected = FALSE; 
			CloseMT(&hMTMgr->hMT);

			LeaveCriticalSection(&hMTMgr->MTCS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (ReadDataMT(hMTMgr->hMT, pData) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_MTMGR(("Connection to a MT lost\n"));
			PRINT_DEBUG_ERROR_MTMGR(("GetDataMTMgr error (%s) : %s"
				"(hMTMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hMTMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hMTMgr->bConnected = FALSE; 
			CloseMT(&hMTMgr->hMT);

			LeaveCriticalSection(&hMTMgr->MTCS);
			return EXIT_FAILURE;
		}

		LeaveCriticalSection(&hMTMgr->MTCS);
	}

	return EXIT_SUCCESS;
}

/*
Release a structure that manage the polling and the reconnection of a MT.

HMTMGR* phMTMgr : (INOUT) Valid pointer to the identifier of the structure  
that manages the polling and the reconnection of a MT.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReleaseMTMgr(HMTMGR* phMTMgr)
{
	BOOL bError = FALSE;

	// Indicate to the thread that it should terminate its loop.
	(*phMTMgr)->bRunThread = FALSE;

	if (WaitForThread((*phMTMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MTMGR(("ReleaseMTMgr error (%s) : %s"
			"(*phMTMgr=%#x)\n", 
			strtime_m(), 
			"Error waiting for the thread. ", 
			*phMTMgr));
		bError = TRUE;
	}

	DeleteCriticalSection(&(*phMTMgr)->dataCS);
	DeleteCriticalSection(&(*phMTMgr)->MTCS);

	free(*phMTMgr);

	*phMTMgr = INVALID_HMTMGR_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // MTMGR_H
