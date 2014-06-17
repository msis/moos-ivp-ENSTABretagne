/***************************************************************************************************************:')

SDFCore.h

SDF SonarPro files and data handling.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef SDFCORE_H
#define SDFCORE_H

#include "OSTime.h"

/*
Debug macros specific to SDFUtils.
*/
#ifdef _DEBUG_MESSAGES
#	define _DEBUG_MESSAGES_SDFUTILS
#endif // _DEBUG_MESSAGES

#ifdef _DEBUG_WARNINGS
#	define _DEBUG_WARNINGS_SDFUTILS
#endif // _DEBUG_WARNINGS

#ifdef _DEBUG_ERRORS
#	define _DEBUG_ERRORS_SDFUTILS
#endif // _DEBUG_ERRORS

#ifdef _DEBUG_MESSAGES_SDFUTILS
#	define PRINT_DEBUG_MESSAGE_SDFUTILS(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_SDFUTILS(params)
#endif // _DEBUG_MESSAGES_SDFUTILS

#ifdef _DEBUG_WARNINGS_SDFUTILS
#	define PRINT_DEBUG_WARNING_SDFUTILS(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_SDFUTILS(params)
#endif // _DEBUG_WARNINGS_SDFUTILS

#ifdef _DEBUG_ERRORS_SDFUTILS
#	define PRINT_DEBUG_ERROR_SDFUTILS(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_SDFUTILS(params)
#endif // _DEBUG_ERRORS_SDFUTILS

/*
Debug macros specific to SDFCore.
*/
#ifdef _DEBUG_MESSAGES_SDFUTILS
#	define _DEBUG_MESSAGES_SDFCORE
#endif // _DEBUG_MESSAGES_SDFUTILS

#ifdef _DEBUG_WARNINGS_SDFUTILS
#	define _DEBUG_WARNINGS_SDFCORE
#endif // _DEBUG_WARNINGS_SDFUTILS

#ifdef _DEBUG_ERRORS_SDFUTILS
#	define _DEBUG_ERRORS_SDFCORE
#endif // _DEBUG_ERRORS_SDFUTILS

#ifdef _DEBUG_MESSAGES_SDFCORE
#	define PRINT_DEBUG_MESSAGE_SDFCORE(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_SDFCORE(params)
#endif // _DEBUG_MESSAGES_SDFCORE

#ifdef _DEBUG_WARNINGS_SDFCORE
#	define PRINT_DEBUG_WARNING_SDFCORE(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_SDFCORE(params)
#endif // _DEBUG_WARNINGS_SDFCORE

#ifdef _DEBUG_ERRORS_SDFCORE
#	define PRINT_DEBUG_ERROR_SDFCORE(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_SDFCORE(params)
#endif // _DEBUG_ERRORS_SDFCORE

#define U8	unsigned char
#define U16	unsigned short
#define U32	unsigned int
#define F32	float

/*
Structure corresponding to the header data contained in a SDF file.
*/
struct _SYS5000HEADER
{
	U32 numberBytes;		// Total number of bytes in page
	U32 pageVersion;
	U32 configuration;
	U32 pingNumber;
	U32 numSamples;
	U32 beamsToDisplay;
	U32 errorFlags;

	U32 range;
	U32 speedFish;		
	U32 speedSound;
	U32 resMode;
	U32 txWaveform;
	U32 respDiv;
	U32 respFreq;
	U32 manualSpeedSwitch;
	U32 despeckleSwitch;
	U32 speedFilterSwitch;

	U32 year;
	U32 month;
	U32 day;
	U32 hour;
	U32 minute;
	U32 second;
	U32 hSecond;
	U32 fixTimeHour;
	U32 fixTimeMinute;
	float fixTimeSecond;

	float heading;		// heading from towfish compass
	float pitch;		// pitch from compass
	float roll;			// roll from compass
	float depth;		// from towfish
	float altitude;		// from towfish
	float temperature;	// from towfish
	float speed;		// from GPS, updated on GPS update,m/s
	float shipHeading;	// from GPS
	float magneticVariation;// from GPS

