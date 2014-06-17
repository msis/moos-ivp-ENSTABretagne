/***************************************************************************************************************:')

MDMMgr.c

Tritech Micron data modem handling.

Fabrice Le Bars

Created : 2012-07-01

Version status : Not finished

***************************************************************************************************************:)*/

#include "MDMMgr.h"

THREAD_PROC_RETURN_VALUE MDMMgrThreadProc(void* lpParam)	
{
	HMDMMGR hMDMMgr = (HMDMMGR)lpParam;

	do
	{
		mSleep(hMDMMgr->PollingTimeInterval);

		if (!hMDMMgr->bConnected)
		{ 
			// The device is not connected, trying to connect or reconnect.
			ConnectToMDM(hMDMMgr);
		}
	} 
	while (hMDMMgr->bRunThread);

	EnterCriticalSection(&hMDMMgr->MDMCS);

	if (hMDMMgr->bConnected)
	{ 
		hMDMMgr->bConnected = FALSE; 

		if (CloseMDM(&hMDMMgr->hMDM) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_ERROR_MDMMGR(("MDMMgrThreadProc error (%s) : %s"
				"\n", 
				strtime_m(), 
				". "));
		}
	}

	LeaveCriticalSection(&hMDMMgr->MDMCS);

	return 0; 
} 
