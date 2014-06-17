/***************************************************************************************************************:')

MicronCfg.h

Micron sonar handling.

Fabrice Le Bars

Created : 2012-07-07

Version status: Not finished

***************************************************************************************************************:)*/

#ifndef MICRONCFG_H
#define MICRONCFG_H

#include "MicronCore.h"

/*
Debug macros specific to MicronCfg.
*/
#ifdef _DEBUG_MESSAGES_MICRONUTILS
#	define _DEBUG_MESSAGES_MICRONCFG
#endif // _DEBUG_MESSAGES_MICRONUTILS

#ifdef _DEBUG_WARNINGS_MICRONUTILS
#	define _DEBUG_WARNINGS_MICRONCFG
#endif // _DEBUG_WARNINGS_MICRONUTILS

#ifdef _DEBUG_ERRORS_MICRONUTILS
#	define _DEBUG_ERRORS_MICRONCFG
#endif // _DEBUG_ERRORS_MICRONUTILS

#ifdef _DEBUG_MESSAGES_MICRONCFG
#	define PRINT_DEBUG_MESSAGE_MICRONCFG(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MICRONCFG(params)
#endif // _DEBUG_MESSAGES_MICRONCFG

#ifdef _DEBUG_WARNINGS_MICRONCFG
#	define PRINT_DEBUG_WARNING_MICRONCFG(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MICRONCFG(params)
#endif // _DEBUG_WARNINGS_MICRONCFG

#ifdef _DEBUG_ERRORS_MICRONCFG
#	define PRINT_DEBUG_ERROR_MICRONCFG(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MICRONCFG(params)
#endif // _DEBUG_ERRORS_MICRONCFG

struct MICRON
{
	HANDLE hDev;
	SNRPARAMS params;
};
typedef struct MICRON* HMICRON;

#define INVALID_HMICRON_VALUE NULL

/*
Open a Micron.

HMICRON* phMicron : (INOUT) Valid pointer that will receive an identifier of the
Micron opened.
char* szDevice : (IN) Micron serial port to open.
int NBins : (IN) Number of bins per scanline.
int NSteps : (IN) Number of scanlines in a loop.
int RangeScale : (IN) Range scale (in m).
double scanDirection : (IN) Direction of the sector to scan (in degrees).
double scanWidth : (IN) Width of the sector to scan, 360 for a continuous scan (in degrees).
int Gain : (IN) Gain (in %).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int OpenMicron(HMICRON* phMicron, char* szDevice, 
						  int NBins, int NSteps, int RangeScale,
						  double scanDirection, double scanWidth, int Gain)
{
	double LeftAngleLimit = scanDirection - scanWidth/2.0 + 360;
	double RightAngleLimit = scanDirection + scanWidth/2.0;

	*phMicron = (HMICRON)calloc(1, sizeof(struct MICRON));

	if (*phMicron == NULL)
	{
		PRINT_DEBUG_ERROR_MICRONCFG(("OpenMicron error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	if (OpenRS232Port(&(*phMicron)->hDev, szDevice) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MICRONCFG(("OpenMicron error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot open the serial port. ", 
			szDevice));
		free(*phMicron);
		return EXIT_FAILURE;
	}

	if (SetOptionsRS232Port((*phMicron)->hDev, 115200, NOPARITY, FALSE, 8, ONESTOPBIT, 1500) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MICRONCFG(("OpenMicron error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot set the options of the serial port. ", 
			szDevice));
		CloseRS232Port(&(*phMicron)->hDev);
		free(*phMicron);
		return EXIT_FAILURE;
	}

	if (PurgeRS232Port((*phMicron)->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MICRONCFG(("OpenMicron error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot purge the serial port. ", 
			szDevice));
		CloseRS232Port(&(*phMicron)->hDev);
		free(*phMicron);
		return EXIT_FAILURE;
	}

	if (WaitForAliveMsgMicron((*phMicron)->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MICRONCFG(("OpenMicron error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phMicron)->hDev);
		free(*phMicron);
		return EXIT_FAILURE;
	}

#ifdef ALWAYS_REBOOT_MICRON
	if (RebootMicron((*phMicron)->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MICRONCFG(("OpenMicron error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phMicron)->hDev);
		free(*phMicron);
		return EXIT_FAILURE;
	}
#endif // ALWAYS_REBOOT_MICRON

	if (GetVersionMicron((*phMicron)->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MICRONCFG(("OpenMicron error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phMicron)->hDev);
		free(*phMicron);
		return EXIT_FAILURE;
	}
/*
	if (GetUserMicron((*phMicron)->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MICRONCFG(("OpenMicron error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phMicron)->hDev);
		free(*phMicron);
		return EXIT_FAILURE;
	}
*/
	// Setting various parameters
	(*phMicron)->params.adc8on = 1;
	if (scanWidth == 360)
	{
		(*phMicron)->params.cont = 1;
	}
	else
	{
		(*phMicron)->params.cont = 0;
	}		
	(*phMicron)->params.LeftAngleLimit = LeftAngleLimit; // 359
	(*phMicron)->params.RightAngleLimit = RightAngleLimit; // 0


