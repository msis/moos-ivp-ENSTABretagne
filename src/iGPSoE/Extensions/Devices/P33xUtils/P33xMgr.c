/***************************************************************************************************************:')

P33xMgr.c

P33x pressure sensor handling.

Fabrice Le Bars

Created : 2009-06-24

Version status : Not finished

***************************************************************************************************************:)*/

#include "P33xMgr.h"

THREAD_PROC_RETURN_VALUE P33xMgrThreadProc(void* lpParam)	
{
	HP33XMGR hP33xMgr = (HP33XMGR)lpParam;
	double pressure = 0;
	double temperature = 0;

	do
	{
		mSleep(hP33xMgr->PollingTimeInterval);

		if (!hP33xMgr->bConnected)
		{ 
			// The device is not connected, trying to connect or reconnect.
			ConnectToP33x(hP33xMgr);
		}
		else	
		{
			// Wait for new data.
			if (hP33xMgr->PollingMode & PRESSURE_POLLING_MODE_P33X)
			{
				EnterCriticalSection(&hP33xMgr->P33xCS);

				if (!hP33xMgr->bConnected)
				{ 
					LeaveCriticalSection(&hP33xMgr->P33xCS);
					continue;
				}

				if (GetPressureP33x(hP33xMgr->hP33x, &pressure) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_P33XMGR(("Connection to a P33x lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hP33xMgr->bConnected = FALSE; 
					CloseP33x(&hP33xMgr->hP33x);

					LeaveCriticalSection(&hP33xMgr->P33xCS);
					continue;
				}

				LeaveCriticalSection(&hP33xMgr->P33xCS);

				EnterCriticalSection(&hP33xMgr->pressureCS);
				hP33xMgr->pressurecounter++;
				hP33xMgr->pressure = pressure;
				LeaveCriticalSection(&hP33xMgr->pressureCS);
			}

			if (hP33xMgr->PollingMode & TEMPERATURE_POLLING_MODE_P33X)
			{
				EnterCriticalSection(&hP33xMgr->P33xCS);

				if (!hP33xMgr->bConnected)
				{ 
					LeaveCriticalSection(&hP33xMgr->P33xCS);
					continue;
				}

				if (GetTemperatureP33x(hP33xMgr->hP33x, &temperature) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_P33XMGR(("Connection to a P33x lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hP33xMgr->bConnected = FALSE; 
					CloseP33x(&hP33xMgr->hP33x);

					LeaveCriticalSection(&hP33xMgr->P33xCS);
					continue;
				}
				
				LeaveCriticalSection(&hP33xMgr->P33xCS);

				EnterCriticalSection(&hP33xMgr->temperatureCS);
				hP33xMgr->temperaturecounter++;
				hP33xMgr->temperature = temperature;
				LeaveCriticalSection(&hP33xMgr->temperatureCS);
			}
		}
	} 
	while (hP33xMgr->bRunThread);

	EnterCriticalSection(&hP33xMgr->P33xCS);

	if (hP33xMgr->bConnected)
	{ 
		hP33xMgr->bConnected = FALSE; 

		if (CloseP33x(&hP33xMgr->hP33x) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_ERROR_P33XMGR(("P33xMgrThreadProc error (%s) : %s"
				"\n", 
				strtime_m(), 
				". "));
		}
	}
	
	LeaveCriticalSection(&hP33xMgr->P33xCS);

	return 0; 
} 
