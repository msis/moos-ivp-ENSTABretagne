/***************************************************************************************************************:')

CvCore.h

IplImage data handling.
2 types of IplImage format should be used in the functions :
_ a default RGB color format than can be created using CreateDefaultColorCvImg() or cvLoadImage("file.ppm", 1).
_ a default gray format than can be created using CreateDefaultGrayCvImg() or cvLoadImage("file.pgm", 0).
Note that you can use most of the OpenCV functions with such IplImage formats.

Fabrice Le Bars

Created : 2009-03-26

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef CVCORE_H
#define CVCORE_H

#include "OSTime.h"

#ifdef _MSC_VER
// Disable some Visual Studio warnings that happen sometimes in OpenCV.
#pragma warning(disable : 4100) 
#pragma warning(disable : 4996)
#endif // _MSC_VER

#ifdef __BORLANDC__
// Disable some Borland C++ Builder warnings that happen sometimes in OpenCV.
#pragma warn -8019
#endif // __BORLANDC__

// OpenCV headers.
#if defined(OPENCV220) || defined(OPENCV230) || defined(OPENCV231) || defined(OPENCV242) || defined(OPENCV243)
//#include "opencv/cv.h" // Sometimes cause strange errors in debug and C++ mode due to the redefinition of free()...
#include "opencv/cvwimage.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
// The following headers do not build in C mode.
#ifdef __cplusplus
#include "opencv/cvaux.h"
#include "opencv/cxmisc.h"
#include "opencv/ml.h"
#endif // __cplusplus
// min and max may be undefined by OpenCV 2.X.X so we need to redefine them here.
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif // max
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif // min
#else
#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#endif // defined(OPENCV220) || defined(OPENCV230) || defined(OPENCV231) || defined(OPENCV242) || defined(OPENCV243)

#ifdef __BORLANDC__
// Restore the Borland C++ Builder warnings previously disabled for OpenCV.
#pragma warn .8019
#endif // __BORLANDC__

#ifdef _MSC_VER
// Restore the Visual Studio warnings previously disabled for OpenCV.
#pragma warning(default : 4100) 
#pragma warning(default : 4996)
#endif // _MSC_VER

/*
Create and initialize an IplImage with a simple default color format. 
Use DestroyDefaultColorCvImg() or cvReleaseImage() to free the picture data at 
the end.
See the documentation of OpenCV for more information.

IplImage** ppImg : (INOUT) Valid pointer to the pointer to the structure.
UINT width : (IN) Width of the picture (in pixels).
UINT height : (IN) Height of the picture (in pixels).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CreateDefaultColorCvImg(IplImage** ppImg, UINT width, UINT height)
{
	*ppImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

	if (*ppImg == NULL)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Create and initialize a structure that is a copy of an IplImage. 
Use DestroyDefaultColorCvImg() or cvReleaseImage() to free pDataDest at the 
end.
See the documentation of OpenCV for more information.

IplImage** ppImgDest : (INOUT) Valid pointer to the pointer to the structure 
that will receive the copy of pImgSrc.
IplImage* pImgSrc : (IN) Pointer to the IplImage to copy. 
Note that depending on how it was created, it must or must not be destroyed by
DestroyDefaultColorCvImg() or cvReleaseImage().
BOOL bOnlyCopyData : (IN) TRUE if pDataDest corresponds already to an IplImage
structure with the same characteristics as pDataSrc, FALSE if it must be 
entirely initialized.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CopyDefaultColorCvImg(IplImage** ppImgDest, IplImage* pImgSrc, BOOL bOnlyCopyData)
{
	if (bOnlyCopyData)
	{
		cvCopy(pImgSrc, *ppImgDest, 0);

		if (*ppImgDest == NULL)
		{
			return EXIT_FAILURE;
		}
	}
	else
	{
		*ppImgDest = cvCloneImage(pImgSrc);

		if (*ppImgDest == NULL)
		{
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

/*
Create and initialize an IplImage with a simple default gray format. 
Use DestroyDefaultGrayCvImg() or cvReleaseImage() to free the picture data at 
the end.
See the documentation of OpenCV for more information.

IplImage** ppImg : (INOUT) Valid pointer to the pointer to the structure.
UINT width : (IN) Width of the picture (in pixels).
UINT height : (IN) Height of the picture (in pixels).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CreateDefaultGrayCvImg(IplImage** ppImg, UINT width, UINT height)
{
	*ppImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);

	if (*ppImg == NULL)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Create and initialize a structure that is a copy of an IplImage. 
Use DestroyDefaultGrayCvImg() or cvReleaseImage() to free pDataDest at the 
end.
See the documentation of OpenCV for more information.

IplImage** ppImgDest : (INOUT) Valid pointer to the pointer to the structure 
that will receive the copy of pImgSrc.
IplImage* pImgSrc : (IN) Pointer to the IplImage to copy. 
Note that depending on how it was created, it must or must not be destroyed by
DestroyDefaultGrayCvImg() or cvReleaseImage().
BOOL bOnlyCopyData : (IN) TRUE if pDataDest corresponds already to an IplImage
structure with the same characteristics as pDataSrc, FALSE if it must be 
entirely initialized.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CopyDefaultGrayCvImg(IplImage** ppImgDest, IplImage* pImgSrc, BOOL bOnlyCopyData)
{
	if (bOnlyCopyData)
	{
		cvCopy(pImgSrc, *ppImgDest, 0);

		if (*ppImgDest == NULL)
		{
			return EXIT_FAILURE;
		}
	}
	else
	{
		*ppImgDest = cvCloneImage(pImgSrc);

		if (*ppImgDest == NULL)
		{
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

/*
Destroy an IplImage created with CreateDefaultColorCvImg() or 
CreateDefaultColorCvImgFromFile(). Note that it is only a call to cvReleaseImage().
See the documentation of OpenCV for more information.

IplImage** ppImg : (INOUT) Valid pointer to the pointer to the structure.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int DestroyDefaultColorCvImg(IplImage** ppImg)
{
	cvReleaseImage(ppImg);

	return EXIT_SUCCESS;
}

/*
Destroy an IplImage created with CreateDefaultGrayCvImg() or 
CreateDefaultGrayCvImgFromFile(). Note that it is only a call to cvReleaseImage().
See the documentation of OpenCV for more information.

IplImage** ppImg : (INOUT) Valid pointer to the pointer to the structure.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int DestroyDefaultGrayCvImg(IplImage** ppImg)
{
	cvReleaseImage(ppImg);

	return EXIT_SUCCESS;
}

/*
Get the color of the pixel at line i and row j of a color IplImage. 
The function does not check any parameter.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR* pRed : (INOUT) Pointer to the red value of the pixel.
UCHAR* pGreen : (INOUT) Pointer to the green value of the pixel.
UCHAR* pBlue : (INOUT) Pointer to the blue value of the pixel.

Return : Nothing.
*/
inline void GetDefaultColorCvImgPixelQuick(IplImage* pImg, UINT i, UINT j, UCHAR* pRed, UCHAR* pGreen, UCHAR* pBlue)
{
	int index = 3*(j+pImg->width*i);

	*pRed = pImg->imageData[index+2];
	*pGreen = pImg->imageData[index+1];
	*pBlue = pImg->imageData[index];
}

