/***************************************************************************************************************:')

U3Mgr.h

Labjack U3 handling.

Fabrice Le Bars

Created : 2009-06-24

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef U3MGR_H
#define U3MGR_H

#include "OSThread.h"
#include "OSCriticalSection.h"
#include "U3Cfg.h"

/*
Debug macros specific to U3Mgr.
*/
#ifdef _DEBUG_MESSAGES_U3UTILS
#	define _DEBUG_MESSAGES_U3MGR
#endif // _DEBUG_MESSAGES_U3UTILS

#ifdef _DEBUG_WARNINGS_U3UTILS
#	define _DEBUG_WARNINGS_U3MGR
#endif // _DEBUG_WARNINGS_U3UTILS

#ifdef _DEBUG_ERRORS_U3UTILS
#	define _DEBUG_ERRORS_U3MGR
#endif // _DEBUG_ERRORS_U3UTILS

#ifdef _DEBUG_MESSAGES_U3MGR
#	define PRINT_DEBUG_MESSAGE_U3MGR(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_U3MGR(params)
#endif // _DEBUG_MESSAGES_U3MGR

#ifdef _DEBUG_WARNINGS_U3MGR
#	define PRINT_DEBUG_WARNING_U3MGR(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_U3MGR(params)
#endif // _DEBUG_WARNINGS_U3MGR

#ifdef _DEBUG_ERRORS_U3MGR
#	define PRINT_DEBUG_ERROR_U3MGR(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_U3MGR(params)
#endif // _DEBUG_ERRORS_U3MGR

#define TEMPERATURE_POLLING_MODE_U3 0x00000001
#define VOLTAGE_POLLING_MODE_U3 0x00000002

struct U3MGR
{
	HU3 hU3;
	char szDevice[256];
	THREAD_IDENTIFIER ThreadId;
	int PollingTimeInterval;
	int PollingMode;
	BOOL bRunThread;
	BOOL bConnected;
	CRITICAL_SECTION U3CS;
	CRITICAL_SECTION temperatureCS;
	UINT temperaturecounter;
	double temperature;
	CRITICAL_SECTION voltageCS;
	UINT voltagecounter;
	double voltage;
	BOOL bWaitForFirstData;
};
typedef struct U3MGR* HU3MGR;

#define INVALID_HU3MGR_VALUE NULL

/*
Private function.
Thread function that handles the polling and automatic reconnection of a U3.

LPVOID lpParam : (INOUT) Parameter of the thread.

Return : 0.
*/
EXTERN_C THREAD_PROC_RETURN_VALUE U3MgrThreadProc(void* lpParam);

