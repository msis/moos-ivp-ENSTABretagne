/***************************************************************************************************************:')

SDF2PGM.c

Conversions from SDF SonarPro files to PGM picture files.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#include "SDF2PGM.h"

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
int SDFData2PGMData(HPGMDATA* phPGMdata, HSDFDATA hSDFdata, BOOL bOnlyCopyData)
{
	int i = 0; // For loop index for the Pings.
	int j = 0; // For loop index for the samples of a ping.
	double value = 0.0; // Temporary value of a sample.

	if ((phPGMdata == NULL)||(hSDFdata == INVALID_HSDFDATA_VALUE))
	{
		PRINT_DEBUG_ERROR_SDF2PGM(("SDFData2PGMData error (%s) : %s"
			"(phPGMdata=%#x, hSDFdata=%#x, bOnlyCopyData=%d)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			phPGMdata, hSDFdata, (int)bOnlyCopyData));
		return EXIT_FAILURE;
	}

	if (bOnlyCopyData)
	{
		if (
			(*phPGMdata == INVALID_HPGMDATA_VALUE) ||
			((*phPGMdata)->Img == NULL) ||
			((*phPGMdata)->Width != hSDFdata->NumSamples) ||
			((*phPGMdata)->Height != hSDFdata->NumPings) ||
			((*phPGMdata)->NbPixels != (*phPGMdata)->Width*(*phPGMdata)->Height) ||
			((*phPGMdata)->NbElements != (*phPGMdata)->NbPixels) ||
			((*phPGMdata)->Size != (*phPGMdata)->NbElements*sizeof(UCHAR)) ||
			((*phPGMdata)->ColorDepth != 255)
			)
		{
			PRINT_DEBUG_ERROR_SDF2PGM(("SDFData2PGMData error (%s) : %s"
				"(phPGMdata=%#x, hSDFdata=%#x, bOnlyCopyData=%d)\n", 
				strtime_m(), 
				"Invalid argument. ", 
				phPGMdata, hSDFdata, (int)bOnlyCopyData));
			return EXIT_FAILURE;
		}
	}
	else
	{
		*phPGMdata = (HPGMDATA)calloc(1, sizeof(struct PGMDATA));

		if (*phPGMdata == NULL)
		{
			PRINT_DEBUG_ERROR_SDF2PGM(("SDFData2PGMData error (%s) : %s"
				"(phPGMdata=%#x, hSDFdata=%#x, bOnlyCopyData=%d)\n", 
				strtime_m(), 
				"Out of memory. ", 
				phPGMdata, hSDFdata, (int)bOnlyCopyData));
			return EXIT_FAILURE;
		}

		(*phPGMdata)->Width = hSDFdata->NumSamples;
		(*phPGMdata)->Height = hSDFdata->NumPings;
		(*phPGMdata)->NbPixels = (*phPGMdata)->Width*(*phPGMdata)->Height;
		(*phPGMdata)->NbElements = (*phPGMdata)->NbPixels;
		(*phPGMdata)->Size = (*phPGMdata)->NbElements*sizeof(UCHAR);
		(*phPGMdata)->ColorDepth = 255;

		(*phPGMdata)->Img = (UCHAR*)calloc((*phPGMdata)->NbElements, sizeof(UCHAR));

		if ((*phPGMdata)->Img == NULL)
		{
			PRINT_DEBUG_ERROR_SDF2PGM(("SDFData2PGMData error (%s) : %s"
				"(phPGMdata=%#x, hSDFdata=%#x, bOnlyCopyData=%d)\n", 
				strtime_m(), 
				"Out of memory. ", 
				phPGMdata, hSDFdata, (int)bOnlyCopyData));
			free(*phPGMdata);*phPGMdata = INVALID_HPGMDATA_VALUE;
			return EXIT_FAILURE;
		}
	}

	for (i = hSDFdata->NumPings-1; i >= 0; i--)
	{
		for (j = hSDFdata->NumSamples-1; j >= 0; j--)
		{
			// Conversion of a sample value from a range [0..2^16-1] to [0..2^8-1]
			// ((unsigned char)(-1) = 2^8-1 = 255 and (unsigned short)(-1) = 2^16-1 = 65535).
			value = hSDFdata->Pings[i*hSDFdata->NumSamples+j]*255.0/65535.0;

			// The image is also flipped vertically.
			(*phPGMdata)->Img[((*phPGMdata)->Height-1-i)*(*phPGMdata)->Width+j] = (UCHAR)value;
		}
	}

	return EXIT_SUCCESS;
}

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
int SDF2PGM(char* szSDFFilename, char* szPGMFilename, int channel, USHORT Threshold)
{
	HSDFDATA hSDFdata = INVALID_HSDFDATA_VALUE;
	HPGMDATA hPGMData = INVALID_HPGMDATA_VALUE;

	if (CreateSDFDataFromFile(&hSDFdata, szSDFFilename, channel) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	ContrastSDFData(hSDFdata, Threshold);

	if (SDFData2PGMData(&hPGMData, hSDFdata, FALSE) != EXIT_SUCCESS)
	{
		DestroySDFData(&hSDFdata);
		return EXIT_FAILURE;
	}

	if (SavePGMDataToFile(hPGMData, szPGMFilename) != EXIT_SUCCESS)
	{
		DestroySDFData(&hSDFdata);
		DestroyPGMData(&hPGMData);
		return EXIT_FAILURE;
	}

	DestroySDFData(&hSDFdata);
	DestroyPGMData(&hPGMData);

	return EXIT_SUCCESS;
}

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
int SDFData2PGMDataForGESMI(HPGMDATA* phPGMdata, HSDFDATA hSDFdata, 
	double TimeInterval, double SonarTimeLag, UINT NbData, BOOL bOnlyCopyData)
{
	int i = 0; // For loop index for the pings.
	int j = 0; // For loop index for the samples of a ping.
	double value = 0; // Temporary value of a sample.
	UINT index = 0;
	int FirstImgIndexWithPing = 0;
	int LastImgIndexWithPing = 0;
	double FirstPingSDFTime = 0;
	double LastPingSDFTime = 0;
	double CurrentPingSDFTime = 0;
	double PingTime = 0;
	BOOL cont = FALSE;

	if ((phPGMdata == NULL)||(hSDFdata == INVALID_HSDFDATA_VALUE))
	{
		PRINT_DEBUG_ERROR_SDF2PGM(("SDFData2PGMData error (%s) : %s"
			"(phPGMdata=%#x, hSDFdata=%#x, bOnlyCopyData=%d)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			phPGMdata, hSDFdata, (int)bOnlyCopyData));
		return EXIT_FAILURE;
	}

	if (bOnlyCopyData)
	{
		if (
			(*phPGMdata == INVALID_HPGMDATA_VALUE) ||
			((*phPGMdata)->Img == NULL) ||
			((*phPGMdata)->Width != hSDFdata->NumSamples) ||
			((*phPGMdata)->Height != NbData) ||
			((*phPGMdata)->NbPixels != (*phPGMdata)->Width*(*phPGMdata)->Height) ||
			((*phPGMdata)->NbElements != (*phPGMdata)->NbPixels) ||
			((*phPGMdata)->Size != (*phPGMdata)->NbElements*sizeof(UCHAR)) ||
			((*phPGMdata)->ColorDepth != 255)
			)
		{
			PRINT_DEBUG_ERROR_SDF2PGM(("SDFData2PGMData error (%s) : %s"
				"(phPGMdata=%#x, hSDFdata=%#x, bOnlyCopyData=%d)\n", 
				strtime_m(), 
				"Invalid argument. ", 
				phPGMdata, hSDFdata, (int)bOnlyCopyData));
			return EXIT_FAILURE;
		}
	}
	else
	{
		*phPGMdata = (HPGMDATA)calloc(1, sizeof(struct PGMDATA));

		if (*phPGMdata == NULL)
		{
			PRINT_DEBUG_ERROR_SDF2PGM(("SDFData2PGMData error (%s) : %s"
				"(phPGMdata=%#x, hSDFdata=%#x, bOnlyCopyData=%d)\n", 
				strtime_m(), 
				"Out of memory. ", 
				phPGMdata, hSDFdata, (int)bOnlyCopyData));
			return EXIT_FAILURE;
		}

		(*phPGMdata)->Width = hSDFdata->NumSamples;
		(*phPGMdata)->Height = NbData;
		(*phPGMdata)->NbPixels = (*phPGMdata)->Width*(*phPGMdata)->Height;
		(*phPGMdata)->NbElements = (*phPGMdata)->NbPixels;
		(*phPGMdata)->Size = (*phPGMdata)->NbElements*sizeof(UCHAR);
		(*phPGMdata)->ColorDepth = 255;

		(*phPGMdata)->Img = (UCHAR*)calloc((*phPGMdata)->NbElements, sizeof(UCHAR));

		if ((*phPGMdata)->Img == NULL)
		{
			PRINT_DEBUG_ERROR_SDF2PGM(("SDFData2PGMData error (%s) : %s"
				"(phPGMdata=%#x, hSDFdata=%#x, bOnlyCopyData=%d)\n", 
				strtime_m(), 
				"Out of memory. ", 
				phPGMdata, hSDFdata, (int)bOnlyCopyData));
			free(*phPGMdata);*phPGMdata = INVALID_HPGMDATA_VALUE;
			return EXIT_FAILURE;
		}
	}

	FirstPingSDFTime = GetSDFPingTime(hSDFdata, 0);
	LastPingSDFTime = GetSDFPingTime(hSDFdata, hSDFdata->NumPings-1);

	// First index that will have ping data in the PGM picture.
	FirstImgIndexWithPing = (int)max((FirstPingSDFTime+SonarTimeLag)/TimeInterval, 0.0);

	// Last index that will have ping data in the PGM picture.
	LastImgIndexWithPing = (int)min(
		(LastPingSDFTime+SonarTimeLag)/TimeInterval, 
		(*phPGMdata)->Height-1.0);

	// If the sonar started after the other sensors, there are no sonar data
	// at the beginning. Otherwise, the first pings are ignored.
	// The first pings correspond to the bottom of the PGM image.

	// If the sonar stopped before the other sensors, there are no sonar data
	// at the end. Otherwise, the last pings are ignored.

	index = 0;
	CurrentPingSDFTime = GetSDFPingTime(hSDFdata, index);

	for (i = FirstImgIndexWithPing; i <= LastImgIndexWithPing; i++)
	{
		for (;;)
		{
			cont = FALSE;
			PingTime = CurrentPingSDFTime + SonarTimeLag;

			// CurrentPingSDFTime == 0 means that there is no sonar data for this time.
			if (
				(CurrentPingSDFTime == 0)||
				(i*TimeInterval > PingTime)
				)
			{
				// Go to next ping in the sonar data.
				index++;
				if (index >= hSDFdata->NumPings)
				{
					return EXIT_SUCCESS;
				}
				CurrentPingSDFTime = GetSDFPingTime(hSDFdata, index);
				continue;
			}

			if (
				(i*TimeInterval <= PingTime)&&
				(PingTime <= (i+1)*TimeInterval)
				)
			{
				// CurrentPingSDFTime and index are right.
				break;
			}

			if (PingTime > (i+1)*TimeInterval)
			{
				// Go to next navigation data time.
				cont = TRUE;
				break;
			}
		}

		if (cont)
		{
			// Go to next navigation data time.
			continue;
		}

		// At this point, CurrentPingSDFTime and index should be right.
		for (j = (*phPGMdata)->Width - 1; j >= 0; j--)
		{
			// Conversion of a sample value from a range [0..2^16-1] to [0..2^8-1]
			// ((unsigned char)(-1) = 2^8-1 = 255 and (unsigned short)(-1) = 2^16-1 = 65535).
			value = hSDFdata->Pings[index*hSDFdata->NumSamples+j]*255.0/65535.0;

			// The image is also flipped vertically.
			(*phPGMdata)->Img[((*phPGMdata)->Height-1-i)*(*phPGMdata)->Width+j] = (UCHAR)value;
		}
	}

	return EXIT_SUCCESS;
}

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
int SDF2PGMForGESMI(char* szSDFFilename, char* szPGMFilename, 
	double TimeInterval, double SonarTimeLag, UINT NbData, int channel, USHORT Threshold)
{
	HSDFDATA hSDFdata = INVALID_HSDFDATA_VALUE;
	HPGMDATA hPGMData = INVALID_HPGMDATA_VALUE;

	if (CreateSDFDataFromFile(&hSDFdata, szSDFFilename, channel) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	ContrastSDFData(hSDFdata, Threshold);

	if (SDFData2PGMDataForGESMI(&hPGMData, hSDFdata, TimeInterval, SonarTimeLag, NbData, FALSE) != EXIT_SUCCESS)
	{
		DestroySDFData(&hSDFdata);
		return EXIT_FAILURE;
	}

	if (SavePGMDataToFile(hPGMData, szPGMFilename) != EXIT_SUCCESS)
	{
		DestroySDFData(&hSDFdata);
		DestroyPGMData(&hPGMData);
		return EXIT_FAILURE;
	}

	DestroySDFData(&hSDFdata);
	DestroyPGMData(&hPGMData);

	return EXIT_SUCCESS;
}
