/***************************************************************************************************************:')

PGMFiles.c

PGM picture files handling.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#include "PGMFiles.h"

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
int CreatePGMDataFromFile(HPGMDATA* phData, char* filename)
{
	FILE* fp = NULL;
	char info[70];
	int width = 0, height = 0, colorDepth = 0;

	if (phData == NULL)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CreatePGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			phData, filename));
		return EXIT_FAILURE;
	}

	fp = fopen(filename, "rb");

	if (fp == NULL)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CreatePGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error opening the file. ", 
			phData, filename));
		return EXIT_FAILURE;
	}

	// Example of PGM file header : 
	// P5
	// # Comments...
	// 640 480
	// 255
	// Binary data...

	if (fgets(info, 70, fp) == NULL)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CreatePGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error reading the file. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (strncmp(info, "P5", 2) != 0)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CreatePGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Invalid file type. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (fgets(info, 70, fp) == NULL)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CreatePGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error reading the file. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (strncmp(info, "#", 1) == 0)
	{
		if (fgets(info, 70, fp) == NULL)
		{
			PRINT_DEBUG_ERROR_PGMFILES(("CreatePGMDataFromFile error (%s) : %s"
				"(phData=%#x, filename=%s)\n", 
				strtime_m(), 
				"Error reading the file. ", 
				phData, filename));
			fclose(fp);fp = NULL;
			return EXIT_FAILURE;
		}
	}

	sscanf(info, "%d %d", &width, &height);

	if ((width == 0)||(height == 0))
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CreatePGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Invalid width or height. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (fgets(info, 70, fp) == NULL)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CreatePGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error reading the file. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	sscanf(info, "%d", &colorDepth);

	if (colorDepth != 255)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CreatePGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Unsupported color depth. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	*phData = (HPGMDATA)calloc(1, sizeof(struct PGMDATA));

	if (*phData == NULL)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CreatePGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	(*phData)->Width = width;
	(*phData)->Height = height;
	(*phData)->NbPixels = width*height;
	(*phData)->NbElements = (*phData)->NbPixels;
	(*phData)->Size = (*phData)->NbElements*sizeof(UCHAR);
	(*phData)->ColorDepth = colorDepth;

	(*phData)->Img = (UCHAR*)calloc((*phData)->NbElements, sizeof(UCHAR));

	if ((*phData)->Img == NULL)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CreatePGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			phData, filename));
		free(*phData);*phData = INVALID_HPGMDATA_VALUE;
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (fread((*phData)->Img, sizeof(UCHAR), (*phData)->NbElements, fp) != (*phData)->NbElements)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CreatePGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error reading the file. ", 
			phData, filename));
		free((*phData)->Img);(*phData)->Img = NULL;
		free(*phData);*phData = INVALID_HPGMDATA_VALUE;
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (fclose(fp) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CreatePGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error closing the file. ", 
			phData, filename));
		free((*phData)->Img);(*phData)->Img = NULL;
		free(*phData);*phData = INVALID_HPGMDATA_VALUE;
		return EXIT_FAILURE;
	}
	fp = NULL;

	return EXIT_SUCCESS;
}

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
int CopyPGMDataFromFile(HPGMDATA* phData, char* filename)
{
	FILE* fp = NULL;
	char info[70];
	int width = 0, height = 0, colorDepth = 0;

	if (phData == NULL)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CopyPGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			phData, filename));
		return EXIT_FAILURE;
	}

	fp = fopen(filename, "rb");

	if (fp == NULL)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CopyPGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error opening the file. ", 
			phData, filename));
		return EXIT_FAILURE;
	}

	// Example of PGM file header : 
	// P5
	// # Comments...
	// 640 480
	// 255
	// Binary data...

	if (fgets(info, 70, fp) == NULL)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CopyPGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error reading the file. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (strncmp(info, "P5", 2) != 0)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CopyPGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Invalid file type. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (fgets(info, 70, fp) == NULL)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CopyPGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error reading the file. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (strncmp(info, "#", 1) == 0)
	{
		if (fgets(info, 70, fp) == NULL)
		{
			PRINT_DEBUG_ERROR_PGMFILES(("CopyPGMDataFromFile error (%s) : %s"
				"(phData=%#x, filename=%s)\n", 
				strtime_m(), 
				"Error reading the file. ", 
				phData, filename));
			fclose(fp);fp = NULL;
			return EXIT_FAILURE;
		}
	}

	sscanf(info, "%d %d", &width, &height);

	if ((width == 0)||(height == 0))
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CopyPGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Invalid width or height. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (fgets(info, 70, fp) == NULL)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CopyPGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error reading the file. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	sscanf(info, "%d", &colorDepth);

	if (colorDepth != 255)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CopyPGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Unsupported color depth. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (
		(*phData == INVALID_HPGMDATA_VALUE) ||
		((*phData)->Img == NULL) ||
		((*phData)->Width != (UINT)width) ||
		((*phData)->Height != (UINT)height) ||
		((*phData)->NbElements != (UINT)(3*width*height)) ||
		((*phData)->ColorDepth != (UINT)colorDepth)
		)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CopyPGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (fread((*phData)->Img, sizeof(UCHAR), (*phData)->NbElements, fp) != (*phData)->NbElements)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CopyPGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error reading the file. ", 
			phData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (fclose(fp) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("CopyPGMDataFromFile error (%s) : %s"
			"(phData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error closing the file. ", 
			phData, filename));
		return EXIT_FAILURE;
	}
	fp = NULL;

	return EXIT_SUCCESS;
}

/*
Save PGM data to a PGM file. 
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (IN) Identifier of the PGMDATA to save.
char* filename : (IN) Path to the file to write.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int SavePGMDataToFile(HPGMDATA hData, char* filename)
{
	FILE* fp = NULL;

	if (hData == INVALID_HPGMDATA_VALUE)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("SavePGMDataToFile error (%s) : %s"
			"(hData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			hData, filename));
		return EXIT_FAILURE;
	}

	fp = fopen(filename, "wb");

	if (fp == NULL)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("SavePGMDataToFile error (%s) : %s"
			"(hData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error opening the file. ", 
			hData, filename));
		return EXIT_FAILURE;
	}

	// Example of PGM file header : 
	// P5
	// # Comments...
	// 640 480
	// 255
	// Binary data...

	if (fprintf(fp, "P5\x0a%d %d\x0a%d\x0a", hData->Width, hData->Height, hData->ColorDepth) < 0)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("SavePGMDataToFile error (%s) : %s"
			"(hData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error writing to the file. ", 
			hData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (fwrite(hData->Img, sizeof(UCHAR), hData->NbElements, fp) != hData->NbElements)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("SavePGMDataToFile error (%s) : %s"
			"(hData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error writing to the file. ", 
			hData, filename));
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (fclose(fp) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_PGMFILES(("SavePGMDataToFile error (%s) : %s"
			"(hData=%#x, filename=%s)\n", 
			strtime_m(), 
			"Error closing the file. ", 
			hData, filename));
		return EXIT_FAILURE;
	}
	fp = NULL;

	return EXIT_SUCCESS;
}