	double shipLat;		// from GPS, radians
	double shipLon;		// from GPS, radians
	double fishLat;		// rads
	double fishLon;		// rads

	/* Added at version 3 */

	U32 tvgPage;
	U32 headerSize;		// number of bytes in header

	U32 fixTimeYear;
	U32 fixTimeMonth;
	U32 fixTimeDay;

	float auxPitch;		// aux data from AUV or other sensors
	float auxRoll;
	float auxDepth;
	float auxAlt;

	float cableOut;

	float fseconds;		// fractional seconds 

	U32 altimeter;		// altimeter off|on 

	U32 sampleFreq;

	U32 depressorType;
	U32 cableType;
	F32 shieveXoff;
	F32 shieveYoff;
	F32 shieveZoff;
	F32 GPSheight;

	U32 rawDataConfig;   // upper word = stbd config, lower = port
};
typedef struct _SYS5000HEADER SYS5000HEADER;

/*
Structure corresponding to the data contained in a SDF SonarPro file.
*/
struct SDFDATA
{
	SYS5000HEADER* Headers; // Information about the pings data. See the 
	// description of the SYS5000HEADER structure for more information.
	USHORT* Pings; // Table of NumPings sonar pings. The value of the
	// sample j for the ping i should be in Pings[j+Headers[i].numSamples*i].
	UINT NumPings; // Number of pings in Pings.
	USHORT NumSamples; // Number of samples per ping.
	UINT Size; // Size of Pings (in bytes).
	UINT NbElements; // Number of elements (USHORT values) in Pings.
};
typedef struct SDFDATA* HSDFDATA;

#define INVALID_HSDFDATA_VALUE NULL

/*
Create and initialize a structure that will contain sonar data corresponding
to the SDF format. Use DestroySDFData() to free the sonar data at the end.
See the description of the SDFDATA structure for more information.

HSDFDATA* phData : (INOUT) Valid pointer that will receive an identifier of
the structure.
UINT NumPings : (IN) Number of pings.
USHORT NumSamples : (IN) Number of samples per ping.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int CreateSDFData(HSDFDATA* phData, UINT NumPings, USHORT NumSamples);

/*
Create and initialize a structure that is a copy of a SDFDATA structure. 
Use DestroySDFData() to free phDataDest at the end.
See the description of the SDFDATA structure for more information.

HSDFDATA* phDataDest : (INOUT) Valid pointer that will receive an identifier of
the structure that will receive the copy of the structure identified by hDataSrc.
HSDFDATA hDataSrc : (IN) Identifier of the SDFDATA to copy. 
BOOL bOnlyCopyData : (IN) TRUE if phDataDest corresponds already to a SDFDATA
structure with the same characteristics as hDataSrc, FALSE if it must be 
entirely initialized.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int CopySDFData(HSDFDATA* phDataDest, HSDFDATA hDataSrc, BOOL bOnlyCopyData);

/*
Create and initialize a structure that is a concatenation of SDF ping data
depending on the ping number (for example to group several files in one file). 
Unavailable ping data are allowed but duplicate are not 
(i.e. file 1 can be from ping number 232 to 436 and file 2 from 567 to 789, 
but you cannot have file 1 from 232 to 567 and file 2 from 436 to 789). 
Use DestroySDFData() to free phDataOut at the end.
See the description of the SDFDATA structure for more information.

HSDFDATA* phDataOut : (INOUT) Valid pointer that will receive an identifier of
the new structure.
HSDFDATA hDataIn1 : (IN) Identifier of the first structure to concatenate.
HSDFDATA hDataIn2 : (IN) Identifier of the second structure to concatenate.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int ConcatSDFData(HSDFDATA* phDataOut, HSDFDATA hDataIn1, HSDFDATA hDataIn2);

/*
Destroy a structure created by CreateSDFData() or CopySDFData() that 
contained sonar data corresponding to the SDF format. See the 
description of the SDFDATA structure for more information.

HSDFDATA* phData : (INOUT) Valid pointer to the identifier of the structure.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int DestroySDFData(HSDFDATA* phData);

/*
Get the value at ping i and sample j of a SDF sonar image. 
The function does not check any parameter.
See the description of the SDFDATA structure for more information. 

HSDFDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Ping number.
UINT j : (IN) Sample number.

Return : The value (between 0 and 65535).
*/
inline USHORT GetSDFPingsSampleQuick(HSDFDATA hData, UINT i, UINT j)
{
	return hData->Pings[j+hData->NumSamples*i];
}

