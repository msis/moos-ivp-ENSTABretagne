/***************************************************************************************************************:')

U6Mgr.h

Labjack U6 handling.

Fabrice Le Bars

Created : 2012-04-03

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef U6MGR_H
#define U6MGR_H

#include "OSThread.h"
#include "OSCriticalSection.h"
#include "U6Cfg.h"

/*
Debug macros specific to U6Mgr.
*/
#ifdef _DEBUG_MESSAGES_U6UTILS
#	define _DEBUG_MESSAGES_U6MGR
#endif // _DEBUG_MESSAGES_U6UTILS

#ifdef _DEBUG_WARNINGS_U6UTILS
#	define _DEBUG_WARNINGS_U6MGR
#endif // _DEBUG_WARNINGS_U6UTILS

#ifdef _DEBUG_ERRORS_U6UTILS
#	define _DEBUG_ERRORS_U6MGR
#endif // _DEBUG_ERRORS_U6UTILS

#ifdef _DEBUG_MESSAGES_U6MGR
#	define PRINT_DEBUG_MESSAGE_U6MGR(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_U6MGR(params)
#endif // _DEBUG_MESSAGES_U6MGR

#ifdef _DEBUG_WARNINGS_U6MGR
#	define PRINT_DEBUG_WARNING_U6MGR(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_U6MGR(params)
#endif // _DEBUG_WARNINGS_U6MGR

#ifdef _DEBUG_ERRORS_U6MGR
#	define PRINT_DEBUG_ERROR_U6MGR(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_U6MGR(params)
#endif // _DEBUG_ERRORS_U6MGR

#define TEMPERATURE_POLLING_MODE_U6 0x00000001
#define VOLTAGE_POLLING_MODE_U6 0x00000002

struct U6MGR
{
	HU6 hU6;
	char szDevice[256];
	THREAD_IDENTIFIER ThreadId;
	int PollingTimeInterval;
	int PollingMode;
	BOOL bRunThread;
	BOOL bConnected;
	CRITICAL_SECTION U6CS;
	CRITICAL_SECTION temperatureCS;
	UINT temperaturecounter;
	double temperature;
	CRITICAL_SECTION voltageCS;
	UINT voltagecounter;
	double voltage;
	BOOL bWaitForFirstData;
};
typedef struct U6MGR* HU6MGR;

#define INVALID_HU6MGR_VALUE NULL

/*
Private function.
Thread function that handles the polling and automatic reconnection of a U6.

LPVOID lpParam : (INOUT) Parameter of the thread.

Return : 0.
*/
EXTERN_C THREAD_PROC_RETURN_VALUE U6MgrThreadProc(void* lpParam);