/*
Get the color of the pixel at line i and row j of a color IplImage. 
The function handles automatically any out of bounds index.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
int i : (IN) Line of the pixel.
int j : (IN) Row of the pixel.
UCHAR* pRed : (INOUT) Pointer to the red value of the pixel.
UCHAR* pGreen : (INOUT) Pointer to the green value of the pixel.
UCHAR* pBlue : (INOUT) Pointer to the blue value of the pixel.

Return : Nothing.
*/
inline void GetDefaultColorCvImgPixel(IplImage* pImg, int i, int j, UCHAR* pRed, UCHAR* pGreen, UCHAR* pBlue)
{
	int index;

	//i = min(max(i, 0), (int)pImg->height-1);
	//j = min(max(j, 0), (int)pImg->width-1);

	if ((i < 0)||(i >= (int)pImg->height)||(j < 0)||(j >= (int)pImg->width))
	{
		return;
	}

	index = 3*(j+pImg->width*i);

	*pRed = pImg->imageData[index+2];
	*pGreen = pImg->imageData[index+1];
	*pBlue = pImg->imageData[index];
}

/*
Set the color of the pixel at line i and row j of a color IplImage. 
The function does not check any parameter.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR red : (IN) New red value of the pixel (between 0 and 255).
UCHAR green : (IN) New green value of the pixel (between 0 and 255).
UCHAR blue : (IN) New blue value of the pixel (between 0 and 255).

Return : Nothing.
*/
inline void SetDefaultColorCvImgPixelQuick(IplImage* pImg, UINT i, UINT j, UCHAR red, UCHAR green, UCHAR blue)
{
	int index = 3*(j+pImg->width*i);

	pImg->imageData[index+2] = red;
	pImg->imageData[index+1] = green;
	pImg->imageData[index] = blue;
}

