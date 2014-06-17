/***************************************************************************************************************:')

SeanetMgr.h

Tritech sonar (compatible with Seanet Pro software) handling.

Fabrice Le Bars

Created : 2012-05-08

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef SEANETMGR_H
#define SEANETMGR_H

#include "OSThread.h"
#include "OSCriticalSection.h"
#include "SeanetCfg.h"

/*
Debug macros specific to SeanetMgr.
*/
#ifdef _DEBUG_MESSAGES_SEANETUTILS
#	define _DEBUG_MESSAGES_SEANETMGR
#endif // _DEBUG_MESSAGES_SEANETUTILS

#ifdef _DEBUG_WARNINGS_SEANETUTILS
#	define _DEBUG_WARNINGS_SEANETMGR
#endif // _DEBUG_WARNINGS_SEANETUTILS

#ifdef _DEBUG_ERRORS_SEANETUTILS
#	define _DEBUG_ERRORS_SEANETMGR
#endif // _DEBUG_ERRORS_SEANETUTILS

#ifdef _DEBUG_MESSAGES_SEANETMGR
#	define PRINT_DEBUG_MESSAGE_SEANETMGR(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_SEANETMGR(params)
#endif // _DEBUG_MESSAGES_SEANETMGR

#ifdef _DEBUG_WARNINGS_SEANETMGR
#	define PRINT_DEBUG_WARNING_SEANETMGR(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_SEANETMGR(params)
#endif // _DEBUG_WARNINGS_SEANETMGR

#ifdef _DEBUG_ERRORS_SEANETMGR
#	define PRINT_DEBUG_ERROR_SEANETMGR(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_SEANETMGR(params)
#endif // _DEBUG_ERRORS_SEANETMGR

#define SCANLINE_POLLING_MODE_SEANET 0x00000001
#define SCANLINES_POLLING_MODE_SEANET 0x00000002

