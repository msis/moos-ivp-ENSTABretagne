/***************************************************************************************************************:')

CVDisp.c

Functions to display CV picture files in windows.
2 types of IplImage format should be used in the functions :
_ a default RGB color format than can be created using CreateDefaultColorCvImg() or cvLoadImage("file.ppm", 1).
_ a default gray format than can be created using CreateDefaultGrayCvImg() or cvLoadImage("file.pgm", 0).
Note that you can use most of the OpenCV functions with such IplImage formats.

Fabrice Le Bars

Created: 2009-02-12

Version status: Not finished

***************************************************************************************************************:)*/

#include "CvDisp.h"

#ifdef __BCPLUSPLUS__
void DispDefaultColorCvImgExQuick(IplImage* pImg, 
				   UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
				   TCanvas* Canvas,
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH)
{
	UINT32* pPixels = NULL;
	int x = 0, y = 0, index = 0; // Stepping variables.
	Graphics::TBitmap* pBitmap = new Graphics::TBitmap();

	pBitmap->Width = widthSrc;
	pBitmap->Height = heightSrc;
	pBitmap->PixelFormat = pf32bit; // pf24bit ?

	// Fill the bitmap with the CV data with flips if needed.
	if (bFlipV)
	{
		if (bFlipH)
		{
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (pImg->width-1-x-xPict) * pImg->nChannels + (pImg->height-1-y-yPict) * pImg->widthStep;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index+2]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index+1]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (x+xPict) * pImg->nChannels + (pImg->height-1-y-yPict) * pImg->widthStep;

					pPixels[x] = 
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
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (pImg->width-1-x-xPict) * pImg->nChannels + (y+yPict) * pImg->widthStep;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index+2]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index+1]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (x+xPict) * pImg->nChannels + (y+yPict) * pImg->widthStep;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index+2]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index+1]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
				}
			}
		}
	}

	Canvas->CopyMode = cmSrcCopy;
	if (bStretch)
	{
		TRect Rect = TRect(xWnd, yWnd, xWnd + widthDest, yWnd + heightDest);
		Canvas->StretchDraw(Rect, pBitmap);
	}
	else
	{
		Canvas->Draw(xWnd, yWnd, pBitmap);
	}

	delete pBitmap;
}

void DispDefaultGrayCvImgExQuick(IplImage* pImg, 
				   UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
				   TCanvas* Canvas,
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH)
{
	UINT32* pPixels = NULL;
	int x = 0, y = 0, index = 0; // Stepping variables.
	Graphics::TBitmap* pBitmap = new Graphics::TBitmap();

	pBitmap->Width = widthSrc;
	pBitmap->Height = heightSrc;
	pBitmap->PixelFormat = pf32bit; // pf24bit ?

	// Fill the bitmap with the CV data with flips if needed.
	if (bFlipV)
	{
		if (bFlipH)
		{
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (pImg->width-1-x-xPict) * pImg->nChannels + (pImg->height-1-y-yPict) * pImg->widthStep;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (x+xPict) * pImg->nChannels + (pImg->height-1-y-yPict) * pImg->widthStep;

					pPixels[x] = 
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
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (pImg->width-1-x-xPict) * pImg->nChannels + (y+yPict) * pImg->widthStep;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (x+xPict) * pImg->nChannels + (y+yPict) * pImg->widthStep;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pImg->imageData[index]) << 16) |  //0x00rr0000
						((UCHAR)(pImg->imageData[index]) << 8) | //0x0000gg00
						((UCHAR)(pImg->imageData[index]));      //0x000000bb
				}
			}
		}
	}

	Canvas->CopyMode = cmSrcCopy;
	if (bStretch)
	{
		TRect Rect = TRect(xWnd, yWnd, xWnd + widthDest, yWnd + heightDest);
		Canvas->StretchDraw(Rect, pBitmap);
	}
	else
	{
		Canvas->Draw(xWnd, yWnd, pBitmap);
	}

	delete pBitmap;
}

void DispDefaultGrayCvImgExSPQuick(IplImage* pImg, 
				   UINT xPict, UINT yPict, UINT widthSrc, UINT heightSrc, 
				   TCanvas* Canvas,
				   int xWnd, int yWnd, UINT widthDest, UINT heightDest, 
				   BOOL bStretch, BOOL bFlipV, BOOL bFlipH, RGBCOLOR* pColors)
{
	UINT32* pPixels = NULL;
	int x = 0, y = 0, index = 0; // Stepping variables.
	Graphics::TBitmap* pBitmap = new Graphics::TBitmap();

	pBitmap->Width = widthSrc;
	pBitmap->Height = heightSrc;
	pBitmap->PixelFormat = pf32bit; // pf24bit ?

	// Fill the bitmap with the CV data with flips if needed.
	if (bFlipV)
	{
		if (bFlipH)
		{
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (pImg->width-1-x-xPict) * pImg->nChannels + (pImg->height-1-y-yPict) * pImg->widthStep;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pColors[pImg->imageData[index]].r) << 16) |  //0x00rr0000
						((UCHAR)(pColors[pImg->imageData[index]].g) << 8) | //0x0000gg00
						((UCHAR)(pColors[pImg->imageData[index]].b));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (x+xPict) * pImg->nChannels + (pImg->height-1-y-yPict) * pImg->widthStep;

					pPixels[x] = 
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
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (pImg->width-1-x-xPict) * pImg->nChannels + (y+yPict) * pImg->widthStep;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pColors[pImg->imageData[index]].r) << 16) |  //0x00rr0000
						((UCHAR)(pColors[pImg->imageData[index]].g) << 8) | //0x0000gg00
						((UCHAR)(pColors[pImg->imageData[index]].b));      //0x000000bb
				}
			}
		}
		else
		{
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (x+xPict) * pImg->nChannels + (y+yPict) * pImg->widthStep;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pColors[pImg->imageData[index]].r) << 16) |  //0x00rr0000
						((UCHAR)(pColors[pImg->imageData[index]].g) << 8) | //0x0000gg00
						((UCHAR)(pColors[pImg->imageData[index]].b));      //0x000000bb
				}
			}
		}
	}

	Canvas->CopyMode = cmSrcCopy;
	if (bStretch)
	{
		TRect Rect = TRect(xWnd, yWnd, xWnd + widthDest, yWnd + heightDest);
		Canvas->StretchDraw(Rect, pBitmap);
	}
	else
	{
		Canvas->Draw(xWnd, yWnd, pBitmap);
	}

	delete pBitmap;
}
#endif // __BCPLUSPLUS__
