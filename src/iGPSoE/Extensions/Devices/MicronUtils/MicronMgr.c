/***************************************************************************************************************:')

MicronMgr.c

Micron sonar handling.

Fabrice Le Bars

Created : 2012-07-07

Version status: Not finished

***************************************************************************************************************:)*/

#include "MicronMgr.h"

THREAD_PROC_RETURN_VALUE MicronMgrThreadProc(void* lpParam)	
{
	HMICRONMGR hMicronMgr = (HMICRONMGR)lpParam;
	double angle1 = 0;
	uint8* scanline1 = (uint8*)calloc(hMicronMgr->NBins, sizeof(uint8));
	double angle2 = 0;
	uint8* scanline2 = (uint8*)calloc(hMicronMgr->NBins, sizeof(uint8));

	do
	{
		if (!hMicronMgr->bConnected)
		{ 
			mSleep(100);

			// The device is not connected, trying to connect or reconnect.
			ConnectToMicron(hMicronMgr);
		}
		else	
		{
			// Wait for new data.
			if (hMicronMgr->PollingMode & SCANLINE_POLLING_MODE_MICRON)
			{
				EnterCriticalSection(&hMicronMgr->MicronCS);

				if (!hMicronMgr->bConnected)
				{ 
					LeaveCriticalSection(&hMicronMgr->MicronCS);
					continue;
				}

				if (GetScanlineMicron(hMicronMgr->hMicron, 
					scanline1, &angle1,
					scanline2, &angle2) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_MICRONMGR(("Connection to a Micron lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hMicronMgr->bConnected = FALSE; 
					CloseMicron(&hMicronMgr->hMicron);

					LeaveCriticalSection(&hMicronMgr->MicronCS);
					continue;
				}

				LeaveCriticalSection(&hMicronMgr->MicronCS);

				EnterCriticalSection(&hMicronMgr->scanlineCS);
				hMicronMgr->scanlinecounter++;
				memcpy((char*)hMicronMgr->scanline1, (char*)scanline1, hMicronMgr->NBins*sizeof(uint8));
				memcpy((char*)hMicronMgr->scanline2, (char*)scanline2, hMicronMgr->NBins*sizeof(uint8));
				hMicronMgr->angle1 = angle1;
				hMicronMgr->angle2 = angle2;
				LeaveCriticalSection(&hMicronMgr->scanlineCS);
			}

			if (hMicronMgr->PollingMode & SCANLINES_POLLING_MODE_MICRON)
			{
				EnterCriticalSection(&hMicronMgr->MicronCS);

				if (!hMicronMgr->bConnected)
				{ 
					LeaveCriticalSection(&hMicronMgr->MicronCS);
					continue;
				}

				if (GetScanlineMicron(hMicronMgr->hMicron, 
					scanline1, &angle1,
					scanline2, &angle2) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_MICRONMGR(("Connection to a Micron lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hMicronMgr->bConnected = FALSE; 
					CloseMicron(&hMicronMgr->hMicron);
				
					LeaveCriticalSection(&hMicronMgr->MicronCS);
					continue;
				}

				LeaveCriticalSection(&hMicronMgr->MicronCS);

				EnterCriticalSection(&hMicronMgr->scanlineCS);
				memcpy((char*)hMicronMgr->scanline1, (char*)scanline1, hMicronMgr->NBins*sizeof(uint8));
				memcpy((char*)hMicronMgr->scanline2, (char*)scanline2, hMicronMgr->NBins*sizeof(uint8));
				hMicronMgr->angle1 = angle1;
				hMicronMgr->angle2 = angle2;
				LeaveCriticalSection(&hMicronMgr->scanlineCS);

				EnterCriticalSection(&hMicronMgr->scanlinesCS);
				hMicronMgr->scanlinescounter++;

				// Make a shift in the table of pAngles.
				memmove(hMicronMgr->pAngles, hMicronMgr->pAngles+2, 
					(hMicronMgr->NSteps-2)*sizeof(double));

				// Put the angles of the 2 last scanlines at the end of the table of pAngles.
				memcpy((char*)&(hMicronMgr->pAngles[hMicronMgr->NSteps-2]), 
					(char*)&angle1, sizeof(double));
				memcpy((char*)&(hMicronMgr->pAngles[hMicronMgr->NSteps-1]), 
					(char*)&angle2, sizeof(double));

				// Make a shift in the table of scanlines.
				memmove(hMicronMgr->pScanlines, hMicronMgr->pScanlines+2*hMicronMgr->NBins, 
					(hMicronMgr->NSteps-2)*hMicronMgr->NBins*sizeof(uint8));

				// Put the 2 last scanlines at the end of the table of scanlines.
				memcpy((char*)(hMicronMgr->pScanlines+(hMicronMgr->NSteps-2)*hMicronMgr->NBins), 
					(char*)scanline1, hMicronMgr->NBins*sizeof(uint8));
				memcpy((char*)(hMicronMgr->pScanlines+(hMicronMgr->NSteps-1)*hMicronMgr->NBins), 
					(char*)scanline2, hMicronMgr->NBins*sizeof(uint8));

				LeaveCriticalSection(&hMicronMgr->scanlinesCS);
			}
		}
	} 
	while (hMicronMgr->bRunThread);

	EnterCriticalSection(&hMicronMgr->MicronCS);

	if (hMicronMgr->bConnected)
	{ 
		hMicronMgr->bConnected = FALSE; 

		if (CloseMicron(&hMicronMgr->hMicron) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_ERROR_MICRONMGR(("MicronMgrThreadProc error (%s) : %s"
				"\n", 
				strtime_m(), 
				". "));
		}
	}
	
	LeaveCriticalSection(&hMicronMgr->MicronCS);

	free(scanline1);scanline1 = NULL;
	free(scanline2);scanline2 = NULL;

	return 0; 
} 

