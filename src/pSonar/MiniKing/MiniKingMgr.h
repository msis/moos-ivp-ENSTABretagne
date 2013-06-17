/***************************************************************************************************************:')

MiniKingMgr.h

MiniKing sonar handling.

Fabrice Le Bars

Created: 2009-06-24

Version status: Not finished

***************************************************************************************************************:)*/

#ifndef MINIKINGMGR_H
#define MINIKINGMGR_H

#include "OSThread.h"
#include "OSCriticalSection.h"
#include "MiniKingCfg.h"

/*
Debug macros specific to MiniKingMgr.
*/
#ifdef _DEBUG_MESSAGES_MINIKINGUTILS
#	define _DEBUG_MESSAGES_MINIKINGMGR
#endif // _DEBUG_MESSAGES_MINIKINGUTILS

#ifdef _DEBUG_WARNINGS_MINIKINGUTILS
#	define _DEBUG_WARNINGS_MINIKINGMGR
#endif // _DEBUG_WARNINGS_MINIKINGUTILS

#ifdef _DEBUG_ERRORS_MINIKINGUTILS
#	define _DEBUG_ERRORS_MINIKINGMGR
#endif // _DEBUG_ERRORS_MINIKINGUTILS

#ifdef _DEBUG_MESSAGES_MINIKINGMGR
#	define PRINT_DEBUG_MESSAGE_MINIKINGMGR(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MINIKINGMGR(params)
#endif // _DEBUG_MESSAGES_MINIKINGMGR

#ifdef _DEBUG_WARNINGS_MINIKINGMGR
#	define PRINT_DEBUG_WARNING_MINIKINGMGR(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MINIKINGMGR(params)
#endif // _DEBUG_WARNINGS_MINIKINGMGR

#ifdef _DEBUG_ERRORS_MINIKINGMGR
#	define PRINT_DEBUG_ERROR_MINIKINGMGR(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MINIKINGMGR(params)
#endif // _DEBUG_ERRORS_MINIKINGMGR

#define SCANLINE_POLLING_MODE_MINIKING 0x00000001
#define SCANLINES_POLLING_MODE_MINIKING 0x00000002

struct MINIKINGMGR
{
	HMINIKING hMiniKing;
	char szDevice[256];
	int NBins;
	int NSteps;
	int RangeScale;
	double scanDirection;
	double scanWidth;
	int Gain;
	THREAD_IDENTIFIER ThreadId;
	int PollingMode;
	BOOL bRunThread;
	BOOL bConnected;
	CRITICAL_SECTION MiniKingCS;
	CRITICAL_SECTION scanlineCS;
	UINT scanlinecounter;
	uint8* scanline1;
	double angle1;
	uint8* scanline2;
	double angle2;
	CRITICAL_SECTION scanlinesCS;
	UINT scanlinescounter;
	uint8* pScanlines;
	double* pAngles;
	BOOL bWaitForFirstData;
};
typedef struct MINIKINGMGR* HMINIKINGMGR;

#define INVALID_HMINIKINGMGR_VALUE NULL

/*
Private function.
Thread function that handles the polling and automatic reconnection of a MiniKing.

LPVOID lpParam : (INOUT) Parameter of the thread.

Return : 0.
*/
EXTERN_C THREAD_PROC_RETURN_VALUE MiniKingMgrThreadProc(void* lpParam);

