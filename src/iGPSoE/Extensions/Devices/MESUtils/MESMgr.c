/***************************************************************************************************************:')

MESMgr.c

Tritech Micron echosounder handling.

Fabrice Le Bars

Created : 2011-11-30

Version status : Not finished

***************************************************************************************************************:)*/

#include "MESMgr.h"

THREAD_PROC_RETURN_VALUE MESMgrThreadProc(void* lpParam)	
{
	HMESMGR hMESMgr = (HMESMGR)lpParam;
	double distance = 0;

	do
	{
		mSleep(hMESMgr->PollingTimeInterval);

		if (!hMESMgr->bConnected)
		{ 
			// The device is not connected, trying to connect or reconnect.
			ConnectToMES(hMESMgr);
		}
		else	
		{
			// Wait for new data.
			if (hMESMgr->PollingMode & DISTANCE_POLLING_MODE_MES)
			{
				EnterCriticalSection(&hMESMgr->MESCS);

				if (!hMESMgr->bConnected)
				{ 
					LeaveCriticalSection(&hMESMgr->MESCS);
					continue;
				}

				if (GetDistanceMES(hMESMgr->hMES, &distance) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_MESMGR(("Connection to a Tritech Micron echosounder lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hMESMgr->bConnected = FALSE; 
					CloseMES(&hMESMgr->hMES);

					LeaveCriticalSection(&hMESMgr->MESCS);
					continue;
				}

				LeaveCriticalSection(&hMESMgr->MESCS);

				EnterCriticalSection(&hMESMgr->distanceCS);
				hMESMgr->distancecounter++;
				hMESMgr->distance = distance;
				LeaveCriticalSection(&hMESMgr->distanceCS);
			}
		}
	} 
	while (hMESMgr->bRunThread);

	EnterCriticalSection(&hMESMgr->MESCS);

	if (hMESMgr->bConnected)
	{ 
		hMESMgr->bConnected = FALSE; 

		if (CloseMES(&hMESMgr->hMES) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_ERROR_MESMGR(("MESMgrThreadProc error (%s) : %s"
				"\n", 
				strtime_m(), 
				". "));
		}
	}
	
	LeaveCriticalSection(&hMESMgr->MESCS);

	return 0; 
} 
