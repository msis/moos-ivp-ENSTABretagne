/***************************************************************************************************************:')

MicronMgr.h

Micron sonar handling.

Fabrice Le Bars

Created : 2012-07-07

Version status: Not finished

***************************************************************************************************************:)*/

#ifndef MICRONMGR_H
#define MICRONMGR_H

#include "OSThread.h"
#include "OSCriticalSection.h"
#include "MicronCfg.h"

/*
Debug macros specific to MicronMgr.
*/
#ifdef _DEBUG_MESSAGES_MICRONUTILS
#	define _DEBUG_MESSAGES_MICRONMGR
#endif // _DEBUG_MESSAGES_MICRONUTILS

#ifdef _DEBUG_WARNINGS_MICRONUTILS
#	define _DEBUG_WARNINGS_MICRONMGR
#endif // _DEBUG_WARNINGS_MICRONUTILS

#ifdef _DEBUG_ERRORS_MICRONUTILS
#	define _DEBUG_ERRORS_MICRONMGR
#endif // _DEBUG_ERRORS_MICRONUTILS

#ifdef _DEBUG_MESSAGES_MICRONMGR
#	define PRINT_DEBUG_MESSAGE_MICRONMGR(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MICRONMGR(params)
#endif // _DEBUG_MESSAGES_MICRONMGR

#ifdef _DEBUG_WARNINGS_MICRONMGR
#	define PRINT_DEBUG_WARNING_MICRONMGR(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MICRONMGR(params)
#endif // _DEBUG_WARNINGS_MICRONMGR

#ifdef _DEBUG_ERRORS_MICRONMGR
#	define PRINT_DEBUG_ERROR_MICRONMGR(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MICRONMGR(params)
#endif // _DEBUG_ERRORS_MICRONMGR

#define SCANLINE_POLLING_MODE_MICRON 0x00000001
#define SCANLINES_POLLING_MODE_MICRON 0x00000002

