/***************************************************************************************************************:')

PGMFiles.h

PGM picture files handling.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef PGMFILES_H
#define PGMFILES_H

#include "PGMCore.h"

/*
Debug macros specific to PGMFiles.
*/
#ifdef _DEBUG_MESSAGES_PGMUTILS
#	define _DEBUG_MESSAGES_PGMFILES
#endif // _DEBUG_MESSAGES_PGMUTILS

#ifdef _DEBUG_WARNINGS_PGMUTILS
#	define _DEBUG_WARNINGS_PGMFILES
#endif // _DEBUG_WARNINGS_PGMUTILS

#ifdef _DEBUG_ERRORS_PGMUTILS
#	define _DEBUG_ERRORS_PGMFILES
#endif // _DEBUG_ERRORS_PGMUTILS

#ifdef _DEBUG_MESSAGES_PGMFILES
#	define PRINT_DEBUG_MESSAGE_PGMFILES(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_PGMFILES(params)
#endif // _DEBUG_MESSAGES_PGMFILES

#ifdef _DEBUG_WARNINGS_PGMFILES
#	define PRINT_DEBUG_WARNING_PGMFILES(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_PGMFILES(params)
#endif // _DEBUG_WARNINGS_PGMFILES

#ifdef _DEBUG_ERRORS_PGMFILES
#	define PRINT_DEBUG_ERROR_PGMFILES(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_PGMFILES(params)
#endif // _DEBUG_ERRORS_PGMFILES

/*
Open, read and then close a PGM file and return a structure that contains 
picture data corresponding to the PGM format. Use DestroyPGMData() to free 
the picture data at the end.
See the description of the PGMDATA structure for more information. 

HPGMDATA* phData : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* filename : (IN) Path to the file to read.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int CreatePGMDataFromFile(HPGMDATA* phData, char* filename);

/*
Open, read and then close a PGM file and copy picture data corresponding to the 
PGM format into a structure whose characteristics match the data in the file 
(already created with the corresponding height, width...).
See the description of the PGMDATA structure for more information. 

HPGMDATA* phData : (INOUT) Valid pointer that will receive an identifier of
the structure.
char* filename : (IN) Path to the file to read.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int CopyPGMDataFromFile(HPGMDATA* phData, char* filename);

/*
Save PGM data to a PGM file. 
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (IN) Identifier of the PGMDATA to save.
char* filename : (IN) Path to the file to write.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int SavePGMDataToFile(HPGMDATA hData, char* filename);

#endif // PGMFILES_H
