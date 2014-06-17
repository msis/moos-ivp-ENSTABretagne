/***************************************************************************************************************:')

UE9Mgr.h

Labjack UE9 handling.

Fabrice Le Bars

Created : 2009-06-24

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef UE9MGR_H
#define UE9MGR_H

#include "OSThread.h"
#include "OSCriticalSection.h"
#include "UE9Cfg.h"

/*
Debug macros specific to UE9Mgr.
*/
#ifdef _DEBUG_MESSAGES_UE9UTILS
#	define _DEBUG_MESSAGES_UE9MGR
#endif // _DEBUG_MESSAGES_UE9UTILS

#ifdef _DEBUG_WARNINGS_UE9UTILS
#	define _DEBUG_WARNINGS_UE9MGR
#endif // _DEBUG_WARNINGS_UE9UTILS

#ifdef _DEBUG_ERRORS_UE9UTILS
#	define _DEBUG_ERRORS_UE9MGR
#endif // _DEBUG_ERRORS_UE9UTILS

#ifdef _DEBUG_MESSAGES_UE9MGR
#	define PRINT_DEBUG_MESSAGE_UE9MGR(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_UE9MGR(params)
#endif // _DEBUG_MESSAGES_UE9MGR

#ifdef _DEBUG_WARNINGS_UE9MGR
#	define PRINT_DEBUG_WARNING_UE9MGR(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_UE9MGR(params)
#endif // _DEBUG_WARNINGS_UE9MGR

#ifdef _DEBUG_ERRORS_UE9MGR
#	define PRINT_DEBUG_ERROR_UE9MGR(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_UE9MGR(params)
#endif // _DEBUG_ERRORS_UE9MGR

#define TEMPERATURE_POLLING_MODE_UE9 0x00000001
#define VOLTAGE_POLLING_MODE_UE9 0x00000002

struct UE9MGR
{
	HUE9 hUE9;
	char szDevice[256];
	THREAD_IDENTIFIER ThreadId;
	int PollingTimeInterval;
	int PollingMode;
	BOOL bRunThread;
	BOOL bConnected;
	CRITICAL_SECTION UE9CS;
	CRITICAL_SECTION temperatureCS;
	UINT temperaturecounter;
	double temperature;
	CRITICAL_SECTION voltageCS;
	UINT voltagecounter;
	double voltage;
	BOOL bWaitForFirstData;
};
typedef struct UE9MGR* HUE9MGR;

#define INVALID_HUE9MGR_VALUE NULL

/*
Private function.
Thread function that handles the polling and automatic reconnection of a UE9.

LPVOID lpParam : (INOUT) Parameter of the thread.

Return : 0.
*/
EXTERN_C THREAD_PROC_RETURN_VALUE UE9MgrThreadProc(void* lpParam);