struct SEANETMGR
{
	HSEANET hSeanet;
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
	CRITICAL_SECTION SeanetCS;
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
typedef struct SEANETMGR* HSEANETMGR;

#define INVALID_HSEANETMGR_VALUE NULL

/*
Private function.
Thread function that handles the polling and automatic reconnection of a Seanet.

LPVOID lpParam : (INOUT) Parameter of the thread.

Return : 0.
*/
EXTERN_C THREAD_PROC_RETURN_VALUE SeanetMgrThreadProc(void* lpParam);

/*
Private function.
Open a Seanet and get data from it.

HSEANET hSeanet : (IN) Identifier of the Seanet.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ConnectToSeanet(HSEANETMGR hSeanetMgr)
{
	int i = 0;

	// No critical section is needed here because the other functions 
	// should not try to access the device or data while the device is
	// not connected (bConnected = FALSE).

	if (OpenSeanet(&hSeanetMgr->hSeanet, hSeanetMgr->szDevice, 
		hSeanetMgr->NBins, hSeanetMgr->NSteps, hSeanetMgr->RangeScale,
		hSeanetMgr->scanDirection, hSeanetMgr->scanWidth, hSeanetMgr->Gain) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_SEANETMGR(("Unable to connect to a Seanet\n"));
		return EXIT_FAILURE;
	}

	// Get data before setting bConnected to TRUE to be sure that
	// the other functions will get valid data.
	if (hSeanetMgr->bWaitForFirstData)
	{
		if (hSeanetMgr->PollingMode & SCANLINE_POLLING_MODE_SEANET)
		{
			// Wait for 2 sonar scanlines.
			if (GetScanlineSeanet(hSeanetMgr->hSeanet, 
				hSeanetMgr->scanline1, &hSeanetMgr->angle1,
				hSeanetMgr->scanline2, &hSeanetMgr->angle2) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_SEANETMGR(("Unable to connect to a Seanet\n"));
				return EXIT_FAILURE;
			}
		}
		if (hSeanetMgr->PollingMode & SCANLINES_POLLING_MODE_SEANET)
		{
			// Wait for a complete sonar loop.
			for (i = 0; i < hSeanetMgr->NSteps; i++)
			{
				if (GetScanlineSeanet(hSeanetMgr->hSeanet, 
					hSeanetMgr->scanline1, &hSeanetMgr->angle1,
					hSeanetMgr->scanline2, &hSeanetMgr->angle2) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_SEANETMGR(("Unable to connect to a Seanet\n"));
					return EXIT_FAILURE;
				}

				// Make a shift in the table of pAngles.
				memmove(hSeanetMgr->pAngles, hSeanetMgr->pAngles+2, 
					(hSeanetMgr->NSteps-2)*sizeof(double));

				// Put the angles of the 2 last scanlines at the end of the table of pAngles.
				memcpy((char*)&(hSeanetMgr->pAngles[hSeanetMgr->NSteps-2]), 
					(char*)&hSeanetMgr->angle1, sizeof(double));
				memcpy((char*)&(hSeanetMgr->pAngles[hSeanetMgr->NSteps-1]), 
					(char*)&hSeanetMgr->angle2, sizeof(double));

				// Make a shift in the table of scanlines.
				memmove(hSeanetMgr->pScanlines, hSeanetMgr->pScanlines+2*hSeanetMgr->NBins, 
					(hSeanetMgr->NSteps-2)*hSeanetMgr->NBins*sizeof(uint8));

				// Put the 2 last scanlines at the end of the table of scanlines.
				memcpy((char*)(hSeanetMgr->pScanlines+(hSeanetMgr->NSteps-2)*hSeanetMgr->NBins), 
					(char*)hSeanetMgr->scanline1, hSeanetMgr->NBins*sizeof(uint8));
				memcpy((char*)(hSeanetMgr->pScanlines+(hSeanetMgr->NSteps-1)*hSeanetMgr->NBins), 
					(char*)hSeanetMgr->scanline2, hSeanetMgr->NBins*sizeof(uint8));
			}
		}
	}

	hSeanetMgr->bConnected = TRUE;
	PRINT_DEBUG_MESSAGE_SEANETMGR(("Seanet connected\n"));

	return EXIT_SUCCESS;
}

/*
Initialize a structure that manage the polling and the reconnection of a Seanet.

HSEANETMGR* phSeanetMgr : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* szDevice : (IN) Seanet serial port to open.
int NBins : (IN) Number of bins per scanline.
int NSteps : (IN) Number of scanlines in a loop.
int RangeScale : (IN) Range scale (in m).
double scanDirection : (IN) Direction of the sector to scan (in degrees).
double scanWidth : (IN) Width of the sector to scan, 360 for a continuous scan (in degrees).
int Gain : (IN) Gain (in %).
int PollingMode : (IN) Either SCANLINE_POLLING_MODE_SEANET or 
SCANLINES_POLLING_MODE_SEANET or 0.
int Priority : (IN) Priority of the thread that manages the polling and the 
reconnection of the device. See SetThreadDefaultPriority().
BOOL bWaitForFirstData : (IN) TRUE to wait for the first entire data in 
InitSeanetMgr(), FALSE if it does not matter if the first data after the 
connection to the device is invalid during some seconds.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitSeanetMgr(HSEANETMGR* phSeanetMgr, char* szDevice, 
							 int NBins, int NSteps, int RangeScale,
							 double scanDirection, double scanWidth, int Gain,
							 int PollingMode,
							 int Priority,
							 BOOL bWaitForFirstData)
{
	*phSeanetMgr = (HSEANETMGR)calloc(1, sizeof(struct SEANETMGR));

	if (*phSeanetMgr == NULL)
	{
		PRINT_DEBUG_ERROR_SEANETMGR(("InitSeanetMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	sprintf((*phSeanetMgr)->szDevice, "%s", szDevice);

	(*phSeanetMgr)->NBins = NBins;
	(*phSeanetMgr)->NSteps = NSteps;
	(*phSeanetMgr)->RangeScale = RangeScale;
	(*phSeanetMgr)->scanDirection = scanDirection;
	(*phSeanetMgr)->scanWidth = scanWidth;
	(*phSeanetMgr)->Gain = Gain;
	(*phSeanetMgr)->PollingMode = PollingMode;
	(*phSeanetMgr)->bConnected = FALSE;
	(*phSeanetMgr)->bRunThread = TRUE;

	(*phSeanetMgr)->scanlinecounter = 0;
	(*phSeanetMgr)->scanlinescounter = 0;

	(*phSeanetMgr)->scanline1 = (uint8*)calloc(NBins, sizeof(uint8));
	(*phSeanetMgr)->scanline2 = (uint8*)calloc(NBins, sizeof(uint8));
	(*phSeanetMgr)->pAngles = (double*)calloc(NSteps, sizeof(double));
	(*phSeanetMgr)->pScanlines = (uint8*)calloc(NSteps*NBins, sizeof(uint8));

	if (
		((*phSeanetMgr)->scanline1 == NULL)||
		((*phSeanetMgr)->scanline2 == NULL)||
		((*phSeanetMgr)->pAngles == NULL)||
		((*phSeanetMgr)->pScanlines == NULL)
		)
	{
		PRINT_DEBUG_ERROR_SEANETMGR(("InitSeanetMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	(*phSeanetMgr)->angle1 = 0;
	(*phSeanetMgr)->angle2 = 0;

	(*phSeanetMgr)->bWaitForFirstData = bWaitForFirstData;

	if (
		(InitCriticalSection(&(*phSeanetMgr)->SeanetCS) != EXIT_SUCCESS)||
		(InitCriticalSection(&(*phSeanetMgr)->scanlineCS) != EXIT_SUCCESS)||		
		(InitCriticalSection(&(*phSeanetMgr)->scanlinesCS) != EXIT_SUCCESS)
		)
	{
		PRINT_DEBUG_ERROR_SEANETMGR(("InitSeanetMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a critical section. ", 
			szDevice));
		DeleteCriticalSection(&(*phSeanetMgr)->scanlinesCS);
		DeleteCriticalSection(&(*phSeanetMgr)->scanlineCS);
		DeleteCriticalSection(&(*phSeanetMgr)->SeanetCS);
		free(*phSeanetMgr);
		return EXIT_FAILURE;
	}

	// First attempt to connect to the device.
	ConnectToSeanet(*phSeanetMgr);

	if (CreateDefaultThread(SeanetMgrThreadProc, (void*)*phSeanetMgr, &(*phSeanetMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_SEANETMGR(("InitSeanetMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a thread. ", 
			szDevice));
		if ((*phSeanetMgr)->bConnected)
		{
			CloseSeanet(&(*phSeanetMgr)->hSeanet);
		}
		DeleteCriticalSection(&(*phSeanetMgr)->scanlinesCS);
		DeleteCriticalSection(&(*phSeanetMgr)->scanlineCS);
		DeleteCriticalSection(&(*phSeanetMgr)->SeanetCS);
		free(*phSeanetMgr);
		return EXIT_FAILURE;
	}

#ifdef ENABLE_PRIORITY_HANDLING
	if (SetThreadDefaultPriority((*phSeanetMgr)->ThreadId, Priority) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_SEANETMGR(("InitSeanetMgr error (%s) : %s"
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
Get 2 scanlines and the corresponding angles from a Seanet (thread safe).
If PollingMode have the SCANLINE_POLLING_MODE_SEANET or 
SCANLINES_POLLING_MODE_SEANET flags set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HSEANETMGR hSeanetMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a Seanet.
uint8* scanline1 : (INOUT) Valid pointer that will receive the first scanline.
double* pAngle1 : (INOUT) Valid pointer that will receive the angle of the 
first scanline (in degrees).
uint8* scanline2 : (INOUT) Valid pointer that will receive the second scanline.
double* pAngle2 : (INOUT) Valid pointer that will receive the angle of the 
second scanline (in degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetScanlineSeanetMgr(HSEANETMGR hSeanetMgr, 
									uint8* scanline1, double* pAngle1, 
									uint8* scanline2, double* pAngle2)
{
	if ((hSeanetMgr->PollingMode & SCANLINE_POLLING_MODE_SEANET)||
		(hSeanetMgr->PollingMode & SCANLINES_POLLING_MODE_SEANET))
	{
		EnterCriticalSection(&hSeanetMgr->scanlineCS);

		if (!hSeanetMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_SEANETMGR(("GetScanlineSeanetMgr error (%s) : %s"
				"(hSeanetMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hSeanetMgr));

			LeaveCriticalSection(&hSeanetMgr->scanlineCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		memcpy((char*)scanline1, (char*)hSeanetMgr->scanline1, hSeanetMgr->NBins*sizeof(uint8));
		memcpy((char*)scanline2, (char*)hSeanetMgr->scanline2, hSeanetMgr->NBins*sizeof(uint8));
		*pAngle1 = hSeanetMgr->angle1;
		*pAngle2 = hSeanetMgr->angle2;

		LeaveCriticalSection(&hSeanetMgr->scanlineCS);
	}
	else
	{
		EnterCriticalSection(&hSeanetMgr->SeanetCS);

		if (!hSeanetMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_SEANETMGR(("GetScanlineSeanetMgr error (%s) : %s"
				"(hSeanetMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hSeanetMgr));

			LeaveCriticalSection(&hSeanetMgr->SeanetCS);
			return EXIT_FAILURE;
		}

		// Flush the serial port of the device because the hardware is 
		// always sending data every 1 s.
		if (DiscardDataSeanet(hSeanetMgr->hSeanet) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_SEANETMGR(("Connection to a Seanet lost\n"));
			PRINT_DEBUG_ERROR_SEANETMGR(("GetScanlineSeanetMgr error (%s) : %s"
				"(hSeanetMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hSeanetMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hSeanetMgr->bConnected = FALSE; 
			CloseSeanet(&hSeanetMgr->hSeanet);

			LeaveCriticalSection(&hSeanetMgr->SeanetCS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetScanlineSeanet(hSeanetMgr->hSeanet, 
			scanline1, pAngle1,
			scanline2, pAngle2) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_SEANETMGR(("Connection to a Seanet lost\n"));
			PRINT_DEBUG_ERROR_SEANETMGR(("GetScanlineSeanetMgr error (%s) : %s"
				"(hSeanetMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hSeanetMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hSeanetMgr->bConnected = FALSE; 
			CloseSeanet(&hSeanetMgr->hSeanet);

			LeaveCriticalSection(&hSeanetMgr->SeanetCS);
			return EXIT_FAILURE;
		}

		LeaveCriticalSection(&hSeanetMgr->SeanetCS);
	}

	return EXIT_SUCCESS;
}

/*
Get the scanlines of the last loop and the corresponding angles from a Seanet 
(thread safe).
If PollingMode have the SCANLINES_POLLING_MODE_SEANET flag set, get the last 
data polled in the last PollingTimeInterval, otherwise a direct thread safe 
request is made to the device.

HSEANETMGR hSeanetMgr : (IN) Identifier of the structure that manages the 
polling and the reconnection of a Seanet.
uint8* pScanlines : (INOUT) Valid pointer that will receive the scanlines of 
the last loop.
double* pAngles : (INOUT) Valid pointer that will receive the angles of the 
scanlines of the last loop (in degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetScanlinesSeanetMgr(HSEANETMGR hSeanetMgr, 
									uint8* pScanlines, double* pAngles)
{
	if (hSeanetMgr->PollingMode & SCANLINES_POLLING_MODE_SEANET)
	{
		EnterCriticalSection(&hSeanetMgr->scanlinesCS);

		if (!hSeanetMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_SEANETMGR(("GetScanlinesSeanetMgr error (%s) : %s"
				"(hSeanetMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hSeanetMgr));

			LeaveCriticalSection(&hSeanetMgr->scanlinesCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		memcpy((char*)pAngles, (char*)hSeanetMgr->pAngles, 
			hSeanetMgr->NSteps*sizeof(double));
		memcpy((char*)pScanlines, (char*)hSeanetMgr->pScanlines, 
			hSeanetMgr->NSteps*hSeanetMgr->NBins*sizeof(uint8));

		LeaveCriticalSection(&hSeanetMgr->scanlinesCS);
	}
	else
	{
		PRINT_DEBUG_ERROR_SEANETMGR(("GetScanlinesSeanetMgr error (%s) : %s"
			"(hSeanetMgr=%#x)\n", 
			strtime_m(), 
			"Data not available in non polling mode. ", 
			hSeanetMgr));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Return the distance to the first obstacle from a Seanet scanline.

HSEANETMGR hSeanetMgr : (IN) Identifier of the structure that manages the polling 
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
inline int ComputeFirstObstacleDistFromScanlineSeanetMgr(HSEANETMGR hSeanetMgr,
								 uint8* scanline, uint8 threshold, 
								 double minDist, double maxDist, double* pDist)
{
	return ComputeFirstObstacleDistFromScanlineSeanet(hSeanetMgr->hSeanet,
								 scanline, threshold, 
								 minDist, maxDist, pDist);
}

/*
Get the distance to the first obstacle from 2 scanlines and the 
corresponding angles from a Seanet (thread safe).
If PollingMode have the SCANLINE_POLLING_MODE_SEANET or 
SCANLINES_POLLING_MODE_SEANET flags set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HSEANETMGR hSeanetMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a Seanet.
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
inline int GetFirstObstacleDistSeanetMgr(HSEANETMGR hSeanetMgr,
								 uint8 threshold, double minDist, double maxDist, 
								 double* pAngle1, double* pDist1, double* pAngle2, double* pDist2)
{
	if ((hSeanetMgr->PollingMode & SCANLINE_POLLING_MODE_SEANET)||
		(hSeanetMgr->PollingMode & SCANLINES_POLLING_MODE_SEANET))
	{
		EnterCriticalSection(&hSeanetMgr->scanlineCS);

		if (!hSeanetMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_SEANETMGR(("GetFirstObstacleDistSeanetMgr error (%s) : %s"
				"(hSeanetMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hSeanetMgr));

			LeaveCriticalSection(&hSeanetMgr->scanlineCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		*pAngle1 = hSeanetMgr->angle1;
		ComputeFirstObstacleDistFromScanlineSeanet(hSeanetMgr->hSeanet,
								 hSeanetMgr->scanline1, threshold, 
								 minDist, maxDist, pDist1);
		*pAngle2 = hSeanetMgr->angle2;
		ComputeFirstObstacleDistFromScanlineSeanet(hSeanetMgr->hSeanet,
								 hSeanetMgr->scanline2, threshold, 
								 minDist, maxDist, pDist2);

		LeaveCriticalSection(&hSeanetMgr->scanlineCS);
	}
	else
	{
		EnterCriticalSection(&hSeanetMgr->SeanetCS);

		if (!hSeanetMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_SEANETMGR(("GetFirstObstacleDistSeanetMgr error (%s) : %s"
				"(hSeanetMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hSeanetMgr));

			LeaveCriticalSection(&hSeanetMgr->SeanetCS);
			return EXIT_FAILURE;
		}

		// Flush the serial port of the device because the hardware is 
		// always sending data every 1 s.
		if (DiscardDataSeanet(hSeanetMgr->hSeanet) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_SEANETMGR(("Connection to a Seanet lost\n"));
			PRINT_DEBUG_ERROR_SEANETMGR(("GetFirstObstacleDistSeanetMgr error (%s) : %s"
				"(hSeanetMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hSeanetMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hSeanetMgr->bConnected = FALSE; 
			CloseSeanet(&hSeanetMgr->hSeanet);

			LeaveCriticalSection(&hSeanetMgr->SeanetCS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetScanlineSeanet(hSeanetMgr->hSeanet, 
			hSeanetMgr->scanline1, &hSeanetMgr->angle1,
			hSeanetMgr->scanline2, &hSeanetMgr->angle2) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_SEANETMGR(("Connection to a Seanet lost\n"));
			PRINT_DEBUG_ERROR_SEANETMGR(("GetFirstObstacleDistSeanetMgr error (%s) : %s"
				"(hSeanetMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hSeanetMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hSeanetMgr->bConnected = FALSE; 
			CloseSeanet(&hSeanetMgr->hSeanet);

			LeaveCriticalSection(&hSeanetMgr->SeanetCS);
			return EXIT_FAILURE;
		}

		*pAngle1 = hSeanetMgr->angle1;
		ComputeFirstObstacleDistFromScanlineSeanet(hSeanetMgr->hSeanet,
								 hSeanetMgr->scanline1, threshold, 
								 minDist, maxDist, pDist1);
		*pAngle2 = hSeanetMgr->angle2;
		ComputeFirstObstacleDistFromScanlineSeanet(hSeanetMgr->hSeanet,
								 hSeanetMgr->scanline2, threshold, 
								 minDist, maxDist, pDist2);

		LeaveCriticalSection(&hSeanetMgr->SeanetCS);
	}

	return EXIT_SUCCESS;
}

/*
Release a structure that manage the polling and the reconnection of a Seanet.

HSEANETMGR* phSeanetMgr : (INOUT) Valid pointer to the identifier of the structure  
that manages the polling and the reconnection of a Seanet.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReleaseSeanetMgr(HSEANETMGR* phSeanetMgr)
{
	BOOL bError = FALSE;

	// Indicate to the thread that it should terminate its loop.
	(*phSeanetMgr)->bRunThread = FALSE;

	if (WaitForThread((*phSeanetMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_SEANETMGR(("ReleaseSeanetMgr error (%s) : %s"
			"(*phSeanetMgr=%#x)\n", 
			strtime_m(), 
			"Error waiting for the thread. ", 
			*phSeanetMgr));
		bError = TRUE;
	}

	free((*phSeanetMgr)->pScanlines);(*phSeanetMgr)->pScanlines = NULL;
	free((*phSeanetMgr)->pAngles);(*phSeanetMgr)->pAngles = NULL;
	free((*phSeanetMgr)->scanline2);(*phSeanetMgr)->scanline2 = NULL;
	free((*phSeanetMgr)->scanline1);(*phSeanetMgr)->scanline1 = NULL;

	DeleteCriticalSection(&(*phSeanetMgr)->scanlinesCS);
	DeleteCriticalSection(&(*phSeanetMgr)->scanlineCS);
	DeleteCriticalSection(&(*phSeanetMgr)->SeanetCS);

	free(*phSeanetMgr);

	*phSeanetMgr = INVALID_HSEANETMGR_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // SEANETMGR_H
