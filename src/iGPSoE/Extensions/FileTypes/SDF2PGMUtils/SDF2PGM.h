/***************************************************************************************************************:')

SDF2PGM.h

Conversions from SDF SonarPro files to PGM picture files.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef SDF2PGM_H
#define SDF2PGM_H

#include "PGMUtils.h"
#include "SDFUtils.h"

/*
Debug macros specific to SDF2PGM.
*/
#ifdef _DEBUG_MESSAGES
#	define _DEBUG_MESSAGES_SDF2PGM
#endif // _DEBUG_MESSAGES

#ifdef _DEBUG_WARNINGS
#	define _DEBUG_WARNINGS_SDF2PGM
#endif // _DEBUG_WARNINGS

#ifdef _DEBUG_ERRORS
#	define _DEBUG_ERRORS_SDF2PGM
#endif // _DEBUG_ERRORS

#ifdef _DEBUG_MESSAGES_SDF2PGM
#	define PRINT_DEBUG_MESSAGE_SDF2PGM(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_SDF2PGM(params)
#endif // _DEBUG_MESSAGES_SDF2PGM

#ifdef _DEBUG_WARNINGS_SDF2PGM
#	define PRINT_DEBUG_WARNING_SDF2PGM(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_SDF2PGM(params)
#endif // _DEBUG_WARNINGS_SDF2PGM

#ifdef _DEBUG_ERRORS_SDF2PGM
#	define PRINT_DEBUG_ERROR_SDF2PGM(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_SDF2PGM(params)
#endif // _DEBUG_ERRORS_SDF2PGM

/*
Convert a SonarPro image to PGM data.
Create and initialize a PGMDATA structure corresponding to a SDFDATA structure. 
Use DestroyPGMData() to free phPGMdata at the end.
See the description of the PGMDATA structure for more information.

HPGMDATA* phPGMdata : (INOUT) Valid pointer that will receive an identifier of
the structure that will receive the PGM data corresponding to the SDF data.
HSDFDATA hSDFdata : (IN) Identifier of the SDFDATA to copy. 
BOOL bOnlyCopyData : (IN) TRUE if phPGMdata corresponds already to a PGMDATA
structure with the correct characteristics to copy hSDFdata, FALSE if it must be 
entirely initialized.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int SDFData2PGMData(HPGMDATA* phPGMdata, HSDFDATA hSDFdata, BOOL bOnlyCopyData);

/*
Convert a SDF file into a PGM file.

char* szSDFFilename : (IN) SDF file to convert.
char* szPGMFilename : (IN) PGM file where to save the picture.
int channel : (IN) Either PORT (left sonar image) or STARBOARD (right sonar image, default option).
USHORT Threshold : (IN) Each sample will be divided by Threshold (between 0 and 65535). 
If it is 0, each sample will be divided by the maximum sample value.
Note that 1000 is often a good value.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int SDF2PGM(char* szSDFFilename, char* szPGMFilename, int channel, USHORT Threshold);

/*
Convert a SonarPro image to PGM data.
Create and initialize a PGMDATA structure corresponding to a SDFDATA structure. 
The lines correspond to data got every TimeInterval to match GESMI needs 
(to correspond with the navigation data used with GESMI).
Use DestroyPGMData() to free phPGMdata at the end.
See the description of the PGMDATA structure for more information.

HPGMDATA* phPGMdata : (INOUT) Valid pointer that will receive an identifier of
the structure that will receive the PGM data corresponding to the SDF data.
HSDFDATA hSDFdata : (IN) Identifier of the SDFDATA to copy. 
double TimeInterval : (IN) Time between 2 data in GESMI (in s).
double SonarTimeLag : (IN) Time lag sensors time - sonar time (in s).
UINT NbData : (IN) Number of sensor data in GESMI.
BOOL bOnlyCopyData : (IN) TRUE if phPGMdata corresponds already to a PGMDATA
structure with the correct characteristics to copy hSDFdata, FALSE if it must be 
entirely initialized.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int SDFData2PGMDataForGESMI(HPGMDATA* phPGMdata, HSDFDATA hSDFdata, 
	double TimeInterval, double SonarTimeLag, UINT NbData, BOOL bOnlyCopyData);

/*
Convert a SDF file into a PGM file to be used with GESMI.

char* szSDFFilename : (IN) SDF file to convert.
char* szPGMFilename : (IN) PGM file where to save the picture.
double TimeInterval : (IN) Time between 2 data in GESMI (in s).
double SonarTimeLag : (IN) Time lag sensors time - sonar time (in s).
UINT NbData : (IN) Number of sensor data in GESMI.
int channel : (IN) Either PORT (left sonar image) or STARBOARD (right sonar image, default option).
USHORT Threshold : (IN) Each sample will be divided by Threshold (between 0 and 65535). 
If it is 0, each sample will be divided by the maximum sample value.
Note that 1000 is often a good value.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int SDF2PGMForGESMI(char* szSDFFilename, char* szPGMFilename, 
	double TimeInterval, double SonarTimeLag, UINT NbData, int channel, USHORT Threshold);

#endif // SDF2PGM_H