/*
Private function.
Open a UE9 and get data from it.

HUE9 hUE9 : (IN) Identifier of the UE9.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ConnectToUE9(HUE9MGR hUE9Mgr)
{
	// No critical section is needed here because the other functions 
	// should not try to access the device or data while the device is
	// not connected (bConnected = FALSE).

	if (OpenUE9(&hUE9Mgr->hUE9, hUE9Mgr->szDevice) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_UE9MGR(("Unable to connect to a UE9\n"));
		return EXIT_FAILURE;
	}

	// Get data before setting bConnected to TRUE to be sure that
	// the other functions will get valid data.
	if (hUE9Mgr->bWaitForFirstData)
	{
		if (hUE9Mgr->PollingMode & VOLTAGE_POLLING_MODE_UE9)
		{
			if (GetVoltageUE9(hUE9Mgr->hUE9, &hUE9Mgr->voltage, AIN0) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_UE9MGR(("Unable to connect to a UE9\n"));
				return EXIT_FAILURE;
			}
		}
		if (hUE9Mgr->PollingMode & TEMPERATURE_POLLING_MODE_UE9)
		{
			if (GetTemperatureUE9(hUE9Mgr->hUE9, &hUE9Mgr->temperature) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_UE9MGR(("Unable to connect to a UE9\n"));
				return EXIT_FAILURE;
			}
		}
	}

	hUE9Mgr->bConnected = TRUE;
	PRINT_DEBUG_MESSAGE_UE9MGR(("UE9 connected\n"));

	return EXIT_SUCCESS;
}

/*
Initialize a structure that manage the polling and the reconnection of a UE9.

HUE9MGR* phUE9Mgr : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* szDevice : (IN) Path to the device. For example, pass "1" to the function
to open the first.
int PollingTimeInterval : (IN) Time interval between 2 requests of data (in ms).
int PollingMode : (IN) A combination of VOLTAGE_POLLING_MODE_UE9 and 
TEMPERATURE_POLLING_MODE_UE9.
int Priority : (IN) Priority of the thread that manages the polling and the 
reconnection of the device. See SetThreadDefaultPriority().
BOOL bWaitForFirstData : (IN) TRUE to wait for the first entire data in 
InitUE9Mgr(), FALSE if it does not matter if the first data after the 
connection to the device is invalid during some seconds.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitUE9Mgr(HUE9MGR* phUE9Mgr, char* szDevice, 
						 int PollingTimeInterval,
						 int PollingMode,
						 int Priority,
						 BOOL bWaitForFirstData)
{
	*phUE9Mgr = (HUE9MGR)calloc(1, sizeof(struct UE9MGR));

	if (*phUE9Mgr == NULL)
	{
		PRINT_DEBUG_ERROR_UE9MGR(("InitUE9Mgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	sprintf((*phUE9Mgr)->szDevice, "%s", szDevice);

	(*phUE9Mgr)->PollingTimeInterval = PollingTimeInterval;
	(*phUE9Mgr)->PollingMode = PollingMode;
	(*phUE9Mgr)->bConnected = FALSE;
	(*phUE9Mgr)->bRunThread = TRUE;
	(*phUE9Mgr)->voltagecounter = 0;
	(*phUE9Mgr)->voltage = 0;
	(*phUE9Mgr)->temperaturecounter = 0;
	(*phUE9Mgr)->temperature = 0;
	(*phUE9Mgr)->bWaitForFirstData = bWaitForFirstData;

	if (
		(InitCriticalSection(&(*phUE9Mgr)->UE9CS) != EXIT_SUCCESS)||
		(InitCriticalSection(&(*phUE9Mgr)->voltageCS) != EXIT_SUCCESS)||		
		(InitCriticalSection(&(*phUE9Mgr)->temperatureCS) != EXIT_SUCCESS)
		)
	{
		PRINT_DEBUG_ERROR_UE9MGR(("InitUE9Mgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a critical section. ", 
			szDevice));
		DeleteCriticalSection(&(*phUE9Mgr)->temperatureCS);
		DeleteCriticalSection(&(*phUE9Mgr)->voltageCS);
		DeleteCriticalSection(&(*phUE9Mgr)->UE9CS);
		free(*phUE9Mgr);
		return EXIT_FAILURE;
	}

	// First attempt to connect to the device.
	ConnectToUE9(*phUE9Mgr);

	if (CreateDefaultThread(UE9MgrThreadProc, (void*)*phUE9Mgr, &(*phUE9Mgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_UE9MGR(("InitUE9Mgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a thread. ", 
			szDevice));
		if ((*phUE9Mgr)->bConnected)
		{
			CloseUE9(&(*phUE9Mgr)->hUE9);
		}
		DeleteCriticalSection(&(*phUE9Mgr)->temperatureCS);
		DeleteCriticalSection(&(*phUE9Mgr)->voltageCS);
		DeleteCriticalSection(&(*phUE9Mgr)->UE9CS);
		free(*phUE9Mgr);
		return EXIT_FAILURE;
	}

#ifdef ENABLE_PRIORITY_HANDLING
	if (SetThreadDefaultPriority((*phUE9Mgr)->ThreadId, Priority) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_UE9MGR(("InitUE9Mgr error (%s) : %s"
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
Get the voltage at AIN0 from a UE9 (thread safe).
If PollingMode have the VOLTAGE_POLLING_MODE_UE9 flag set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HUE9MGR hUE9Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a UE9.
double* pVoltage : (INOUT) Valid pointer receiving the voltage (in V).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetVoltageUE9Mgr(HUE9MGR hUE9Mgr, double* pVoltage)
{
	if (hUE9Mgr->PollingMode & VOLTAGE_POLLING_MODE_UE9)
	{
		EnterCriticalSection(&hUE9Mgr->voltageCS);

		if (!hUE9Mgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_UE9MGR(("GetVoltageUE9Mgr error (%s) : %s"
				"(hUE9Mgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hUE9Mgr));

			LeaveCriticalSection(&hUE9Mgr->voltageCS);		
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		*pVoltage = hUE9Mgr->voltage;

		LeaveCriticalSection(&hUE9Mgr->voltageCS);
	}
	else
	{
		EnterCriticalSection(&hUE9Mgr->UE9CS);

		if (!hUE9Mgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_UE9MGR(("GetVoltageUE9Mgr error (%s) : %s"
					"(hUE9Mgr=%#x)\n", 
					strtime_m(), 
					"Device not currently connected. ", 
					hUE9Mgr));

			LeaveCriticalSection(&hUE9Mgr->UE9CS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetVoltageUE9(hUE9Mgr->hUE9, pVoltage, AIN0) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_UE9MGR(("Connection to a UE9 lost\n"));
			PRINT_DEBUG_ERROR_UE9MGR(("GetVoltageUE9Mgr error (%s) : %s"
				"(hUE9Mgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hUE9Mgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hUE9Mgr->bConnected = FALSE; 
			CloseUE9(&hUE9Mgr->hUE9);

			LeaveCriticalSection(&hUE9Mgr->UE9CS);
			return EXIT_FAILURE;
		}

		LeaveCriticalSection(&hUE9Mgr->UE9CS);
	}

	return EXIT_SUCCESS;
}

/*
Get the temperature from a UE9 (thread safe).
If PollingMode have the VOLTAGE_POLLING_MODE_UE9 flag set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HUE9MGR hUE9Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a UE9.
double* pTemperature : (INOUT) Valid pointer receiving the temperature (in 
Celsius degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetTemperatureUE9Mgr(HUE9MGR hUE9Mgr, double* pTemperature)
{
	if (hUE9Mgr->PollingMode & TEMPERATURE_POLLING_MODE_UE9)
	{
		EnterCriticalSection(&hUE9Mgr->temperatureCS);

		if (!hUE9Mgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_UE9MGR(("GetTemperatureUE9Mgr error (%s) : %s"
				"(hUE9Mgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hUE9Mgr));

			LeaveCriticalSection(&hUE9Mgr->temperatureCS);
			return EXIT_FAILURE;
		}
	
		// Get the last value stored.
		*pTemperature = hUE9Mgr->temperature;
	
		LeaveCriticalSection(&hUE9Mgr->temperatureCS);
	}
	else
	{
		EnterCriticalSection(&hUE9Mgr->UE9CS);

		if (!hUE9Mgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_UE9MGR(("GetTemperatureUE9Mgr error (%s) : %s"
				"(hUE9Mgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hUE9Mgr));

			LeaveCriticalSection(&hUE9Mgr->UE9CS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetTemperatureUE9(hUE9Mgr->hUE9, pTemperature) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_UE9MGR(("Connection to a UE9 lost\n"));
			PRINT_DEBUG_ERROR_UE9MGR(("GetTemperatureUE9Mgr error (%s) : %s"
				"(hUE9Mgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hUE9Mgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hUE9Mgr->bConnected = FALSE; 
			CloseUE9(&hUE9Mgr->hUE9);
		
			LeaveCriticalSection(&hUE9Mgr->UE9CS);
			return EXIT_FAILURE;
		}

		LeaveCriticalSection(&hUE9Mgr->UE9CS);
	}

	return EXIT_SUCCESS;
}

/*
Modify all PWM pulse width of a Labjack UE9 (thread safe).

HUE9MGR hUE9Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a UE9.
double pulseWidths[MAX_NB_TIMERS_UE9] : (IN) Desired pulse width in ms. For example, if a
servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
or 2 in the other side.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetAllPWMPulseWidthUE9Mgr(HUE9MGR hUE9Mgr, double pulseWidths[MAX_NB_TIMERS_UE9])
{
	EnterCriticalSection(&hUE9Mgr->UE9CS);

	if (!hUE9Mgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_UE9MGR(("SetAllPWMPulseWidthUE9Mgr error (%s) : %s"
			"(hUE9Mgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hUE9Mgr));

		LeaveCriticalSection(&hUE9Mgr->UE9CS);
		return EXIT_FAILURE;
	}

	if (SetAllPWMPulseWidthUE9(hUE9Mgr->hUE9, pulseWidths) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_UE9MGR(("Connection to a UE9 lost\n"));
		PRINT_DEBUG_ERROR_UE9MGR(("SetAllPWMPulseWidthUE9Mgr error (%s) : %s"
			"(hUE9Mgr=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hUE9Mgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hUE9Mgr->bConnected = FALSE; 
		CloseUE9(&hUE9Mgr->hUE9);
	
		LeaveCriticalSection(&hUE9Mgr->UE9CS);
		return EXIT_FAILURE;
	}
		
	LeaveCriticalSection(&hUE9Mgr->UE9CS);

	return EXIT_SUCCESS;
}

/*
Modify a PWM pulse width of a Labjack UE9 (thread safe).

HUE9MGR hUE9Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a UE9.
double pulseWidth : (IN) Desired pulse width in ms. For example, if a
servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
or 2 in the other side.
long pin : (IN) Should be set to FIO0, 1, 2, 3, 4 or FIO5.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetPWMPulseWidthUE9Mgr(HUE9MGR hUE9Mgr, double pulseWidth, long pin)
{
	EnterCriticalSection(&hUE9Mgr->UE9CS);

	if (!hUE9Mgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_UE9MGR(("SetPWMPulseWidthUE9Mgr error (%s) : %s"
			"(hUE9Mgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hUE9Mgr));

		LeaveCriticalSection(&hUE9Mgr->UE9CS);
		return EXIT_FAILURE;
	}

	if (SetPWMPulseWidthUE9(hUE9Mgr->hUE9, pulseWidth, pin) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_UE9MGR(("Connection to a UE9 lost\n"));
		PRINT_DEBUG_ERROR_UE9MGR(("SetPWMPulseWidthUE9Mgr error (%s) : %s"
			"(hUE9Mgr=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hUE9Mgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hUE9Mgr->bConnected = FALSE; 
		CloseUE9(&hUE9Mgr->hUE9);

		LeaveCriticalSection(&hUE9Mgr->UE9CS);
		return EXIT_FAILURE;
	}

	LeaveCriticalSection(&hUE9Mgr->UE9CS);

	return EXIT_SUCCESS;
}

/*
Output a voltage from a Labjack UE9 (thread safe).

HUE9MGR hUE9Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a UE9.
double voltage : (IN) Desired voltage in V. 
long pin : (IN) Should be set to DAC0 or DAC1.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetVoltageUE9Mgr(HUE9MGR hUE9Mgr, double voltage, long pin)
{
	EnterCriticalSection(&hUE9Mgr->UE9CS);

	if (!hUE9Mgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_UE9MGR(("SetVoltageUE9Mgr error (%s) : %s"
			"(hUE9Mgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hUE9Mgr));

		LeaveCriticalSection(&hUE9Mgr->UE9CS);
		return EXIT_FAILURE;
	}

	if (SetVoltageUE9(hUE9Mgr->hUE9, voltage, pin) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_UE9MGR(("Connection to a UE9 lost\n"));
		PRINT_DEBUG_ERROR_UE9MGR(("SetVoltageUE9Mgr error (%s) : %s"
			"(hUE9Mgr=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hUE9Mgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hUE9Mgr->bConnected = FALSE; 
		CloseUE9(&hUE9Mgr->hUE9);

		LeaveCriticalSection(&hUE9Mgr->UE9CS);
		return EXIT_FAILURE;
	}
	
	LeaveCriticalSection(&hUE9Mgr->UE9CS);

	return EXIT_SUCCESS;
}

/*
Release a structure that manage the polling and the reconnection of a UE9.

HUE9MGR* phUE9Mgr : (INOUT) Valid pointer to the identifier of the structure  
that manages the polling and the reconnection of a UE9.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReleaseUE9Mgr(HUE9MGR* phUE9Mgr)
{
	BOOL bError = FALSE;

	// Indicate to the thread that it should terminate its loop.
	(*phUE9Mgr)->bRunThread = FALSE;

	if (WaitForThread((*phUE9Mgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_UE9MGR(("ReleaseUE9Mgr error (%s) : %s"
			"(*phUE9Mgr=%#x)\n", 
			strtime_m(), 
			"Error waiting for the thread. ", 
			*phUE9Mgr));
		bError = TRUE;
	}

	DeleteCriticalSection(&(*phUE9Mgr)->temperatureCS);
	DeleteCriticalSection(&(*phUE9Mgr)->voltageCS);
	DeleteCriticalSection(&(*phUE9Mgr)->UE9CS);

	free(*phUE9Mgr);

	*phUE9Mgr = INVALID_HUE9MGR_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // UE9MGR_H
