/***************************************************************************************************************:')

U6Mgr.c

Labjack U6 handling.

Fabrice Le Bars

Created : 2012-04-03

Version status : Not finished

***************************************************************************************************************:)*/

#include "U6Mgr.h"

THREAD_PROC_RETURN_VALUE U6MgrThreadProc(void* lpParam)	
{
	HU6MGR hU6Mgr = (HU6MGR)lpParam;
	double voltage = 0;
	double temperature = 0;

	do
	{
		mSleep(hU6Mgr->PollingTimeInterval);

		if (!hU6Mgr->bConnected)
		{ 
			// The device is not connected, trying to connect or reconnect.
			ConnectToU6(hU6Mgr);
		}
		else	
		{
			// Wait for new data.
			if (hU6Mgr->PollingMode & VOLTAGE_POLLING_MODE_U6)
			{
				EnterCriticalSection(&hU6Mgr->U6CS);

				if (!hU6Mgr->bConnected)
				{ 
					LeaveCriticalSection(&hU6Mgr->U6CS);
					continue;
				}

				if (GetVoltageU6(hU6Mgr->hU6, &voltage, AIN0) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_U6MGR(("Connection to a U6 lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hU6Mgr->bConnected = FALSE; 
					CloseU6(&hU6Mgr->hU6);
				
					LeaveCriticalSection(&hU6Mgr->U6CS);
					continue;
				}

				LeaveCriticalSection(&hU6Mgr->U6CS);

				EnterCriticalSection(&hU6Mgr->voltageCS);
				hU6Mgr->voltagecounter++;
				hU6Mgr->voltage = voltage;
				LeaveCriticalSection(&hU6Mgr->voltageCS);
			}

			if (hU6Mgr->PollingMode & TEMPERATURE_POLLING_MODE_U6)
			{
				EnterCriticalSection(&hU6Mgr->U6CS);

				if (!hU6Mgr->bConnected)
				{ 
					LeaveCriticalSection(&hU6Mgr->U6CS);
					continue;
				}

				if (GetTemperatureU6(hU6Mgr->hU6, &temperature) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_U6MGR(("Connection to a U6 lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hU6Mgr->bConnected = FALSE; 
					CloseU6(&hU6Mgr->hU6);

					LeaveCriticalSection(&hU6Mgr->U6CS);
					continue;
				}

				LeaveCriticalSection(&hU6Mgr->U6CS);

				EnterCriticalSection(&hU6Mgr->temperatureCS);
				hU6Mgr->temperaturecounter++;
				hU6Mgr->temperature = temperature;
				LeaveCriticalSection(&hU6Mgr->temperatureCS);
			}
		}
	} 
	while (hU6Mgr->bRunThread);

	EnterCriticalSection(&hU6Mgr->U6CS);

	if (hU6Mgr->bConnected)
	{ 
		hU6Mgr->bConnected = FALSE; 

		if (CloseU6(&hU6Mgr->hU6) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_ERROR_U6MGR(("U6MgrThreadProc error (%s) : %s"
				"\n", 
				strtime_m(), 
				". "));
		}
	}
	
	LeaveCriticalSection(&hU6Mgr->U6CS);

	return 0; 
} 
