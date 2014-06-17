/***************************************************************************************************************:')

PGMDisp.h

Functions to display PGM picture files in windows.

Fabrice Le Bars

Created : 2009-02-12

Version status : Not finished

***************************************************************************************************************:)*/

// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#ifndef PGMDISP_H
#define PGMDISP_H

#include "PGMCore.h"

/*
Debug macros specific to PGMDisp.
*/
#ifdef _DEBUG_MESSAGES_PGMUTILS
#	define _DEBUG_MESSAGES_PGMDISP
#endif // _DEBUG_MESSAGES_PGMUTILS

#ifdef _DEBUG_WARNINGS_PGMUTILS
#	define _DEBUG_WARNINGS_PGMDISP
#endif // _DEBUG_WARNINGS_PGMUTILS

#ifdef _DEBUG_ERRORS_PGMUTILS
#	define _DEBUG_ERRORS_PGMDISP
#endif // _DEBUG_ERRORS_PGMUTILS

#ifdef _DEBUG_MESSAGES_PGMDISP
#	define PRINT_DEBUG_MESSAGE_PGMDISP(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_PGMDISP(params)
#endif // _DEBUG_MESSAGES_PGMDISP

#ifdef _DEBUG_WARNINGS_PGMDISP
#	define PRINT_DEBUG_WARNING_PGMDISP(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_PGMDISP(params)
#endif // _DEBUG_WARNINGS_PGMDISP

#ifdef _DEBUG_ERRORS_PGMDISP
#	define PRINT_DEBUG_ERROR_PGMDISP(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_PGMDISP(params)
#endif // _DEBUG_ERRORS_PGMDISP

#ifdef __BCPLUSPLUS__
#include <ExtCtrls.hpp>
#endif // __BCPLUSPLUS__

#ifdef _WIN32
/*
Display a PGM picture in a window. The picture is stretched to fit in the 
destination rectangle.
See the description of the PGMDATA structure for more information.

HPGMDATA hData : (IN) Identifier of the structure.
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
EXTERN_C void DispPGMData(HPGMDATA hData, HDC hdcWnd, int xWnd, int yWnd, UINT width, UINT height);

/*
Display a PGM picture in a window.
See the description of the PGMDATA structure for more information.

HPGMDATA hData : (IN) Identifier of the structure.
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
EXTERN_C void DispPGMDataExQuick(HPGMDATA hData, 
				   UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
				   HDC hdcWnd, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH);

inline void DispPGMDataEx(HPGMDATA hData, 
				   int xPict, int yPict, int widthSrc, int heightSrc, 
				   HDC hdcWnd, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH)
{
	xPict = min(max(xPict, 0), (int)hData->Width-1);
	yPict = min(max(yPict, 0), (int)hData->Height-1);
	widthSrc = min(max(widthSrc, 0), (int)hData->Width-xPict);
	heightSrc = min(max(heightSrc, 0), (int)hData->Height-yPict);

	DispPGMDataExQuick(hData, 
				   (UINT)xPict, (UINT)yPict, (UINT)widthSrc, (UINT)heightSrc, 
				   hdcWnd, 
				   xWnd, yWnd, widthDest, heightDest, 
				   bStretch, bFlipV, bFlipH);
}

/*
Display a PGM picture in a window. The picture is stretched to fit in the 
destination rectangle.
See the description of the PGMDATA structure for more information.
Contrary to DispPGMData(), the picture data are not interpreted the same way :
values of a pixel will be displayed as colors as defined in the table pColors.

HPGMDATA hData : (IN) Identifier of the structure.
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
PGM pixel with a value of 12 will be dispayed with the color at pColors[12].

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C void DispPGMDataSP(HPGMDATA hData, HDC hdcWnd, int xWnd, int yWnd, UINT width, UINT height, RGBCOLOR* pColors);

/*
Display a PGM picture in a window.
See the description of the PGMDATA structure for more information.
Contrary to DispPGMData(), the picture data are not interpreted the same way :
values of a pixel will be displayed as colors as defined in the table pColors.

HPGMDATA hData : (IN) Identifier of the structure.
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
PGM pixel with a value of 12 will be dispayed with the color at pColors[12].

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
EXTERN_C void DispPGMDataExSPQuick(HPGMDATA hData, 
					 UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
					 HDC hdcWnd, 
					 int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
					 BOOL bStretch, BOOL bFlipV, BOOL bFlipH, RGBCOLOR* pColors);

inline void DispPGMDataExSP(HPGMDATA hData, 
				   int xPict, int yPict, int widthSrc, int heightSrc, 
				   HDC hdcWnd, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH, RGBCOLOR* pColors)
{
	xPict = min(max(xPict, 0), (int)hData->Width-1);
	yPict = min(max(yPict, 0), (int)hData->Height-1);
	widthSrc = min(max(widthSrc, 0), (int)hData->Width-xPict);
	heightSrc = min(max(heightSrc, 0), (int)hData->Height-yPict);

	DispPGMDataExSPQuick(hData, 
				   (UINT)xPict, (UINT)yPict, (UINT)widthSrc, (UINT)heightSrc, 
				   hdcWnd, 
				   xWnd, yWnd, widthDest, heightDest, 
				   bStretch, bFlipV, bFlipH, pColors);
}
#endif // _WIN32

#ifdef __BCPLUSPLUS__
void DispPGMDataExQuick(HPGMDATA hData, 
				   UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
				   TCanvas* Canvas, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH);

inline void DispPGMDataEx(HPGMDATA hData, 
				   int xPict, int yPict, int widthSrc, int heightSrc, 
				   TCanvas* Canvas, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH)
{
	xPict = min(max(xPict, 0), (int)hData->Width-1);
	yPict = min(max(yPict, 0), (int)hData->Height-1);
	widthSrc = min(max(widthSrc, 0), (int)hData->Width-xPict);
	heightSrc = min(max(heightSrc, 0), (int)hData->Height-yPict);

	DispPGMDataExQuick(hData, 
				   (UINT)xPict, (UINT)yPict, (UINT)widthSrc, (UINT)heightSrc, 
				   Canvas, 
				   xWnd, yWnd, widthDest, heightDest, 
				   bStretch, bFlipV, bFlipH);
}

void DispPGMDataExSPQuick(HPGMDATA hData, 
				   UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
				   TCanvas* Canvas, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH, RGBCOLOR* pColors);

inline void DispPGMDataExSP(HPGMDATA hData, 
				   int xPict, int yPict, int widthSrc, int heightSrc, 
				   TCanvas* Canvas, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH, RGBCOLOR* pColors)
{
	xPict = min(max(xPict, 0), (int)hData->Width-1);
	yPict = min(max(yPict, 0), (int)hData->Height-1);
	widthSrc = min(max(widthSrc, 0), (int)hData->Width-xPict);
	heightSrc = min(max(heightSrc, 0), (int)hData->Height-yPict);

	DispPGMDataExSPQuick(hData, 
				   (UINT)xPict, (UINT)yPict, (UINT)widthSrc, (UINT)heightSrc, 
				   Canvas, 
				   xWnd, yWnd, widthDest, heightDest, 
				   bStretch, bFlipV, bFlipH, pColors);
}
#endif // __BCPLUSPLUS__

#endif // PGMDISP_H