/*
Private function.
Open a U6 and get data from it.

HU6 hU6 : (IN) Identifier of the U6.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ConnectToU6(HU6MGR hU6Mgr)
{
	// No critical section is needed here because the other functions 
	// should not try to access the device or data while the device is
	// not connected (bConnected = FALSE).

	if (OpenU6(&hU6Mgr->hU6, hU6Mgr->szDevice) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_U6MGR(("Unable to connect to a U6\n"));
		return EXIT_FAILURE;
	}

	// Get data before setting bConnected to TRUE to be sure that
	// the other functions will get valid data.
	if (hU6Mgr->bWaitForFirstData)
	{
		if (hU6Mgr->PollingMode & VOLTAGE_POLLING_MODE_U6)
		{
			if (GetVoltageU6(hU6Mgr->hU6, &hU6Mgr->voltage, AIN0) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_U6MGR(("Unable to connect to a U6\n"));
				return EXIT_FAILURE;
			}
		}
		if (hU6Mgr->PollingMode & TEMPERATURE_POLLING_MODE_U6)
		{
			if (GetTemperatureU6(hU6Mgr->hU6, &hU6Mgr->temperature) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_U6MGR(("Unable to connect to a U6\n"));
				return EXIT_FAILURE;
			}
		}
	}

	hU6Mgr->bConnected = TRUE;
	PRINT_DEBUG_MESSAGE_U6MGR(("U6 connected\n"));

	return EXIT_SUCCESS;
}

/*
Initialize a structure that manage the polling and the reconnection of a U6.

HU6MGR* phU6Mgr : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* szDevice : (IN) Path to the device. For example, pass "1" to the function
to open the first.
int PollingTimeInterval : (IN) Time interval between 2 requests of data (in ms).
int PollingMode : (IN) A combination of VOLTAGE_POLLING_MODE_U6 and 
TEMPERATURE_POLLING_MODE_U6.
int Priority : (IN) Priority of the thread that manages the polling and the 
reconnection of the device. See SetThreadDefaultPriority().
BOOL bWaitForFirstData : (IN) TRUE to wait for the first entire data in 
InitU6Mgr(), FALSE if it does not matter if the first data after the 
connection to the device is invalid during some seconds.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitU6Mgr(HU6MGR* phU6Mgr, char* szDevice, 
						 int PollingTimeInterval,
						 int PollingMode,
						 int Priority,
						 BOOL bWaitForFirstData)
{
	*phU6Mgr = (HU6MGR)calloc(1, sizeof(struct U6MGR));

	if (*phU6Mgr == NULL)
	{
		PRINT_DEBUG_ERROR_U6MGR(("InitU6Mgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	sprintf((*phU6Mgr)->szDevice, "%s", szDevice);

	(*phU6Mgr)->PollingTimeInterval = PollingTimeInterval;
	(*phU6Mgr)->PollingMode = PollingMode;
	(*phU6Mgr)->bConnected = FALSE;
	(*phU6Mgr)->bRunThread = TRUE;
	(*phU6Mgr)->voltagecounter = 0;
	(*phU6Mgr)->voltage = 0;
	(*phU6Mgr)->temperaturecounter = 0;
	(*phU6Mgr)->temperature = 0;
	(*phU6Mgr)->bWaitForFirstData = bWaitForFirstData;

	if (
		(InitCriticalSection(&(*phU6Mgr)->U6CS) != EXIT_SUCCESS)||
		(InitCriticalSection(&(*phU6Mgr)->voltageCS) != EXIT_SUCCESS)||		
		(InitCriticalSection(&(*phU6Mgr)->temperatureCS) != EXIT_SUCCESS)
		)
	{
		PRINT_DEBUG_ERROR_U6MGR(("InitU6Mgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a critical section. ", 
			szDevice));
		DeleteCriticalSection(&(*phU6Mgr)->temperatureCS);
		DeleteCriticalSection(&(*phU6Mgr)->voltageCS);
		DeleteCriticalSection(&(*phU6Mgr)->U6CS);
		free(*phU6Mgr);
		return EXIT_FAILURE;
	}

	// First attempt to connect to the device.
	ConnectToU6(*phU6Mgr);

	if (CreateDefaultThread(U6MgrThreadProc, (void*)*phU6Mgr, &(*phU6Mgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_U6MGR(("InitU6Mgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a thread. ", 
			szDevice));
		if ((*phU6Mgr)->bConnected)
		{
			CloseU6(&(*phU6Mgr)->hU6);
		}
		DeleteCriticalSection(&(*phU6Mgr)->temperatureCS);
		DeleteCriticalSection(&(*phU6Mgr)->voltageCS);
		DeleteCriticalSection(&(*phU6Mgr)->U6CS);
		free(*phU6Mgr);
		return EXIT_FAILURE;
	}

#ifdef ENABLE_PRIORITY_HANDLING
	if (SetThreadDefaultPriority((*phU6Mgr)->ThreadId, Priority) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_U6MGR(("InitU6Mgr error (%s) : %s"
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
Get the voltage at AIN0 from a U6 (thread safe).
If PollingMode have the VOLTAGE_POLLING_MODE_U6 flag set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HU6MGR hU6Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a U6.
double* pVoltage : (INOUT) Valid pointer receiving the voltage (in V).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetVoltageU6Mgr(HU6MGR hU6Mgr, double* pVoltage)
{
	if (hU6Mgr->PollingMode & VOLTAGE_POLLING_MODE_U6)
	{
		EnterCriticalSection(&hU6Mgr->voltageCS);

		if (!hU6Mgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_U6MGR(("GetVoltageU6Mgr error (%s) : %s"
				"(hU6Mgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hU6Mgr));

			LeaveCriticalSection(&hU6Mgr->voltageCS);		
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		*pVoltage = hU6Mgr->voltage;

		LeaveCriticalSection(&hU6Mgr->voltageCS);
	}
	else
	{
		EnterCriticalSection(&hU6Mgr->U6CS);

		if (!hU6Mgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_U6MGR(("GetVoltageU6Mgr error (%s) : %s"
					"(hU6Mgr=%#x)\n", 
					strtime_m(), 
					"Device not currently connected. ", 
					hU6Mgr));

			LeaveCriticalSection(&hU6Mgr->U6CS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetVoltageU6(hU6Mgr->hU6, pVoltage, AIN0) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_U6MGR(("Connection to a U6 lost\n"));
			PRINT_DEBUG_ERROR_U6MGR(("GetVoltageU6Mgr error (%s) : %s"
				"(hU6Mgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hU6Mgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hU6Mgr->bConnected = FALSE; 
			CloseU6(&hU6Mgr->hU6);

			LeaveCriticalSection(&hU6Mgr->U6CS);
			return EXIT_FAILURE;
		}

		LeaveCriticalSection(&hU6Mgr->U6CS);
	}

	return EXIT_SUCCESS;
}

/*
Get the temperature from a U6 (thread safe).
If PollingMode have the VOLTAGE_POLLING_MODE_U6 flag set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HU6MGR hU6Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a U6.
double* pTemperature : (INOUT) Valid pointer receiving the temperature (in 
Celsius degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetTemperatureU6Mgr(HU6MGR hU6Mgr, double* pTemperature)
{
	if (hU6Mgr->PollingMode & TEMPERATURE_POLLING_MODE_U6)
	{
		EnterCriticalSection(&hU6Mgr->temperatureCS);

		if (!hU6Mgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_U6MGR(("GetTemperatureU6Mgr error (%s) : %s"
				"(hU6Mgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hU6Mgr));

			LeaveCriticalSection(&hU6Mgr->temperatureCS);
			return EXIT_FAILURE;
		}
	
		// Get the last value stored.
		*pTemperature = hU6Mgr->temperature;
	
		LeaveCriticalSection(&hU6Mgr->temperatureCS);
	}
	else
	{
		EnterCriticalSection(&hU6Mgr->U6CS);

		if (!hU6Mgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_U6MGR(("GetTemperatureU6Mgr error (%s) : %s"
				"(hU6Mgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hU6Mgr));

			LeaveCriticalSection(&hU6Mgr->U6CS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetTemperatureU6(hU6Mgr->hU6, pTemperature) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_U6MGR(("Connection to a U6 lost\n"));
			PRINT_DEBUG_ERROR_U6MGR(("GetTemperatureU6Mgr error (%s) : %s"
				"(hU6Mgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hU6Mgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hU6Mgr->bConnected = FALSE; 
			CloseU6(&hU6Mgr->hU6);
		
			LeaveCriticalSection(&hU6Mgr->U6CS);
			return EXIT_FAILURE;
		}

		LeaveCriticalSection(&hU6Mgr->U6CS);
	}

	return EXIT_SUCCESS;
}

/*
Modify all PWM pulse width of a Labjack U6 (thread safe).

HU6MGR hU6Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a U6.
double pulseWidths[MAX_NB_TIMERS_U6] : (IN) Desired pulse width in ms. For example, if a
servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
or 2 in the other side.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetAllPWMPulseWidthU6Mgr(HU6MGR hU6Mgr, double pulseWidths[MAX_NB_TIMERS_U6])
{
	EnterCriticalSection(&hU6Mgr->U6CS);

	if (!hU6Mgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_U6MGR(("SetAllPWMPulseWidthU6Mgr error (%s) : %s"
			"(hU6Mgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hU6Mgr));

		LeaveCriticalSection(&hU6Mgr->U6CS);
		return EXIT_FAILURE;
	}

	if (SetAllPWMPulseWidthU6(hU6Mgr->hU6, pulseWidths) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_U6MGR(("Connection to a U6 lost\n"));
		PRINT_DEBUG_ERROR_U6MGR(("SetAllPWMPulseWidthU6Mgr error (%s) : %s"
			"(hU6Mgr=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hU6Mgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hU6Mgr->bConnected = FALSE; 
		CloseU6(&hU6Mgr->hU6);
	
		LeaveCriticalSection(&hU6Mgr->U6CS);
		return EXIT_FAILURE;
	}
		
	LeaveCriticalSection(&hU6Mgr->U6CS);

	return EXIT_SUCCESS;
}

/*
Modify a PWM pulse width of a Labjack U6 (thread safe).

HU6MGR hU6Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a U6.
double pulseWidth : (IN) Desired pulse width in ms. For example, if a
servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
or 2 in the other side.
long pin : (IN) Should be set to FIO0, 1, 2 or FIO3.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetPWMPulseWidthU6Mgr(HU6MGR hU6Mgr, double pulseWidth, long pin)
{
	EnterCriticalSection(&hU6Mgr->U6CS);

	if (!hU6Mgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_U6MGR(("SetPWMPulseWidthU6Mgr error (%s) : %s"
			"(hU6Mgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hU6Mgr));

		LeaveCriticalSection(&hU6Mgr->U6CS);
		return EXIT_FAILURE;
	}

	if (SetPWMPulseWidthU6(hU6Mgr->hU6, pulseWidth, pin) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_U6MGR(("Connection to a U6 lost\n"));
		PRINT_DEBUG_ERROR_U6MGR(("SetPWMPulseWidthU6Mgr error (%s) : %s"
			"(hU6Mgr=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hU6Mgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hU6Mgr->bConnected = FALSE; 
		CloseU6(&hU6Mgr->hU6);

		LeaveCriticalSection(&hU6Mgr->U6CS);
		return EXIT_FAILURE;
	}

	LeaveCriticalSection(&hU6Mgr->U6CS);

	return EXIT_SUCCESS;
}

/*
Output a voltage from a Labjack U6 (thread safe).

HU6MGR hU6Mgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a U6.
double voltage : (IN) Desired voltage in V. 
long pin : (IN) Should be set to DAC0 or DAC1.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetVoltageU6Mgr(HU6MGR hU6Mgr, double voltage, long pin)
{
	EnterCriticalSection(&hU6Mgr->U6CS);

	if (!hU6Mgr->bConnected)
	{ 
		PRINT_DEBUG_ERROR_U6MGR(("SetVoltageU6Mgr error (%s) : %s"
			"(hU6Mgr=%#x)\n", 
			strtime_m(), 
			"Device not currently connected. ", 
			hU6Mgr));

		LeaveCriticalSection(&hU6Mgr->U6CS);
		return EXIT_FAILURE;
	}

	if (SetVoltageU6(hU6Mgr->hU6, voltage, pin) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_U6MGR(("Connection to a U6 lost\n"));
		PRINT_DEBUG_ERROR_U6MGR(("SetVoltageU6Mgr error (%s) : %s"
			"(hU6Mgr=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hU6Mgr));

		// The device was supposed to be connected before.
		// Notify the thread to try to reconnect the device.
		hU6Mgr->bConnected = FALSE; 
		CloseU6(&hU6Mgr->hU6);

		LeaveCriticalSection(&hU6Mgr->U6CS);
		return EXIT_FAILURE;
	}
	
	LeaveCriticalSection(&hU6Mgr->U6CS);

	return EXIT_SUCCESS;
}

/*
Release a structure that manage the polling and the reconnection of a U6.

HU6MGR* phU6Mgr : (INOUT) Valid pointer to the identifier of the structure  
that manages the polling and the reconnection of a U6.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReleaseU6Mgr(HU6MGR* phU6Mgr)
{
	BOOL bError = FALSE;

	// Indicate to the thread that it should terminate its loop.
	(*phU6Mgr)->bRunThread = FALSE;

	if (WaitForThread((*phU6Mgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_U6MGR(("ReleaseU6Mgr error (%s) : %s"
			"(*phU6Mgr=%#x)\n", 
			strtime_m(), 
			"Error waiting for the thread. ", 
			*phU6Mgr));
		bError = TRUE;
	}

	DeleteCriticalSection(&(*phU6Mgr)->temperatureCS);
	DeleteCriticalSection(&(*phU6Mgr)->voltageCS);
	DeleteCriticalSection(&(*phU6Mgr)->U6CS);

	free(*phU6Mgr);

	*phU6Mgr = INVALID_HU6MGR_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // U6MGR_H