/*
Set the pixel at line i and row j of a color IplImage. 
The function handles automatically any out of bounds index.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
int i : (IN) Line of the pixel.
int j : (IN) Row of the pixel.
UCHAR red : (IN) New red value of the pixel (between 0 and 255).
UCHAR green : (IN) New green value of the pixel (between 0 and 255).
UCHAR blue : (IN) New blue value of the pixel (between 0 and 255).

Return : Nothing.
*/
inline void SetDefaultColorCvImgPixel(IplImage* pImg, int i, int j, UCHAR red, UCHAR green, UCHAR blue)
{
	int index;

	//i = min(max(i, 0), (int)pImg->height-1);
	//j = min(max(j, 0), (int)pImg->width-1);

	if ((i < 0)||(i >= (int)pImg->height)||(j < 0)||(j >= (int)pImg->width))
	{
		return;
	}

	index = 3*(j+pImg->width*i);

	pImg->imageData[index+2] = red;
	pImg->imageData[index+1] = green;
	pImg->imageData[index] = blue;
}

/*
Get the red value of the pixel at line i and row j of a color IplImage. 
The function does not check any parameter.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.

Return : The red value (between 0 and 255).
*/
inline UCHAR GetDefaultColorCvImgPixelR(IplImage* pImg, UINT i, UINT j)
{
	return pImg->imageData[3*(j+pImg->width*i)+2];
}

/*
Get the green value of the pixel at line i and row j of a color IplImage. 
The function does not check any parameter.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.

Return : The green value (between 0 and 255).
*/
inline UCHAR GetDefaultColorCvImgPixelG(IplImage* pImg, UINT i, UINT j)
{
	return pImg->imageData[3*(j+pImg->width*i)+1];
}

/*
Get the blue value of the pixel at line i and row j of a color IplImage. 
The function does not check any parameter.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.

Return : The blue value (between 0 and 255).
*/
inline UCHAR GetDefaultColorCvImgPixelB(IplImage* pImg, UINT i, UINT j)
{
	return pImg->imageData[3*(j+pImg->width*i)];
}

/*
Set the red value of the pixel at line i and row j of a color IplImage. 
The function does not check any parameter.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR red : (IN) New red value (between 0 and 255).

Return : Nothing.
*/
inline void SetDefaultColorCvImgPixelR(IplImage* pImg, UINT i, UINT j, UCHAR red)
{
	pImg->imageData[3*(j+pImg->width*i)+2] = red;
}

/*
Set the green value of the pixel at line i and row j of a color IplImage. 
The function does not check any parameter.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR red : (IN) New green value (between 0 and 255).

Return : Nothing.
*/
inline void SetDefaultColorCvImgPixelG(IplImage* pImg, UINT i, UINT j, UCHAR green)
{
	pImg->imageData[3*(j+pImg->width*i)+1] = green;
}

/*
Set the blue value of the pixel at line i and row j of a color IplImage. 
The function does not check any parameter.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR red : (IN) New blue value (between 0 and 255).

Return : Nothing.
*/
inline void SetDefaultColorCvImgPixelB(IplImage* pImg, UINT i, UINT j, UCHAR blue)
{
	pImg->imageData[3*(j+pImg->width*i)] = blue;
}

/*
Get the gray value of the pixel at line i and row j of a gray IplImage. 
The function does not check any parameter.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.

Return : The gray value (between 0 and 255).
*/
inline UCHAR GetDefaultGrayCvImgPixelQuick(IplImage* pImg, UINT i, UINT j)
{
	return pImg->imageData[j+pImg->width*i];
}

/*
Get the gray value of the pixel at line i and row j of a gray IplImage. 
The function handles automatically any out of bounds index.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.

Return : The gray value (between 0 and 255).
*/
inline UCHAR GetDefaultGrayCvImgPixel(IplImage* pImg, int i, int j)
{
	//i = min(max(i, 0), (int)pImg->height-1);
	//j = min(max(j, 0), (int)pImg->width-1);

	if ((i < 0)||(i >= (int)pImg->height)||(j < 0)||(j >= (int)pImg->width))
	{
		return 0;
	}

	return pImg->imageData[j+pImg->width*i];
}

/*
Set the gray value of the pixel at line i and row j of a gray IplImage. 
The function does not check any parameter.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR value : (IN) New gray value (between 0 and 255).

Return : Nothing.
*/
inline void SetDefaultGrayCvImgPixelQuick(IplImage* pImg, UINT i, UINT j, UCHAR value)
{
	pImg->imageData[j+pImg->width*i] = value;
}

/*
Set the gray value of the pixel at line i and row j of a gray IplImage. 
The function handles automatically any out of bounds index.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR value : (IN) New gray value (between 0 and 255).

Return : Nothing.
*/
inline void SetDefaultGrayCvImgPixel(IplImage* pImg, int i, int j, UCHAR value)
{
	//i = min(max(i, 0), (int)pImg->height-1);
	//j = min(max(j, 0), (int)pImg->width-1);

	if ((i < 0)||(i >= (int)pImg->height)||(j < 0)||(j >= (int)pImg->width))
	{
		return;
	}

	pImg->imageData[j+pImg->width*i] = value;
}

#endif // CVCORE_H
