/***************************************************************************************************************:')

CVDisp.c

Functions to display CV picture files in windows.
2 types of IplImage format should be used in the functions :
_ a default RGB color format than can be created using CreateDefaultColorCvImg() or cvLoadImage("file.ppm", 1).
_ a default gray format than can be created using CreateDefaultGrayCvImg() or cvLoadImage("file.pgm", 0).
Note that you can use most of the OpenCV functions with such IplImage formats.

Fabrice Le Bars

Created : 2009-02-12

Version status : Not finished

***************************************************************************************************************:)*/

// Prevent Visual Studio Intellisense from defining _WIN32 when we use it to edit Linux
// code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__

#include "CvDisp.h"

#ifdef _WIN32
/*
Display an IplImage in a window. The picture is stretched to fit in the 
destination rectangle.
See the documentation of OpenCV for more information.

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
void DispDefaultColorCvImg(IplImage* pImg, HDC hdcWnd, int xWnd, int yWnd, UINT width, UINT height)
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
	bmi.bmiHeader.biWidth = pImg->width;
	bmi.bmiHeader.biHeight = pImg->height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32; // Four 8-bit components.
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;

	// Create our DIB section and select the bitmap into the dc.
	hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
	hBitmapOld = (HBITMAP)SelectObject(hdc, hBitmap);

	// Fill the bitmap with the CV data.
	for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
	{
		for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
		{
			index = x * pImg->nChannels + (pImg->height-y-1) * pImg->widthStep;

			((UINT32*)pvBits)[x + y * bmi.bmiHeader.biWidth] = 
				(((UCHAR)0) << 24) |       //0xaa000000
				(((UCHAR)(pImg->imageData[index+2])) << 16) |  //0x00rr0000
				(((UCHAR)(pImg->imageData[index+1])) << 8) | //0x0000gg00
				(((UCHAR)(pImg->imageData[index])));      //0x000000bb
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
Display an IplImage in a window.
See the documentation of OpenCV for more information.

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
void DispDefaultColorCvImgExQuick(IplImage* pImg, 
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

	// Fill the bitmap with the CV data with flips if needed.
	if (bFlipV)
	{
		if (bFlipH)
		{
			for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
			{
				for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
				{
					index = (pImg->width-1-(x+xPict)) * pImg->nChannels + (y+pImg->height-1-(bmi.bmiHeader.biHeight-1)-yPict) * pImg->widthStep;

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index+2]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index+1]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
			{
				for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
				{
					index = (x+xPict) * pImg->nChannels + (y+pImg->height-1-(bmi.bmiHeader.biHeight-1)-yPict) * pImg->widthStep;

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index+2]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index+1]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
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
					index = (pImg->width-1-(x+xPict)) * pImg->nChannels + (bmi.bmiHeader.biHeight-1-y+yPict) * pImg->widthStep;

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index+2]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index+1]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
			{
				for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
				{
					index = (x+xPict) * pImg->nChannels + (bmi.bmiHeader.biHeight-1-y+yPict) * pImg->widthStep;

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index+2]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index+1]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
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

/*
Display an IplImage in a window. The picture is stretched to fit in the 
destination rectangle.
See the documentation of OpenCV for more information.

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
void DispDefaultGrayCvImg(IplImage* pImg, HDC hdcWnd, int xWnd, int yWnd, UINT width, UINT height)
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
	bmi.bmiHeader.biWidth = pImg->width;
	bmi.bmiHeader.biHeight = pImg->height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32; // Four 8-bit components.
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;

	// Create our DIB section and select the bitmap into the dc.
	hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
	hBitmapOld = (HBITMAP)SelectObject(hdc, hBitmap);

	// Fill the bitmap with the CV data.
	for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
	{
		for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
		{
			index = x * pImg->nChannels + (pImg->height-y-1) * pImg->widthStep;

			((UINT32*)pvBits)[x + y * bmi.bmiHeader.biWidth] = 
				(((UCHAR)0) << 24) |       //0xaa000000
				(((UCHAR)(pImg->imageData[index])) << 16) |  //0x00rr0000
				(((UCHAR)(pImg->imageData[index])) << 8) | //0x0000gg00
				(((UCHAR)(pImg->imageData[index])));      //0x000000bb
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
Display an IplImage in a window.
See the documentation of OpenCV for more information.

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
void DispDefaultGrayCvImgExQuick(IplImage* pImg, 
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

	// Fill the bitmap with the CV data with flips if needed.
	if (bFlipV)
	{
		if (bFlipH)
		{
			for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
			{
				for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
				{
					index = (pImg->width-1-(x+xPict)) * pImg->nChannels + (y+pImg->height-1-(bmi.bmiHeader.biHeight-1)-yPict) * pImg->widthStep;

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
			{
				for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
				{
					index = (x+xPict) * pImg->nChannels + (y+pImg->height-1-(bmi.bmiHeader.biHeight-1)-yPict) * pImg->widthStep;

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
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
					index = (pImg->width-1-(x+xPict)) * pImg->nChannels + (bmi.bmiHeader.biHeight-1-y+yPict) * pImg->widthStep;

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
			{
				for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
				{
					index = (x+xPict) * pImg->nChannels + (bmi.bmiHeader.biHeight-1-y+yPict) * pImg->widthStep;

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
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

/*
Display an IplImage in a window. The picture is stretched to fit in the 
destination rectangle.
See the documentation of OpenCV for more information.
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
void DispDefaultGrayCvImgSP(IplImage* pImg, HDC hdcWnd, int xWnd, int yWnd, UINT width, UINT height, RGBCOLOR* pColors)
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
	bmi.bmiHeader.biWidth = pImg->width;
	bmi.bmiHeader.biHeight = pImg->height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32; // Four 8-bit components.
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;

	// Create our DIB section and select the bitmap into the dc.
	hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
	hBitmapOld = (HBITMAP)SelectObject(hdc, hBitmap);

	// Fill the bitmap with the CV data.
	for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
	{
		for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
		{
			index = x * pImg->nChannels + (pImg->height-y-1) * pImg->widthStep;

			((UINT32*)pvBits)[x + y * bmi.bmiHeader.biWidth] = 
				((UCHAR)(0) << 24) |       //0xaa000000
				((UCHAR)(pColors[pImg->imageData[index]].r) << 16) |  //0x00rr0000
				((UCHAR)(pColors[pImg->imageData[index]].g) << 8) | //0x0000gg00
				((UCHAR)(pColors[pImg->imageData[index]].b));      //0x000000bb
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
Display an IplImage in a window.
See the documentation of OpenCV for more information.
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
void DispDefaultGrayCvImgExSPQuick(IplImage* pImg, 
					 UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
					 HDC hdcWnd, 
					 int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
					 BOOL bStretch, BOOL bFlipV, BOOL bFlipH, RGBCOLOR* pColors)
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

	// Fill the bitmap with the CV data with flips if needed.
	if (bFlipV)
	{
		if (bFlipH)
		{
			for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
			{
				for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
				{
					index = (pImg->width-1-(x+xPict)) * pImg->nChannels + (y+pImg->height-1-(bmi.bmiHeader.biHeight-1)-yPict) * pImg->widthStep;

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pColors[pImg->imageData[index]].r) << 16) |  //0x00rr0000
						((UCHAR)(pColors[pImg->imageData[index]].g) << 8) | //0x0000gg00
						((UCHAR)(pColors[pImg->imageData[index]].b));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
			{
				for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
				{
					index = (x+xPict) * pImg->nChannels + (y+pImg->height-1-(bmi.bmiHeader.biHeight-1)-yPict) * pImg->widthStep;

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pColors[pImg->imageData[index]].r) << 16) |  //0x00rr0000
						((UCHAR)(pColors[pImg->imageData[index]].g) << 8) | //0x0000gg00
						((UCHAR)(pColors[pImg->imageData[index]].b));      //0x000000bb
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
					index = (pImg->width-1-(x+xPict)) * pImg->nChannels + (bmi.bmiHeader.biHeight-1-y+yPict) * pImg->widthStep;

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pColors[pImg->imageData[index]].r) << 16) |  //0x00rr0000
						((UCHAR)(pColors[pImg->imageData[index]].g) << 8) | //0x0000gg00
						((UCHAR)(pColors[pImg->imageData[index]].b));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
			{
				for (x = bmi.bmiHeader.biWidth-1; x >= 0; x--)
				{
					index = (x+xPict) * pImg->nChannels + (bmi.bmiHeader.biHeight-1-y+yPict) * pImg->widthStep;

					pvBits[x + y * bmi.bmiHeader.biWidth] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pColors[pImg->imageData[index]].r) << 16) |  //0x00rr0000
						((UCHAR)(pColors[pImg->imageData[index]].g) << 8) | //0x0000gg00
						((UCHAR)(pColors[pImg->imageData[index]].b));      //0x000000bb
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
#endif // _WIN32
