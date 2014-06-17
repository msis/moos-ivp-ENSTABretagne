/***************************************************************************************************************:')

SDFFiles.c

SDF SonarPro files and data handling.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#include "SDFFiles.h"

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
int CreateSDFDataFromFile(HSDFDATA* phData, char* filename, int channel)
{
	FILE* fp = NULL;
	size_t countRead = 0; // Number of elements read or written.
	UINT PingMarker = 0; // 0xFFFFFFFF before each ping header in the file.
	UINT NumPings = 0; // Number of Pings in Pings.
	USHORT NumSamples = 0; // Number of samples per Pings.
	size_t bytesLeft = 0; // Size of unused information after each ping data.
	UINT n = 0; // For loop index for the sonar channels.

	if (phData == NULL)
	{
		PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s, channel=%d)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			phData, filename, channel));
		return EXIT_FAILURE;
	}

	fp = fopen(filename, "rb");

	if (fp == NULL)
	{
		PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s, channel=%d)\n", 
			strtime_m(), 
			"Error opening the file. ", 
			phData, filename, channel));
		return EXIT_FAILURE;
	}

	*phData = (HSDFDATA)calloc(1, sizeof(struct SDFDATA));

	if (*phData == NULL)
	{
		PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s, channel=%d)\n", 
			strtime_m(), 
			"Out of memory. ", 
			phData, filename, channel));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	(*phData)->Headers = NULL;
	(*phData)->Pings = NULL;


	// The SonarPro generated Sonar Data Files (SDF) have a .sdf file extension.
	// These files are in the form
	// [data page][data page] ... etc 
	// where each data page is the ping marker followed by the SDF data page.
	// The ping marker is a 32-bit value that never changes and is
	// equal to 0xFFFFFFFF (2^32-1).


	// This loop is for each ping found in the file.
	for (;;)
	{
		// Get the ping marker.
		countRead = fread(&PingMarker, sizeof(UINT), (size_t)1, fp);

		if (countRead != (size_t)1)
		{
			if (feof(fp) != 0)
			{
				break;
			}
			PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
				"(phData=%#x, filename=%s, channel=%d)\n", 
				strtime_m(), 
				"Error reading the file. ", 
				phData, filename, channel));
			free((*phData)->Pings);(*phData)->Pings = NULL;
			free((*phData)->Headers);(*phData)->Headers = NULL;
			free(*phData);*phData = INVALID_HSDFDATA_VALUE;
			fclose(fp);fp = NULL;
			return EXIT_FAILURE;
		}

		if (PingMarker != 0xffffffff)
		{
			PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
				"(phData=%#x, filename=%s, channel=%d)\n", 
				strtime_m(), 
				"Invalid ping marker. ", 
				phData, filename, channel));
			free((*phData)->Pings);(*phData)->Pings = NULL;
			free((*phData)->Headers);(*phData)->Headers = NULL;
			free(*phData);*phData = INVALID_HSDFDATA_VALUE;
			fclose(fp);fp = NULL;
			return EXIT_FAILURE;
		}

		// Because the ping marker should be correct at this point,
		// increment the number of pings for the current file.
		NumPings++;

		(*phData)->Headers = (SYS5000HEADER*)realloc((*phData)->Headers, (size_t)NumPings*sizeof(SYS5000HEADER));

		if ((*phData)->Headers == NULL)
		{
			PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
				"(phData=%#x, filename=%s, channel=%d)\n", 
				strtime_m(), 
				"Out of memory. ", 
				phData, filename, channel));
			free((*phData)->Pings);(*phData)->Pings = NULL;
			free(*phData);*phData = INVALID_HSDFDATA_VALUE;
			fclose(fp);fp = NULL;
			return EXIT_FAILURE;
		}

		// Get the ping header.
		countRead = fread(&(*phData)->Headers[NumPings-1], sizeof(SYS5000HEADER), (size_t)1, fp);

		if (countRead != (size_t)1)
		{
			if (feof(fp) != 0)
			{
				break;
			}
			PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
				"(phData=%#x, filename=%s, channel=%d)\n", 
				strtime_m(), 
				"Error reading the file. ", 
				phData, filename, channel));
			free((*phData)->Pings);(*phData)->Pings = NULL;
			free((*phData)->Headers);(*phData)->Headers = NULL;
			free(*phData);*phData = INVALID_HSDFDATA_VALUE;
			fclose(fp);fp = NULL;
			return EXIT_FAILURE;
		}

		// We only handle this type of file
		if ((*phData)->Headers[NumPings-1].pageVersion != 5000)
		{
			PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
				"(phData=%#x, filename=%s, channel=%d)\n", 
				strtime_m(), 
				"Unsupported SDF file format. ", 
				phData, filename, channel));
			free((*phData)->Pings);(*phData)->Pings = NULL;
			free((*phData)->Headers);(*phData)->Headers = NULL;
			free(*phData);*phData = INVALID_HSDFDATA_VALUE;
			fclose(fp);fp = NULL;
			return EXIT_FAILURE;
		}

		// After the ping header (256 bytes) and the ping data (unknown size at this point)
		// there is additional uneeded information. bytesLeft will be its size.
		bytesLeft = (*phData)->Headers[NumPings-1].numberBytes-sizeof(SYS5000HEADER);

		// The sonar has 10 channels, 5 on port and 5 on starboard.
		for (n = 0; n < 10; n++)
		{
			// For each channel the first word of 2 bytes is the number of samples for the channel.
			countRead = fread(&NumSamples, sizeof(USHORT), (size_t)1, fp);

			if (countRead != (size_t)1)
			{
				if (feof(fp) != 0)
				{
					break;
				}
				PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
					"(phData=%#x, filename=%s, channel=%d)\n", 
					strtime_m(), 
					"Error reading the file. ", 
					phData, filename, channel));
				free((*phData)->Pings);(*phData)->Pings = NULL;
				free((*phData)->Headers);(*phData)->Headers = NULL;
				free(*phData);*phData = INVALID_HSDFDATA_VALUE;
				fclose(fp);fp = NULL;
				return EXIT_FAILURE;
			}

			bytesLeft -= sizeof(USHORT);

			if (NumSamples != 0)
			{
				// n < 5 : channels on port (left).
				// n >= 5 : channels on starboard (right).
				// The 10 first bits of header.beamsToDisplay indicates which channels should be read.
				if (
					((1 << n) & (*phData)->Headers[NumPings-1].beamsToDisplay)&& 
					((UINT)NumSamples == (*phData)->Headers[NumPings-1].numSamples)&& 
					(((n >= 5)&&(channel != PORT))||/*????????????????????????????????????*/
					((n < 5)&&(channel == PORT)))/*????????????????????????????????????*/
					//(((n == 7)&&(channel != PORT))||
					//((n == 2)&&(channel == PORT)))
					)
				{
					// This is the ping data. A ping has NumSamples samples and a sample is an USHORT.
					(*phData)->Pings = (USHORT*)realloc((*phData)->Pings, NumPings*NumSamples*sizeof(USHORT));

					if ((*phData)->Pings == NULL)
					{
						PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
							"(phData=%#x, filename=%s, channel=%d)\n", 
							strtime_m(), 
							"Out of memory. ", 
							phData, filename, channel));
						free((*phData)->Headers);(*phData)->Headers = NULL;
						free(*phData);*phData = INVALID_HSDFDATA_VALUE;
						fclose(fp);fp = NULL;
						return EXIT_FAILURE;
					}

					countRead = fread(&(*phData)->Pings[(NumPings-1)*NumSamples], sizeof(USHORT), (size_t)NumSamples, fp);

					if (countRead != (size_t)NumSamples)
					{
						if (feof(fp) != 0)
						{
							break;
						}
						PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
							"(phData=%#x, filename=%s, channel=%d)\n", 
							strtime_m(), 
							"Error reading the file. ", 
							phData, filename, channel));
						free((*phData)->Pings);(*phData)->Pings = NULL;
						free((*phData)->Headers);(*phData)->Headers = NULL;
						free(*phData);*phData = INVALID_HSDFDATA_VALUE;
						fclose(fp);fp = NULL;
						return EXIT_FAILURE;
					}
				}
				else
				{
					// The current channel is not read.
					if (fseek(fp, NumSamples*sizeof(USHORT), SEEK_CUR) != 0)
					{
						if (feof(fp) != 0)
						{
							break;
						}
						PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
							"(phData=%#x, filename=%s, channel=%d)\n", 
							strtime_m(), 
							"Error reading the file. ", 
							phData, filename, channel));
						free((*phData)->Pings);(*phData)->Pings = NULL;
						free((*phData)->Headers);(*phData)->Headers = NULL;
						free(*phData);*phData = INVALID_HSDFDATA_VALUE;
						fclose(fp);fp = NULL;
						return EXIT_FAILURE;
					}
				}

				bytesLeft -= (size_t)NumSamples*sizeof(USHORT);
			}
		}

		// The additional data after the channels data are not read.
		if (fseek(fp, (long)bytesLeft, SEEK_CUR) != 0)
		{
			if (feof(fp) != 0)
			{
				break;
			}
			PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
				"(phData=%#x, filename=%s, channel=%d)\n", 
				strtime_m(), 
				"Error reading the file. ", 
				phData, filename, channel));
			free((*phData)->Pings);(*phData)->Pings = NULL;
			free((*phData)->Headers);(*phData)->Headers = NULL;
			free(*phData);*phData = INVALID_HSDFDATA_VALUE;
			fclose(fp);fp = NULL;
			return EXIT_FAILURE;
		}
	}

	// The number of Pings in the file and the number of samples for each ping is now known.
	// The header data for the ith ping will be in Headers[i].
	// The jth sample value at the ith ping will be in Pings[i*NumSamples+j].
	(*phData)->NumPings = NumPings;
	(*phData)->NumSamples = NumSamples;
	(*phData)->NbElements = NumPings*NumSamples;
	(*phData)->Size = (*phData)->NbElements*sizeof(USHORT);

	if (fclose(fp) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_SDFFILES(("CreateSDFDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s, channel=%d)\n", 
			strtime_m(), 
			"Error closing the file. ", 
			phData, filename, channel));
		free((*phData)->Pings);(*phData)->Pings = NULL;
		free((*phData)->Headers);(*phData)->Headers = NULL;
		free(*phData);*phData = INVALID_HSDFDATA_VALUE;
		return EXIT_FAILURE;
	}
	fp = NULL;

	return EXIT_SUCCESS;
}