/*
Private function.
Open a U3 and get data from it.

HU3 hU3 : (IN) Identifier of the U3.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ConnectToU3(HU3MGR hU3Mgr)
{
	// No critical section is needed here because the other functions 
	// should not try to access the device or data while the device is
	// not connected (bConnected = FALSE).

	if (OpenU3(&hU3Mgr->hU3, hU3Mgr->szDevice) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_U3MGR(("Unable to connect to a U3\n"));
		return EXIT_FAILURE;
	}

	// Get data before setting bConnected to TRUE to be sure that
	// the other functions will get valid data.
	if (hU3Mgr->bWaitForFirstData)
	{
		if (hU3Mgr->PollingMode & VOLTAGE_POLLING_MODE_U3)
		{
			if (GetVoltageU3(hU3Mgr->hU3, &hU3Mgr->voltage, AIN0) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_U3MGR(("Unable to connect to a U3\n"));
				return EXIT_FAILURE;
			}
		}
		if (hU3Mgr->PollingMode & TEMPERATURE_POLLING_MODE_U3)
		{
			if (GetTemperatureU3(hU3Mgr->hU3, &hU3Mgr->temperature) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_U3MGR(("Unable to connect to a U3\n"));
				return EXIT_FAILURE;
			}
		}
	}

	hU3Mgr->bConnected = TRUE;
	PRINT_DEBUG_MESSAGE_U3MGR(("U3 connected\n"));

	return EXIT_SUCCESS;
}

/*
Initialize a structure that manage the polling and the reconnection of a U3.

HU3MGR* phU3Mgr : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* szDevice : (IN) Path to the device. For example, pass "1" to the function
to open the first.
int PollingTimeInterval : (IN) Time interval between 2 requests of data (in ms).
int PollingMode : (IN) A combination of VOLTAGE_POLLING_MODE_U3 and 
TEMPERATURE_POLLING_MODE_U3.
int Priority : (IN) Priority of the thread that manages the polling and the 
reconnection of the device. See SetThreadDefaultPriority().
BOOL bWaitForFirstData : (IN) TRUE to wait for the first entire data in 
InitU3Mgr(), FALSE if it does not matter if the first data after the 
connection to the device is invalid during some seconds.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitU3Mgr(HU3MGR* phU3Mgr, char* szDevice, 
						 int PollingTimeInterval,
						 int PollingMode,
						 int Priority,
						 BOOL bWaitForFirstData)
{
	*phU3Mgr = (HU3MGR)calloc(1, sizeof(struct U3MGR));

	if (*phU3Mgr == NULL)
	{
		PRINT_DEBUG_ERROR_U3MGR(("InitU3Mgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	sprintf((*phU3Mgr)->szDevice, "%s", szDevice);

	(*phU3Mgr)->PollingTimeInterval = PollingTimeInterval;
	(*phU3Mgr)->PollingMode = PollingMode;
	(*phU3Mgr)->bConnected = FALSE;
	(*phU3Mgr)->bRunThread = TRUE;
	(*phU3Mgr)->voltagecounter = 0;
	(*phU3Mgr)->voltage = 0;
	(*phU3Mgr)->temperaturecounter = 0;
	(*phU3Mgr)->temperature = 0;
	(*phU3Mgr)->bWaitForFirstData = bWaitForFirstData;

	if (
		(InitCriticalSection(&(*phU3Mgr)->U3CS) != EXIT_SUCCESS)||
		(InitCriticalSection(&(*phU3Mgr)->voltageCS) != EXIT_SUCCESS)||		
		(InitCriticalSection(&(*phU3Mgr)->temperatureCS) != EXIT_SUCCESS)
		)
	{
		PRINT_DEBUG_ERROR_U3MGR(("InitU3Mgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a critical section. ", 
			szDevice));
		DeleteCriticalSection(&(*phU3Mgr)->temperatureCS);
		DeleteCriticalSection(&(*phU3Mgr)->voltageCS);
		DeleteCriticalSection(&(*phU3Mgr)->U3CS);
		free(*phU3Mgr);
		return EXIT_FAILURE;
	}

	// First attempt to connect to the device.
	ConnectToU3(*phU3Mgr);

	if (CreateDefaultThread(U3MgrThreadProc, (void*)*phU3Mgr, &(*phU3Mgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_U3MGR(("InitU3Mgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a thread. ", 
			szDevice));
		if ((*phU3Mgr)->bConnected)
		{
			CloseU3(&(*phU3Mgr)->hU3);
		}
		DeleteCriticalSection(&(*phU3Mgr)->temperatureCS);
		DeleteCriticalSection(&(*phU3Mgr)->voltageCS);
		DeleteCriticalSection(&(*phU3Mgr)->U3CS);
		free(*phU3Mgr);
		return EXIT_FAILURE;
	}

#ifdef ENABLE_PRIORITY_HANDLING
	if (SetThreadDefaultPriority((*phU3Mgr)->ThreadId, Priority) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_U3MGR(("InitU3Mgr error (%s) : %s"
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
Get the voltage at AIN0 from a U3 (thread safe).
If PollingMode have the VOLTAGE_POLLING_MODE_U3 flag set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HU3MGR hU3Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a U3.
double* pVoltage : (INOUT) Valid pointer receiving the voltage (in V).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetVoltageU3Mgr(HU3MGR hU3Mgr, double* pVoltage)
{
	if (hU3Mgr->PollingMode & VOLTAGE_POLLING_MODE_U3)
	{
		EnterCriticalSection(&hU3Mgr->voltageCS);

		if (!hU3Mgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_U3MGR(("GetVoltageU3Mgr error (%s) : %s"
				"(hU3Mgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hU3Mgr));

			LeaveCriticalSection(&hU3Mgr->voltageCS);		
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		*pVoltage = hU3Mgr->voltage;

		LeaveCriticalSection(&hU3Mgr->voltageCS);
	}
	else
	{
		EnterCriticalSection(&hU3Mgr->U3CS);

		if (!hU3Mgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_U3MGR(("GetVoltageU3Mgr error (%s) : %s"
					"(hU3Mgr=%#x)\n", 
					strtime_m(), 
					"Device not currently connected. ", 
					hU3Mgr));

			LeaveCriticalSection(&hU3Mgr->U3CS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetVoltageU3(hU3Mgr->hU3, pVoltage, AIN0) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_U3MGR(("Connection to a U3 lost\n"));
			PRINT_DEBUG_ERROR_U3MGR(("GetVoltageU3Mgr error (%s) : %s"
				"(hU3Mgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hU3Mgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hU3Mgr->bConnected = FALSE; 
			CloseU3(&hU3Mgr->hU3);

			LeaveCriticalSection(&hU3Mgr->U3CS);
			return EXIT_FAILURE;
		}

		LeaveCriticalSection(&hU3Mgr->U3CS);
	}

	return EXIT_SUCCESS;
}

/*
Get the temperature from a U3 (thread safe).
If PollingMode have the VOLTAGE_POLLING_MODE_U3 flag set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HU3MGR hU3Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a U3.
double* pTemperature : (INOUT) Valid pointer receiving the temperature (in 
Celsius degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetTemperatureU3Mgr(HU3MGR hU3Mgr, double* pTemperature)
{
	if (hU3Mgr->PollingMode & TEMPERATURE_POLLING_MODE_U3)
	{
		EnterCriticalSection(&hU3Mgr->temperatureCS);

		if (!hU3Mgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_U3MGR(("GetTemperatureU3Mgr error (%s) : %s"
				"(hU3Mgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hU3Mgr));

			LeaveCriticalSection(&hU3Mgr->temperatureCS);
			return EXIT_FAILURE;
		}
	
		// Get the last value stored.
		*pTemperature = hU3Mgr->temperature;
	
		LeaveCriticalSection(&hU3Mgr->temperatureCS);
	}
	else
	{
		EnterCriticalSection(&hU3Mgr->U3CS);

		if (!hU3Mgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_U3MGR(("GetTemperatureU3Mgr error (%s) : %s"
				"(hU3Mgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hU3Mgr));

			LeaveCriticalSection(&hU3Mgr->U3CS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetTemperatureU3(hU3Mgr->hU3, pTemperature) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_U3MGR(("Connection to a U3 lost\n"));
			PRINT_DEBUG_ERROR_U3MGR(("GetTemperatureU3Mgr error (%s) : %s"
				"(hU3Mgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hU3Mgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hU3Mgr->bConnected = FALSE; 
			CloseU3(&hU3Mgr->hU3);
		
			LeaveCriticalSection(&hU3Mgr->U3CS);
			return EXIT_FAILURE;
		}

		LeaveCriticalSection(&hU3Mgr->U3CS);
	}

	return EXIT_SUCCESS;
}

/*
Modify all PWM pulse width of a Labjack U3 (thread safe).

HU3MGR hU3Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a U3.
double pulseWidths[MAX_NB_TIMERS_U3] : (IN) Desired pulse width in ms. For example, if a
servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
or 2 in the other side.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetAllPWMPulseWidthU3Mgr(HU3MGR hU3Mgr, double pulseWidths[MAX_NB_TIMERS_U3])
{
	EnterCriticalSection(&hU3Mgr->U3CS);

	if (!hU3Mgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_U3MGR(("SetAllPWMPulseWidthU3Mgr error (%s) : %s"
			"(hU3Mgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hU3Mgr));

		LeaveCriticalSection(&hU3Mgr->U3CS);
		return EXIT_FAILURE;
	}

	if (SetAllPWMPulseWidthU3(hU3Mgr->hU3, pulseWidths) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_U3MGR(("Connection to a U3 lost\n"));
		PRINT_DEBUG_ERROR_U3MGR(("SetAllPWMPulseWidthU3Mgr error (%s) : %s"
			"(hU3Mgr=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hU3Mgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hU3Mgr->bConnected = FALSE; 
		CloseU3(&hU3Mgr->hU3);
	
		LeaveCriticalSection(&hU3Mgr->U3CS);
		return EXIT_FAILURE;
	}
		
	LeaveCriticalSection(&hU3Mgr->U3CS);

	return EXIT_SUCCESS;
}

/*
Modify a PWM pulse width of a Labjack U3 (thread safe).

HU3MGR hU3Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a U3.
double pulseWidth : (IN) Desired pulse width in ms. For example, if a
servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
or 2 in the other side.
long pin : (IN) Should be set to FIO4 or FIO5.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetPWMPulseWidthU3Mgr(HU3MGR hU3Mgr, double pulseWidth, long pin)
{
	EnterCriticalSection(&hU3Mgr->U3CS);

	if (!hU3Mgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_U3MGR(("SetPWMPulseWidthU3Mgr error (%s) : %s"
			"(hU3Mgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hU3Mgr));

		LeaveCriticalSection(&hU3Mgr->U3CS);
		return EXIT_FAILURE;
	}

	if (SetPWMPulseWidthU3(hU3Mgr->hU3, pulseWidth, pin) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_U3MGR(("Connection to a U3 lost\n"));
		PRINT_DEBUG_ERROR_U3MGR(("SetPWMPulseWidthU3Mgr error (%s) : %s"
			"(hU3Mgr=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hU3Mgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hU3Mgr->bConnected = FALSE; 
		CloseU3(&hU3Mgr->hU3);

		LeaveCriticalSection(&hU3Mgr->U3CS);
		return EXIT_FAILURE;
	}

	LeaveCriticalSection(&hU3Mgr->U3CS);

	return EXIT_SUCCESS;
}

/*
Output a voltage from a Labjack U3 (thread safe).

HU3MGR hU3Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a U3.
double voltage : (IN) Desired voltage in V. 
long pin : (IN) Should be set to DAC0 or DAC1.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetVoltageU3Mgr(HU3MGR hU3Mgr, double voltage, long pin)
{
	EnterCriticalSection(&hU3Mgr->U3CS);

	if (!hU3Mgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_U3MGR(("SetVoltageU3Mgr error (%s) : %s"
			"(hU3Mgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hU3Mgr));

		LeaveCriticalSection(&hU3Mgr->U3CS);
		return EXIT_FAILURE;
	}

	if (SetVoltageU3(hU3Mgr->hU3, voltage, pin) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_U3MGR(("Connection to a U3 lost\n"));
		PRINT_DEBUG_ERROR_U3MGR(("SetVoltageU3Mgr error (%s) : %s"
			"(hU3Mgr=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hU3Mgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hU3Mgr->bConnected = FALSE; 
		CloseU3(&hU3Mgr->hU3);

		LeaveCriticalSection(&hU3Mgr->U3CS);
		return EXIT_FAILURE;
	}
	
	LeaveCriticalSection(&hU3Mgr->U3CS);

	return EXIT_SUCCESS;
}

/*
Release a structure that manage the polling and the reconnection of a U3.

HU3MGR* phU3Mgr : (INOUT) Valid pointer to the identifier of the structure  
that manages the polling and the reconnection of a U3.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReleaseU3Mgr(HU3MGR* phU3Mgr)
{
	BOOL bError = FALSE;

	// Indicate to the thread that it should terminate its loop.
	(*phU3Mgr)->bRunThread = FALSE;

	if (WaitForThread((*phU3Mgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_U3MGR(("ReleaseU3Mgr error (%s) : %s"
			"(*phU3Mgr=%#x)\n", 
			strtime_m(), 
			"Error waiting for the thread. ", 
			*phU3Mgr));
		bError = TRUE;
	}

	DeleteCriticalSection(&(*phU3Mgr)->temperatureCS);
	DeleteCriticalSection(&(*phU3Mgr)->voltageCS);
	DeleteCriticalSection(&(*phU3Mgr)->U3CS);

	free(*phU3Mgr);

	*phU3Mgr = INVALID_HU3MGR_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // U3MGR_H
