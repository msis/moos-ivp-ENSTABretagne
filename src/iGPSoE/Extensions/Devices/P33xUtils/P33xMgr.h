/***************************************************************************************************************:')

P33xMgr.h

P33x pressure sensor handling.

Fabrice Le Bars

Created : 2009-06-24

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef P33XMGR_H
#define P33XMGR_H

#include "OSThread.h"
#include "OSCriticalSection.h"
#include "P33xCfg.h"

/*
Debug macros specific to P33xMgr.
*/
#ifdef _DEBUG_MESSAGES_P33XUTILS
#	define _DEBUG_MESSAGES_P33XMGR
#endif // _DEBUG_MESSAGES_P33XUTILS

#ifdef _DEBUG_WARNINGS_P33XUTILS
#	define _DEBUG_WARNINGS_P33XMGR
#endif // _DEBUG_WARNINGS_P33XUTILS

#ifdef _DEBUG_ERRORS_P33XUTILS
#	define _DEBUG_ERRORS_P33XMGR
#endif // _DEBUG_ERRORS_P33XUTILS

#ifdef _DEBUG_MESSAGES_P33XMGR
#	define PRINT_DEBUG_MESSAGE_P33XMGR(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_P33XMGR(params)
#endif // _DEBUG_MESSAGES_P33XMGR

#ifdef _DEBUG_WARNINGS_P33XMGR
#	define PRINT_DEBUG_WARNING_P33XMGR(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_P33XMGR(params)
#endif // _DEBUG_WARNINGS_P33XMGR

#ifdef _DEBUG_ERRORS_P33XMGR
#	define PRINT_DEBUG_ERROR_P33XMGR(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_P33XMGR(params)
#endif // _DEBUG_ERRORS_P33XMGR

#define PRESSURE_POLLING_MODE_P33X 0x00000001
#define TEMPERATURE_POLLING_MODE_P33X 0x00000002

struct P33XMGR
{
	HP33X hP33x;
	char szDevice[256];
	THREAD_IDENTIFIER ThreadId;
	int PollingTimeInterval;
	int PollingMode;
	BOOL bRunThread;
	BOOL bConnected;
	CRITICAL_SECTION P33xCS;
	CRITICAL_SECTION pressureCS;
	UINT pressurecounter;
	double pressure;
	CRITICAL_SECTION temperatureCS;
	UINT temperaturecounter;
	double temperature;
	BOOL bWaitForFirstData;
};
typedef struct P33XMGR* HP33XMGR;

#define INVALID_HP33XMGR_VALUE NULL

/*
Private function.
Thread function that handles the polling and automatic reconnection of a P33x.

LPVOID lpParam : (INOUT) Parameter of the thread.

Return : 0.
*/
EXTERN_C THREAD_PROC_RETURN_VALUE P33xMgrThreadProc(void* lpParam);

