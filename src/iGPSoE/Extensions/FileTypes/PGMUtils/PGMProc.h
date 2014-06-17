/***************************************************************************************************************:')

PGMProc.h

PGM data processing.

Fabrice Le Bars

Created : 2009-02-12

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef PGMPROC_H
#define PGMPROC_H

#include "PGMCore.h"

/*
Debug macros specific to PGMProc.
*/
#ifdef _DEBUG_MESSAGES_PGMUTILS
#	define _DEBUG_MESSAGES_PGMPROC
#endif // _DEBUG_MESSAGES_PGMUTILS

#ifdef _DEBUG_WARNINGS_PGMUTILS
#	define _DEBUG_WARNINGS_PGMPROC
#endif // _DEBUG_WARNINGS_PGMUTILS

#ifdef _DEBUG_ERRORS_PGMUTILS
#	define _DEBUG_ERRORS_PGMPROC
#endif // _DEBUG_ERRORS_PGMUTILS

#ifdef _DEBUG_MESSAGES_PGMPROC
#	define PRINT_DEBUG_MESSAGE_PGMPROC(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_PGMPROC(params)
#endif // _DEBUG_MESSAGES_PGMPROC

#ifdef _DEBUG_WARNINGS_PGMPROC
#	define PRINT_DEBUG_WARNING_PGMPROC(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_PGMPROC(params)
#endif // _DEBUG_WARNINGS_PGMPROC

#ifdef _DEBUG_ERRORS_PGMPROC
#	define PRINT_DEBUG_ERROR_PGMPROC(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_PGMPROC(params)
#endif // _DEBUG_ERRORS_PGMPROC

/*
Smooth the pixel at line i and row j of a PGM picture using the values of 4 
neighbors. 
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (IN) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR* pValue : (INOUT) Pointer to the new gray value of the pixel.

Return : Nothing.
*/
inline void Smooth4PGMImgPixel(HPGMDATA hData, UINT i, UINT j, UCHAR* pValue)
{
	// Color values of the current pixel.
	int index = j+hData->Width*i;
	double value = (double)hData->Img[index];

	int n = 1; // Number of pixels used to smooth.

	if (i > 0) 
	{ 
		index = j+hData->Width*(i-1);
		value += (double)hData->Img[index]; 
		n++;
	}

	if (i < hData->Height-1)
	{
		index = j+hData->Width*(i+1);
		value += (double)hData->Img[index]; 
		n++;
	}

	if (j > 0) 
	{ 
		index = j-1+hData->Width*i;
		value += (double)hData->Img[index]; 
		n++;
	}

	if (j < hData->Width-1)
	{
		index = j+1+hData->Width*i;
		value += (double)hData->Img[index]; 
		n++;
	}

	value /= (double)n;

	*pValue = (UCHAR)value;
}

/*
Smooth the pixel at line i and row j of a PGM picture using the values of 8 
neighbors. 
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (IN) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR* pValue : (INOUT) Pointer to the new gray value of the pixel.

Return : Nothing.
*/
//inline void Smooth8PGMImgPixel(HPGMDATA hData, UINT i, UINT j, UCHAR* pValue)
//{
// To be implemented.
//}

#endif // PGMPROC_H
