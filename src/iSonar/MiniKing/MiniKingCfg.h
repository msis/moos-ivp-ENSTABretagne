/***************************************************************************************************************:')

MiniKingCfg.h

MiniKing sonar handling.

Fabrice Le Bars

Created: 2009-06-06

Version status: Not finished

***************************************************************************************************************:)*/

#ifndef MINIKINGCFG_H
#define MINIKINGCFG_H

#include "MiniKingCore.h"

/*
Debug macros specific to MiniKingCfg.
*/
#ifdef _DEBUG_MESSAGES_MINIKINGUTILS
#	define _DEBUG_MESSAGES_MINIKINGCFG
#endif // _DEBUG_MESSAGES_MINIKINGUTILS

#ifdef _DEBUG_WARNINGS_MINIKINGUTILS
#	define _DEBUG_WARNINGS_MINIKINGCFG
#endif // _DEBUG_WARNINGS_MINIKINGUTILS

#ifdef _DEBUG_ERRORS_MINIKINGUTILS
#	define _DEBUG_ERRORS_MINIKINGCFG
#endif // _DEBUG_ERRORS_MINIKINGUTILS

#ifdef _DEBUG_MESSAGES_MINIKINGCFG
#	define PRINT_DEBUG_MESSAGE_MINIKINGCFG(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MINIKINGCFG(params)
#endif // _DEBUG_MESSAGES_MINIKINGCFG

#ifdef _DEBUG_WARNINGS_MINIKINGCFG
#	define PRINT_DEBUG_WARNING_MINIKINGCFG(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MINIKINGCFG(params)
#endif // _DEBUG_WARNINGS_MINIKINGCFG

#ifdef _DEBUG_ERRORS_MINIKINGCFG
#	define PRINT_DEBUG_ERROR_MINIKINGCFG(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MINIKINGCFG(params)
#endif // _DEBUG_ERRORS_MINIKINGCFG

struct MINIKING
{
	HANDLE hDev;
	SNRPARAMS params;
};
typedef struct MINIKING* HMINIKING;

#define INVALID_HMINIKING_VALUE NULL

