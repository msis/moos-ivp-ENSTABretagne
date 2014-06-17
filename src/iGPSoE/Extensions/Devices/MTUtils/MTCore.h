/***************************************************************************************************************:')

MTCore.h

MT inertial measurement unit handling.

Fabrice Le Bars

Created : 2009-06-18

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef MTCORE_H
#define MTCORE_H

#include "OSCore.h"
#include "OSTime.h"

#ifdef _MSC_VER
// Disable some Visual Studio warnings that happen in Xbus.
#pragma warning(disable : 4244) 
#pragma warning(disable : 4127) 
#pragma warning(disable : 4996)
#endif // _MSC_VER

// Class provided by Xsens, the manufacturer.
#include "Xbus.h"

#ifdef _MSC_VER
// Restore the Visual Studio warnings previously disabled for Xbus.
#pragma warning(default : 4244) 
#pragma warning(default : 4127) 
#pragma warning(default : 4996)
#endif // _MSC_VER

/*
Debug macros specific to MTUtils.
*/
#ifdef _DEBUG_MESSAGES
#	define _DEBUG_MESSAGES_MTUTILS
#endif // _DEBUG_MESSAGES

#ifdef _DEBUG_WARNINGS
#	define _DEBUG_WARNINGS_MTUTILS
#endif // _DEBUG_WARNINGS

#ifdef _DEBUG_ERRORS
#	define _DEBUG_ERRORS_MTUTILS
#endif // _DEBUG_ERRORS

#ifdef _DEBUG_MESSAGES_MTUTILS
#	define PRINT_DEBUG_MESSAGE_MTUTILS(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MTUTILS(params)
#endif // _DEBUG_MESSAGES_MTUTILS

#ifdef _DEBUG_WARNINGS_MTUTILS
#	define PRINT_DEBUG_WARNING_MTUTILS(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MTUTILS(params)
#endif // _DEBUG_WARNINGS_MTUTILS

#ifdef _DEBUG_ERRORS_MTUTILS
#	define PRINT_DEBUG_ERROR_MTUTILS(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MTUTILS(params)
#endif // _DEBUG_ERRORS_MTUTILS

/*
Debug macros specific to MTCore.
*/
#ifdef _DEBUG_MESSAGES_MTUTILS
#	define _DEBUG_MESSAGES_MTCORE
#endif // _DEBUG_MESSAGES_MTUTILS

#ifdef _DEBUG_WARNINGS_MTUTILS
#	define _DEBUG_WARNINGS_MTCORE
#endif // _DEBUG_WARNINGS_MTUTILS

#ifdef _DEBUG_ERRORS_MTUTILS
#	define _DEBUG_ERRORS_MTCORE
#endif // _DEBUG_ERRORS_MTUTILS

#ifdef _DEBUG_MESSAGES_MTCORE
#	define PRINT_DEBUG_MESSAGE_MTCORE(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MTCORE(params)
#endif // _DEBUG_MESSAGES_MTCORE

#ifdef _DEBUG_WARNINGS_MTCORE
#	define PRINT_DEBUG_WARNING_MTCORE(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MTCORE(params)
#endif // _DEBUG_WARNINGS_MTCORE

#ifdef _DEBUG_ERRORS_MTCORE
#	define PRINT_DEBUG_ERROR_MTCORE(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MTCORE(params)
#endif // _DEBUG_ERRORS_MTCORE

// MT data
struct _MTDATA	{
	// In m/s^2
	double AccX;
	double AccY;
	double AccZ;
	// In rad/s
	double GyrX;
	double GyrY;
	double GyrZ;
	// In a.u.
	double MagX;
	double MagY;
	double MagZ;
	// In degrees
	double Roll;
	double Pitch;
	double Yaw;
};
typedef struct _MTDATA MTDATA;

#endif // MTCORE_H
