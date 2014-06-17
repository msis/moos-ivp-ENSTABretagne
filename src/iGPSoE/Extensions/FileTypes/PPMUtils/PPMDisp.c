/***************************************************************************************************************:')

PPMDisp.c

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

#include "PPMDisp.h"

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
void DispPPMData(HPPMDATA hData, HDC hdcWnd, int xWnd, int yWnd, UINT width, UINT height)
{
	HDC hdc;  // Handle of the DC we will create.
	HBITMAP hBitmap; // Bitmap handle.
	HBITMAP hBitmapOld; // Old bitmap handle.
	BITMAPINFO bmi; // Bitmap header.
	VOID* pvBits; // Pointer to DIB section.
	int x = 0, y = 0, index = 0; // Stepping variables.
	//HRGN hRgn; // Handle of the clipping region.

	// Create a DC for our bitmap -- the source DC. 
	hdc = CreateCompatibleDC(hdcWnd);

	// Zero the memory for the bitmap info.
	ZeroMemory(&bmi, sizeof(BITMAPINFO));

	// Setup bitmap info. 
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = hData->Width;
	bmi.bmiHeader.biHeight = hData->Height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32; // Four 8-bit components.
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;

	// Create our DIB section and select the bitmap into the dc.
	hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
	hBitmapOld = (HBITMAP)SelectObject(hdc, hBitmap);

	// Fill the bitmap with the PPM data.
	for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
	{
		for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
		{
			index = 3 * (x + (hData->Height-y-1) * hData->Width);

			((UINT32*)pvBits)[x + y * bmi.bmiHeader.biWidth] = 
				((UCHAR)(0) << 24) |       //0xaa000000
				((UCHAR)(hData->Img[index + 0]) << 16) |  //0x00rr0000
				((UCHAR)(hData->Img[index + 1]) << 8) | //0x0000gg00
				((UCHAR)(hData->Img[index + 2]));      //0x000000bb
		}
	}

	// Define a clipping region. 
	//hRgn = CreateRectRgn(xWnd, yWnd, xWnd + width, yWnd + height); 
	//SelectClipRgn(hdcWnd, hRgn); 

	StretchBlt(
		hdcWnd, // handle to destination DC
		xWnd,  // x-coord of destination upper-left corner
		yWnd,  // y-coord of destination upper-left corner
		(int)width,  // width of destination rectangle
		(int)height, // height of destination rectangle
		hdc,  // handle to source DC
		0,   // x-coordinate of source upper-left corner
		0,   // y-coordinate of source upper-left corner
		(int)bmi.bmiHeader.biWidth,  // width of source rectangle
		(int)bmi.bmiHeader.biHeight, // height of source rectangle
		SRCCOPY  // raster operation code
		);

	// Do cleanup.
	//SelectClipRgn(hdcWnd, NULL); 
	//DeleteObject(hRgn);
	SelectObject(hdc, hBitmapOld);
	DeleteObject(hBitmap);
	DeleteDC(hdc);    
}

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
void DispPPMDataExQuick(HPPMDATA hData, 
				   UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
				   HDC hdcWnd, 
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH)
{
	HDC hdc;  // Handle of the DC we will create.
	HBITMAP hBitmap; // Bitmap handle.
	HBITMAP hBitmapOld; // Old bitmap handle.
	BITMAPINFO bmi; // Bitmap header.
	UINT32* pvBits = NULL; // Pointer to DIB section.
	int x = 0, y = 0, index = 0; // Stepping variables.
	//HRGN hRgn; // Handle of the clipping region.

	// Create a DC for our bitmap -- the source DC. 
	hdc = CreateCompatibleDC(hdcWnd);

	// Zero the memory for the bitmap info.
	ZeroMemory(&bmi, sizeof(BITMAPINFO));

	// Setup bitmap info. 
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = widthSrc;
	bmi.bmiHeader.biHeight = heightSrc;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32; // Four 8-bit components.
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;

	// Create our DIB section and select the bitmap into the dc.
	hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (VOID**)&pvBits, NULL, 0x0);
	hBitmapOld = (HBITMAP)SelectObject(hdc, hBitmap);

	// Fill the bitmap with the PPM data with flips if needed.
	if (bFlipV)
	{
		if (bFlipH)
		{
			for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
			{
				for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
				{
					index = 3 * ((hData->Width-1-(x+xPict)) + (y+hData->Height-1-(bmi.bmiHeader.biHeight-1)-yPict) * hData->Width);

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(hData->Img[index + 0]) << 16) |  //0x00rr0000
						((UCHAR)(hData->Img[index + 1]) << 8) | //0x0000gg00
						((UCHAR)(hData->Img[index + 2]));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
			{
				for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
				{
					index = 3 * ((x+xPict) + (y+hData->Height-1-(bmi.bmiHeader.biHeight-1)-yPict) * hData->Width);

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(hData->Img[index + 0]) << 16) |  //0x00rr0000
						((UCHAR)(hData->Img[index + 1]) << 8) | //0x0000gg00
						((UCHAR)(hData->Img[index + 2]));      //0x000000bb
				}
			}
		}
	}
	else
	{
		if (bFlipH)
		{
			for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
			{
				for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
				{
					index = 3 * ((hData->Width-1-(x+xPict)) + (bmi.bmiHeader.biHeight-1-y+yPict) * hData->Width);

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(hData->Img[index + 0]) << 16) |  //0x00rr0000
						((UCHAR)(hData->Img[index + 1]) << 8) | //0x0000gg00
						((UCHAR)(hData->Img[index + 2]));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
			{
				for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
				{
					index = 3 * ((x+xPict) + (bmi.bmiHeader.biHeight-1-y+yPict) * hData->Width);

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(hData->Img[index + 0]) << 16) |  //0x00rr0000
						((UCHAR)(hData->Img[index + 1]) << 8) | //0x0000gg00
						((UCHAR)(hData->Img[index + 2]));      //0x000000bb
				}
			}
		}
	}

	// Define a clipping region. 
	//hRgn = CreateRectRgn(xWnd, yWnd, xWnd + widthDest, yWnd + heightDest); 
	//SelectClipRgn(hdcWnd, hRgn); 

	if (bStretch)
	{
		StretchBlt(
			hdcWnd, // handle to destination DC
			xWnd,  // x-coord of destination upper-left corner
			yWnd,  // y-coord of destination upper-left corner
			(int)widthDest,  // width of destination rectangle
			(int)heightDest, // height of destination rectangle
			hdc,  // handle to source DC
			0,   // x-coordinate of source upper-left corner
			0,   // y-coordinate of source upper-left corner
			(int)widthSrc,  // width of source rectangle
			(int)heightSrc, // height of source rectangle
			SRCCOPY  // raster operation code
			);
	}
	else
	{
		BitBlt(
			hdcWnd, // handle to destination DC
			xWnd,  // x-coord of destination upper-left corner
			yWnd,  // y-coord of destination upper-left corner
			(int)widthDest,  // width of destination rectangle
			(int)heightDest, // height of destination rectangle
			hdc,  // handle to source DC
			0,   // x-coordinate of source upper-left corner
			0,   // y-coordinate of source upper-left corner
			SRCCOPY  // raster operation code
			);
	}

	// Do cleanup.
	//SelectClipRgn(hdcWnd, NULL); 
	//DeleteObject(hRgn);
	SelectObject(hdc, hBitmapOld);
	DeleteObject(hBitmap);
	DeleteDC(hdc);    
}
#else
/*
Displays a picture (from a ppm file for example) in a window.

Display* pDisplay : (IN) display of the window
Window win : (IN) window where to display the picture
HPPMDATA hData : (IN) Identifier of the PPM data structure.
int x : (IN) position in the window where to draw the picture (x-axis)
int y : (IN) position in the window where to draw the picture (y-axis)
int topdown : (IN) 1 if the picture is in a top-down format, 0 otherwise

Returns : nothing
*/
void RenderPPMImg(Display* pDisplay, Window win, HPPMDATA hData, int x, int y, int topdown)	{

	char* image_data;
	XImage* pXimage;
	unsigned int i = 0, j = 0, i0 = 0, i1 = 0;

	image_data = (char*)calloc(1, hData->NbPixels*sizeof(unsigned int));
	pXimage = XCreateImage(pDisplay, 
		DefaultVisual(pDisplay, DefaultScreen(pDisplay)), 
		DisplayPlanes(pDisplay, DefaultScreen(pDisplay)), 
		ZPixmap, 0, (char*)image_data, hData->Width, hData->Height, 8 * sizeof(unsigned int), 0) ;

	if (topdown)	{
		for (i=0; i<hData->Height; i++)	{
			for (j=0; j<hData->Width; j++)	{
				pXimage->data[0+4*j+4*hData->Width*i] = (char)hData->Img[2+3*j+3*hData->Width*(hData->Height-1-i)]; // B
				pXimage->data[1+4*j+4*hData->Width*i] = (char)hData->Img[1+3*j+3*hData->Width*(hData->Height-1-i)]; // G
				pXimage->data[2+4*j+4*hData->Width*i] = (char)hData->Img[0+3*j+3*hData->Width*(hData->Height-1-i)]; // R
				pXimage->data[3+4*j+4*hData->Width*i] = (char)0;
			}
		}
	}
	else	{
		i0 = 0;
		i1 = 0;
		for (i=0; i<hData->Height; i++)	{
			for (j=0; j<hData->Width; j++)	{
				pXimage->data[i0+0] = (char)hData->Img[i1+2]; // B
				pXimage->data[i0+1] = (char)hData->Img[i1+1]; // G
				pXimage->data[i0+2] = (char)hData->Img[i1+0]; // R
				pXimage->data[i0+3] = (char)0;
				i0+=4;
				i1+=3;
			}
		}
	}

	XPutImage(pDisplay, win, DefaultGC(pDisplay, DefaultScreen(pDisplay)), pXimage, 0, 0, x, y, hData->Width, hData->Height);	

	XDestroyImage(pXimage);
}
#endif // _WIN32
