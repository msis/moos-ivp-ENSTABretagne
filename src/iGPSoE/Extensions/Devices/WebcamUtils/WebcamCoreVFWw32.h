/***************************************************************************************************************:')

WebcamCore.h

Webcam handling.

Fabrice Le Bars

Created: 2009-06-17

Version status: Not finished

***************************************************************************************************************:)*/

#ifndef WEBCAMCORE_H
#define WEBCAMCORE_H

#include "OSCore.h"
#include "OSTime.h"

#include "PPMUtils.h"

#ifdef _MSC_VER
// Disable some Visual Studio warnings that happen in Video for Windows.
#pragma warning(disable : 4201) 
#endif // _MSC_VER

// Video for Windows header.
#include <Vfw.h>

#ifdef _MSC_VER
// Restore the Visual Studio warnings previously disabled for Video for Windows.
#pragma warning(default : 4201) 
#endif // _MSC_VER

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
#	define _DEBUG_MESSAGES_WEBCAMCORE
#	define _DEBUG_MESSAGES_WEBCAMCFG
#	define _DEBUG_MESSAGES_WEBCAMMGR
#endif // _DEBUG_MESSAGES_WEBCAMUTILS

#ifdef _DEBUG_WARNINGS_WEBCAMUTILS
#	define _DEBUG_WARNINGS_WEBCAMCORE
#	define _DEBUG_WARNINGS_WEBCAMCFG
#	define _DEBUG_WARNINGS_WEBCAMMGR
#endif // _DEBUG_WARNINGS_WEBCAMUTILS

#ifdef _DEBUG_ERRORS_WEBCAMUTILS
#	define _DEBUG_ERRORS_WEBCAMCORE
#	define _DEBUG_ERRORS_WEBCAMCFG
#	define _DEBUG_ERRORS_WEBCAMMGR
#endif // _DEBUG_ERRORS_WEBCAMUTILS

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

LRESULT CALLBACK capFrameCallback(HWND hWnd, LPVIDEOHDR lpVHdr);

#endif // WEBCAMCORE_H
