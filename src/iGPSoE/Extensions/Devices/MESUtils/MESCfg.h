/***************************************************************************************************************:')

MESCfg.h

Tritech Micron echosounder handling.

Fabrice Le Bars

Created : 2011-11-30

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef MESCFG_H
#define MESCFG_H

#include "MESCore.h"

/*
Debug macros specific to MESCfg.
*/
#ifdef _DEBUG_MESSAGES_MESUTILS
#	define _DEBUG_MESSAGES_MESCFG
#endif // _DEBUG_MESSAGES_MESUTILS

#ifdef _DEBUG_WARNINGS_MESUTILS
#	define _DEBUG_WARNINGS_MESCFG
#endif // _DEBUG_WARNINGS_MESUTILS

#ifdef _DEBUG_ERRORS_MESUTILS
#	define _DEBUG_ERRORS_MESCFG
#endif // _DEBUG_ERRORS_MESUTILS

#ifdef _DEBUG_MESSAGES_MESCFG
#	define PRINT_DEBUG_MESSAGE_MESCFG(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MESCFG(params)
#endif // _DEBUG_MESSAGES_MESCFG

#ifdef _DEBUG_WARNINGS_MESCFG
#	define PRINT_DEBUG_WARNING_MESCFG(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MESCFG(params)
#endif // _DEBUG_WARNINGS_MESCFG

#ifdef _DEBUG_ERRORS_MESCFG
#	define PRINT_DEBUG_ERROR_MESCFG(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MESCFG(params)
#endif // _DEBUG_ERRORS_MESCFG

typedef HANDLE HMES;

#define INVALID_HMES_VALUE INVALID_HANDLE_VALUE

/*
Open a Tritech Micron echosounder.

HMES* phMES : (INOUT) Valid pointer that will receive an identifier of the
device opened.
char* szDevice : (IN) Device serial port to open.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int OpenMES(HMES* phMES, char* szDevice)
{
	if (OpenRS232Port(phMES, szDevice) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MESCFG(("OpenMES error (%s) : %s"
								   "(szDevice=%s)\n", 
								   strtime_m(), 
								   "Cannot open the serial port. ", 
								   szDevice));
		return EXIT_FAILURE;
	}

	if (SetOptionsRS232Port(*phMES, 9600, NOPARITY, FALSE, 8, ONESTOPBIT, 500) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MESCFG(("OpenMES error (%s) : %s"
								   "(szDevice=%s)\n", 
								   strtime_m(), 
								   "Cannot set the options of the serial port. ", 
								   szDevice));
		CloseRS232Port(phMES);
		return EXIT_FAILURE;
	}

	if (PurgeRS232Port(*phMES) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MESCFG(("OpenMES error (%s) : %s"
								   "(szDevice=%s)\n", 
								   strtime_m(), 
								   "Cannot purge the serial port. ", 
								   szDevice));
		CloseRS232Port(phMES);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Discard old data from a Tritech Micron echosounder.
Note that a call to this function can last about 100 ms.

HMES hMES : (IN) Identifier of the device.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int DiscardDataMES(HMES hMES)
{
	if (PurgeRS232Port(hMES) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MESCFG(("DiscardDataMES error (%s) : %s"
			"(hMES=%#x)\n", 
			strtime_m(), 
			"Cannot purge the serial port. ", 
			hMES));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Get the distance from a Tritech Micron echosounder.

HMES hMES : (IN) Identifier of the device.
double* pDistance : (INOUT) Valid pointer receiving the distance (in m).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetDistanceMES(HMES hMES, double* pDistance)
{
	if (ReadMsgMES(hMES, pDistance) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MESCFG(("GetDistanceMES error (%s) : %s"
								   "(hMES=%#x)\n", 
								   strtime_m(), 
								   "Error getting data from the device. ", 
								   hMES));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Close a Tritech Micron echosounder.

HMES* phMES : (INOUT) Valid pointer to the identifier of the device.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CloseMES(HMES* phMES)
{
	if (CloseRS232Port(phMES) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MESCFG(("CloseMES error (%s) : %s"
								   "(*phMES=%#x)\n", 
								   strtime_m(), 
								   "Error closing the serial port. ", 
								   *phMES));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // MESCFG_H
