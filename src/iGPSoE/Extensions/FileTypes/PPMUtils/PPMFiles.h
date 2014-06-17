/***************************************************************************************************************:')

PPMFiles.h

PPM picture files handling.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef PPMFILES_H
#define PPMFILES_H

#include "PPMCore.h"

/*
Debug macros specific to PPMFiles.
*/
#ifdef _DEBUG_MESSAGES_PPMUTILS
#	define _DEBUG_MESSAGES_PPMFILES
#endif // _DEBUG_MESSAGES_PPMUTILS

#ifdef _DEBUG_WARNINGS_PPMUTILS
#	define _DEBUG_WARNINGS_PPMFILES
#endif // _DEBUG_WARNINGS_PPMUTILS

#ifdef _DEBUG_ERRORS_PPMUTILS
#	define _DEBUG_ERRORS_PPMFILES
#endif // _DEBUG_ERRORS_PPMUTILS

#ifdef _DEBUG_MESSAGES_PPMFILES
#	define PRINT_DEBUG_MESSAGE_PPMFILES(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_PPMFILES(params)
#endif // _DEBUG_MESSAGES_PPMFILES

#ifdef _DEBUG_WARNINGS_PPMFILES
#	define PRINT_DEBUG_WARNING_PPMFILES(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_PPMFILES(params)
#endif // _DEBUG_WARNINGS_PPMFILES

#ifdef _DEBUG_ERRORS_PPMFILES
#	define PRINT_DEBUG_ERROR_PPMFILES(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_PPMFILES(params)
#endif // _DEBUG_ERRORS_PPMFILES

/*
Open, read and then close a PPM file and return a structure that contains 
picture data corresponding to the PPM format. Use DestroyPPMData() to free 
the picture data at the end.
See the description of the PPMDATA structure for more information. 

HPPMDATA* phData : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* filename : (IN) Path to the file to read.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int CreatePPMDataFromFile(HPPMDATA* phData, char* filename);

/*
Open, read and then close a PPM file and copy picture data corresponding to the 
PPM format into a structure whose characteristics match the data in the file 
(already created with the corresponding height, width...).
See the description of the PPMDATA structure for more information. 

HPPMDATA* phData : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* filename : (IN) Path to the file to read.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int CopyPPMDataFromFile(HPPMDATA* phData, char* filename);

/*
Save PPM data to a PPM file. 
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (IN) Identifier of the PPMDATA to save.
char* filename : (IN) Path to the file to write.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int SavePPMDataToFile(HPPMDATA hData, char* filename);

#endif // PPMFILES_H
