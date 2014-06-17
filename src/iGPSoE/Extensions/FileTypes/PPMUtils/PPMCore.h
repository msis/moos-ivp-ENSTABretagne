/***************************************************************************************************************:')

PPMCore.h

PPM picture data handling.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef PPMCORE_H
#define PPMCORE_H

#include "OSTime.h"

/*
Debug macros specific to PPMUtils.
*/
#ifdef _DEBUG_MESSAGES
#	define _DEBUG_MESSAGES_PPMUTILS
#endif // _DEBUG_MESSAGES

#ifdef _DEBUG_WARNINGS
#	define _DEBUG_WARNINGS_PPMUTILS
#endif // _DEBUG_WARNINGS

#ifdef _DEBUG_ERRORS
#	define _DEBUG_ERRORS_PPMUTILS
#endif // _DEBUG_ERRORS

#ifdef _DEBUG_MESSAGES_PPMUTILS
#	define PRINT_DEBUG_MESSAGE_PPMUTILS(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_PPMUTILS(params)
#endif // _DEBUG_MESSAGES_PPMUTILS

#ifdef _DEBUG_WARNINGS_PPMUTILS
#	define PRINT_DEBUG_WARNING_PPMUTILS(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_PPMUTILS(params)
#endif // _DEBUG_WARNINGS_PPMUTILS

#ifdef _DEBUG_ERRORS_PPMUTILS
#	define PRINT_DEBUG_ERROR_PPMUTILS(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_PPMUTILS(params)
#endif // _DEBUG_ERRORS_PPMUTILS

/*
Debug macros specific to PPMCore.
*/
#ifdef _DEBUG_MESSAGES_PPMUTILS
#	define _DEBUG_MESSAGES_PPMCORE
#endif // _DEBUG_MESSAGES_PPMUTILS

#ifdef _DEBUG_WARNINGS_PPMUTILS
#	define _DEBUG_WARNINGS_PPMCORE
#endif // _DEBUG_WARNINGS_PPMUTILS

#ifdef _DEBUG_ERRORS_PPMUTILS
#	define _DEBUG_ERRORS_PPMCORE
#endif // _DEBUG_ERRORS_PPMUTILS

#ifdef _DEBUG_MESSAGES_PPMCORE
#	define PRINT_DEBUG_MESSAGE_PPMCORE(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_PPMCORE(params)
#endif // _DEBUG_MESSAGES_PPMCORE

#ifdef _DEBUG_WARNINGS_PPMCORE
#	define PRINT_DEBUG_WARNING_PPMCORE(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_PPMCORE(params)
#endif // _DEBUG_WARNINGS_PPMCORE

#ifdef _DEBUG_ERRORS_PPMCORE
#	define PRINT_DEBUG_ERROR_PPMCORE(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_PPMCORE(params)
#endif // _DEBUG_ERRORS_PPMCORE

/*
Structure corresponding to the data contained in a PPM picture file
(uncompressed, in a RGB format (red, green and blue levels)).
*/
struct PPMDATA
{
	UCHAR* Img; // Picture data. The red value of the pixel at line i
	// and column j in the picture Img of Width * Height pixels should be in
	// Img[3*(j+Width*i)+0], the green in Img[3*(j+Width*i)+1] and the blue in 
	// Img[3*(j+Width*i)+2].
	UINT Width; // Width of the picture (in pixels).
	UINT Height; // Height of the picture (in pixels).
	UINT NbPixels; // Number of pixels in the picture.
	UINT Size; // Size of Img (in bytes).
	UINT NbElements; // Number of elements (UCHAR values) in Img.
	UINT ColorDepth; // Maximum value for a color level (red, green
	// or blue) of a pixel. It is typically 255 (other values are not really 
	// supported for the moment).
};
typedef struct PPMDATA* HPPMDATA;

#define INVALID_HPPMDATA_VALUE NULL

/*
Structure corresponding to a pixel in a PPM picture file
(in a RGB format (red, green and blue levels)).
*/
typedef RGBCOLOR PPMPIXEL;

