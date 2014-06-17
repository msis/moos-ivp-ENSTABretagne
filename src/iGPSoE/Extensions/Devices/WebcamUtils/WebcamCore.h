/***************************************************************************************************************:')

WebcamCore.h

Webcam handling.

Fabrice Le Bars

Created : 2009-06-17

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef WEBCAMCORE_H
#define WEBCAMCORE_H

#include "OSCore.h"
#include "OSTime.h"

#include "CvUtils.h"

/*
Debug macros specific to WebcamUtils.
*/
#ifdef _DEBUG_MESSAGES
#	define _DEBUG_MESSAGES_WEBCAMUTILS
#endif // _DEBUG_MESSAGES

#ifdef _DEBUG_WARNINGS
#	define _DEBUG_WARNINGS_WEBCAMUTILS
#endif // _DEBUG_WARNINGS

#ifdef _DEBUG_ERRORS
#	define _DEBUG_ERRORS_WEBCAMUTILS
#endif // _DEBUG_ERRORS

#ifdef _DEBUG_MESSAGES_WEBCAMUTILS
#	define PRINT_DEBUG_MESSAGE_WEBCAMUTILS(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_WEBCAMUTILS(params)
#endif // _DEBUG_MESSAGES_WEBCAMUTILS

#ifdef _DEBUG_WARNINGS_WEBCAMUTILS
#	define PRINT_DEBUG_WARNING_WEBCAMUTILS(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_WEBCAMUTILS(params)
#endif // _DEBUG_WARNINGS_WEBCAMUTILS

#ifdef _DEBUG_ERRORS_WEBCAMUTILS
#	define PRINT_DEBUG_ERROR_WEBCAMUTILS(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_WEBCAMUTILS(params)
#endif // _DEBUG_ERRORS_WEBCAMUTILS

/*
Debug macros specific to WebcamCore.
*/
#ifdef _DEBUG_MESSAGES_WEBCAMUTILS
#	define _DEBUG_MESSAGES_WEBCAMCORE
#endif // _DEBUG_MESSAGES_WEBCAMUTILS

#ifdef _DEBUG_WARNINGS_WEBCAMUTILS
#	define _DEBUG_WARNINGS_WEBCAMCORE
#endif // _DEBUG_WARNINGS_WEBCAMUTILS

#ifdef _DEBUG_ERRORS_WEBCAMUTILS
#	define _DEBUG_ERRORS_WEBCAMCORE
#endif // _DEBUG_ERRORS_WEBCAMUTILS

#ifdef _DEBUG_MESSAGES_WEBCAMCORE
#	define PRINT_DEBUG_MESSAGE_WEBCAMCORE(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_WEBCAMCORE(params)
#endif // _DEBUG_MESSAGES_WEBCAMCORE

#ifdef _DEBUG_WARNINGS_WEBCAMCORE
#	define PRINT_DEBUG_WARNING_WEBCAMCORE(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_WEBCAMCORE(params)
#endif // _DEBUG_WARNINGS_WEBCAMCORE

#ifdef _DEBUG_ERRORS_WEBCAMCORE
#	define PRINT_DEBUG_ERROR_WEBCAMCORE(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_WEBCAMCORE(params)
#endif // _DEBUG_ERRORS_WEBCAMCORE



#endif // WEBCAMCORE_H
