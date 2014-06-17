/***************************************************************************************************************:')

SeanetCfg.h

Tritech sonar (compatible with Seanet Pro software) handling.

Fabrice Le Bars

Created : 2012-05-08

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef SEANETCFG_H
#define SEANETCFG_H

#include "SeanetCore.h"

/*
Debug macros specific to SeanetCfg.
*/
#ifdef _DEBUG_MESSAGES_SEANETUTILS
#	define _DEBUG_MESSAGES_SEANETCFG
#endif // _DEBUG_MESSAGES_SEANETUTILS

#ifdef _DEBUG_WARNINGS_SEANETUTILS
#	define _DEBUG_WARNINGS_SEANETCFG
#endif // _DEBUG_WARNINGS_SEANETUTILS

#ifdef _DEBUG_ERRORS_SEANETUTILS
#	define _DEBUG_ERRORS_SEANETCFG
#endif // _DEBUG_ERRORS_SEANETUTILS

#ifdef _DEBUG_MESSAGES_SEANETCFG
#	define PRINT_DEBUG_MESSAGE_SEANETCFG(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_SEANETCFG(params)
#endif // _DEBUG_MESSAGES_SEANETCFG

#ifdef _DEBUG_WARNINGS_SEANETCFG
#	define PRINT_DEBUG_WARNING_SEANETCFG(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_SEANETCFG(params)
#endif // _DEBUG_WARNINGS_SEANETCFG

#ifdef _DEBUG_ERRORS_SEANETCFG
#	define PRINT_DEBUG_ERROR_SEANETCFG(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_SEANETCFG(params)
#endif // _DEBUG_ERRORS_SEANETCFG

struct SEANET
{
	HANDLE hDev;
	BOOL bDST; // Determined using mtBBUserData.
	BOOL bHalfDuplex; // RS485, determined using mtBBUserData.
	BOOL bSingleChannel;
int NBins; // Number of bins generated for a ping;
int NSteps;
	int RangeScale; // In meters
double scanDirection;
double scanWidth;
int Gain;
int Contrast;
int Sensitivity;
	int adc8on; // The head will return 4-bit packed echo data (0..15) representing the amplitude
	// of received echoes in a databin if it is set to 0. Otherwise, it will be in 8 bits (0..255)
	int cont; // Scanning will be restricted to a sector defined by the direction LeftAngleLimit 
	// and RightAngleLimit if it is set to 0. Otherwise, it will be a continuous rotation and 
	// LeftAngleLimit and RightAngleLimit will be ignored
	int invert; // Allow the rotation direction to be reversed if the sonar head is mounted inverted, 
	// i.e. when the sonar transducer boot is pointing downward rather than up (Default = 0 = Sonar 
	// mounted upright, transducer boot pointing up)
	double LeftAngleLimit; // For a sector scan (cont = 0), in degrees
	double RightAngleLimit; // For a sector scan (cont = 0), in degrees
	int VelocityOfSound; // In m/s
	double StepAngleSize; // In degrees: Ultimate Resolution (0.225), High Resolution (0.45), Medium Resolution (0.9), Low Resolution (1.8), Very Low Resolution (3.6)
	// StepAngleSize = (scanWidth/NSteps)
	
};
typedef struct SEANET* HSEANET;

#define INVALID_HSEANET_VALUE NULL