/*
Get the first ping header in a SDF file. 

SYS5000HEADER* pHeader : (INOUT) Pointer to the first ping header.
char* filename : (IN) Path to the file to read.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int ReadFirstSDFHeader(SYS5000HEADER* pHeader, char* filename)
{
	FILE* fp = NULL;
	size_t countRead = 0; // Number of elements read or written.
	UINT PingMarker = 0; // 0xFFFFFFFF before each ping header in the file.

	if (pHeader == NULL)
	{
		PRINT_DEBUG_ERROR_SDFFILES(("ReadFirstSDFHeader error (%s) : %s"
			"(pHeader=%#x, filename=%s)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			pHeader, filename));
		return EXIT_FAILURE;
	}

	fp = fopen(filename, "rb");

	if (fp == NULL)
	{
		PRINT_DEBUG_ERROR_SDFFILES(("ReadFirstSDFHeader error (%s) : %s"
			"(pHeader=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error opening the file. ", 
			pHeader, filename));
		return EXIT_FAILURE;
	}

	// Get the ping marker.
	countRead = fread(&PingMarker, sizeof(UINT), (size_t)1, fp);

	if (countRead != (size_t)1)
	{
		PRINT_DEBUG_ERROR_SDFFILES(("ReadFirstSDFHeader error (%s) : %s"
			"(pHeader=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error reading the file. ", 
			pHeader, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (PingMarker != 0xffffffff)
	{
		PRINT_DEBUG_ERROR_SDFFILES(("ReadFirstSDFHeader error (%s) : %s"
			"(pHeader=%#x, filename=%s)\n", 
			strtime_m(), 
			"Invalid ping marker. ", 
			pHeader, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	// Gets the ping header.
	countRead = fread(pHeader, sizeof(SYS5000HEADER), (size_t)1, fp);

	if (countRead != (size_t)1)
	{
		PRINT_DEBUG_ERROR_SDFFILES(("ReadFirstSDFHeader error (%s) : %s"
			"(pHeader=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error reading the file. ", 
			pHeader, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	// We only handle this type of file
	if (pHeader->pageVersion != 5000)
	{
		PRINT_DEBUG_ERROR_SDFFILES(("ReadFirstSDFHeader error (%s) : %s"
			"(pHeader=%#x, filename=%s)\n", 
			strtime_m(), 
			"Unsupported SDF file format. ", 
			pHeader, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (fclose(fp) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_SDFFILES(("ReadFirstSDFHeader error (%s) : %s"
			"(pHeader=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error closing the file. ", 
			pHeader, filename));
		return EXIT_FAILURE;
	}
	fp = NULL;

	return EXIT_SUCCESS;
}
