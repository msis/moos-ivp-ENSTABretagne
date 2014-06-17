/***************************************************************************************************************:')

SeanetMgr.c

Tritech sonar (compatible with Seanet Pro software) handling.

Fabrice Le Bars

Created : 2012-05-08

Version status : Not finished

***************************************************************************************************************:)*/

#include "SeanetMgr.h"

THREAD_PROC_RETURN_VALUE SeanetMgrThreadProc(void* lpParam)	
{
	HSEANETMGR hSeanetMgr = (HSEANETMGR)lpParam;
	double angle1 = 0;
	uint8* scanline1 = (uint8*)calloc(hSeanetMgr->NBins, sizeof(uint8));
	double angle2 = 0;
	uint8* scanline2 = (uint8*)calloc(hSeanetMgr->NBins, sizeof(uint8));

	do
	{
		if (!hSeanetMgr->bConnected)
		{ 
			mSleep(100);

			// The device is not connected, trying to connect or reconnect.
			ConnectToSeanet(hSeanetMgr);
		}
		else	
		{
			// Wait for new data.
			if (hSeanetMgr->PollingMode & SCANLINE_POLLING_MODE_SEANET)
			{
				EnterCriticalSection(&hSeanetMgr->SeanetCS);

				if (!hSeanetMgr->bConnected)
				{ 
					LeaveCriticalSection(&hSeanetMgr->SeanetCS);
					continue;
				}

				if (GetScanlineSeanet(hSeanetMgr->hSeanet, 
					scanline1, &angle1,
					scanline2, &angle2) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_SEANETMGR(("Connection to a Seanet lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hSeanetMgr->bConnected = FALSE; 
					CloseSeanet(&hSeanetMgr->hSeanet);

					LeaveCriticalSection(&hSeanetMgr->SeanetCS);
					continue;
				}

				LeaveCriticalSection(&hSeanetMgr->SeanetCS);

				EnterCriticalSection(&hSeanetMgr->scanlineCS);
				hSeanetMgr->scanlinecounter++;
				memcpy((char*)hSeanetMgr->scanline1, (char*)scanline1, hSeanetMgr->NBins*sizeof(uint8));
				memcpy((char*)hSeanetMgr->scanline2, (char*)scanline2, hSeanetMgr->NBins*sizeof(uint8));
				hSeanetMgr->angle1 = angle1;
				hSeanetMgr->angle2 = angle2;
				LeaveCriticalSection(&hSeanetMgr->scanlineCS);
			}

			if (hSeanetMgr->PollingMode & SCANLINES_POLLING_MODE_SEANET)
			{
				EnterCriticalSection(&hSeanetMgr->SeanetCS);

				if (!hSeanetMgr->bConnected)
				{ 
					LeaveCriticalSection(&hSeanetMgr->SeanetCS);
					continue;
				}

				if (GetScanlineSeanet(hSeanetMgr->hSeanet, 
					scanline1, &angle1,
					scanline2, &angle2) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_SEANETMGR(("Connection to a Seanet lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hSeanetMgr->bConnected = FALSE; 
					CloseSeanet(&hSeanetMgr->hSeanet);
				
					LeaveCriticalSection(&hSeanetMgr->SeanetCS);
					continue;
				}

				LeaveCriticalSection(&hSeanetMgr->SeanetCS);

				EnterCriticalSection(&hSeanetMgr->scanlineCS);
				memcpy((char*)hSeanetMgr->scanline1, (char*)scanline1, hSeanetMgr->NBins*sizeof(uint8));
				memcpy((char*)hSeanetMgr->scanline2, (char*)scanline2, hSeanetMgr->NBins*sizeof(uint8));
				hSeanetMgr->angle1 = angle1;
				hSeanetMgr->angle2 = angle2;
				LeaveCriticalSection(&hSeanetMgr->scanlineCS);

				EnterCriticalSection(&hSeanetMgr->scanlinesCS);
				hSeanetMgr->scanlinescounter++;

				// Make a shift in the table of pAngles.
				memmove(hSeanetMgr->pAngles, hSeanetMgr->pAngles+2, 
					(hSeanetMgr->NSteps-2)*sizeof(double));

				// Put the angles of the 2 last scanlines at the end of the table of pAngles.
				memcpy((char*)&(hSeanetMgr->pAngles[hSeanetMgr->NSteps-2]), 
					(char*)&angle1, sizeof(double));
				memcpy((char*)&(hSeanetMgr->pAngles[hSeanetMgr->NSteps-1]), 
					(char*)&angle2, sizeof(double));

				// Make a shift in the table of scanlines.
				memmove(hSeanetMgr->pScanlines, hSeanetMgr->pScanlines+2*hSeanetMgr->NBins, 
					(hSeanetMgr->NSteps-2)*hSeanetMgr->NBins*sizeof(uint8));

				// Put the 2 last scanlines at the end of the table of scanlines.
				memcpy((char*)(hSeanetMgr->pScanlines+(hSeanetMgr->NSteps-2)*hSeanetMgr->NBins), 
					(char*)scanline1, hSeanetMgr->NBins*sizeof(uint8));
				memcpy((char*)(hSeanetMgr->pScanlines+(hSeanetMgr->NSteps-1)*hSeanetMgr->NBins), 
					(char*)scanline2, hSeanetMgr->NBins*sizeof(uint8));

				LeaveCriticalSection(&hSeanetMgr->scanlinesCS);
			}
		}
	} 
	while (hSeanetMgr->bRunThread);

	EnterCriticalSection(&hSeanetMgr->SeanetCS);

	if (hSeanetMgr->bConnected)
	{ 
		hSeanetMgr->bConnected = FALSE; 

		if (CloseSeanet(&hSeanetMgr->hSeanet) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_ERROR_SEANETMGR(("SeanetMgrThreadProc error (%s) : %s"
				"\n", 
				strtime_m(), 
				". "));
		}
	}
	
	LeaveCriticalSection(&hSeanetMgr->SeanetCS);

	free(scanline1);scanline1 = NULL;
	free(scanline2);scanline2 = NULL;

	return 0; 
}
