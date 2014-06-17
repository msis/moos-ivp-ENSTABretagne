/***************************************************************************************************************:')

PPMDisp.h

Functions to display PPM picture files in windows.

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

#ifndef PPMDISP_H
#define PPMDISP_H

#include "PPMCore.h"

/*
Debug macros specific to PPMDisp.
*/
#ifdef _DEBUG_MESSAGES_PPMUTILS
#	define _DEBUG_MESSAGES_PPMDISP
#endif // _DEBUG_MESSAGES_PPMUTILS

#ifdef _DEBUG_WARNINGS_PPMUTILS
#	define _DEBUG_WARNINGS_PPMDISP
#endif // _DEBUG_WARNINGS_PPMUTILS

#ifdef _DEBUG_ERRORS_PPMUTILS
#	define _DEBUG_ERRORS_PPMDISP
#endif // _DEBUG_ERRORS_PPMUTILS

#ifdef _DEBUG_MESSAGES_PPMDISP
#	define PRINT_DEBUG_MESSAGE_PPMDISP(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_PPMDISP(params)
#endif // _DEBUG_MESSAGES_PPMDISP

#ifdef _DEBUG_WARNINGS_PPMDISP
#	define PRINT_DEBUG_WARNING_PPMDISP(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_PPMDISP(params)
#endif // _DEBUG_WARNINGS_PPMDISP

#ifdef _DEBUG_ERRORS_PPMDISP
#	define PRINT_DEBUG_ERROR_PPMDISP(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_PPMDISP(params)
#endif // _DEBUG_ERRORS_PPMDISP

#ifdef _WIN32
#else 
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif // _WIN32

#ifdef __BCPLUSPLUS__
#include <ExtCtrls.hpp>
#endif // __BCPLUSPLUS__

#ifdef __cplusplus_cli 
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
#endif // __cplusplus_cli 

#ifdef _WIN32
/*
Display a PPM picture in a window. The picture is stretched to fit in the 
destination rectangle.
See the description of the PPMDATA structure for more information.

HPPMDATA hData : (IN) Identifier of the structure.
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
EXTERN_C void DispPPMData(HPPMDATA hData, HDC hdcWnd, int xWnd, int yWnd, UINT width, UINT height);

/*
Display a PPM picture in a window.
See the description of the PPMDATA structure for more information.

HPPMDATA hData : (IN) Identifier of the structure.
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
EXTERN_C void DispPPMDataExQuick(HPPMDATA hData, 
								 UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
								 HDC hdcWnd, 
								 int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
								 BOOL bStretch, BOOL bFlipV, BOOL bFlipH);

inline void DispPPMDataEx(HPPMDATA hData, 
							int xPict, int yPict, int widthSrc, int heightSrc, 
							HDC hdcWnd, 
							int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
							BOOL bStretch, BOOL bFlipV, BOOL bFlipH)
{
	xPict = min(max(xPict, 0), (int)hData->Width-1);
	yPict = min(max(yPict, 0), (int)hData->Height-1);
	widthSrc = min(max(widthSrc, 0), (int)hData->Width-xPict);
	heightSrc = min(max(heightSrc, 0), (int)hData->Height-yPict);

	DispPPMDataExQuick(hData, 
		(UINT)xPict, (UINT)yPict, (UINT)widthSrc, (UINT)heightSrc, 
		hdcWnd, 
		xWnd, yWnd, widthDest, heightDest, 
		bStretch, bFlipV, bFlipH);
}
#else
EXTERN_C void RenderPPMImg(Display* pDisplay, Window win, HPPMDATA hData, int x, int y, int topdown);
#endif // _WIN32

#ifdef __cplusplus_cli  
/*
Display a PPM picture in a Windows Forms control. The picture is stretched to fit 
in the destination rectangle.
See the description of the PPMDATA structure for more information.

HPPMDATA hData : (IN) Identifier of the structure.
Graphics^ g : (IN) Graphics of the control (create it with panel1->CreateGraphics() 
or get it with e->Graphics if e is a PaintEventArgs object and release it with 
delete g if you used panel1->CreateGraphics()).
int xCtrl : (IN) X coordinate of the picture in the control (in pixels, from
the upper-left corner).
int yCtrl : (IN) Y coordinate of the picture in the control (in pixels, from
the upper-left corner).
UINT width : (IN) Width of the picture in the control (in pixels, from the
upper-left corner).
UINT height : (IN) Height of the picture in the control (in pixels, from the
upper-left corner).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
void DispPPMData(HPPMDATA hData, Graphics^ g, int xCtrl, int yCtrl, UINT width, UINT height);

/*
Display a PPM picture in a Windows Forms control.
See the description of the PPMDATA structure for more information.

HPPMDATA hData : (IN) Identifier of the structure.
UINT xPict : (IN) X coordinate of the original picture from where to display 
(in pixels, from the upper-left corner).
UINT yPict : (IN) Y coordinate of the original picture from where to display 
(in pixels, from the upper-left corner).
UINT widthSrc : (IN) Width of the rectangle in the original picture to
display (in pixels, only used if bStretch = TRUE).
UINT heightSrc : (IN) Height of the rectangle in the original picture to
display (in pixels, only used if bStretch = TRUE).
Graphics^ g : (IN) Graphics of the control (create it with panel1->CreateGraphics() 
or get it with e->Graphics if e is a PaintEventArgs object and release it with 
delete g if you used panel1->CreateGraphics()).
int xCtrl : (IN) X coordinate of the picture in the control (in pixels, from
the upper-left corner).
int yCtrl : (IN) Y coordinate of the picture in the control (in pixels, from
the upper-left corner).
UINT width : (IN) Width of the picture in the control (in pixels, from the
upper-left corner).
UINT height : (IN) Height of the picture in the control (in pixels, from the
upper-left corner).
BOOL bStretch : (IN) TRUE if the picture must be stretched to fit in the control.
BOOL bFlipV : (IN) TRUE if a vertical flip must be applied at the beginning.
BOOL bFlipH : (IN) TRUE if a horizontal flip must be applied at the beginning.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
void DispPPMDataExQuick(HPPMDATA hData, 
						UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
						Graphics^ g, 
						int xCtrl, int yCtrl, UINT widthDest, UINT heightDest, 
						BOOL bStretch, BOOL bFlipV, BOOL bFlipH);

inline void DispPPMDataEx(HPPMDATA hData, 
							int xPict, int yPict, int widthSrc, int heightSrc, 
							Graphics^ g, 
							int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
							BOOL bStretch, BOOL bFlipV, BOOL bFlipH)
{
	xPict = min(max(xPict, 0), (int)hData->Width-1);
	yPict = min(max(yPict, 0), (int)hData->Height-1);
	widthSrc = min(max(widthSrc, 0), (int)hData->Width-xPict);
	heightSrc = min(max(heightSrc, 0), (int)hData->Height-yPict);

	DispPPMDataExQuick(hData, 
		(UINT)xPict, (UINT)yPict, (UINT)widthSrc, (UINT)heightSrc, 
		g, 
		xWnd, yWnd, widthDest, heightDest, 
		bStretch, bFlipV, bFlipH);
}
#endif // __cplusplus_cli  

#ifdef __BCPLUSPLUS__
void DispPPMDataExQuick(HPPMDATA hData, 
						UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
						TCanvas* Canvas, 
						int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
						BOOL bStretch, BOOL bFlipV, BOOL bFlipH);
#endif // __BCPLUSPLUS__

#endif // PPMDISP_H
