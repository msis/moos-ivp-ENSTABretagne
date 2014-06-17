/***************************************************************************************************************:')

MESCore.h

Tritech Micron echosounder handling.

Fabrice Le Bars

Created : 2011-11-30

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef MESCORE_H
#define MESCORE_H

#include "OSRS232Port.h"

/*
Debug macros specific to MESUtils.
*/
#ifdef _DEBUG_MESSAGES
#	define _DEBUG_MESSAGES_MESUTILS
#endif // _DEBUG_MESSAGES

#ifdef _DEBUG_WARNINGS
#	define _DEBUG_WARNINGS_MESUTILS
#endif // _DEBUG_WARNINGS

#ifdef _DEBUG_ERRORS
#	define _DEBUG_ERRORS_MESUTILS
#endif // _DEBUG_ERRORS

#ifdef _DEBUG_MESSAGES_MESUTILS
#	define PRINT_DEBUG_MESSAGE_MESUTILS(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MESUTILS(params)
#endif // _DEBUG_MESSAGES_MESUTILS

#ifdef _DEBUG_WARNINGS_MESUTILS
#	define PRINT_DEBUG_WARNING_MESUTILS(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MESUTILS(params)
#endif // _DEBUG_WARNINGS_MESUTILS

#ifdef _DEBUG_ERRORS_MESUTILS
#	define PRINT_DEBUG_ERROR_MESUTILS(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MESUTILS(params)
#endif // _DEBUG_ERRORS_MESUTILS

/*
Debug macros specific to MESCore.
*/
#ifdef _DEBUG_MESSAGES_MESUTILS
#	define _DEBUG_MESSAGES_MESCORE
#endif // _DEBUG_MESSAGES_MESUTILS

#ifdef _DEBUG_WARNINGS_MESUTILS
#	define _DEBUG_WARNINGS_MESCORE
#endif // _DEBUG_WARNINGS_MESUTILS

#ifdef _DEBUG_ERRORS_MESUTILS
#	define _DEBUG_ERRORS_MESCORE
#endif // _DEBUG_ERRORS_MESUTILS

#ifdef _DEBUG_MESSAGES_MESCORE
#	define PRINT_DEBUG_MESSAGE_MESCORE(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MESCORE(params)
#endif // _DEBUG_MESSAGES_MESCORE

#ifdef _DEBUG_WARNINGS_MESCORE
#	define PRINT_DEBUG_WARNING_MESCORE(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MESCORE(params)
#endif // _DEBUG_WARNINGS_MESCORE

#ifdef _DEBUG_ERRORS_MESCORE
#	define PRINT_DEBUG_ERROR_MESCORE(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MESCORE(params)
#endif // _DEBUG_ERRORS_MESCORE

#define MAX_NB_BYTES_MES 1024

/*
Read a message from a Tritech Micron echosounder.

HANDLE hDev : (IN) Identifier of the device serial port.
double* pValue : (INOUT) Valid pointer receiving the value.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReadMsgMES(HANDLE hDev, double* pValue)
{
	uint8 readbuf[11];

	if (PurgeRS232Port(hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MESCORE(("ReadMsgMES error (%s) : %s"
									"(hDev=%#x)\n", 
									strtime_m(), 
									"Cannot purge the serial port. ", 
									hDev));
		return EXIT_FAILURE;
	}

	memset(readbuf, 0, sizeof(readbuf));

	if (ReadAllRS232Port(hDev, readbuf, 10) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MESCORE(("ReadMsgMES error (%s) : %s"
									"(hDev=%#x)\n", 
									strtime_m(), 
									"Error reading data from the serial port. ", 
									hDev));
		return EXIT_FAILURE;
	}

	if (sscanf((char*)readbuf, "%lfm\r\n", pValue) != 1)
	{
		PRINT_DEBUG_ERROR_MESCORE(("ReadMsgMES error (%s) : %s"
									"(hDev=%#x)\n", 
									strtime_m(), 
									"Invalid data. ", 
									hDev));
		return EXIT_FAILURE;
	}

	PRINT_DEBUG_MESSAGE_MESCORE(("Distance : %f m\n", *pValue));

	return EXIT_SUCCESS;
}

#endif // MESCORE_H
