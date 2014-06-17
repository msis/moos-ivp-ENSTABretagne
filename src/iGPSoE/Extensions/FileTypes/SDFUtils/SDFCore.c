/***************************************************************************************************************:')

SDFCore.c

SDF SonarPro files and data handling.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#include "SDFCore.h"

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
int CreateSDFData(HSDFDATA* phData, UINT NumPings, USHORT NumSamples)
{
	if ((phData == NULL)||(NumPings == 0)||(NumSamples == 0))
	{
		PRINT_DEBUG_ERROR_SDFCORE(("CreateSDFData error (%s) : %s"
			"(phData=%#x, NumPings=%u, NumSamples=%u)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			phData, NumPings, (UINT)NumSamples));
		return EXIT_FAILURE;
	}

	*phData = (HSDFDATA)calloc(1, sizeof(struct SDFDATA));

	if (*phData == NULL)
	{
		PRINT_DEBUG_ERROR_SDFCORE(("CreateSDFData error (%s) : %s"
			"(phData=%#x, NumPings=%u, NumSamples=%u)\n", 
			strtime_m(), 
			"Out of memory. ", 
			phData, NumPings, (UINT)NumSamples));
		return EXIT_FAILURE;
	}

	(*phData)->NumPings = NumPings;
	(*phData)->NumSamples = NumSamples;
	(*phData)->NbElements = NumPings*NumSamples;
	(*phData)->Size = (*phData)->NbElements*sizeof(USHORT);

	(*phData)->Headers = (SYS5000HEADER*)calloc((*phData)->NumPings, sizeof(SYS5000HEADER));

	if ((*phData)->Headers == NULL)
	{
		PRINT_DEBUG_ERROR_SDFCORE(("CreateSDFData error (%s) : %s"
			"(phData=%#x, NumPings=%u, NumSamples=%u)\n", 
			strtime_m(), 
			"Out of memory. ", 
			phData, NumPings, (UINT)NumSamples));
		free(*phData);*phData = INVALID_HSDFDATA_VALUE;
		return EXIT_FAILURE;
	}

	(*phData)->Pings = (USHORT*)calloc((*phData)->NbElements, sizeof(USHORT));

	if ((*phData)->Pings == NULL)
	{
		PRINT_DEBUG_ERROR_SDFCORE(("CreateSDFData error (%s) : %s"
			"(phData=%#x, NumPings=%u, NumSamples=%u)\n", 
			strtime_m(), 
			"Out of memory. ", 
			phData, NumPings, (UINT)NumSamples));
		free((*phData)->Headers);(*phData)->Headers = NULL;
		free(*phData);*phData = INVALID_HSDFDATA_VALUE;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

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
int CopySDFData(HSDFDATA* phDataDest, HSDFDATA hDataSrc, BOOL bOnlyCopyData)
{
	if ((phDataDest == NULL)||(hDataSrc == INVALID_HSDFDATA_VALUE))
	{
		PRINT_DEBUG_ERROR_SDFCORE(("CopySDFData error (%s) : %s"
			"(phDataDest=%#x, hDataSrc=%#x, bOnlyCopyData=%d)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			phDataDest, hDataSrc, (int)bOnlyCopyData));
		return EXIT_FAILURE;
	}

	if (bOnlyCopyData)
	{
		if (
			(*phDataDest == INVALID_HSDFDATA_VALUE) ||
			((*phDataDest)->Headers == NULL) ||
			((*phDataDest)->Pings == NULL) ||
			((*phDataDest)->NumSamples != hDataSrc->NumSamples) ||
			((*phDataDest)->NumPings != hDataSrc->NumPings) ||
			((*phDataDest)->NbElements != hDataSrc->NbElements) ||
			((*phDataDest)->Size != hDataSrc->Size)
			)
		{
			PRINT_DEBUG_ERROR_SDFCORE(("CopySDFData error (%s) : %s"
				"(phDataDest=%#x, hDataSrc=%#x, bOnlyCopyData=%d)\n", 
				strtime_m(), 
				"Invalid argument. ", 
				phDataDest, hDataSrc, (int)bOnlyCopyData));
			return EXIT_FAILURE;
		}
	}
	else
	{
		*phDataDest = (HSDFDATA)calloc(1, sizeof(struct SDFDATA));

		if (*phDataDest == NULL)
		{
			PRINT_DEBUG_ERROR_SDFCORE(("CopySDFData error (%s) : %s"
				"(phDataDest=%#x, hDataSrc=%#x, bOnlyCopyData=%d)\n", 
				strtime_m(), 
				"Out of memory. ", 
				phDataDest, hDataSrc, (int)bOnlyCopyData));
			return EXIT_FAILURE;
		}

		(*phDataDest)->NumSamples = hDataSrc->NumSamples;
		(*phDataDest)->NumPings = hDataSrc->NumPings;
		(*phDataDest)->NbElements = hDataSrc->NbElements;
		(*phDataDest)->Size = hDataSrc->Size;

		(*phDataDest)->Headers = (SYS5000HEADER*)calloc((*phDataDest)->NumPings, sizeof(SYS5000HEADER));

		if ((*phDataDest)->Headers == NULL)
		{
			PRINT_DEBUG_ERROR_SDFCORE(("CopySDFData error (%s) : %s"
				"(phDataDest=%#x, hDataSrc=%#x, bOnlyCopyData=%d)\n", 
				strtime_m(), 
				"Out of memory. ", 
				phDataDest, hDataSrc, (int)bOnlyCopyData));
			free(*phDataDest);*phDataDest = INVALID_HSDFDATA_VALUE;
			return EXIT_FAILURE;
		}

		(*phDataDest)->Pings = (USHORT*)calloc((*phDataDest)->NbElements, sizeof(USHORT));

		if ((*phDataDest)->Pings == NULL)
		{
			PRINT_DEBUG_ERROR_SDFCORE(("CopySDFData error (%s) : %s"
				"(phDataDest=%#x, hDataSrc=%#x, bOnlyCopyData=%d)\n", 
				strtime_m(), 
				"Out of memory. ", 
				phDataDest, hDataSrc, (int)bOnlyCopyData));
			free((*phDataDest)->Headers);(*phDataDest)->Headers = NULL;
			free(*phDataDest);*phDataDest = INVALID_HSDFDATA_VALUE;
			return EXIT_FAILURE;
		}
	}

	memcpy((*phDataDest)->Headers, hDataSrc->Headers, hDataSrc->NumPings*sizeof(SYS5000HEADER));
	memcpy((*phDataDest)->Pings, hDataSrc->Pings, hDataSrc->Size);

	return EXIT_SUCCESS;
}

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
int ConcatSDFData(HSDFDATA* phDataOut, HSDFDATA hDataIn1, HSDFDATA hDataIn2)
{
	if ((phDataOut == NULL)||
		(hDataIn1 == INVALID_HSDFDATA_VALUE)||(hDataIn2 == INVALID_HSDFDATA_VALUE)||
		(hDataIn1->Headers == NULL)||(hDataIn1->Pings == NULL)||
		(hDataIn2->Headers == NULL)||(hDataIn2->Pings == NULL)||
		(hDataIn1->NumSamples != hDataIn2->NumSamples)	
		)
	{
		PRINT_DEBUG_ERROR_SDFCORE(("ConcatSDFData error (%s) : %s"
			"(phDataOut=%#x, hDataIn1=%#x, hDataIn2=%#x)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			phDataOut, hDataIn1, hDataIn2));
		return EXIT_FAILURE;
	}

	*phDataOut = (HSDFDATA)calloc(1, sizeof(struct SDFDATA));

	if (*phDataOut == NULL)
	{
		PRINT_DEBUG_ERROR_SDFCORE(("ConcatSDFData error (%s) : %s"
			"(phDataOut=%#x, hDataIn1=%#x, hDataIn2=%#x)\n", 
			strtime_m(), 
			"Out of memory. ", 
			phDataOut, hDataIn1, hDataIn2));
		return EXIT_FAILURE;
	}

	(*phDataOut)->NumSamples = hDataIn1->NumSamples;
	(*phDataOut)->NumPings = 
		max(hDataIn1->Headers[hDataIn1->NumPings-1].pingNumber, hDataIn2->Headers[hDataIn2->NumPings-1].pingNumber)
		- min(hDataIn1->Headers[0].pingNumber, hDataIn2->Headers[0].pingNumber);
	(*phDataOut)->NbElements = (*phDataOut)->NumPings*(*phDataOut)->NumSamples;
	(*phDataOut)->Size = (*phDataOut)->NbElements*sizeof(USHORT);

	(*phDataOut)->Headers = (SYS5000HEADER*)calloc((*phDataOut)->NumPings, sizeof(SYS5000HEADER));

	if ((*phDataOut)->Headers == NULL)
	{
		PRINT_DEBUG_ERROR_SDFCORE(("ConcatSDFData error (%s) : %s"
			"(phDataOut=%#x, hDataIn1=%#x, hDataIn2=%#x)\n", 
			strtime_m(), 
			"Out of memory. ", 
			phDataOut, hDataIn1, hDataIn2));
		free(*phDataOut);*phDataOut = INVALID_HSDFDATA_VALUE;
		return EXIT_FAILURE;
	}

	(*phDataOut)->Pings = (USHORT*)calloc((*phDataOut)->NbElements, sizeof(USHORT));

	if ((*phDataOut)->Pings == NULL)
	{
		PRINT_DEBUG_ERROR_SDFCORE(("ConcatSDFData error (%s) : %s"
			"(phDataOut=%#x, hDataIn1=%#x, hDataIn2=%#x)\n", 
			strtime_m(), 
			"Out of memory. ", 
			phDataOut, hDataIn1, hDataIn2));
		free((*phDataOut)->Headers);(*phDataOut)->Headers = NULL;
		free(*phDataOut);*phDataOut = INVALID_HSDFDATA_VALUE;
		return EXIT_FAILURE;
	}

	if (hDataIn1->Headers[0].pingNumber < hDataIn2->Headers[0].pingNumber)
	{
		memcpy((*phDataOut)->Headers, hDataIn1->Headers, hDataIn1->NumPings*sizeof(SYS5000HEADER));
		memcpy((*phDataOut)->Pings, hDataIn1->Pings, hDataIn1->Size);
		memcpy(&(*phDataOut)->Headers[(*phDataOut)->NumPings-hDataIn2->NumPings], hDataIn2->Headers, hDataIn2->NumPings*sizeof(SYS5000HEADER));
		memcpy(&(*phDataOut)->Pings[((*phDataOut)->NumPings-hDataIn2->NumPings)*(*phDataOut)->NumSamples], hDataIn2->Pings, hDataIn2->Size);
	}
	else
	{
		memcpy((*phDataOut)->Headers, hDataIn2->Headers, hDataIn2->NumPings*sizeof(SYS5000HEADER));
		memcpy((*phDataOut)->Pings, hDataIn2->Pings, hDataIn2->Size);
		memcpy(&(*phDataOut)->Headers[(*phDataOut)->NumPings-hDataIn1->NumPings], hDataIn1->Headers, hDataIn1->NumPings*sizeof(SYS5000HEADER));
		memcpy(&(*phDataOut)->Pings[((*phDataOut)->NumPings-hDataIn1->NumPings)*(*phDataOut)->NumSamples], hDataIn1->Pings, hDataIn1->Size);
	}

	return EXIT_SUCCESS;
}

/*
Destroy a structure created by CreateSDFData() or CopySDFData() that 
contained sonar data corresponding to the SDF format. See the 
description of the SDFDATA structure for more information.

HSDFDATA* phData : (INOUT) Valid pointer to the identifier of the structure.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int DestroySDFData(HSDFDATA* phData)
{
	if ((phData == NULL)||(*phData == INVALID_HSDFDATA_VALUE))
	{
		PRINT_DEBUG_ERROR_SDFCORE(("DestroySDFData error (%s) : %s"
			"(phData=%#x)\n", 
			strtime_m(), 
			"Invalid argument. ", 
			phData));
		return EXIT_FAILURE;
	}

	free((*phData)->Headers);
	free((*phData)->Pings);
#ifdef _DEBUG
	(*phData)->Headers = NULL;
	(*phData)->Pings = NULL;
	(*phData)->NumSamples = 0;
	(*phData)->NumPings = 0;
	(*phData)->NbElements = 0;
	(*phData)->Size = 0;
#endif // _DEBUG

	free(*phData);

	*phData = INVALID_HSDFDATA_VALUE;

	return EXIT_SUCCESS;
}
