/***************************************************************************************************************:')

MiniKingMgr.c

MiniKing sonar handling.

Fabrice Le Bars

Created: 2009-06-24

Version status: Not finished

***************************************************************************************************************:)*/

#include "MiniKingMgr.h"

THREAD_PROC_RETURN_VALUE MiniKingMgrThreadProc(void* lpParam)	
{
	HMINIKINGMGR hMiniKingMgr = (HMINIKINGMGR)lpParam;
	double angle1 = 0;
	uint8* scanline1 = (uint8*)calloc(hMiniKingMgr->NBins, sizeof(uint8));
	double angle2 = 0;
	uint8* scanline2 = (uint8*)calloc(hMiniKingMgr->NBins, sizeof(uint8));

	do
	{
		if (!hMiniKingMgr->bConnected)
		{ 
			mSleep(100);

			// The device is not connected, trying to connect or reconnect.
			ConnectToMiniKing(hMiniKingMgr);
		}
		else	
		{
			// Wait for new data.
			if (hMiniKingMgr->PollingMode & SCANLINE_POLLING_MODE_MINIKING)
			{
				EnterCriticalSection(&hMiniKingMgr->MiniKingCS);

				if (!hMiniKingMgr->bConnected)
				{ 
					LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);
					continue;
				}

				if (GetScanlineMiniKing(hMiniKingMgr->hMiniKing, 
					scanline1, &angle1,
					scanline2, &angle2) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_MINIKINGMGR(("Connection to a MiniKing lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hMiniKingMgr->bConnected = FALSE; 
					CloseMiniKing(&hMiniKingMgr->hMiniKing);

					LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);
					continue;
				}

				LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);

				EnterCriticalSection(&hMiniKingMgr->scanlineCS);
				hMiniKingMgr->scanlinecounter++;
				memcpy((char*)hMiniKingMgr->scanline1, (char*)scanline1, hMiniKingMgr->NBins*sizeof(uint8));
				memcpy((char*)hMiniKingMgr->scanline2, (char*)scanline2, hMiniKingMgr->NBins*sizeof(uint8));
				hMiniKingMgr->angle1 = angle1;
				hMiniKingMgr->angle2 = angle2;
				LeaveCriticalSection(&hMiniKingMgr->scanlineCS);
			}

			if (hMiniKingMgr->PollingMode & SCANLINES_POLLING_MODE_MINIKING)
			{
				EnterCriticalSection(&hMiniKingMgr->MiniKingCS);

				if (!hMiniKingMgr->bConnected)
				{ 
					LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);
					continue;
				}

				if (GetScanlineMiniKing(hMiniKingMgr->hMiniKing, 
					scanline1, &angle1,
					scanline2, &angle2) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_MINIKINGMGR(("Connection to a MiniKing lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hMiniKingMgr->bConnected = FALSE; 
					CloseMiniKing(&hMiniKingMgr->hMiniKing);
				
					LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);
					continue;
				}

				LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);

				EnterCriticalSection(&hMiniKingMgr->scanlineCS);
				memcpy((char*)hMiniKingMgr->scanline1, (char*)scanline1, hMiniKingMgr->NBins*sizeof(uint8));
				memcpy((char*)hMiniKingMgr->scanline2, (char*)scanline2, hMiniKingMgr->NBins*sizeof(uint8));
				hMiniKingMgr->angle1 = angle1;
				hMiniKingMgr->angle2 = angle2;
				LeaveCriticalSection(&hMiniKingMgr->scanlineCS);

				EnterCriticalSection(&hMiniKingMgr->scanlinesCS);
				hMiniKingMgr->scanlinescounter++;

				// Make a shift in the table of pAngles.
				memmove(hMiniKingMgr->pAngles, hMiniKingMgr->pAngles+2, 
					(hMiniKingMgr->NSteps-2)*sizeof(double));

				// Put the angles of the 2 last scanlines at the end of the table of pAngles.
				memcpy((char*)&(hMiniKingMgr->pAngles[hMiniKingMgr->NSteps-2]), 
					(char*)&angle1, sizeof(double));
				memcpy((char*)&(hMiniKingMgr->pAngles[hMiniKingMgr->NSteps-1]), 
					(char*)&angle2, sizeof(double));

				// Make a shift in the table of scanlines.
				memmove(hMiniKingMgr->pScanlines, hMiniKingMgr->pScanlines+2*hMiniKingMgr->NBins, 
					(hMiniKingMgr->NSteps-2)*hMiniKingMgr->NBins*sizeof(uint8));

				// Put the 2 last scanlines at the end of the table of scanlines.
				memcpy((char*)(hMiniKingMgr->pScanlines+(hMiniKingMgr->NSteps-2)*hMiniKingMgr->NBins), 
					(char*)scanline1, hMiniKingMgr->NBins*sizeof(uint8));
				memcpy((char*)(hMiniKingMgr->pScanlines+(hMiniKingMgr->NSteps-1)*hMiniKingMgr->NBins), 
					(char*)scanline2, hMiniKingMgr->NBins*sizeof(uint8));

				LeaveCriticalSection(&hMiniKingMgr->scanlinesCS);
			}
		}
	} 
	while (hMiniKingMgr->bRunThread);

	EnterCriticalSection(&hMiniKingMgr->MiniKingCS);

	if (hMiniKingMgr->bConnected)
	{ 
		hMiniKingMgr->bConnected = FALSE; 

		if (CloseMiniKing(&hMiniKingMgr->hMiniKing) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_ERROR_MINIKINGMGR(("MiniKingMgrThreadProc error (%s) : %s"
				"\n", 
				strtime_m(), 
				". "));
		}
	}
	
	LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);

	free(scanline1);scanline1 = NULL;
	free(scanline2);scanline2 = NULL;

	return 0; 
} 