/*
Get the value at ping i and sample j of a SDF sonar image. 
The function handles automatically any out of bounds index.
See the description of the SDFDATA structure for more information. 

HSDFDATA hData : (INOUT) Identifier of the structure.
int i : (IN) Ping number.
int j : (IN) Sample number.

Return : The value (between 0 and 65535).
*/
inline USHORT GetSDFPingsSample(HSDFDATA hData, int i, int j)
{
	//i = min(max(i, 0), (int)hData->NumPings-1);
	//j = min(max(j, 0), (int)hData->NumSamples-1);

	if ((i < 0)||(i >= (int)hData->NumPings)||(j < 0)||(j >= (int)hData->NumSamples))
	{
		return 0;
	}

	return hData->Pings[j+hData->NumSamples*i];
}

/*
Set the value at ping i and sample j of a SDF sonar image. 
The function does not check any parameter.
See the description of the SDFDATA structure for more information. 

HSDFDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Ping number.
UINT j : (IN) Sample number.
USHORT value : (IN) New sample value (between 0 and 65535).

Return : Nothing.
*/
inline void SetSDFPingsSampleQuick(HSDFDATA hData, UINT i, UINT j, USHORT value)
{
	hData->Pings[j+hData->NumSamples*i] = value;
}

/*
Set the value at ping i and sample j of a SDF sonar image. 
The function handles automatically any out of bounds index.
See the description of the SDFDATA structure for more information. 

HSDFDATA hData : (INOUT) Identifier of the structure.
int i : (IN) Ping number.
int j : (IN) Sample number.
USHORT value : (IN) New sample value (between 0 and 65535).

Return : Nothing.
*/
inline void SetSDFPingsSample(HSDFDATA hData, int i, int j, USHORT value)
{
	//i = min(max(i, 0), (int)hData->NumPings-1);
	//j = min(max(j, 0), (int)hData->NumSamples-1);

	if ((i < 0)||(i >= (int)hData->NumPings)||(j < 0)||(j >= (int)hData->NumSamples))
	{
		return;
	}

	hData->Pings[j+hData->NumSamples*i] = value;
}

inline double GetSDFPingTime(HSDFDATA hData, UINT index)
{
	// To be improved...
	return 	
		//(double)pSDFdata->headers[index].day*86400.0 + 
		(double)hData->Headers[index].hour*3600.0 + 
		(double)hData->Headers[index].minute*60.0 + 
		(double)hData->Headers[index].second + 
		(double)hData->Headers[index].hSecond/100.0;
}

// Getters and setters.

inline UINT GetSDFPingsNumPings(HSDFDATA hData)
{
	return hData->NumPings;
}

inline USHORT GetSDFPingsNumSamples(HSDFDATA hData)
{
	return hData->NumSamples;
}

inline UINT GetSDFPingsSize(HSDFDATA hData)
{
	return hData->Size;
}

inline UINT GetSDFPingsNbElements(HSDFDATA hData)
{
	return hData->NbElements;
}

inline USHORT* GetPtrToSDFPings(HSDFDATA hData)
{
	return hData->Pings;
}

inline SYS5000HEADER* GetPtrToSDFHeaders(HSDFDATA hData)
{
	return hData->Headers;
}

#endif // SDFCORE_H
