/***************************************************************************************************************:')

SDFProc.h

SDF data processing.

Fabrice Le Bars

Created : 2009-02-12

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef SDFPROC_H
#define SDFPROC_H

#include "SDFCore.h"

/*
Debug macros specific to SDFProc.
*/
#ifdef _DEBUG_MESSAGES_SDFUTILS
#	define _DEBUG_MESSAGES_SDFPROC
#endif // _DEBUG_MESSAGES_SDFUTILS

#ifdef _DEBUG_WARNINGS_SDFUTILS
#	define _DEBUG_WARNINGS_SDFPROC
#endif // _DEBUG_WARNINGS_SDFUTILS

#ifdef _DEBUG_ERRORS_SDFUTILS
#	define _DEBUG_ERRORS_SDFPROC
#endif // _DEBUG_ERRORS_SDFUTILS

#ifdef _DEBUG_MESSAGES_SDFPROC
#	define PRINT_DEBUG_MESSAGE_SDFPROC(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_SDFPROC(params)
#endif // _DEBUG_MESSAGES_SDFPROC

#ifdef _DEBUG_WARNINGS_SDFPROC
#	define PRINT_DEBUG_WARNING_SDFPROC(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_SDFPROC(params)
#endif // _DEBUG_WARNINGS_SDFPROC

#ifdef _DEBUG_ERRORS_SDFPROC
#	define PRINT_DEBUG_ERROR_SDFPROC(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_SDFPROC(params)
#endif // _DEBUG_ERRORS_SDFPROC

/*
Improve the contrast in sonar data corresponding to the SDF format. 
See the description of the SDFDATA structure for more information.

HSDFDATA hData : (IN) Identifier of the structure.
USHORT Threshold : (IN) Each sample will be divided by Threshold (between 0 and 65535). 
If it is 0, each sample will be divided by the maximum sample value.
Note that 1000 is often a good value.

Return : Nothing.
*/
EXTERN_C void ContrastSDFData(HSDFDATA hData, USHORT Threshold);

#endif // SDFPROC_H
