/***************************************************************************************************************:')

MDMCore.h

Tritech Micron data modem handling.

Fabrice Le Bars

Created : 2012-07-01

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef MDMCORE_H
#define MDMCORE_H

#include "OSRS232Port.h"

/*
Debug macros specific to MDMUtils.
*/
#ifdef _DEBUG_MESSAGES
#	define _DEBUG_MESSAGES_MDMUTILS
#endif // _DEBUG_MESSAGES

#ifdef _DEBUG_WARNINGS
#	define _DEBUG_WARNINGS_MDMUTILS
#endif // _DEBUG_WARNINGS

#ifdef _DEBUG_ERRORS
#	define _DEBUG_ERRORS_MDMUTILS
#endif // _DEBUG_ERRORS

#ifdef _DEBUG_MESSAGES_MDMUTILS
#	define PRINT_DEBUG_MESSAGE_MDMUTILS(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MDMUTILS(params)
#endif // _DEBUG_MESSAGES_MDMUTILS

#ifdef _DEBUG_WARNINGS_MDMUTILS
#	define PRINT_DEBUG_WARNING_MDMUTILS(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MDMUTILS(params)
#endif // _DEBUG_WARNINGS_MDMUTILS

#ifdef _DEBUG_ERRORS_MDMUTILS
#	define PRINT_DEBUG_ERROR_MDMUTILS(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MDMUTILS(params)
#endif // _DEBUG_ERRORS_MDMUTILS

/*
Debug macros specific to MDMCore.
*/
#ifdef _DEBUG_MESSAGES_MDMUTILS
#	define _DEBUG_MESSAGES_MDMCORE
#endif // _DEBUG_MESSAGES_MDMUTILS

#ifdef _DEBUG_WARNINGS_MDMUTILS
#	define _DEBUG_WARNINGS_MDMCORE
#endif // _DEBUG_WARNINGS_MDMUTILS

#ifdef _DEBUG_ERRORS_MDMUTILS
#	define _DEBUG_ERRORS_MDMCORE
#endif // _DEBUG_ERRORS_MDMUTILS

#ifdef _DEBUG_MESSAGES_MDMCORE
#	define PRINT_DEBUG_MESSAGE_MDMCORE(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MDMCORE(params)
#endif // _DEBUG_MESSAGES_MDMCORE

#ifdef _DEBUG_WARNINGS_MDMCORE
#	define PRINT_DEBUG_WARNING_MDMCORE(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MDMCORE(params)
#endif // _DEBUG_WARNINGS_MDMCORE

#ifdef _DEBUG_ERRORS_MDMCORE
#	define PRINT_DEBUG_ERROR_MDMCORE(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MDMCORE(params)
#endif // _DEBUG_ERRORS_MDMCORE

// Device internal buffer (in bytes).
#define INTERNAL_BUFFER_MDM 256

// Acoustic transimission rate (in bps).
#define ACOUSTIC_TRANSMISSION_RATE_MDM 40

#endif // MDMCORE_H