/*
Create and initialize a structure that will contain picture data corresponding
to the PPM format. Use DestroyPPMData() to free the picture data at the end.
See the description of the PPMDATA structure for more information.

HPPMDATA* phData : (INOUT) Valid pointer that will receive an identifier of
the structure.
UINT width : (IN) Width of the picture (in pixels).
UINT height : (IN) Height of the picture (in pixels).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int CreatePPMData(HPPMDATA* phData, UINT width, UINT height);

/*
Create and initialize a structure that is a copy of a PPMDATA structure. 
Use DestroyPPMData() to free phDataDest at the end.
See the description of the PPMDATA structure for more information.

HPPMDATA* phDataDest : (INOUT) Valid pointer that will receive an identifier of
the structure that will receive the copy of the structure identified by hDataSrc.
HPPMDATA hDataSrc : (IN) Identifier of the PPMDATA to copy. 
BOOL bOnlyCopyData : (IN) TRUE if phDataDest corresponds already to a PPMDATA
 structure with the same characteristics as hDataSrc, FALSE if it must be 
 entirely initialized.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int CopyPPMData(HPPMDATA* phDataDest, HPPMDATA hDataSrc, BOOL bOnlyCopyData);

/*
Destroy a structure created by CreatePPMData() or CopyPPMData() that 
contained picture data corresponding to the PPM format. See the 
description of the PPMDATA structure for more information.

HPPMDATA* phData : (INOUT) Valid pointer to the identifier of the structure.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C int DestroyPPMData(HPPMDATA* phData);

/*
Get the color of the pixel at line i and row j of a PPM picture. 
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR* pRed : (INOUT) Pointer to the red value of the pixel.
UCHAR* pGreen : (INOUT) Pointer to the green value of the pixel.
UCHAR* pBlue : (INOUT) Pointer to the blue value of the pixel.

Return : Nothing.
*/
inline void GetPPMImgPixelQuick(HPPMDATA hData, UINT i, UINT j, UCHAR* pRed, UCHAR* pGreen, UCHAR* pBlue)
{
	int index = 3*(j+hData->Width*i);

	*pRed = hData->Img[index];
	*pGreen = hData->Img[index+1];
	*pBlue = hData->Img[index+2];
}

/*
Get the color of the pixel at line i and row j of a PPM picture. 
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
RGBCOLOR* pColor : (INOUT) Pointer to the color of the pixel.

Return : Nothing.
*/
inline void GetPPMImgPixelRGBCOLORQuick(HPPMDATA hData, UINT i, UINT j, RGBCOLOR* pColor)
{
	int index = 3*(j+hData->Width*i);

	pColor->r = hData->Img[index];
	pColor->g = hData->Img[index+1];
	pColor->b = hData->Img[index+2];
}

/*
Get the color of the pixel at line i and row j of a PPM picture. 
The function handles automatically any out of bounds index.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
int i : (IN) Line of the pixel.
int j : (IN) Row of the pixel.
UCHAR* pRed : (INOUT) Pointer to the red value of the pixel.
UCHAR* pGreen : (INOUT) Pointer to the green value of the pixel.
UCHAR* pBlue : (INOUT) Pointer to the blue value of the pixel.

Return : Nothing.
*/
inline void GetPPMImgPixel(HPPMDATA hData, int i, int j, UCHAR* pRed, UCHAR* pGreen, UCHAR* pBlue)
{
	int index;

	//i = min(max(i, 0), (int)hData->Height-1);
	//j = min(max(j, 0), (int)hData->Width-1);

	if ((i < 0)||(i >= (int)hData->Height)||(j < 0)||(j >= (int)hData->Width))
	{
		return;
	}

	index = 3*(j+hData->Width*i);

	*pRed = hData->Img[index];
	*pGreen = hData->Img[index+1];
	*pBlue = hData->Img[index+2];
}

/*
Get the color of the pixel at line i and row j of a PPM picture. 
The function handles automatically any out of bounds index.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
int i : (IN) Line of the pixel.
int j : (IN) Row of the pixel.
RGBCOLOR* pColor : (INOUT) Pointer to the color of the pixel.

Return : Nothing.
*/
inline void GetPPMImgPixelRGBCOLOR(HPPMDATA hData, int i, int j, RGBCOLOR* pColor)
{
	int index;

	//i = min(max(i, 0), (int)hData->Height-1);
	//j = min(max(j, 0), (int)hData->Width-1);

	if ((i < 0)||(i >= (int)hData->Height)||(j < 0)||(j >= (int)hData->Width))
	{
		return;
	}

	index = 3*(j+hData->Width*i);

	pColor->r = hData->Img[index];
	pColor->g = hData->Img[index+1];
	pColor->b = hData->Img[index+2];
}

/*
Set color of the pixel at line i and row j of a PPM picture. 
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR red : (IN) New red value of the pixel (between 0 and 255).
UCHAR green : (IN) New green value of the pixel (between 0 and 255).
UCHAR blue : (IN) New blue value of the pixel (between 0 and 255).

Return : Nothing.
*/
inline void SetPPMImgPixelQuick(HPPMDATA hData, UINT i, UINT j, UCHAR red, UCHAR green, UCHAR blue)
{
	int index = 3*(j+hData->Width*i);

	hData->Img[index] = red;
	hData->Img[index+1] = green;
	hData->Img[index+2] = blue;
}

/*
Set color of the pixel at line i and row j of a PPM picture. 
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
RGBCOLOR color : (IN) New color of the pixel.

Return : Nothing.
*/
inline void SetPPMImgPixelRGBCOLORQuick(HPPMDATA hData, UINT i, UINT j, RGBCOLOR color)
{
	int index = 3*(j+hData->Width*i);

	hData->Img[index] = color.r;
	hData->Img[index+1] = color.g;
	hData->Img[index+2] = color.b;
}