/*
Private function.
Open a P33x and get data from it.

HP33X hP33x : (IN) Identifier of the P33x.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ConnectToP33x(HP33XMGR hP33xMgr)
{
	// No critical section is needed here because the other functions 
	// should not try to access the device or data while the device is
	// not connected (bConnected = FALSE).

	if (OpenP33x(&hP33xMgr->hP33x, hP33xMgr->szDevice) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_P33XMGR(("Unable to connect to a P33x\n"));
		return EXIT_FAILURE;
	}

	// Get data before setting bConnected to TRUE to be sure that
	// the other functions will get valid data.
	if (hP33xMgr->bWaitForFirstData)
	{
		if (hP33xMgr->PollingMode & PRESSURE_POLLING_MODE_P33X)
		{
			if (GetPressureP33x(hP33xMgr->hP33x, &hP33xMgr->pressure) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_P33XMGR(("Unable to connect to a P33x\n"));
				return EXIT_FAILURE;
			}
		}
		if (hP33xMgr->PollingMode & TEMPERATURE_POLLING_MODE_P33X)
		{
			if (GetTemperatureP33x(hP33xMgr->hP33x, &hP33xMgr->temperature) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_P33XMGR(("Unable to connect to a P33x\n"));
				return EXIT_FAILURE;
			}
		}
	}

	hP33xMgr->bConnected = TRUE;
	PRINT_DEBUG_MESSAGE_P33XMGR(("P33x connected\n"));

	return EXIT_SUCCESS;
}

/*
Initialize a structure that manage the polling and the reconnection of a P33x.

HP33XMGR* phP33xMgr : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* szDevice : (IN) P33x serial port to open.
int PollingTimeInterval : (IN) Time interval between 2 requests of data (in ms).
int PollingMode : (IN) A combination of PRESSURE_POLLING_MODE_P33X and 
TEMPERATURE_POLLING_MODE_P33X.
int Priority : (IN) Priority of the thread that manages the polling and the 
reconnection of the device. See SetThreadDefaultPriority().
BOOL bWaitForFirstData : (IN) TRUE to wait for the first entire data in 
InitP33xMgr(), FALSE if it does not matter if the first data after the 
connection to the device is invalid during some seconds.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitP33xMgr(HP33XMGR* phP33xMgr, char* szDevice, 
						 int PollingTimeInterval,
						 int PollingMode,
						 int Priority,
						 BOOL bWaitForFirstData)
{
	*phP33xMgr = (HP33XMGR)calloc(1, sizeof(struct P33XMGR));

	if (*phP33xMgr == NULL)
	{
		PRINT_DEBUG_ERROR_P33XMGR(("InitP33xMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	sprintf((*phP33xMgr)->szDevice, "%s", szDevice);

	(*phP33xMgr)->PollingTimeInterval = PollingTimeInterval;
	(*phP33xMgr)->PollingMode = PollingMode;
	(*phP33xMgr)->bConnected = FALSE;
	(*phP33xMgr)->bRunThread = TRUE;
	(*phP33xMgr)->pressurecounter = 0;
	(*phP33xMgr)->pressure = 0;
	(*phP33xMgr)->temperaturecounter = 0;
	(*phP33xMgr)->temperature = 0;
	(*phP33xMgr)->bWaitForFirstData = bWaitForFirstData;

	if (
		(InitCriticalSection(&(*phP33xMgr)->P33xCS) != EXIT_SUCCESS)||
		(InitCriticalSection(&(*phP33xMgr)->pressureCS) != EXIT_SUCCESS)||		
		(InitCriticalSection(&(*phP33xMgr)->temperatureCS) != EXIT_SUCCESS)
		)
	{
		PRINT_DEBUG_ERROR_P33XMGR(("InitP33xMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a critical section. ", 
			szDevice));
		DeleteCriticalSection(&(*phP33xMgr)->temperatureCS);
		DeleteCriticalSection(&(*phP33xMgr)->pressureCS);
		DeleteCriticalSection(&(*phP33xMgr)->P33xCS);
		free(*phP33xMgr);
		return EXIT_FAILURE;
	}

	// First attempt to connect to the device.
	ConnectToP33x(*phP33xMgr);

	if (CreateDefaultThread(P33xMgrThreadProc, (void*)*phP33xMgr, &(*phP33xMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_P33XMGR(("InitP33xMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a thread. ", 
			szDevice));
		if ((*phP33xMgr)->bConnected)
		{
			CloseP33x(&(*phP33xMgr)->hP33x);
		}
		DeleteCriticalSection(&(*phP33xMgr)->temperatureCS);
		DeleteCriticalSection(&(*phP33xMgr)->pressureCS);
		DeleteCriticalSection(&(*phP33xMgr)->P33xCS);
		free(*phP33xMgr);
		return EXIT_FAILURE;
	}

#ifdef ENABLE_PRIORITY_HANDLING
	if (SetThreadDefaultPriority((*phP33xMgr)->ThreadId, Priority) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_P33XMGR(("InitP33xMgr error (%s) : %s"
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
Get the pressure from a P33x (thread safe).
If PollingMode have the PRESSURE_POLLING_MODE_P33X flag set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HP33XMGR hP33xMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a P33x.
double* pPressure : (INOUT) Valid pointer receiving the pressure (in bar).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetPressureP33xMgr(HP33XMGR hP33xMgr, double* pPressure)
{
	if (hP33xMgr->PollingMode & PRESSURE_POLLING_MODE_P33X)
	{
		EnterCriticalSection(&hP33xMgr->pressureCS);

		if (!hP33xMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_P33XMGR(("GetPressureP33xMgr error (%s) : %s"
				"(hP33xMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hP33xMgr));

			LeaveCriticalSection(&hP33xMgr->pressureCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		*pPressure = hP33xMgr->pressure;

		LeaveCriticalSection(&hP33xMgr->pressureCS);
	}
	else
	{
		EnterCriticalSection(&hP33xMgr->P33xCS);

		if (!hP33xMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_P33XMGR(("GetPressureP33xMgr error (%s) : %s"
				"(hP33xMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hP33xMgr));

			LeaveCriticalSection(&hP33xMgr->P33xCS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetPressureP33x(hP33xMgr->hP33x, pPressure) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_P33XMGR(("Connection to a P33x lost\n"));
			PRINT_DEBUG_ERROR_P33XMGR(("GetPressureP33xMgr error (%s) : %s"
				"(hP33xMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hP33xMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hP33xMgr->bConnected = FALSE; 
			CloseP33x(&hP33xMgr->hP33x);

			LeaveCriticalSection(&hP33xMgr->P33xCS);
			return EXIT_FAILURE;
		}
		
		LeaveCriticalSection(&hP33xMgr->P33xCS);
	}

	return EXIT_SUCCESS;
}

/*
Get the temperature from a P33x (thread safe).
If PollingMode have the PRESSURE_POLLING_MODE_P33X flag set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HP33XMGR hP33xMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a P33x.
double* pTemperature : (INOUT) Valid pointer receiving the temperature (in 
Celsius degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetTemperatureP33xMgr(HP33XMGR hP33xMgr, double* pTemperature)
{
	if (hP33xMgr->PollingMode & TEMPERATURE_POLLING_MODE_P33X)
	{
		EnterCriticalSection(&hP33xMgr->temperatureCS);

		if (!hP33xMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_P33XMGR(("GetTemperatureP33xMgr error (%s) : %s"
				"(hP33xMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hP33xMgr));

			LeaveCriticalSection(&hP33xMgr->temperatureCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		*pTemperature = hP33xMgr->temperature;

		LeaveCriticalSection(&hP33xMgr->temperatureCS);
	}
	else
	{
		EnterCriticalSection(&hP33xMgr->P33xCS);

		if (!hP33xMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_P33XMGR(("GetTemperatureP33xMgr error (%s) : %s"
				"(hP33xMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hP33xMgr));

			LeaveCriticalSection(&hP33xMgr->P33xCS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetTemperatureP33x(hP33xMgr->hP33x, pTemperature) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_P33XMGR(("Connection to a P33x lost\n"));
			PRINT_DEBUG_ERROR_P33XMGR(("GetTemperatureP33xMgr error (%s) : %s"
				"(hP33xMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hP33xMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hP33xMgr->bConnected = FALSE; 
			CloseP33x(&hP33xMgr->hP33x);

			LeaveCriticalSection(&hP33xMgr->P33xCS);
			return EXIT_FAILURE;
		}

		LeaveCriticalSection(&hP33xMgr->P33xCS);
	}

	return EXIT_SUCCESS;
}

/*
Release a structure that manage the polling and the reconnection of a P33x.

HP33XMGR* phP33xMgr : (INOUT) Valid pointer to the identifier of the structure  
that manages the polling and the reconnection of a P33x.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReleaseP33xMgr(HP33XMGR* phP33xMgr)
{
	BOOL bError = FALSE;

	// Indicate to the thread that it should terminate its loop.
	(*phP33xMgr)->bRunThread = FALSE;

	if (WaitForThread((*phP33xMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_P33XMGR(("ReleaseP33xMgr error (%s) : %s"
			"(*phP33xMgr=%#x)\n", 
			strtime_m(), 
			"Error waiting for the thread. ", 
			*phP33xMgr));
		bError = TRUE;
	}

	DeleteCriticalSection(&(*phP33xMgr)->temperatureCS);
	DeleteCriticalSection(&(*phP33xMgr)->pressureCS);
	DeleteCriticalSection(&(*phP33xMgr)->P33xCS);

	free(*phP33xMgr);

	*phP33xMgr = INVALID_HP33XMGR_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // P33XMGR_H
