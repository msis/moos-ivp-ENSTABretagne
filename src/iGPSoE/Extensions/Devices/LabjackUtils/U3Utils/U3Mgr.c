/***************************************************************************************************************:')

U3Mgr.c

Labjack U3 handling.

Fabrice Le Bars

Created : 2009-06-24

Version status : Not finished

***************************************************************************************************************:)*/

#include "U3Mgr.h"

THREAD_PROC_RETURN_VALUE U3MgrThreadProc(void* lpParam)	
{
	HU3MGR hU3Mgr = (HU3MGR)lpParam;
	double voltage = 0;
	double temperature = 0;

	do
	{
		mSleep(hU3Mgr->PollingTimeInterval);

		if (!hU3Mgr->bConnected)
		{ 
			// The device is not connected, trying to connect or reconnect.
			ConnectToU3(hU3Mgr);
		}
		else	
		{
			// Wait for new data.
			if (hU3Mgr->PollingMode & VOLTAGE_POLLING_MODE_U3)
			{
				EnterCriticalSection(&hU3Mgr->U3CS);

				if (!hU3Mgr->bConnected)
				{ 
					LeaveCriticalSection(&hU3Mgr->U3CS);
					continue;
				}

				if (GetVoltageU3(hU3Mgr->hU3, &voltage, AIN0) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_U3MGR(("Connection to a U3 lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hU3Mgr->bConnected = FALSE; 
					CloseU3(&hU3Mgr->hU3);
				
					LeaveCriticalSection(&hU3Mgr->U3CS);
					continue;
				}

				LeaveCriticalSection(&hU3Mgr->U3CS);

				EnterCriticalSection(&hU3Mgr->voltageCS);
				hU3Mgr->voltagecounter++;
				hU3Mgr->voltage = voltage;
				LeaveCriticalSection(&hU3Mgr->voltageCS);
			}

			if (hU3Mgr->PollingMode & TEMPERATURE_POLLING_MODE_U3)
			{
				EnterCriticalSection(&hU3Mgr->U3CS);

				if (!hU3Mgr->bConnected)
				{ 
					LeaveCriticalSection(&hU3Mgr->U3CS);
					continue;
				}

				if (GetTemperatureU3(hU3Mgr->hU3, &temperature) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_U3MGR(("Connection to a U3 lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hU3Mgr->bConnected = FALSE; 
					CloseU3(&hU3Mgr->hU3);

					LeaveCriticalSection(&hU3Mgr->U3CS);
					continue;
				}

				LeaveCriticalSection(&hU3Mgr->U3CS);

				EnterCriticalSection(&hU3Mgr->temperatureCS);
				hU3Mgr->temperaturecounter++;
				hU3Mgr->temperature = temperature;
				LeaveCriticalSection(&hU3Mgr->temperatureCS);
			}
		}
	} 
	while (hU3Mgr->bRunThread);

	EnterCriticalSection(&hU3Mgr->U3CS);

	if (hU3Mgr->bConnected)
	{ 
		hU3Mgr->bConnected = FALSE; 

		if (CloseU3(&hU3Mgr->hU3) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_ERROR_U3MGR(("U3MgrThreadProc error (%s) : %s"
				"\n", 
				strtime_m(), 
				". "));
		}
	}
	
	LeaveCriticalSection(&hU3Mgr->U3CS);

	return 0; 
} 
