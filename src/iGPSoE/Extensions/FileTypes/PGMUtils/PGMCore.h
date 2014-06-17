/***************************************************************************************************************:')

PGMCore.h

PGM picture data handling.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef PGMCORE_H
#define PGMCORE_H

#include "OSTime.h"

/*
Debug macros specific to PGMUtils.
*/
#ifdef _DEBUG_MESSAGES
#	define _DEBUG_MESSAGES_PGMUTILS
#endif // _DEBUG_MESSAGES

#ifdef _DEBUG_WARNINGS
#	define _DEBUG_WARNINGS_PGMUTILS
#endif // _DEBUG_WARNINGS

#ifdef _DEBUG_ERRORS
#	define _DEBUG_ERRORS_PGMUTILS
#endif // _DEBUG_ERRORS

#ifdef _DEBUG_MESSAGES_PGMUTILS
#	define PRINT_DEBUG_MESSAGE_PGMUTILS(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_PGMUTILS(params)
#endif // _DEBUG_MESSAGES_PGMUTILS

#ifdef _DEBUG_WARNINGS_PGMUTILS
#	define PRINT_DEBUG_WARNING_PGMUTILS(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_PGMUTILS(params)
#endif // _DEBUG_WARNINGS_PGMUTILS

#ifdef _DEBUG_ERRORS_PGMUTILS
#	define PRINT_DEBUG_ERROR_PGMUTILS(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_PGMUTILS(params)
#endif // _DEBUG_ERRORS_PGMUTILS

/*
Debug macros specific to PGMCore.
*/
#ifdef _DEBUG_MESSAGES_PGMUTILS
#	define _DEBUG_MESSAGES_PGMCORE
#endif // _DEBUG_MESSAGES_PGMUTILS

#ifdef _DEBUG_WARNINGS_PGMUTILS
#	define _DEBUG_WARNINGS_PGMCORE
#endif // _DEBUG_WARNINGS_PGMUTILS

#ifdef _DEBUG_ERRORS_PGMUTILS
#	define _DEBUG_ERRORS_PGMCORE
#endif // _DEBUG_ERRORS_PGMUTILS

#ifdef _DEBUG_MESSAGES_PGMCORE
#	define PRINT_DEBUG_MESSAGE_PGMCORE(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_PGMCORE(params)
#endif // _DEBUG_MESSAGES_PGMCORE

#ifdef _DEBUG_WARNINGS_PGMCORE
#	define PRINT_DEBUG_WARNING_PGMCORE(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_PGMCORE(params)
#endif // _DEBUG_WARNINGS_PGMCORE

#ifdef _DEBUG_ERRORS_PGMCORE
#	define PRINT_DEBUG_ERROR_PGMCORE(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_PGMCORE(params)
#endif // _DEBUG_ERRORS_PGMCORE

/*
Structure corresponding to the data contained in a PGM picture file
(uncompressed, in a gray format).
*/
struct PGMDATA
{
	UCHAR* Img; // Picture data. The gray value of the pixel at line i
	// and column j in the picture Img of Width * Height pixels should be in
	// Img[j+Width*i].
	UINT Width; // Width of the picture (in pixels).
	UINT Height; // Height of the picture (in pixels).
	UINT NbPixels; // Number of pixels in the picture.
	UINT Size; // Size of Img (in bytes).
	UINT NbElements; // Number of elements (UCHAR values) in Img.
	UINT ColorDepth; // Maximum value for a color level of a pixel. It is 
	// typically 255 (other values are not really supported for the moment).
};
typedef struct PGMDATA* HPGMDATA;

#define INVALID_HPGMDATA_VALUE NULL

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
EXTERN_C int CreatePGMData(HPGMDATA* phData, UINT width, UINT height);

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
EXTERN_C int CopyPGMData(HPGMDATA* phDataDest, HPGMDATA hDataSrc, BOOL bOnlyCopyData);

/*
Destroy a structure created by CreatePGMData() or CopyPGMData() that 
contained picture data corresponding to the PGM format. See the 
description of the PGMDATA structure for more information.

HPGMDATA* phData : (INOUT) Valid pointer to the identifier of the structure.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int DestroyPGMData(HPGMDATA* phData);

/*
Get the gray value of the pixel at line i and row j of a PGM picture. 
The function does not check any parameter.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.

Return : The gray value (between 0 and 255).
*/
inline UCHAR GetPGMImgPixelQuick(HPGMDATA hData, UINT i, UINT j)
{
	return hData->Img[j+hData->Width*i];
}

/*
Get the gray value of the pixel at line i and row j of a PGM picture. 
The function handles automatically any out of bounds index.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
int i : (IN) Line of the pixel.
int j : (IN) Row of the pixel.

Return : The gray value (between 0 and 255).
*/
inline UCHAR GetPGMImgPixel(HPGMDATA hData, int i, int j)
{
	//i = min(max(i, 0), (int)hData->Height-1);
	//j = min(max(j, 0), (int)hData->Width-1);

	if ((i < 0)||(i >= (int)hData->Height)||(j < 0)||(j >= (int)hData->Width))
	{
		return 0;
	}

	return hData->Img[j+hData->Width*i];
}

/*
Set the gray value of the pixel at line i and row j of a PGM picture. 
The function does not check any parameter.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR value : (IN) New gray value (between 0 and 255).

Return : Nothing.
*/
inline void SetPGMImgPixelQuick(HPGMDATA hData, UINT i, UINT j, UCHAR value)
{
	hData->Img[j+hData->Width*i] = value;
}

/*
Set the gray value of the pixel at line i and row j of a PGM picture. 
The function handles automatically any out of bounds index.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
int i : (IN) Line of the pixel.
int j : (IN) Row of the pixel.
UCHAR value : (IN) New gray value (between 0 and 255).

Return : Nothing.
*/
inline void SetPGMImgPixel(HPGMDATA hData, int i, int j, UCHAR value)
{
	//i = min(max(i, 0), (int)hData->Height-1);
	//j = min(max(j, 0), (int)hData->Width-1);

	if ((i < 0)||(i >= (int)hData->Height)||(j < 0)||(j >= (int)hData->Width))
	{
		return;
	}

	hData->Img[j+hData->Width*i] = value;
}

// Getters and setters.

inline UINT GetPGMImgWidth(HPGMDATA hData)
{
	return hData->Width;
}

inline UINT GetPGMImgHeight(HPGMDATA hData)
{
	return hData->Height;
}

inline UINT GetPGMImgNbPixels(HPGMDATA hData)
{
	return hData->NbPixels;
}

inline UINT GetPGMImgSize(HPGMDATA hData)
{
	return hData->Size;
}

inline UINT GetPGMImgNbElements(HPGMDATA hData)
{
	return hData->NbElements;
}

inline UCHAR* GetPtrToPGMImg(HPGMDATA hData)
{
	return hData->Img;
}

#endif // PGMCORE_H
