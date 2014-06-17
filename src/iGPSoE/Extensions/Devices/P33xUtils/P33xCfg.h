/***************************************************************************************************************:')

P33xCfg.h

P33x pressure sensor handling.

Fabrice Le Bars

Created : 2009-03-28

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef P33XCFG_H
#define P33XCFG_H

#include "P33xCore.h"

/*
Debug macros specific to P33xCfg.
*/
#ifdef _DEBUG_MESSAGES_P33XUTILS
#	define _DEBUG_MESSAGES_P33XCFG
#endif // _DEBUG_MESSAGES_P33XUTILS

#ifdef _DEBUG_WARNINGS_P33XUTILS
#	define _DEBUG_WARNINGS_P33XCFG
#endif // _DEBUG_WARNINGS_P33XUTILS

#ifdef _DEBUG_ERRORS_P33XUTILS
#	define _DEBUG_ERRORS_P33XCFG
#endif // _DEBUG_ERRORS_P33XUTILS

#ifdef _DEBUG_MESSAGES_P33XCFG
#	define PRINT_DEBUG_MESSAGE_P33XCFG(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_P33XCFG(params)
#endif // _DEBUG_MESSAGES_P33XCFG

#ifdef _DEBUG_WARNINGS_P33XCFG
#	define PRINT_DEBUG_WARNING_P33XCFG(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_P33XCFG(params)
#endif // _DEBUG_WARNINGS_P33XCFG

#ifdef _DEBUG_ERRORS_P33XCFG
#	define PRINT_DEBUG_ERROR_P33XCFG(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_P33XCFG(params)
#endif // _DEBUG_ERRORS_P33XCFG

typedef HANDLE HP33X;

#define INVALID_HP33X_VALUE INVALID_HANDLE_VALUE

/*
Open a P33x.

HP33X* phP33x : (INOUT) Valid pointer that will receive an identifier of the
P33x opened.
char* szDevice : (IN) P33x serial port to open.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int OpenP33x(HP33X* phP33x, char* szDevice)
{
	if (OpenRS232Port(phP33x, szDevice) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_P33XCFG(("OpenP33x error (%s) : %s"
								   "(szDevice=%s)\n", 
								   strtime_m(), 
								   "Cannot open the serial port. ", 
								   szDevice));
		return EXIT_FAILURE;
	}

	if (SetOptionsRS232Port(*phP33x, 9600, NOPARITY, FALSE, 8, ONESTOPBIT, 200) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_P33XCFG(("OpenP33x error (%s) : %s"
								   "(szDevice=%s)\n", 
								   strtime_m(), 
								   "Cannot set the options of the serial port. ", 
								   szDevice));
		CloseRS232Port(phP33x);
		return EXIT_FAILURE;
	}

	if (PurgeRS232Port(*phP33x) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_P33XCFG(("OpenP33x error (%s) : %s"
								   "(szDevice=%s)\n", 
								   strtime_m(), 
								   "Cannot purge the serial port. ", 
								   szDevice));
		CloseRS232Port(phP33x);
		return EXIT_FAILURE;
	}

	if (InitP33x(*phP33x) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_P33XCFG(("OpenP33x error (%s) : %s"
								   "(szDevice=%s)\n", 
								   strtime_m(), 
								   "Error initializing the device. ", 
								   szDevice));
		CloseRS232Port(phP33x);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Get the pressure from a P33x.

HP33X hP33x : (IN) Identifier of the P33x.
double* pPressure : (INOUT) Valid pointer receiving the pressure (in bar).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetPressureP33x(HP33X hP33x, double* pPressure)
{
	float fValue = 0;

	if (ReadChannelP33x(hP33x, 1, &fValue) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_P33XCFG(("GetPressureP33x error (%s) : %s"
								   "(hP33x=%#x)\n", 
								   strtime_m(), 
								   "Error getting data from the device. ", 
								   hP33x));
		return EXIT_FAILURE;
	}

	*pPressure = (double)fValue;

	return EXIT_SUCCESS;
}

/*
Get the temperature from a P33x.

HP33X hP33x : (IN) Identifier of the P33x.
double* pTemperature : (INOUT) Valid pointer receiving the temperature (in 
Celsius degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetTemperatureP33x(HP33X hP33x, double* pTemperature)
{
	float fValue = 0;

	if (ReadChannelP33x(hP33x, 4, &fValue) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_P33XCFG(("GetTemperatureP33x error (%s) : %s"
								   "(hP33x=%#x)\n", 
								   strtime_m(), 
								   "Error getting data from the device. ", 
								   hP33x));
		return EXIT_FAILURE;
	}

	*pTemperature = (double)fValue;

	return EXIT_SUCCESS;
}

/*
Close a P33x.

HP33X* phP33x : (INOUT) Valid pointer to the identifier of the P33x.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CloseP33x(HP33X* phP33x)
{
	if (CloseRS232Port(phP33x) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_P33XCFG(("CloseP33x error (%s) : %s"
								   "(*phP33x=%#x)\n", 
								   strtime_m(), 
								   "Error closing the serial port. ", 
								   *phP33x));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // P33XCFG_H
