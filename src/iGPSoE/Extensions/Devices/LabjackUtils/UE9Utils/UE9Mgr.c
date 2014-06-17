/***************************************************************************************************************:')

UE9Mgr.c

Labjack UE9 handling.

Fabrice Le Bars

Created : 2009-06-24

Version status : Not finished

***************************************************************************************************************:)*/

#include "UE9Mgr.h"

THREAD_PROC_RETURN_VALUE UE9MgrThreadProc(void* lpParam)	
{
	HUE9MGR hUE9Mgr = (HUE9MGR)lpParam;
	double voltage = 0;
	double temperature = 0;

	do
	{
		mSleep(hUE9Mgr->PollingTimeInterval);

		if (!hUE9Mgr->bConnected)
		{ 
			// The device is not connected, trying to connect or reconnect.
			ConnectToUE9(hUE9Mgr);
		}
		else	
		{
			// Wait for new data.
			if (hUE9Mgr->PollingMode & VOLTAGE_POLLING_MODE_UE9)
			{
				EnterCriticalSection(&hUE9Mgr->UE9CS);

				if (!hUE9Mgr->bConnected)
				{ 
					LeaveCriticalSection(&hUE9Mgr->UE9CS);
					continue;
				}

				if (GetVoltageUE9(hUE9Mgr->hUE9, &voltage, AIN0) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_UE9MGR(("Connection to a UE9 lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hUE9Mgr->bConnected = FALSE; 
					CloseUE9(&hUE9Mgr->hUE9);
				
					LeaveCriticalSection(&hUE9Mgr->UE9CS);
					continue;
				}

				LeaveCriticalSection(&hUE9Mgr->UE9CS);

				EnterCriticalSection(&hUE9Mgr->voltageCS);
				hUE9Mgr->voltagecounter++;
				hUE9Mgr->voltage = voltage;
				LeaveCriticalSection(&hUE9Mgr->voltageCS);
			}

			if (hUE9Mgr->PollingMode & TEMPERATURE_POLLING_MODE_UE9)
			{
				EnterCriticalSection(&hUE9Mgr->UE9CS);

				if (!hUE9Mgr->bConnected)
				{ 
					LeaveCriticalSection(&hUE9Mgr->UE9CS);
					continue;
				}

				if (GetTemperatureUE9(hUE9Mgr->hUE9, &temperature) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_UE9MGR(("Connection to a UE9 lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hUE9Mgr->bConnected = FALSE; 
					CloseUE9(&hUE9Mgr->hUE9);

					LeaveCriticalSection(&hUE9Mgr->UE9CS);
					continue;
				}

				LeaveCriticalSection(&hUE9Mgr->UE9CS);

				EnterCriticalSection(&hUE9Mgr->temperatureCS);
				hUE9Mgr->temperaturecounter++;
				hUE9Mgr->temperature = temperature;
				LeaveCriticalSection(&hUE9Mgr->temperatureCS);
			}
		}
	} 
	while (hUE9Mgr->bRunThread);

	EnterCriticalSection(&hUE9Mgr->UE9CS);

	if (hUE9Mgr->bConnected)
	{ 
		hUE9Mgr->bConnected = FALSE; 

		if (CloseUE9(&hUE9Mgr->hUE9) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_ERROR_UE9MGR(("UE9MgrThreadProc error (%s) : %s"
				"\n", 
				strtime_m(), 
				". "));
		}
	}
	
	LeaveCriticalSection(&hUE9Mgr->UE9CS);

	return 0; 
} 
