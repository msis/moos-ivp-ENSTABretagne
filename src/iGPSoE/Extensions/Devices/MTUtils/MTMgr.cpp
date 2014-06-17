/***************************************************************************************************************:')

MTMgr.c

MT inertial measurement unit handling.

Fabrice Le Bars

Created : 2009-06-24

Version status : Not finished

***************************************************************************************************************:)*/

#include "MTMgr.h"

THREAD_PROC_RETURN_VALUE MTMgrThreadProc(void* lpParam)	
{
	HMTMGR hMTMgr = (HMTMGR)lpParam;
	MTDATA data;

	do
	{
		if (!hMTMgr->bConnected)
		{ 
			mSleep(100);

			// The device is not connected, trying to connect or reconnect.
			ConnectToMT(hMTMgr);
		}
		else	
		{
			// Wait for new data.
			if (hMTMgr->PollingMode & MTDATA_POLLING_MODE_MT)
			{
				EnterCriticalSection(&hMTMgr->MTCS);

				if (!hMTMgr->bConnected)
				{ 
					LeaveCriticalSection(&hMTMgr->MTCS);
					continue;
				}

				if (ReadDataMT(hMTMgr->hMT, &data) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_MTMGR(("Connection to a MT lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hMTMgr->bConnected = FALSE; 
					CloseMT(&hMTMgr->hMT);
				
					LeaveCriticalSection(&hMTMgr->MTCS);
					continue;
				}

				LeaveCriticalSection(&hMTMgr->MTCS);

				EnterCriticalSection(&hMTMgr->dataCS);
				hMTMgr->datacounter++;
				hMTMgr->data = data;
				LeaveCriticalSection(&hMTMgr->dataCS);
			}
		}
	} 
	while (hMTMgr->bRunThread);

	EnterCriticalSection(&hMTMgr->MTCS);

	if (hMTMgr->bConnected)
	{ 
		hMTMgr->bConnected = FALSE; 

		if (CloseMT(&hMTMgr->hMT) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_ERROR_MTMGR(("MTMgrThreadProc error (%s) : %s"
				"\n", 
				strtime_m(), 
				". "));
		}
	}
	
	LeaveCriticalSection(&hMTMgr->MTCS);

	return 0; 
} 