/*
Set the pixel at line i and row j of a PPM picture. 
The function handles automatically any out of bounds index.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
int i : (IN) Line of the pixel.
int j : (IN) Row of the pixel.
UCHAR red : (IN) New red value of the pixel (between 0 and 255).
UCHAR green : (IN) New green value of the pixel (between 0 and 255).
UCHAR blue : (IN) New blue value of the pixel (between 0 and 255).

Return : Nothing.
*/
inline void SetPPMImgPixel(HPPMDATA hData, int i, int j, UCHAR red, UCHAR green, UCHAR blue)
{
	int index;

	//i = min(max(i, 0), (int)hData->Height-1);
	//j = min(max(j, 0), (int)hData->Width-1);

	if ((i < 0)||(i >= (int)hData->Height)||(j < 0)||(j >= (int)hData->Width))
	{
		return;
	}

	index = 3*(j+hData->Width*i);

	hData->Img[index] = red;
	hData->Img[index+1] = green;
	hData->Img[index+2] = blue;
}

/*
Set the pixel at line i and row j of a PPM picture. 
The function handles automatically any out of bounds index.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
int i : (IN) Line of the pixel.
int j : (IN) Row of the pixel.
RGBCOLOR color : (IN) New color of the pixel.

Return : Nothing.
*/
inline void SetPPMImgPixelRGBCOLOR(HPPMDATA hData, int i, int j, RGBCOLOR color)
{
	int index;

	//i = min(max(i, 0), (int)hData->Height-1);
	//j = min(max(j, 0), (int)hData->Width-1);

	if ((i < 0)||(i >= (int)hData->Height)||(j < 0)||(j >= (int)hData->Width))
	{
		return;
	}

	index = 3*(j+hData->Width*i);

	hData->Img[index] = color.r;
	hData->Img[index+1] = color.g;
	hData->Img[index+2] = color.b;
}

/*
Get the color of the pixel at line i and row j of a PPM picture. 
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.

Return : The color.
*/
inline RGBCOLOR GetPPMImgPixelRGB(HPPMDATA hData, UINT i, UINT j)
{
	int index = 3*(j+hData->Width*i);
	RGBCOLOR color;

	color.r = hData->Img[index];
	color.g = hData->Img[index+1];
	color.b = hData->Img[index+2];

	return color;
}

/*
Get the red value of the pixel at line i and row j of a PPM picture. 
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.

Return : The red value (between 0 and 255).
*/
inline UCHAR GetPPMImgPixelR(HPPMDATA hData, UINT i, UINT j)
{
	return hData->Img[3*(j+hData->Width*i)];
}

/*
Get the green value of the pixel at line i and row j of a PPM picture. 
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.

Return : The green value (between 0 and 255).
*/
inline UCHAR GetPPMImgPixelG(HPPMDATA hData, UINT i, UINT j)
{
	return hData->Img[3*(j+hData->Width*i)+1];
}

/*
Get the blue value of the pixel at line i and row j of a PPM picture. 
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.

Return : The blue value (between 0 and 255).
*/
inline UCHAR GetPPMImgPixelB(HPPMDATA hData, UINT i, UINT j)
{
	return hData->Img[3*(j+hData->Width*i)+2];
}

/*
Set the red value of the pixel at line i and row j of a PPM picture. 
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR red : (IN) New red value (between 0 and 255).

Return : Nothing.
*/
inline void SetPPMImgPixelR(HPPMDATA hData, UINT i, UINT j, UCHAR red)
{
	hData->Img[3*(j+hData->Width*i)] = red;
}

/*
Set the green value of the pixel at line i and row j of a PPM picture. 
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR red : (IN) New green value (between 0 and 255).

Return : Nothing.
*/
inline void SetPPMImgPixelG(HPPMDATA hData, UINT i, UINT j, UCHAR green)
{
	hData->Img[3*(j+hData->Width*i)+1] = green;
}

/*
Set the blue value of the pixel at line i and row j of a PPM picture. 
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR red : (IN) New blue value (between 0 and 255).

Return : Nothing.
*/
inline void SetPPMImgPixelB(HPPMDATA hData, UINT i, UINT j, UCHAR blue)
{
	hData->Img[3*(j+hData->Width*i)+2] = blue;
}

// Getters and setters.

inline UINT GetPPMImgWidth(HPPMDATA hData)
{
	return hData->Width;
}

inline UINT GetPPMImgHeight(HPPMDATA hData)
{
	return hData->Height;
}

inline UINT GetPPMImgNbPixels(HPPMDATA hData)
{
	return hData->NbPixels;
}

inline UINT GetPPMImgSize(HPPMDATA hData)
{
	return hData->Size;
}

inline UINT GetPPMImgNbElements(HPPMDATA hData)
{
	return hData->NbElements;
}

inline UCHAR* GetPtrToPPMImg(HPPMDATA hData)
{
	return hData->Img;
}

#endif // PPMCORE_H