/*
Open a MiniKing.

HMINIKING* phMiniKing : (INOUT) Valid pointer that will receive an identifier of the
MiniKing opened.
char* szDevice : (IN) MiniKing serial port to open.
int NBins : (IN) Number of bins per scanline.
int NSteps : (IN) Number of scanlines in a loop.
int RangeScale : (IN) Range scale (in m).
double scanDirection : (IN) Direction of the sector to scan (in degrees).
double scanWidth : (IN) Width of the sector to scan, 360 for a continuous scan (in degrees).
int Gain : (IN) Gain (in %).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int OpenMiniKing(HMINIKING* phMiniKing, char* szDevice, 
						  int NBins, int NSteps, int RangeScale,
						  double scanDirection, double scanWidth, int Gain)
{
	double LeftAngleLimit = scanDirection - scanWidth/2.0 + 360;
	double RightAngleLimit = scanDirection + scanWidth/2.0;

	*phMiniKing = (HMINIKING)calloc(1, sizeof(struct MINIKING));

	if (*phMiniKing == NULL)
	{
		PRINT_DEBUG_ERROR_MINIKINGCFG(("OpenMiniKing error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	if (OpenRS232Port(&(*phMiniKing)->hDev, szDevice) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MINIKINGCFG(("OpenMiniKing error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot open the serial port. ", 
			szDevice));
		free(*phMiniKing);
		return EXIT_FAILURE;
	}

	if (SetOptionsRS232Port((*phMiniKing)->hDev, 115200, NOPARITY, FALSE, 8, ONESTOPBIT, 1500) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MINIKINGCFG(("OpenMiniKing error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot set the options of the serial port. ", 
			szDevice));
		CloseRS232Port(&(*phMiniKing)->hDev);
		free(*phMiniKing);
		return EXIT_FAILURE;
	}

	if (PurgeRS232Port((*phMiniKing)->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MINIKINGCFG(("OpenMiniKing error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot purge the serial port. ", 
			szDevice));
		CloseRS232Port(&(*phMiniKing)->hDev);
		free(*phMiniKing);
		return EXIT_FAILURE;
	}

	if (WaitForAliveMsgMiniKing((*phMiniKing)->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MINIKINGCFG(("OpenMiniKing error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phMiniKing)->hDev);
		free(*phMiniKing);
		return EXIT_FAILURE;
	}

#ifdef ALWAYS_REBOOT_MINIKING
	if (RebootMiniKing((*phMiniKing)->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MINIKINGCFG(("OpenMiniKing error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phMiniKing)->hDev);
		free(*phMiniKing);
		return EXIT_FAILURE;
	}
#endif // ALWAYS_REBOOT_MINIKING

	if (GetVersionMiniKing((*phMiniKing)->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MINIKINGCFG(("OpenMiniKing error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phMiniKing)->hDev);
		free(*phMiniKing);
		return EXIT_FAILURE;
	}

	if (GetUserMiniKing((*phMiniKing)->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MINIKINGCFG(("OpenMiniKing error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phMiniKing)->hDev);
		free(*phMiniKing);
		return EXIT_FAILURE;
	}

	// Setting various parameters
	(*phMiniKing)->params.adc8on = 1;
	if (scanWidth == 360)
	{
		(*phMiniKing)->params.cont = 1;
	}
	else
	{
		(*phMiniKing)->params.cont = 0;
	}		
	(*phMiniKing)->params.LeftAngleLimit = LeftAngleLimit; // 359
	(*phMiniKing)->params.RightAngleLimit = RightAngleLimit; // 0


/*
	(*phMiniKing)->params.cont = 0;
	(*phMiniKing)->params.LeftAngleLimit = 180+90; // 359
	(*phMiniKing)->params.RightAngleLimit = 0+90; // 0
*/

	(*phMiniKing)->params.VelocityOfSound = 1500;
	(*phMiniKing)->params.RangeScale = RangeScale;
	(*phMiniKing)->params.StepAngleSize = scanWidth/(double)NSteps;
	(*phMiniKing)->params.NBins = NBins; // 200
	(*phMiniKing)->params.IGain = (int)(((double)Gain/100.0)*210.0); // 73

	if (SetParamMiniKing((*phMiniKing)->hDev, (*phMiniKing)->params) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MINIKINGCFG(("OpenMiniKing error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phMiniKing)->hDev);
		free(*phMiniKing);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Discard old data from a MiniKing.
Because the MiniKing sends data every second, you must delete old data if you
did not read it during some time (> 1 s).
Note that a call to this function can last about 100 ms.

HMINIKING hMiniKing : (IN) Identifier of the MiniKing.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int DiscardDataMiniKing(HMINIKING hMiniKing)
{
	if (PurgeRS232Port(hMiniKing->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MINIKINGCFG(("DiscardDataMiniKing error (%s) : %s"
			"(hMiniKing=%#x)\n", 
			strtime_m(), 
			"Cannot purge the serial port. ", 
			hMiniKing));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Get 2 scanlines and the corresponding angles from a MiniKing.
We have 2 scanlines because the MiniKing sends 2 replies for 1 request of data.

HMINIKING hMiniKing : (IN) Identifier of the MiniKing.
uint8* scanline1 : (INOUT) Valid pointer that will receive the first scanline.
double* pAngle1 : (INOUT) Valid pointer that will receive the angle of the 
first scanline (in degrees).
uint8* scanline2 : (INOUT) Valid pointer that will receive the second scanline.
double* pAngle2 : (INOUT) Valid pointer that will receive the angle of the 
second scanline (in degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetScanlineMiniKing(HMINIKING hMiniKing,
								 uint8* scanline1, double* pAngle1, 
								 uint8* scanline2, double* pAngle2)
{
	if (SendDataRequestMiniKing(hMiniKing->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MINIKINGCFG(("GetScanlineMiniKing error (%s) : %s"
			"(hMiniKing=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hMiniKing));
		return EXIT_FAILURE;
	}

	if (SendDataReplyMiniKing(hMiniKing->hDev, hMiniKing->params, 
		scanline1, pAngle1, scanline2, pAngle2) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MINIKINGCFG(("GetScanlineMiniKing error (%s) : %s"
			"(hMiniKing=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hMiniKing));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Return the distance to the first obstacle from a MiniKing scanline.

HMINIKING hMiniKing : (IN) Identifier of the MiniKing.
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
inline int ComputeFirstObstacleDistFromScanlineMiniKing(HMINIKING hMiniKing,
								 uint8* scanline, uint8 threshold, 
								 double minDist, double maxDist, double* pDist)
{
	if ((maxDist > hMiniKing->params.RangeScale)||
		(minDist >= maxDist)||
		(minDist < 0)
		)
	{
		PRINT_DEBUG_ERROR_MINIKINGCFG(("ComputeFirstObstacleDistFromScanlineMiniKing error (%s) : %s"
			"(hMiniKing=%#x)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			hMiniKing));
		return EXIT_FAILURE;
	}

	GetFirstObstacleDist(scanline, threshold, minDist, maxDist, 
						 hMiniKing->params.NBins, hMiniKing->params.RangeScale, 
						 pDist);

	return EXIT_SUCCESS;
}

/*
Close a MiniKing.

HMINIKING* phMiniKing : (INOUT) Valid pointer to the identifier of the 
MiniKing.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CloseMiniKing(HMINIKING* phMiniKing)
{
	BOOL bError = FALSE;

	if (CloseRS232Port(&(*phMiniKing)->hDev) != EXIT_SUCCESS)
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_MINIKINGCFG(("CloseMiniKing error (%s) : %s"
			"(*phMiniKing=%#x)\n", 
			strtime_m(), 
			"Error closing the serial port. ", 
			*phMiniKing));
	}

	free(*phMiniKing);

	*phMiniKing = INVALID_HMINIKING_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // MINIKINGCFG_H