/*
Open a Seanet Pro-compatible Tritech sonar.

HSEANET* phSeanet : (INOUT) Valid pointer that will receive an identifier of the
device opened.
char* szDevice : (IN) Device serial port to open.
int NBins : (IN) Number of bins per scanline (< 800, or 1500 for DST).
int NSteps : (IN) Number of scanlines in a loop (preferably in [100;1600]).
int RangeScale : (IN) Range scale (in [2;100] m).
double scanDirection : (IN) Direction of the sector to scan (in [0,359] degrees).
double scanWidth : (IN) Width of the sector to scan, 360 for a continuous scan (in [2,360] degrees).
int Gain : (IN) Gain (in %).
int Contrast : (IN) Contrast (in [2;47] dB, cf ADSpan).
int Sensitivity : (IN) Sensitivity (in [0;78] dB, cf ADLow).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int OpenSeanet(HSEANET* phSeanet, char* szDevice, 
						  int NBins, int NSteps, int RangeScale,
						  double scanDirection, double scanWidth, 
						  int Gain, int Contrast, int Sensitivity)
{
	*phSeanet = (HSEANET)calloc(1, sizeof(struct SEANET));

	if (*phSeanet == NULL)
	{
		PRINT_DEBUG_ERROR_SEANETCFG(("OpenSeanet error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	if (OpenRS232Port(&(*phSeanet)->hDev, szDevice) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_SEANETCFG(("OpenSeanet error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot open the serial port. ", 
			szDevice));
		free(*phSeanet);
		return EXIT_FAILURE;
	}

	if (SetOptionsRS232Port((*phSeanet)->hDev, 115200, NOPARITY, FALSE, 8, ONESTOPBIT, 1500) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_SEANETCFG(("OpenSeanet error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot set the options of the serial port. ", 
			szDevice));
		CloseRS232Port(&(*phSeanet)->hDev);
		free(*phSeanet);
		return EXIT_FAILURE;
	}

	if (PurgeRS232Port((*phSeanet)->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_SEANETCFG(("OpenSeanet error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot purge the serial port. ", 
			szDevice));
		CloseRS232Port(&(*phSeanet)->hDev);
		free(*phSeanet);
		return EXIT_FAILURE;
	}

	if (RebootSeanet((*phSeanet)->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_SEANETCFG(("OpenSeanet error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phSeanet)->hDev);
		free(*phSeanet);
		return EXIT_FAILURE;
	}

	if (GetVersionSeanet((*phSeanet)->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_SEANETCFG(("OpenSeanet error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phSeanet)->hDev);
		free(*phSeanet);
		return EXIT_FAILURE;
	}

	if (GetBBUserSeanet((*phSeanet)->hDev, &(*phSeanet)->bDST, &(*phSeanet)->bHalfDuplex) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_SEANETCFG(("OpenSeanet error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phSeanet)->hDev);
		free(*phSeanet);
		return EXIT_FAILURE;
	}

	if (SetHeadCommandSeanet((*phSeanet)->hDev, (*phSeanet)->params) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_SEANETCFG(("OpenSeanet error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error initializing the device. ", 
			szDevice));
		CloseRS232Port(&(*phSeanet)->hDev);
		free(*phSeanet);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Discard old data from a Seanet.
Because the Seanet sends data every second, you must delete old data if you
did not read it during some time (> 1 s).
Note that a call to this function can last about 100 ms.

HSEANET hSeanet : (IN) Identifier of the Seanet.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int DiscardDataSeanet(HSEANET hSeanet)
{
	if (PurgeRS232Port(hSeanet->hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_SEANETCFG(("DiscardDataSeanet error (%s) : %s"
			"(hSeanet=%#x)\n", 
			strtime_m(), 
			"Cannot purge the serial port. ", 
			hSeanet));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Get 2 scanlines and the corresponding angles from a Seanet.
We have 2 scanlines because the Seanet sends 2 replies for 1 request of data.

HSEANET hSeanet : (IN) Identifier of the Seanet.
uint8* scanline1 : (INOUT) Valid pointer that will receive the first scanline.
double* pAngle1 : (INOUT) Valid pointer that will receive the angle of the 
first scanline (in degrees).
uint8* scanline2 : (INOUT) Valid pointer that will receive the second scanline.
double* pAngle2 : (INOUT) Valid pointer that will receive the angle of the 
second scanline (in degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetScanlineSeanet(HSEANET hSeanet,
								 uint8* scanline1, double* pAngle1, 
								 uint8* scanline2, double* pAngle2)
{
	if (GetHeadDataSeanet(hSeanet->hDev, hSeanet->params, 
		scanline1, pAngle1, scanline2, pAngle2) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_SEANETCFG(("GetScanlineSeanet error (%s) : %s"
			"(hSeanet=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hSeanet));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Return the distance to the first obstacle from a Seanet scanline.

HSEANET hSeanet : (IN) Identifier of the Seanet.
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
inline int ComputeFirstObstacleDistFromScanlineSeanet(HSEANET hSeanet,
								 uint8* scanline, uint8 threshold, 
								 double minDist, double maxDist, double* pDist)
{
	if ((maxDist > hSeanet->params.RangeScale)||
		(minDist >= maxDist)||
		(minDist < 0)
		)
	{
		PRINT_DEBUG_ERROR_SEANETCFG(("ComputeFirstObstacleDistFromScanlineSeanet error (%s) : %s"
			"(hSeanet=%#x)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			hSeanet));
		return EXIT_FAILURE;
	}

	GetFirstObstacleDist(scanline, threshold, minDist, maxDist, 
						 hSeanet->params.NBins, hSeanet->params.RangeScale, 
						 pDist);

	return EXIT_SUCCESS;
}

/*
Close a Seanet.

HSEANET* phSeanet : (INOUT) Valid pointer to the identifier of the 
Seanet.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CloseSeanet(HSEANET* phSeanet)
{
	BOOL bError = FALSE;

	if (CloseRS232Port(&(*phSeanet)->hDev) != EXIT_SUCCESS)
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_SEANETCFG(("CloseSeanet error (%s) : %s"
			"(*phSeanet=%#x)\n", 
			strtime_m(), 
			"Error closing the serial port. ", 
			*phSeanet));
	}

	free(*phSeanet);

	*phSeanet = INVALID_HSEANET_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // SEANETCFG_H