/*
	(*phMicron)->params.cont = 0;
	(*phMicron)->params.LeftAngleLimit = 180+90; // 359
	(*phMicron)->params.RightAngleLimit = 0+90; // 0
*/

	(*phMicron)->params.VelocityOfSound = 1500;
	(*phMicron)->params.RangeScale = RangeScale;
	(*phMicron)->params.StepAngleSize = scanWidth/(double)NSteps;
	(*phMicron)->params.NBins = NBins; // 200
	(*phMicron)->params.IGain = (int)(((double)Gain/100.0)*210.0); // 73

	if (SetParamMicron((*phMicron)->hDev, (*phMicron)->params) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MICRONCFG(("OpenMicron error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phMicron)->hDev);
		free(*phMicron);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Discard old data from a Micron.
Because the Micron sends data every second, you must delete old data if you
did not read it during some time (> 1 s).
Note that a call to this function can last about 100 ms.

HMICRON hMicron : (IN) Identifier of the Micron.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int DiscardDataMicron(HMICRON hMicron)
{
	if (PurgeRS232Port(hMicron->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MICRONCFG(("DiscardDataMicron error (%s) : %s"
			"(hMicron=%#x)\n", 
			strtime_m(), 
			"Cannot purge the serial port. ", 
			hMicron));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Get 2 scanlines and the corresponding angles from a Micron.
We have 2 scanlines because the Micron sends 2 replies for 1 request of data.

HMICRON hMicron : (IN) Identifier of the Micron.
uint8* scanline1 : (INOUT) Valid pointer that will receive the first scanline.
double* pAngle1 : (INOUT) Valid pointer that will receive the angle of the 
first scanline (in degrees).
uint8* scanline2 : (INOUT) Valid pointer that will receive the second scanline.
double* pAngle2 : (INOUT) Valid pointer that will receive the angle of the 
second scanline (in degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetScanlineMicron(HMICRON hMicron,
								 uint8* scanline1, double* pAngle1, 
								 uint8* scanline2, double* pAngle2)
{
	if (SendDataRequestMicron(hMicron->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MICRONCFG(("GetScanlineMicron error (%s) : %s"
			"(hMicron=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hMicron));
		return EXIT_FAILURE;
	}

	if (SendDataReplyMicron(hMicron->hDev, hMicron->params, 
		scanline1, pAngle1, scanline2, pAngle2) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MICRONCFG(("GetScanlineMicron error (%s) : %s"
			"(hMicron=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hMicron));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Return the distance to the first obstacle from a Micron scanline.

HMICRON hMicron : (IN) Identifier of the Micron.
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
inline int ComputeFirstObstacleDistFromScanlineMicron(HMICRON hMicron,
								 uint8* scanline, uint8 threshold, 
								 double minDist, double maxDist, double* pDist)
{
	if ((maxDist > hMicron->params.RangeScale)||
		(minDist >= maxDist)||
		(minDist < 0)
		)
	{
		PRINT_DEBUG_ERROR_MICRONCFG(("ComputeFirstObstacleDistFromScanlineMicron error (%s) : %s"
			"(hMicron=%#x)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			hMicron));
		return EXIT_FAILURE;
	}

	GetFirstObstacleDist(scanline, threshold, minDist, maxDist, 
						 hMicron->params.NBins, hMicron->params.RangeScale, 
						 pDist);

	return EXIT_SUCCESS;
}

/*
Close a Micron.

HMICRON* phMicron : (INOUT) Valid pointer to the identifier of the 
Micron.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CloseMicron(HMICRON* phMicron)
{
	BOOL bError = FALSE;

	if (CloseRS232Port(&(*phMicron)->hDev) != EXIT_SUCCESS)
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_MICRONCFG(("CloseMicron error (%s) : %s"
			"(*phMicron=%#x)\n", 
			strtime_m(), 
			"Error closing the serial port. ", 
			*phMicron));
	}

	free(*phMicron);

	*phMicron = INVALID_HMICRON_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // MICRONCFG_H