/*
Private function.
Open a MiniKing and get data from it.

HMINIKING hMiniKing : (IN) Identifier of the MiniKing.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ConnectToMiniKing(HMINIKINGMGR hMiniKingMgr)
{
	int i = 0;

	// No critical section is needed here because the other functions 
	// should not try to access the device or data while the device is
	// not connected (bConnected = FALSE).

	if (OpenMiniKing(&hMiniKingMgr->hMiniKing, hMiniKingMgr->szDevice, 
		hMiniKingMgr->NBins, hMiniKingMgr->NSteps, hMiniKingMgr->RangeScale,
		hMiniKingMgr->scanDirection, hMiniKingMgr->scanWidth, hMiniKingMgr->Gain) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_MINIKINGMGR(("Unable to connect to a MiniKing\n"));
		return EXIT_FAILURE;
	}

	// Get data before setting bConnected to TRUE to be sure that
	// the other functions will get valid data.
	if (hMiniKingMgr->bWaitForFirstData)
	{
		if (hMiniKingMgr->PollingMode & SCANLINE_POLLING_MODE_MINIKING)
		{
			// Wait for 2 sonar scanlines.
			if (GetScanlineMiniKing(hMiniKingMgr->hMiniKing, 
				hMiniKingMgr->scanline1, &hMiniKingMgr->angle1,
				hMiniKingMgr->scanline2, &hMiniKingMgr->angle2) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_MINIKINGMGR(("Unable to connect to a MiniKing\n"));
				return EXIT_FAILURE;
			}
		}
		if (hMiniKingMgr->PollingMode & SCANLINES_POLLING_MODE_MINIKING)
		{
			// Wait for a complete sonar loop.
			for (i = 0; i < hMiniKingMgr->NSteps; i++)
			{
				if (GetScanlineMiniKing(hMiniKingMgr->hMiniKing, 
					hMiniKingMgr->scanline1, &hMiniKingMgr->angle1,
					hMiniKingMgr->scanline2, &hMiniKingMgr->angle2) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_MINIKINGMGR(("Unable to connect to a MiniKing\n"));
					return EXIT_FAILURE;
				}

				// Make a shift in the table of pAngles.
				memmove(hMiniKingMgr->pAngles, hMiniKingMgr->pAngles+2, 
					(hMiniKingMgr->NSteps-2)*sizeof(double));

				// Put the angles of the 2 last scanlines at the end of the table of pAngles.
				memcpy((char*)&(hMiniKingMgr->pAngles[hMiniKingMgr->NSteps-2]), 
					(char*)&hMiniKingMgr->angle1, sizeof(double));
				memcpy((char*)&(hMiniKingMgr->pAngles[hMiniKingMgr->NSteps-1]), 
					(char*)&hMiniKingMgr->angle2, sizeof(double));

				// Make a shift in the table of scanlines.
				memmove(hMiniKingMgr->pScanlines, hMiniKingMgr->pScanlines+2*hMiniKingMgr->NBins, 
					(hMiniKingMgr->NSteps-2)*hMiniKingMgr->NBins*sizeof(uint8));

				// Put the 2 last scanlines at the end of the table of scanlines.
				memcpy((char*)(hMiniKingMgr->pScanlines+(hMiniKingMgr->NSteps-2)*hMiniKingMgr->NBins), 
					(char*)hMiniKingMgr->scanline1, hMiniKingMgr->NBins*sizeof(uint8));
				memcpy((char*)(hMiniKingMgr->pScanlines+(hMiniKingMgr->NSteps-1)*hMiniKingMgr->NBins), 
					(char*)hMiniKingMgr->scanline2, hMiniKingMgr->NBins*sizeof(uint8));
			}
		}
	}

	hMiniKingMgr->bConnected = TRUE;
	PRINT_DEBUG_MESSAGE_MINIKINGMGR(("MiniKing connected\n"));

	return EXIT_SUCCESS;
}

/*
Initialize a structure that manage the polling and the reconnection of a MiniKing.

HMINIKINGMGR* phMiniKingMgr : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* szDevice : (IN) MiniKing serial port to open.
int NBins : (IN) Number of bins per scanline.
int NSteps : (IN) Number of scanlines in a loop.
int RangeScale : (IN) Range scale (in m).
double scanDirection : (IN) Direction of the sector to scan (in degrees).
double scanWidth : (IN) Width of the sector to scan, 360 for a continuous scan (in degrees).
int Gain : (IN) Gain (in %).
int PollingMode : (IN) Either SCANLINE_POLLING_MODE_MINIKING or 
SCANLINES_POLLING_MODE_MINIKING or 0.
int Priority : (IN) Priority of the thread that manages the polling and the 
reconnection of the device. See SetThreadDefaultPriority().
BOOL bWaitForFirstData : (IN) TRUE to wait for the first entire data in 
InitMiniKingMgr(), FALSE if it does not matter if the first data after the 
connection to the device is invalid during some seconds.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitMiniKingMgr(HMINIKINGMGR* phMiniKingMgr, char* szDevice, 
							 int NBins, int NSteps, int RangeScale,
							 double scanDirection, double scanWidth, int Gain,
							 int PollingMode,
							 int Priority,
							 BOOL bWaitForFirstData)
{
	*phMiniKingMgr = (HMINIKINGMGR)calloc(1, sizeof(struct MINIKINGMGR));

	if (*phMiniKingMgr == NULL)
	{
		PRINT_DEBUG_ERROR_MINIKINGMGR(("InitMiniKingMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	sprintf((*phMiniKingMgr)->szDevice, "%s", szDevice);

	(*phMiniKingMgr)->NBins = NBins;
	(*phMiniKingMgr)->NSteps = NSteps;
	(*phMiniKingMgr)->RangeScale = RangeScale;
	(*phMiniKingMgr)->scanDirection = scanDirection;
	(*phMiniKingMgr)->scanWidth = scanWidth;
	(*phMiniKingMgr)->Gain = Gain;
	(*phMiniKingMgr)->PollingMode = PollingMode;
	(*phMiniKingMgr)->bConnected = FALSE;
	(*phMiniKingMgr)->bRunThread = TRUE;

	(*phMiniKingMgr)->scanlinecounter = 0;
	(*phMiniKingMgr)->scanlinescounter = 0;

	(*phMiniKingMgr)->scanline1 = (uint8*)calloc(NBins, sizeof(uint8));
	(*phMiniKingMgr)->scanline2 = (uint8*)calloc(NBins, sizeof(uint8));
	(*phMiniKingMgr)->pAngles = (double*)calloc(NSteps, sizeof(double));
	(*phMiniKingMgr)->pScanlines = (uint8*)calloc(NSteps*NBins, sizeof(uint8));

	if (
		((*phMiniKingMgr)->scanline1 == NULL)||
		((*phMiniKingMgr)->scanline2 == NULL)||
		((*phMiniKingMgr)->pAngles == NULL)||
		((*phMiniKingMgr)->pScanlines == NULL)
		)
	{
		PRINT_DEBUG_ERROR_MINIKINGMGR(("InitMiniKingMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}
	PRINT_DEBUG_ERROR_MINIKINGMGR(("InitMiniKingMgr error"));
	(*phMiniKingMgr)->angle1 = 0;
	(*phMiniKingMgr)->angle2 = 0;

	(*phMiniKingMgr)->bWaitForFirstData = bWaitForFirstData;

	if (
		(InitCriticalSection(&(*phMiniKingMgr)->MiniKingCS) != EXIT_SUCCESS)||
		(InitCriticalSection(&(*phMiniKingMgr)->scanlineCS) != EXIT_SUCCESS)||		
		(InitCriticalSection(&(*phMiniKingMgr)->scanlinesCS) != EXIT_SUCCESS)
		)
	{
		PRINT_DEBUG_ERROR_MINIKINGMGR(("InitMiniKingMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a critical section. ", 
			szDevice));
		DeleteCriticalSection(&(*phMiniKingMgr)->scanlinesCS);
		DeleteCriticalSection(&(*phMiniKingMgr)->scanlineCS);
		DeleteCriticalSection(&(*phMiniKingMgr)->MiniKingCS);
		free(*phMiniKingMgr);
		return EXIT_FAILURE;
	}

	// First attempt to connect to the device.
	ConnectToMiniKing(*phMiniKingMgr);

	if (CreateDefaultThread(MiniKingMgrThreadProc, (void*)*phMiniKingMgr, &(*phMiniKingMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MINIKINGMGR(("InitMiniKingMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a thread. ", 
			szDevice));
		if ((*phMiniKingMgr)->bConnected)
		{
			CloseMiniKing(&(*phMiniKingMgr)->hMiniKing);
		}
		DeleteCriticalSection(&(*phMiniKingMgr)->scanlinesCS);
		DeleteCriticalSection(&(*phMiniKingMgr)->scanlineCS);
		DeleteCriticalSection(&(*phMiniKingMgr)->MiniKingCS);
		free(*phMiniKingMgr);
		return EXIT_FAILURE;
	}

#ifdef ENABLE_PRIORITY_HANDLING
	if (SetThreadDefaultPriority((*phMiniKingMgr)->ThreadId, Priority) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_MINIKINGMGR(("InitMiniKingMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error setting the priority of a thread. ", 
			szDevice));
	}
#else
	UNREFERENCED_PARAMETER(Priority);
#endif // ENABLE_PRIORITY_HANDLING

	return EXIT_SUCCESS;
}

/*
Get 2 scanlines and the corresponding angles from a MiniKing (thread safe).
If PollingMode have the SCANLINE_POLLING_MODE_MINIKING or 
SCANLINES_POLLING_MODE_MINIKING flags set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HMINIKINGMGR hMiniKingMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a MiniKing.
uint8* scanline1 : (INOUT) Valid pointer that will receive the first scanline.
double* pAngle1 : (INOUT) Valid pointer that will receive the angle of the 
first scanline (in degrees).
uint8* scanline2 : (INOUT) Valid pointer that will receive the second scanline.
double* pAngle2 : (INOUT) Valid pointer that will receive the angle of the 
second scanline (in degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetScanlineMiniKingMgr(HMINIKINGMGR hMiniKingMgr, 
									uint8* scanline1, double* pAngle1, 
									uint8* scanline2, double* pAngle2)
{
	if ((hMiniKingMgr->PollingMode & SCANLINE_POLLING_MODE_MINIKING)||
		(hMiniKingMgr->PollingMode & SCANLINES_POLLING_MODE_MINIKING))
	{
		EnterCriticalSection(&hMiniKingMgr->scanlineCS);

		if (!hMiniKingMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MINIKINGMGR(("GetScanlineMiniKingMgr error (%s) : %s"
				"(hMiniKingMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMiniKingMgr));

			LeaveCriticalSection(&hMiniKingMgr->scanlineCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		memcpy((char*)scanline1, (char*)hMiniKingMgr->scanline1, hMiniKingMgr->NBins*sizeof(uint8));
		memcpy((char*)scanline2, (char*)hMiniKingMgr->scanline2, hMiniKingMgr->NBins*sizeof(uint8));
		*pAngle1 = hMiniKingMgr->angle1;
		*pAngle2 = hMiniKingMgr->angle2;

		LeaveCriticalSection(&hMiniKingMgr->scanlineCS);
	}
	else
	{
		EnterCriticalSection(&hMiniKingMgr->MiniKingCS);

		if (!hMiniKingMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MINIKINGMGR(("GetScanlineMiniKingMgr error (%s) : %s"
				"(hMiniKingMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMiniKingMgr));

			LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);
			return EXIT_FAILURE;
		}

		// Flush the serial port of the device because the hardware is 
		// always sending data every 1 s.
		if (DiscardDataMiniKing(hMiniKingMgr->hMiniKing) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_MINIKINGMGR(("Connection to a MiniKing lost\n"));
			PRINT_DEBUG_ERROR_MINIKINGMGR(("GetScanlineMiniKingMgr error (%s) : %s"
				"(hMiniKingMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hMiniKingMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hMiniKingMgr->bConnected = FALSE; 
			CloseMiniKing(&hMiniKingMgr->hMiniKing);

			LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetScanlineMiniKing(hMiniKingMgr->hMiniKing, 
			scanline1, pAngle1,
			scanline2, pAngle2) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_MINIKINGMGR(("Connection to a MiniKing lost\n"));
			PRINT_DEBUG_ERROR_MINIKINGMGR(("GetScanlineMiniKingMgr error (%s) : %s"
				"(hMiniKingMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hMiniKingMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hMiniKingMgr->bConnected = FALSE; 
			CloseMiniKing(&hMiniKingMgr->hMiniKing);

			LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);
			return EXIT_FAILURE;
		}

		LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);
	}

	return EXIT_SUCCESS;
}

/*
Get the scanlines of the last loop and the corresponding angles from a MiniKing 
(thread safe).
If PollingMode have the SCANLINES_POLLING_MODE_MINIKING flag set, get the last 
data polled in the last PollingTimeInterval, otherwise a direct thread safe 
request is made to the device.

HMINIKINGMGR hMiniKingMgr : (IN) Identifier of the structure that manages the 
polling and the reconnection of a MiniKing.
uint8* pScanlines : (INOUT) Valid pointer that will receive the scanlines of 
the last loop.
double* pAngles : (INOUT) Valid pointer that will receive the angles of the 
scanlines of the last loop (in degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetScanlinesMiniKingMgr(HMINIKINGMGR hMiniKingMgr, 
									uint8* pScanlines, double* pAngles)
{
	if (hMiniKingMgr->PollingMode & SCANLINES_POLLING_MODE_MINIKING)
	{
		EnterCriticalSection(&hMiniKingMgr->scanlinesCS);

		if (!hMiniKingMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MINIKINGMGR(("GetScanlinesMiniKingMgr error (%s) : %s"
				"(hMiniKingMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMiniKingMgr));

			LeaveCriticalSection(&hMiniKingMgr->scanlinesCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		memcpy((char*)pAngles, (char*)hMiniKingMgr->pAngles, 
			hMiniKingMgr->NSteps*sizeof(double));
		memcpy((char*)pScanlines, (char*)hMiniKingMgr->pScanlines, 
			hMiniKingMgr->NSteps*hMiniKingMgr->NBins*sizeof(uint8));

		LeaveCriticalSection(&hMiniKingMgr->scanlinesCS);
	}
	else
	{
		PRINT_DEBUG_ERROR_MINIKINGMGR(("GetScanlinesMiniKingMgr error (%s) : %s"
			"(hMiniKingMgr=%#x)\n", 
			strtime_m(), 
			"Data not available in non polling mode. ", 
			hMiniKingMgr));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Return the distance to the first obstacle from a MiniKing scanline.

HMINIKINGMGR hMiniKingMgr : (IN) Identifier of the structure that manages the polling 
uint8* scanline : (IN) Pointer to the sonar scanline to process.
uint8 threshold : (IN) Threshold that indicates an obstacle (from 0 to 255).
double minDist : (IN) Distance from which we begin the search of the first 
obstacle (in m).
double maxDist : (IN) Distance to which we stop the search of the first 
obstacle (in m).
double* pDist : (INOUT) Valid pointer that will receive the distance to the 
first obstacle in m. Return -1 if all the bins are under the threshold (i.e. 
no obstacle).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ComputeFirstObstacleDistFromScanlineMiniKingMgr(HMINIKINGMGR hMiniKingMgr,
								 uint8* scanline, uint8 threshold, 
								 double minDist, double maxDist, double* pDist)
{
	return ComputeFirstObstacleDistFromScanlineMiniKing(hMiniKingMgr->hMiniKing,
								 scanline, threshold, 
								 minDist, maxDist, pDist);
}

/*
Get the distance to the first obstacle from 2 scanlines and the 
corresponding angles from a MiniKing (thread safe).
If PollingMode have the SCANLINE_POLLING_MODE_MINIKING or 
SCANLINES_POLLING_MODE_MINIKING flags set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HMINIKINGMGR hMiniKingMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a MiniKing.
uint8 threshold : (IN) Threshold that indicates an obstacle (from 0 to 255).
double minDist : (IN) Distance from which we begin the search of the first 
obstacle (in m).
double maxDist : (IN) Distance to which we stop the search of the first 
obstacle (in m).
double* pAngle1 : (INOUT) Valid pointer that will receive the angle of the 
first scanline (in degrees).
double* pDist1 : (INOUT) Valid pointer that will receive the distance to the 
first obstacle in m. Return -1 if all the bins are under the threshold (i.e. 
no obstacle).
double* pAngle2 : (INOUT) Valid pointer that will receive the angle of the 
second scanline (in degrees).
double* pDist2 : (INOUT) Valid pointer that will receive the distance to the 
first obstacle in m. Return -1 if all the bins are under the threshold (i.e. 
no obstacle).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetFirstObstacleDistMiniKingMgr(HMINIKINGMGR hMiniKingMgr,
								 uint8 threshold, double minDist, double maxDist, 
								 double* pAngle1, double* pDist1, double* pAngle2, double* pDist2)
{
	if ((hMiniKingMgr->PollingMode & SCANLINE_POLLING_MODE_MINIKING)||
		(hMiniKingMgr->PollingMode & SCANLINES_POLLING_MODE_MINIKING))
	{
		EnterCriticalSection(&hMiniKingMgr->scanlineCS);

		if (!hMiniKingMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MINIKINGMGR(("GetFirstObstacleDistMiniKingMgr error (%s) : %s"
				"(hMiniKingMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMiniKingMgr));

			LeaveCriticalSection(&hMiniKingMgr->scanlineCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		*pAngle1 = hMiniKingMgr->angle1;
		ComputeFirstObstacleDistFromScanlineMiniKing(hMiniKingMgr->hMiniKing,
								 hMiniKingMgr->scanline1, threshold, 
								 minDist, maxDist, pDist1);
		*pAngle2 = hMiniKingMgr->angle2;
		ComputeFirstObstacleDistFromScanlineMiniKing(hMiniKingMgr->hMiniKing,
								 hMiniKingMgr->scanline2, threshold, 
								 minDist, maxDist, pDist2);

		LeaveCriticalSection(&hMiniKingMgr->scanlineCS);
	}
	else
	{
		EnterCriticalSection(&hMiniKingMgr->MiniKingCS);

		if (!hMiniKingMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MINIKINGMGR(("GetFirstObstacleDistMiniKingMgr error (%s) : %s"
				"(hMiniKingMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMiniKingMgr));

			LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);
			return EXIT_FAILURE;
		}

		// Flush the serial port of the device because the hardware is 
		// always sending data every 1 s.
		if (DiscardDataMiniKing(hMiniKingMgr->hMiniKing) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_MINIKINGMGR(("Connection to a MiniKing lost\n"));
			PRINT_DEBUG_ERROR_MINIKINGMGR(("GetFirstObstacleDistMiniKingMgr error (%s) : %s"
				"(hMiniKingMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hMiniKingMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hMiniKingMgr->bConnected = FALSE; 
			CloseMiniKing(&hMiniKingMgr->hMiniKing);

			LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetScanlineMiniKing(hMiniKingMgr->hMiniKing, 
			hMiniKingMgr->scanline1, &hMiniKingMgr->angle1,
			hMiniKingMgr->scanline2, &hMiniKingMgr->angle2) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_MINIKINGMGR(("Connection to a MiniKing lost\n"));
			PRINT_DEBUG_ERROR_MINIKINGMGR(("GetFirstObstacleDistMiniKingMgr error (%s) : %s"
				"(hMiniKingMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hMiniKingMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hMiniKingMgr->bConnected = FALSE; 
			CloseMiniKing(&hMiniKingMgr->hMiniKing);

			LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);
			return EXIT_FAILURE;
		}

		*pAngle1 = hMiniKingMgr->angle1;
		ComputeFirstObstacleDistFromScanlineMiniKing(hMiniKingMgr->hMiniKing,
								 hMiniKingMgr->scanline1, threshold, 
								 minDist, maxDist, pDist1);
		*pAngle2 = hMiniKingMgr->angle2;
		ComputeFirstObstacleDistFromScanlineMiniKing(hMiniKingMgr->hMiniKing,
								 hMiniKingMgr->scanline2, threshold, 
								 minDist, maxDist, pDist2);

		LeaveCriticalSection(&hMiniKingMgr->MiniKingCS);
	}

	return EXIT_SUCCESS;
}

/*
Release a structure that manage the polling and the reconnection of a MiniKing.

HMINIKINGMGR* phMiniKingMgr : (INOUT) Valid pointer to the identifier of the structure  
that manages the polling and the reconnection of a MiniKing.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReleaseMiniKingMgr(HMINIKINGMGR* phMiniKingMgr)
{
	BOOL bError = FALSE;

	// Indicate to the thread that it should terminate its loop.
	(*phMiniKingMgr)->bRunThread = FALSE;

	if (WaitForThread((*phMiniKingMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MINIKINGMGR(("ReleaseMiniKingMgr error (%s) : %s"
			"(*phMiniKingMgr=%#x)\n", 
			strtime_m(), 
			"Error waiting for the thread. ", 
			*phMiniKingMgr));
		bError = TRUE;
	}

	free((*phMiniKingMgr)->pScanlines);(*phMiniKingMgr)->pScanlines = NULL;
	free((*phMiniKingMgr)->pAngles);(*phMiniKingMgr)->pAngles = NULL;
	free((*phMiniKingMgr)->scanline2);(*phMiniKingMgr)->scanline2 = NULL;
	free((*phMiniKingMgr)->scanline1);(*phMiniKingMgr)->scanline1 = NULL;

	DeleteCriticalSection(&(*phMiniKingMgr)->scanlinesCS);
	DeleteCriticalSection(&(*phMiniKingMgr)->scanlineCS);
	DeleteCriticalSection(&(*phMiniKingMgr)->MiniKingCS);

	free(*phMiniKingMgr);

	*phMiniKingMgr = INVALID_HMINIKINGMGR_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // MINIKINGMGR_H
