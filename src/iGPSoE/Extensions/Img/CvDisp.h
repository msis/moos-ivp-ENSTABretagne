/***************************************************************************************************************:')

CVDisp.h

Functions to display IplImage in windows.
2 types of IplImage format should be used in the functions :
_ a default RGB color format than can be created using CreateDefaultColorCvImg() or cvLoadImage("file.ppm", 1).
_ a default gray format than can be created using CreateDefaultGrayCvImg() or cvLoadImage("file.pgm", 0).
Note that you can use most of the OpenCV functions with such IplImage formats.

Fabrice Le Bars

Created : 2009-02-12

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef CVDISP_H
#define CVDISP_H

#include "CvCore.h"

#ifdef __BCPLUSPLUS__
#include <ExtCtrls.hpp>
#endif // __BCPLUSPLUS__

#ifdef _WIN32
/*
Display an IplImage in a window. The picture is stretched to fit in the 
destination rectangle.
See the description of the IplImage structure for more information.

IplImage* pImg : (IN) Valid pointer to the structure.
HDC hdcWnd : (IN) Handle to the device context of the window (get it with 
 BeginPaint(hWnd, &ps) or GetDC(hWnd) and release it with EndPaint(hWnd, &ps)
 or ReleaseDC(hWnd, hdcWnd)).
int xWnd : (IN) X coordinate of the picture in the window (in pixels, from
 the upper-left corner).
int yWnd : (IN) Y coordinate of the picture in the window (in pixels, from
 the upper-left corner).
UINT width : (IN) Width of the picture in the window (in pixels, from the
 upper-left corner).
UINT height : (IN) Height of the picture in the window (in pixels, from the
 upper-left corner).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C void DispDefaultColorCvImg(IplImage* pImg, HDC hdcWnd, int xWnd, int yWnd, UINT width, UINT height);

/*
Display an IplImage in a window.
See the description of the IplImage structure for more information.

IplImage* pImg : (IN) Valid pointer to the structure.
UINT xPict : (IN) X coordinate of the original picture from where to display 
 (in pixels, from the upper-left corner).
UINT yPict : (IN) Y coordinate of the original picture from where to display 
 (in pixels, from the upper-left corner).
UINT widthSrc : (IN) Width of the rectangle in the original picture to
 display (in pixels, only used if bStretch = TRUE).
UINT heightSrc : (IN) Height of the rectangle in the original picture to
 display (in pixels, only used if bStretch = TRUE).
HDC hdcWnd : (IN) Handle to the device context of the window (get it with 
 BeginPaint(hWnd, &ps) or GetDC(hWnd) and release it with EndPaint(hWnd, &ps)
 or ReleaseDC(hWnd, hdcWnd)).
int xWnd : (IN) X coordinate of the picture in the window (in pixels, from
 the upper-left corner).
int yWnd : (IN) Y coordinate of the picture in the window (in pixels, from
 the upper-left corner).
UINT widthDest : (IN) Width of the picture in the window (in pixels, from the
 upper-left corner).
UINT heightDest : (IN) Height of the picture in the window (in pixels, from the
 upper-left corner).
BOOL bStretch : (IN) TRUE if the picture must be stretched to fit in the window.
BOOL bFlipV : (IN) TRUE if a vertical flip must be applied at the beginning.
BOOL bFlipH : (IN) TRUE if a horizontal flip must be applied at the beginning.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C void DispDefaultColorCvImgExQuick(IplImage* pImg, 
				   UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
				   HDC hdcWnd, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH);

inline void DispDefaultColorCvImgEx(IplImage* pImg, 
				   int xPict, int yPict, int widthSrc, int heightSrc, 
				   HDC hdcWnd, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH)
{
	xPict = min(max(xPict, 0), (int)pImg->width-1);
	yPict = min(max(yPict, 0), (int)pImg->height-1);
	widthSrc = min(max(widthSrc, 0), (int)pImg->width-xPict);
	heightSrc = min(max(heightSrc, 0), (int)pImg->height-yPict);

	DispDefaultColorCvImgExQuick(pImg, 
				   (UINT)xPict, (UINT)yPict, (UINT)widthSrc, (UINT)heightSrc, 
				   hdcWnd, 
				   xWnd, yWnd, widthDest, heightDest, 
				   bStretch, bFlipV, bFlipH);
}

/*
Display an IplImage in a window. The picture is stretched to fit in the 
destination rectangle.
See the description of the IplImage structure for more information.

IplImage* pImg : (IN) Valid pointer to the structure.
HDC hdcWnd : (IN) Handle to the device context of the window (get it with 
 BeginPaint(hWnd, &ps) or GetDC(hWnd) and release it with EndPaint(hWnd, &ps)
 or ReleaseDC(hWnd, hdcWnd)).
int xWnd : (IN) X coordinate of the picture in the window (in pixels, from
 the upper-left corner).
int yWnd : (IN) Y coordinate of the picture in the window (in pixels, from
 the upper-left corner).
UINT width : (IN) Width of the picture in the window (in pixels, from the
 upper-left corner).
UINT height : (IN) Height of the picture in the window (in pixels, from the
 upper-left corner).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C void DispDefaultGrayCvImg(IplImage* pImg, HDC hdcWnd, int xWnd, int yWnd, UINT width, UINT height);

/*
Display an IplImage in a window.
See the description of the IplImage structure for more information.

IplImage* pImg : (IN) Valid pointer to the structure.
UINT xPict : (IN) X coordinate of the original picture from where to display 
 (in pixels, from the upper-left corner).
UINT yPict : (IN) Y coordinate of the original picture from where to display 
 (in pixels, from the upper-left corner).
UINT widthSrc : (IN) Width of the rectangle in the original picture to
 display (in pixels, only used if bStretch = TRUE).
UINT heightSrc : (IN) Height of the rectangle in the original picture to
 display (in pixels, only used if bStretch = TRUE).
HDC hdcWnd : (IN) Handle to the device context of the window (get it with 
 BeginPaint(hWnd, &ps) or GetDC(hWnd) and release it with EndPaint(hWnd, &ps)
 or ReleaseDC(hWnd, hdcWnd)).
int xWnd : (IN) X coordinate of the picture in the window (in pixels, from
 the upper-left corner).
int yWnd : (IN) Y coordinate of the picture in the window (in pixels, from
 the upper-left corner).
UINT widthDest : (IN) Width of the picture in the window (in pixels, from the
 upper-left corner).
UINT heightDest : (IN) Height of the picture in the window (in pixels, from the
 upper-left corner).
BOOL bStretch : (IN) TRUE if the picture must be stretched to fit in the window.
BOOL bFlipV : (IN) TRUE if a vertical flip must be applied at the beginning.
BOOL bFlipH : (IN) TRUE if a horizontal flip must be applied at the beginning.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C void DispDefaultGrayCvImgExQuick(IplImage* pImg, 
				   UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
				   HDC hdcWnd, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH);

inline void DispDefaultGrayCvImgEx(IplImage* pImg, 
				   int xPict, int yPict, int widthSrc, int heightSrc, 
				   HDC hdcWnd, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH)
{
	xPict = min(max(xPict, 0), (int)pImg->width-1);
	yPict = min(max(yPict, 0), (int)pImg->height-1);
	widthSrc = min(max(widthSrc, 0), (int)pImg->width-xPict);
	heightSrc = min(max(heightSrc, 0), (int)pImg->height-yPict);

	DispDefaultGrayCvImgExQuick(pImg, 
				   (UINT)xPict, (UINT)yPict, (UINT)widthSrc, (UINT)heightSrc, 
				   hdcWnd, 
				   xWnd, yWnd, widthDest, heightDest, 
				   bStretch, bFlipV, bFlipH);
}

/*
Display an IplImage in a window. The picture is stretched to fit in the 
destination rectangle.
See the description of the IplImage structure for more information.
Contrary to DispIplImage(), the picture data are not interpreted the same way :
values of a pixel will be displayed as colors as defined in the table pColors.

IplImage* pImg : (IN) Valid pointer to the structure.
HDC hdcWnd : (IN) Handle to the device context of the window (get it with 
BeginPaint(hWnd, &ps) or GetDC(hWnd) and release it with EndPaint(hWnd, &ps)
or ReleaseDC(hWnd, hdcWnd)).
int xWnd : (IN) X coordinate of the picture in the window (in pixels, from
the upper-left corner).
int yWnd : (IN) Y coordinate of the picture in the window (in pixels, from
the upper-left corner).
UINT width : (IN) Width of the picture in the window (in pixels, from the
upper-left corner).
UINT height : (IN) Height of the picture in the window (in pixels, from the
upper-left corner).
RGBCOLOR* pColors : (IN) Colors to use. The table must be of 256 elements. A 
CV pixel with a value of 12 will be dispayed with the color at pColors[12].

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C void DispDefaultGrayCvImgSP(IplImage* pImg, HDC hdcWnd, int xWnd, int yWnd, UINT width, UINT height, RGBCOLOR* pColors);

/*
Display an IplImage in a window.
See the description of the IplImage structure for more information.
Contrary to DispIplImage(), the picture data are not interpreted the same way :
values of a pixel will be displayed as colors as defined in the table pColors.

IplImage* pImg : (IN) Valid pointer to the structure.
UINT xPict : (IN) X coordinate of the original picture from where to display 
(in pixels, from the upper-left corner).
UINT yPict : (IN) Y coordinate of the original picture from where to display 
(in pixels, from the upper-left corner).
UINT widthSrc : (IN) Width of the rectangle in the original picture to
display (in pixels, only used if bStretch = TRUE).
UINT heightSrc : (IN) Height of the rectangle in the original picture to
display (in pixels, only used if bStretch = TRUE).
HDC hdcWnd : (IN) Handle to the device context of the window (get it with 
BeginPaint(hWnd, &ps) or GetDC(hWnd) and release it with EndPaint(hWnd, &ps)
or ReleaseDC(hWnd, hdcWnd)).
int xWnd : (IN) X coordinate of the picture in the window (in pixels, from
the upper-left corner).
int yWnd : (IN) Y coordinate of the picture in the window (in pixels, from
the upper-left corner).
UINT widthDest : (IN) Width of the picture in the window (in pixels, from the
upper-left corner).
UINT heightDest : (IN) Height of the picture in the window (in pixels, from the
upper-left corner).
BOOL bStretch : (IN) TRUE if the picture must be stretched to fit in the window.
BOOL bFlipV : (IN) TRUE if a vertical flip must be applied at the beginning.
BOOL bFlipH : (IN) TRUE if a horizontal flip must be applied at the beginning.
RGBCOLOR* pColors : (IN) Colors to use. The table must be of 256 elements. A 
CV pixel with a value of 12 will be dispayed with the color at pColors[12].

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C void DispDefaultGrayCvImgExSPQuick(IplImage* pImg, 
					 UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
					 HDC hdcWnd, 
					 int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
					 BOOL bStretch, BOOL bFlipV, BOOL bFlipH, RGBCOLOR* pColors);

inline void DispDefaultGrayCvImgExSP(IplImage* pImg, 
				   int xPict, int yPict, int widthSrc, int heightSrc, 
				   HDC hdcWnd, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH, RGBCOLOR* pColors)
{
	xPict = min(max(xPict, 0), (int)pImg->width-1);
	yPict = min(max(yPict, 0), (int)pImg->height-1);
	widthSrc = min(max(widthSrc, 0), (int)pImg->width-xPict);
	heightSrc = min(max(heightSrc, 0), (int)pImg->height-yPict);

	DispDefaultGrayCvImgExSPQuick(pImg, 
				   (UINT)xPict, (UINT)yPict, (UINT)widthSrc, (UINT)heightSrc, 
				   hdcWnd, 
				   xWnd, yWnd, widthDest, heightDest, 
				   bStretch, bFlipV, bFlipH, pColors);
}
#endif // _WIN32

#ifdef __BCPLUSPLUS__
void DispDefaultColorCvImgExQuick(IplImage* pImg, 
				   UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
				   TCanvas* Canvas,
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH);

inline void DispDefaultColorCvImgEx(IplImage* pImg, 
				   int xPict, int yPict, int widthSrc, int heightSrc, 
				   TCanvas* Canvas, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH)
{
	xPict = min(max(xPict, 0), (int)pImg->width-1);
	yPict = min(max(yPict, 0), (int)pImg->height-1);
	widthSrc = min(max(widthSrc, 0), (int)pImg->width-xPict);
	heightSrc = min(max(heightSrc, 0), (int)pImg->height-yPict);

	DispDefaultColorCvImgExQuick(pImg, 
				   (UINT)xPict, (UINT)yPict, (UINT)widthSrc, (UINT)heightSrc, 
				   Canvas, 
				   xWnd, yWnd, widthDest, heightDest, 
				   bStretch, bFlipV, bFlipH);
}

void DispDefaultGrayCvImgExQuick(IplImage* pImg, 
				   UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
				   TCanvas* Canvas, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH);

inline void DispDefaultGrayCvImgEx(IplImage* pImg, 
				   int xPict, int yPict, int widthSrc, int heightSrc, 
				   TCanvas* Canvas, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH)
{
	xPict = min(max(xPict, 0), (int)pImg->width-1);
	yPict = min(max(yPict, 0), (int)pImg->height-1);
	widthSrc = min(max(widthSrc, 0), (int)pImg->width-xPict);
	heightSrc = min(max(heightSrc, 0), (int)pImg->height-yPict);

	DispDefaultGrayCvImgExQuick(pImg, 
				   (UINT)xPict, (UINT)yPict, (UINT)widthSrc, (UINT)heightSrc, 
				   Canvas, 
				   xWnd, yWnd, widthDest, heightDest, 
				   bStretch, bFlipV, bFlipH);
}

void DispDefaultGrayCvImgExSPQuick(IplImage* pImg, 
				   UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
				   TCanvas* Canvas, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH, RGBCOLOR* pColors);

inline void DispDefaultGrayCvImgExSP(IplImage* pImg, 
				   int xPict, int yPict, int widthSrc, int heightSrc, 
				   TCanvas* Canvas, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH, RGBCOLOR* pColors)
{
	xPict = min(max(xPict, 0), (int)pImg->width-1);
	yPict = min(max(yPict, 0), (int)pImg->height-1);
	widthSrc = min(max(widthSrc, 0), (int)pImg->width-xPict);
	heightSrc = min(max(heightSrc, 0), (int)pImg->height-yPict);

	DispDefaultGrayCvImgExSPQuick(pImg, 
				   (UINT)xPict, (UINT)yPict, (UINT)widthSrc, (UINT)heightSrc, 
				   Canvas, 
				   xWnd, yWnd, widthDest, heightDest, 
				   bStretch, bFlipV, bFlipH, pColors);
}
#endif // __BCPLUSPLUS__

#endif // CVDISP_H
