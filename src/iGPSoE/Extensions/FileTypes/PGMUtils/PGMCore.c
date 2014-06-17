/***************************************************************************************************************:')

PGMCore.c

PGM picture data handling.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#include "PGMCore.h"

/*
Create and initialize a structure that will contain picture data corresponding
to the PGM format. Use DestroyPGMData() to free the picture data at the end.
See the description of the PGMDATA structure for more information.

HPGMDATA* phData : (INOUT) Valid pointer that will receive an identifier of
the structure.
UINT width : (IN) Width of the picture (in pixels).
UINT height : (IN) Height of the picture (in pixels).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int CreatePGMData(HPGMDATA* phData, UINT width, UINT height)
{
	if ((phData == NULL)||(width == 0)||(height == 0))
	{
		PRINT_DEBUG_ERROR_PGMCORE(("CreatePGMData error (%s) : %s"
			"(phData=%#x, width=%u, height=%u)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			phData, width, height));
		return EXIT_FAILURE;
	}

	*phData = (HPGMDATA)calloc(1, sizeof(struct PGMDATA));

	if (*phData == NULL)
	{
		PRINT_DEBUG_ERROR_PGMCORE(("CreatePGMData error (%s) : %s"
			"(phData=%#x, width=%u, height=%u)\n", 
			strtime_m(), 
			"Out of memory. ", 
			phData, width, height));
		return EXIT_FAILURE;
	}

	(*phData)->Width = width;
	(*phData)->Height = height;
	(*phData)->NbPixels = width*height;
	(*phData)->NbElements = (*phData)->NbPixels;
	(*phData)->Size = (*phData)->NbElements*sizeof(UCHAR);
	(*phData)->ColorDepth = 255;

	(*phData)->Img = (UCHAR*)calloc((*phData)->NbElements, sizeof(UCHAR));

	if ((*phData)->Img == NULL)
	{
		PRINT_DEBUG_ERROR_PGMCORE(("CreatePGMData error (%s) : %s"
			"(phData=%#x, width=%u, height=%u)\n", 
			strtime_m(), 
			"Out of memory. ", 
			phData, width, height));
		free(*phData);*phData = INVALID_HPGMDATA_VALUE;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Create and initialize a structure that is a copy of a PGMDATA structure. 
Use DestroyPGMData() to free phDataDest at the end.
See the description of the PGMDATA structure for more information.

HPGMDATA* phDataDest : (INOUT) Valid pointer that will receive an identifier of
the structure that will receive the copy of the structure identified by hDataSrc.
HPGMDATA hDataSrc : (IN) Identifier of the PGMDATA to copy. 
BOOL bOnlyCopyData : (IN) TRUE if phDataDest corresponds already to a PGMDATA
structure with the same characteristics as hDataSrc, FALSE if it must be 
entirely initialized.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int CopyPGMData(HPGMDATA* phDataDest, HPGMDATA hDataSrc, BOOL bOnlyCopyData)
{
	if ((phDataDest == NULL)||(hDataSrc == INVALID_HPGMDATA_VALUE))
	{
		PRINT_DEBUG_ERROR_PGMCORE(("CopyPGMData error (%s) : %s"
			"(phDataDest=%#x, hDataSrc=%#x, bOnlyCopyData=%d)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			phDataDest, hDataSrc, (int)bOnlyCopyData));
		return EXIT_FAILURE;
	}

	if (bOnlyCopyData)
	{
		if (
			(*phDataDest == INVALID_HPGMDATA_VALUE) ||
			((*phDataDest)->Img == NULL) ||
			((*phDataDest)->Width != hDataSrc->Width) ||
			((*phDataDest)->Height != hDataSrc->Height) ||
			((*phDataDest)->NbPixels != hDataSrc->NbPixels) ||
			((*phDataDest)->NbElements != hDataSrc->NbElements) ||
			((*phDataDest)->Size != hDataSrc->Size) ||
			((*phDataDest)->ColorDepth != hDataSrc->ColorDepth)
			)
		{
			PRINT_DEBUG_ERROR_PGMCORE(("CopyPGMData error (%s) : %s"
				"(phDataDest=%#x, hDataSrc=%#x, bOnlyCopyData=%d)\n", 
				strtime_m(), 
				"Invalid argument. ", 
				phDataDest, hDataSrc, (int)bOnlyCopyData));
			return EXIT_FAILURE;
		}
	}
	else
	{
		*phDataDest = (HPGMDATA)calloc(1, sizeof(struct PGMDATA));

		if (*phDataDest == NULL)
		{
			PRINT_DEBUG_ERROR_PGMCORE(("CopyPGMData error (%s) : %s"
				"(phDataDest=%#x, hDataSrc=%#x, bOnlyCopyData=%d)\n", 
				strtime_m(), 
				"Out of memory. ", 
				phDataDest, hDataSrc, (int)bOnlyCopyData));
			return EXIT_FAILURE;
		}

		(*phDataDest)->Width = hDataSrc->Width;
		(*phDataDest)->Height = hDataSrc->Height;
		(*phDataDest)->NbPixels = hDataSrc->NbPixels;
		(*phDataDest)->NbElements = hDataSrc->NbElements;
		(*phDataDest)->Size = hDataSrc->Size;
		(*phDataDest)->ColorDepth = hDataSrc->ColorDepth;

		(*phDataDest)->Img = (UCHAR*)calloc((*phDataDest)->NbElements, sizeof(UCHAR));

		if ((*phDataDest)->Img == NULL)
		{
			PRINT_DEBUG_ERROR_PGMCORE(("CopyPGMData error (%s) : %s"
				"(phDataDest=%#x, hDataSrc=%#x, bOnlyCopyData=%d)\n", 
				strtime_m(), 
				"Out of memory. ", 
				phDataDest, hDataSrc, (int)bOnlyCopyData));
			free(*phDataDest);*phDataDest = INVALID_HPGMDATA_VALUE;
			return EXIT_FAILURE;
		}
	}

	memcpy((*phDataDest)->Img, hDataSrc->Img, hDataSrc->Size);

	return EXIT_SUCCESS;
}

/*
Destroy a structure created by CreatePGMData() or CopyPGMData() that 
contained picture data corresponding to the PGM format. See the 
description of the PGMDATA structure for more information.

HPGMDATA* phData : (INOUT) Valid pointer to the identifier of the structure.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int DestroyPGMData(HPGMDATA* phData)
{
	if ((phData == NULL)||(*phData == INVALID_HPGMDATA_VALUE))
	{
		PRINT_DEBUG_ERROR_PGMCORE(("DestroyPGMData error (%s) : %s"
			"(phData=%#x)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			phData));
		return EXIT_FAILURE;
	}

	free((*phData)->Img);
#ifdef _DEBUG
	(*phData)->Img = NULL;
	(*phData)->Width = 0;
	(*phData)->Height = 0;
	(*phData)->NbPixels = 0;
	(*phData)->NbElements = 0;
	(*phData)->Size = 0;
	(*phData)->ColorDepth = 0;
#endif // _DEBUG

	free(*phData);

	*phData = INVALID_HPGMDATA_VALUE;

	return EXIT_SUCCESS;
}
