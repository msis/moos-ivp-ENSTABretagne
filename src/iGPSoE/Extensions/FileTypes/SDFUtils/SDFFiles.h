/***************************************************************************************************************:')

SDFFiles.h

SDF SonarPro files and data handling.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef SDFFILES_H
#define SDFFILES_H

#include "SDFCore.h"

/*
Debug macros specific to SDFFiles.
*/
#ifdef _DEBUG_MESSAGES_SDFUTILS
#	define _DEBUG_MESSAGES_SDFFILES
#endif // _DEBUG_MESSAGES_SDFUTILS

#ifdef _DEBUG_WARNINGS_SDFUTILS
#	define _DEBUG_WARNINGS_SDFFILES
#endif // _DEBUG_WARNINGS_SDFUTILS

#ifdef _DEBUG_ERRORS_SDFUTILS
#	define _DEBUG_ERRORS_SDFFILES
#endif // _DEBUG_ERRORS_SDFUTILS

#ifdef _DEBUG_MESSAGES_SDFFILES
#	define PRINT_DEBUG_MESSAGE_SDFFILES(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_SDFFILES(params)
#endif // _DEBUG_MESSAGES_SDFFILES

#ifdef _DEBUG_WARNINGS_SDFFILES
#	define PRINT_DEBUG_WARNING_SDFFILES(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_SDFFILES(params)
#endif // _DEBUG_WARNINGS_SDFFILES

#ifdef _DEBUG_ERRORS_SDFFILES
#	define PRINT_DEBUG_ERROR_SDFFILES(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_SDFFILES(params)
#endif // _DEBUG_ERRORS_SDFFILES

#define STARBOARD 0
#define PORT 1

/*
Open, read and then close a SDF file and return a structure that contains 
sonar data corresponding to the SDF format. Use DestroySDFData() to free 
the sonar data at the end.
See the description of the SDFDATA structure for more information. 

HSDFDATA* phData : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* filename : (IN) Path to the file to read.
int channel : (IN) Either PORT (left sonar image) or STARBOARD (right sonar image, default option).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int CreateSDFDataFromFile(HSDFDATA* phData, char* filename, int channel);

/*
Get the first ping header in a SDF file. 

SYS5000HEADER* pHeader : (INOUT) Pointer to the first ping header.
char* filename : (IN) Path to the file to read.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int ReadFirstSDFHeader(SYS5000HEADER* pHeader, char* filename);

#endif // SDFFILES_H