struct MICRONMGR
{
	HMICRON hMicron;
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
	CRITICAL_SECTION MicronCS;
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
typedef struct MICRONMGR* HMICRONMGR;

#define INVALID_HMICRONMGR_VALUE NULL

/*
Private function.
Thread function that handles the polling and automatic reconnection of a Micron.

LPVOID lpParam : (INOUT) Parameter of the thread.

Return : 0.
*/
EXTERN_C THREAD_PROC_RETURN_VALUE MicronMgrThreadProc(void* lpParam);

/*
Private function.
Open a Micron and get data from it.

HMICRON hMicron : (IN) Identifier of the Micron.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ConnectToMicron(HMICRONMGR hMicronMgr)
{
	int i = 0;

	// No critical section is needed here because the other functions 
	// should not try to access the device or data while the device is
	// not connected (bConnected = FALSE).

	if (OpenMicron(&hMicronMgr->hMicron, hMicronMgr->szDevice, 
		hMicronMgr->NBins, hMicronMgr->NSteps, hMicronMgr->RangeScale,
		hMicronMgr->scanDirection, hMicronMgr->scanWidth, hMicronMgr->Gain) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_MICRONMGR(("Unable to connect to a Micron\n"));
		return EXIT_FAILURE;
	}

	// Get data before setting bConnected to TRUE to be sure that
	// the other functions will get valid data.
	if (hMicronMgr->bWaitForFirstData)
	{
		if (hMicronMgr->PollingMode & SCANLINE_POLLING_MODE_MICRON)
		{
			// Wait for 2 sonar scanlines.
			if (GetScanlineMicron(hMicronMgr->hMicron, 
				hMicronMgr->scanline1, &hMicronMgr->angle1,
				hMicronMgr->scanline2, &hMicronMgr->angle2) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_MICRONMGR(("Unable to connect to a Micron\n"));
				return EXIT_FAILURE;
			}
		}
		if (hMicronMgr->PollingMode & SCANLINES_POLLING_MODE_MICRON)
		{
			// Wait for a complete sonar loop.
			for (i = 0; i < hMicronMgr->NSteps; i++)
			{
				if (GetScanlineMicron(hMicronMgr->hMicron, 
					hMicronMgr->scanline1, &hMicronMgr->angle1,
					hMicronMgr->scanline2, &hMicronMgr->angle2) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_MICRONMGR(("Unable to connect to a Micron\n"));
					return EXIT_FAILURE;
				}

				// Make a shift in the table of pAngles.
				memmove(hMicronMgr->pAngles, hMicronMgr->pAngles+2, 
					(hMicronMgr->NSteps-2)*sizeof(double));

				// Put the angles of the 2 last scanlines at the end of the table of pAngles.
				memcpy((char*)&(hMicronMgr->pAngles[hMicronMgr->NSteps-2]), 
					(char*)&hMicronMgr->angle1, sizeof(double));
				memcpy((char*)&(hMicronMgr->pAngles[hMicronMgr->NSteps-1]), 
					(char*)&hMicronMgr->angle2, sizeof(double));

				// Make a shift in the table of scanlines.
				memmove(hMicronMgr->pScanlines, hMicronMgr->pScanlines+2*hMicronMgr->NBins, 
					(hMicronMgr->NSteps-2)*hMicronMgr->NBins*sizeof(uint8));

				// Put the 2 last scanlines at the end of the table of scanlines.
				memcpy((char*)(hMicronMgr->pScanlines+(hMicronMgr->NSteps-2)*hMicronMgr->NBins), 
					(char*)hMicronMgr->scanline1, hMicronMgr->NBins*sizeof(uint8));
				memcpy((char*)(hMicronMgr->pScanlines+(hMicronMgr->NSteps-1)*hMicronMgr->NBins), 
					(char*)hMicronMgr->scanline2, hMicronMgr->NBins*sizeof(uint8));
			}
		}
	}

	hMicronMgr->bConnected = TRUE;
	PRINT_DEBUG_MESSAGE_MICRONMGR(("Micron connected\n"));

	return EXIT_SUCCESS;
}

/*
Initialize a structure that manage the polling and the reconnection of a Micron.

HMICRONMGR* phMicronMgr : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* szDevice : (IN) Micron serial port to open.
int NBins : (IN) Number of bins per scanline.
int NSteps : (IN) Number of scanlines in a loop.
int RangeScale : (IN) Range scale (in m).
double scanDirection : (IN) Direction of the sector to scan (in degrees).
double scanWidth : (IN) Width of the sector to scan, 360 for a continuous scan (in degrees).
int Gain : (IN) Gain (in %).
int PollingMode : (IN) Either SCANLINE_POLLING_MODE_MICRON or 
SCANLINES_POLLING_MODE_MICRON or 0.
int Priority : (IN) Priority of the thread that manages the polling and the 
reconnection of the device. See SetThreadDefaultPriority().
BOOL bWaitForFirstData : (IN) TRUE to wait for the first entire data in 
InitMicronMgr(), FALSE if it does not matter if the first data after the 
connection to the device is invalid during some seconds.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitMicronMgr(HMICRONMGR* phMicronMgr, char* szDevice, 
							 int NBins, int NSteps, int RangeScale,
							 double scanDirection, double scanWidth, int Gain,
							 int PollingMode,
							 int Priority,
							 BOOL bWaitForFirstData)
{
	*phMicronMgr = (HMICRONMGR)calloc(1, sizeof(struct MICRONMGR));

	if (*phMicronMgr == NULL)
	{
		PRINT_DEBUG_ERROR_MICRONMGR(("InitMicronMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	sprintf((*phMicronMgr)->szDevice, "%s", szDevice);

	(*phMicronMgr)->NBins = NBins;
	(*phMicronMgr)->NSteps = NSteps;
	(*phMicronMgr)->RangeScale = RangeScale;
	(*phMicronMgr)->scanDirection = scanDirection;
	(*phMicronMgr)->scanWidth = scanWidth;
	(*phMicronMgr)->Gain = Gain;
	(*phMicronMgr)->PollingMode = PollingMode;
	(*phMicronMgr)->bConnected = FALSE;
	(*phMicronMgr)->bRunThread = TRUE;

	(*phMicronMgr)->scanlinecounter = 0;
	(*phMicronMgr)->scanlinescounter = 0;

	(*phMicronMgr)->scanline1 = (uint8*)calloc(NBins, sizeof(uint8));
	(*phMicronMgr)->scanline2 = (uint8*)calloc(NBins, sizeof(uint8));
	(*phMicronMgr)->pAngles = (double*)calloc(NSteps, sizeof(double));
	(*phMicronMgr)->pScanlines = (uint8*)calloc(NSteps*NBins, sizeof(uint8));

	if (
		((*phMicronMgr)->scanline1 == NULL)||
		((*phMicronMgr)->scanline2 == NULL)||
		((*phMicronMgr)->pAngles == NULL)||
		((*phMicronMgr)->pScanlines == NULL)
		)
	{
		PRINT_DEBUG_ERROR_MICRONMGR(("InitMicronMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	(*phMicronMgr)->angle1 = 0;
	(*phMicronMgr)->angle2 = 0;

	(*phMicronMgr)->bWaitForFirstData = bWaitForFirstData;

	if (
		(InitCriticalSection(&(*phMicronMgr)->MicronCS) != EXIT_SUCCESS)||
		(InitCriticalSection(&(*phMicronMgr)->scanlineCS) != EXIT_SUCCESS)||		
		(InitCriticalSection(&(*phMicronMgr)->scanlinesCS) != EXIT_SUCCESS)
		)
	{
		PRINT_DEBUG_ERROR_MICRONMGR(("InitMicronMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a critical section. ", 
			szDevice));
		DeleteCriticalSection(&(*phMicronMgr)->scanlinesCS);
		DeleteCriticalSection(&(*phMicronMgr)->scanlineCS);
		DeleteCriticalSection(&(*phMicronMgr)->MicronCS);
		free(*phMicronMgr);
		return EXIT_FAILURE;
	}

	// First attempt to connect to the device.
	ConnectToMicron(*phMicronMgr);

	if (CreateDefaultThread(MicronMgrThreadProc, (void*)*phMicronMgr, &(*phMicronMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MICRONMGR(("InitMicronMgr error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error creating a thread. ", 
			szDevice));
		if ((*phMicronMgr)->bConnected)
		{
			CloseMicron(&(*phMicronMgr)->hMicron);
		}
		DeleteCriticalSection(&(*phMicronMgr)->scanlinesCS);
		DeleteCriticalSection(&(*phMicronMgr)->scanlineCS);
		DeleteCriticalSection(&(*phMicronMgr)->MicronCS);
		free(*phMicronMgr);
		return EXIT_FAILURE;
	}

#ifdef ENABLE_PRIORITY_HANDLING
	if (SetThreadDefaultPriority((*phMicronMgr)->ThreadId, Priority) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_MICRONMGR(("InitMicronMgr error (%s) : %s"
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
Get 2 scanlines and the corresponding angles from a Micron (thread safe).
If PollingMode have the SCANLINE_POLLING_MODE_MICRON or 
SCANLINES_POLLING_MODE_MICRON flags set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HMICRONMGR hMicronMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a Micron.
uint8* scanline1 : (INOUT) Valid pointer that will receive the first scanline.
double* pAngle1 : (INOUT) Valid pointer that will receive the angle of the 
first scanline (in degrees).
uint8* scanline2 : (INOUT) Valid pointer that will receive the second scanline.
double* pAngle2 : (INOUT) Valid pointer that will receive the angle of the 
second scanline (in degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetScanlineMicronMgr(HMICRONMGR hMicronMgr, 
									uint8* scanline1, double* pAngle1, 
									uint8* scanline2, double* pAngle2)
{
	if ((hMicronMgr->PollingMode & SCANLINE_POLLING_MODE_MICRON)||
		(hMicronMgr->PollingMode & SCANLINES_POLLING_MODE_MICRON))
	{
		EnterCriticalSection(&hMicronMgr->scanlineCS);

		if (!hMicronMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MICRONMGR(("GetScanlineMicronMgr error (%s) : %s"
				"(hMicronMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMicronMgr));

			LeaveCriticalSection(&hMicronMgr->scanlineCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		memcpy((char*)scanline1, (char*)hMicronMgr->scanline1, hMicronMgr->NBins*sizeof(uint8));
		memcpy((char*)scanline2, (char*)hMicronMgr->scanline2, hMicronMgr->NBins*sizeof(uint8));
		*pAngle1 = hMicronMgr->angle1;
		*pAngle2 = hMicronMgr->angle2;

		LeaveCriticalSection(&hMicronMgr->scanlineCS);
	}
	else
	{
		EnterCriticalSection(&hMicronMgr->MicronCS);

		if (!hMicronMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MICRONMGR(("GetScanlineMicronMgr error (%s) : %s"
				"(hMicronMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMicronMgr));

			LeaveCriticalSection(&hMicronMgr->MicronCS);
			return EXIT_FAILURE;
		}

		// Flush the serial port of the device because the hardware is 
		// always sending data every 1 s.
		if (DiscardDataMicron(hMicronMgr->hMicron) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_MICRONMGR(("Connection to a Micron lost\n"));
			PRINT_DEBUG_ERROR_MICRONMGR(("GetScanlineMicronMgr error (%s) : %s"
				"(hMicronMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hMicronMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hMicronMgr->bConnected = FALSE; 
			CloseMicron(&hMicronMgr->hMicron);

			LeaveCriticalSection(&hMicronMgr->MicronCS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetScanlineMicron(hMicronMgr->hMicron, 
			scanline1, pAngle1,
			scanline2, pAngle2) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_MICRONMGR(("Connection to a Micron lost\n"));
			PRINT_DEBUG_ERROR_MICRONMGR(("GetScanlineMicronMgr error (%s) : %s"
				"(hMicronMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hMicronMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hMicronMgr->bConnected = FALSE; 
			CloseMicron(&hMicronMgr->hMicron);

			LeaveCriticalSection(&hMicronMgr->MicronCS);
			return EXIT_FAILURE;
		}

		LeaveCriticalSection(&hMicronMgr->MicronCS);
	}

	return EXIT_SUCCESS;
}

/*
Get the scanlines of the last loop and the corresponding angles from a Micron 
(thread safe).
If PollingMode have the SCANLINES_POLLING_MODE_MICRON flag set, get the last 
data polled in the last PollingTimeInterval, otherwise a direct thread safe 
request is made to the device.

HMICRONMGR hMicronMgr : (IN) Identifier of the structure that manages the 
polling and the reconnection of a Micron.
uint8* pScanlines : (INOUT) Valid pointer that will receive the scanlines of 
the last loop.
double* pAngles : (INOUT) Valid pointer that will receive the angles of the 
scanlines of the last loop (in degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetScanlinesMicronMgr(HMICRONMGR hMicronMgr, 
									uint8* pScanlines, double* pAngles)
{
	if (hMicronMgr->PollingMode & SCANLINES_POLLING_MODE_MICRON)
	{
		EnterCriticalSection(&hMicronMgr->scanlinesCS);

		if (!hMicronMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MICRONMGR(("GetScanlinesMicronMgr error (%s) : %s"
				"(hMicronMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMicronMgr));

			LeaveCriticalSection(&hMicronMgr->scanlinesCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		memcpy((char*)pAngles, (char*)hMicronMgr->pAngles, 
			hMicronMgr->NSteps*sizeof(double));
		memcpy((char*)pScanlines, (char*)hMicronMgr->pScanlines, 
			hMicronMgr->NSteps*hMicronMgr->NBins*sizeof(uint8));

		LeaveCriticalSection(&hMicronMgr->scanlinesCS);
	}
	else
	{
		PRINT_DEBUG_ERROR_MICRONMGR(("GetScanlinesMicronMgr error (%s) : %s"
			"(hMicronMgr=%#x)\n", 
			strtime_m(), 
			"Data not available in non polling mode. ", 
			hMicronMgr));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Return the distance to the first obstacle from a Micron scanline.

HMICRONMGR hMicronMgr : (IN) Identifier of the structure that manages the polling 
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
inline int ComputeFirstObstacleDistFromScanlineMicronMgr(HMICRONMGR hMicronMgr,
								 uint8* scanline, uint8 threshold, 
								 double minDist, double maxDist, double* pDist)
{
	return ComputeFirstObstacleDistFromScanlineMicron(hMicronMgr->hMicron,
								 scanline, threshold, 
								 minDist, maxDist, pDist);
}

/*
Get the distance to the first obstacle from 2 scanlines and the 
corresponding angles from a Micron (thread safe).
If PollingMode have the SCANLINE_POLLING_MODE_MICRON or 
SCANLINES_POLLING_MODE_MICRON flags set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HMICRONMGR hMicronMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a Micron.
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
inline int GetFirstObstacleDistMicronMgr(HMICRONMGR hMicronMgr,
								 uint8 threshold, double minDist, double maxDist, 
								 double* pAngle1, double* pDist1, double* pAngle2, double* pDist2)
{
	if ((hMicronMgr->PollingMode & SCANLINE_POLLING_MODE_MICRON)||
		(hMicronMgr->PollingMode & SCANLINES_POLLING_MODE_MICRON))
	{
		EnterCriticalSection(&hMicronMgr->scanlineCS);

		if (!hMicronMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MICRONMGR(("GetFirstObstacleDistMicronMgr error (%s) : %s"
				"(hMicronMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMicronMgr));

			LeaveCriticalSection(&hMicronMgr->scanlineCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		*pAngle1 = hMicronMgr->angle1;
		ComputeFirstObstacleDistFromScanlineMicron(hMicronMgr->hMicron,
								 hMicronMgr->scanline1, threshold, 
								 minDist, maxDist, pDist1);
		*pAngle2 = hMicronMgr->angle2;
		ComputeFirstObstacleDistFromScanlineMicron(hMicronMgr->hMicron,
								 hMicronMgr->scanline2, threshold, 
								 minDist, maxDist, pDist2);

		LeaveCriticalSection(&hMicronMgr->scanlineCS);
	}
	else
	{
		EnterCriticalSection(&hMicronMgr->MicronCS);

		if (!hMicronMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_MICRONMGR(("GetFirstObstacleDistMicronMgr error (%s) : %s"
				"(hMicronMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hMicronMgr));

			LeaveCriticalSection(&hMicronMgr->MicronCS);
			return EXIT_FAILURE;
		}

		// Flush the serial port of the device because the hardware is 
		// always sending data every 1 s.
		if (DiscardDataMicron(hMicronMgr->hMicron) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_MICRONMGR(("Connection to a Micron lost\n"));
			PRINT_DEBUG_ERROR_MICRONMGR(("GetFirstObstacleDistMicronMgr error (%s) : %s"
				"(hMicronMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hMicronMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hMicronMgr->bConnected = FALSE; 
			CloseMicron(&hMicronMgr->hMicron);

			LeaveCriticalSection(&hMicronMgr->MicronCS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetScanlineMicron(hMicronMgr->hMicron, 
			hMicronMgr->scanline1, &hMicronMgr->angle1,
			hMicronMgr->scanline2, &hMicronMgr->angle2) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_MICRONMGR(("Connection to a Micron lost\n"));
			PRINT_DEBUG_ERROR_MICRONMGR(("GetFirstObstacleDistMicronMgr error (%s) : %s"
				"(hMicronMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hMicronMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hMicronMgr->bConnected = FALSE; 
			CloseMicron(&hMicronMgr->hMicron);

			LeaveCriticalSection(&hMicronMgr->MicronCS);
			return EXIT_FAILURE;
		}

		*pAngle1 = hMicronMgr->angle1;
		ComputeFirstObstacleDistFromScanlineMicron(hMicronMgr->hMicron,
								 hMicronMgr->scanline1, threshold, 
								 minDist, maxDist, pDist1);
		*pAngle2 = hMicronMgr->angle2;
		ComputeFirstObstacleDistFromScanlineMicron(hMicronMgr->hMicron,
								 hMicronMgr->scanline2, threshold, 
								 minDist, maxDist, pDist2);

		LeaveCriticalSection(&hMicronMgr->MicronCS);
	}

	return EXIT_SUCCESS;
}

/*
Release a structure that manage the polling and the reconnection of a Micron.

HMICRONMGR* phMicronMgr : (INOUT) Valid pointer to the identifier of the structure  
that manages the polling and the reconnection of a Micron.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReleaseMicronMgr(HMICRONMGR* phMicronMgr)
{
	BOOL bError = FALSE;

	// Indicate to the thread that it should terminate its loop.
	(*phMicronMgr)->bRunThread = FALSE;

	if (WaitForThread((*phMicronMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MICRONMGR(("ReleaseMicronMgr error (%s) : %s"
			"(*phMicronMgr=%#x)\n", 
			strtime_m(), 
			"Error waiting for the thread. ", 
			*phMicronMgr));
		bError = TRUE;
	}

	free((*phMicronMgr)->pScanlines);(*phMicronMgr)->pScanlines = NULL;
	free((*phMicronMgr)->pAngles);(*phMicronMgr)->pAngles = NULL;
	free((*phMicronMgr)->scanline2);(*phMicronMgr)->scanline2 = NULL;
	free((*phMicronMgr)->scanline1);(*phMicronMgr)->scanline1 = NULL;

	DeleteCriticalSection(&(*phMicronMgr)->scanlinesCS);
	DeleteCriticalSection(&(*phMicronMgr)->scanlineCS);
	DeleteCriticalSection(&(*phMicronMgr)->MicronCS);

	free(*phMicronMgr);

	*phMicronMgr = INVALID_HMICRONMGR_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // MICRONMGR_H
