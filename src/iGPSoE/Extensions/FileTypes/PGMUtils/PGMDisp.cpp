/***************************************************************************************************************:')

PGMDisp.c

Functions to display PGM picture files in windows.

Fabrice Le Bars

Created : 2009-02-12

Version status : Not finished

***************************************************************************************************************:)*/

#include "PGMDisp.h"

#ifdef __BCPLUSPLUS__
void DispPGMDataExQuick(HPGMDATA hData, 
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

	// Fill the bitmap with the PGM data with flips if needed.
	if (bFlipV)
	{
		if (bFlipH)
		{
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (hData->Width-1-x-xPict) + (hData->Height-1-y-yPict) * hData->Width;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(hData->Img[index]) << 16) |  //0x00rr0000
						((UCHAR)(hData->Img[index]) << 8) | //0x0000gg00
						((UCHAR)(hData->Img[index]));      //0x000000bb
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
					index = (x+xPict) + (hData->Height-1-y-yPict) * hData->Width;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(hData->Img[index]) << 16) |  //0x00rr0000
						((UCHAR)(hData->Img[index]) << 8) | //0x0000gg00
						((UCHAR)(hData->Img[index]));      //0x000000bb
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
					index = (hData->Width-1-x-xPict) + (y+yPict) * hData->Width;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(hData->Img[index]) << 16) |  //0x00rr0000
						((UCHAR)(hData->Img[index]) << 8) | //0x0000gg00
						((UCHAR)(hData->Img[index]));      //0x000000bb
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
					index = (x+xPict) + (y+yPict) * hData->Width;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(hData->Img[index]) << 16) |  //0x00rr0000
						((UCHAR)(hData->Img[index]) << 8) | //0x0000gg00
						((UCHAR)(hData->Img[index]));      //0x000000bb
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

void DispPGMDataExSPQuick(HPGMDATA hData, 
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

	// Fill the bitmap with the PGM data with flips if needed.
	if (bFlipV)
	{
		if (bFlipH)
		{
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = (hData->Width-1-x-xPict) + (hData->Height-1-y-yPict) * hData->Width;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pColors[hData->Img[index]].r) << 16) |  //0x00rr0000
						((UCHAR)(pColors[hData->Img[index]].g) << 8) | //0x0000gg00
						((UCHAR)(pColors[hData->Img[index]].b));      //0x000000bb
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
					index = (x+xPict) + (hData->Height-1-y-yPict) * hData->Width;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pColors[hData->Img[index]].r) << 16) |  //0x00rr0000
						((UCHAR)(pColors[hData->Img[index]].g) << 8) | //0x0000gg00
						((UCHAR)(pColors[hData->Img[index]].b));      //0x000000bb
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
					index = (hData->Width-1-x-xPict) + (y+yPict) * hData->Width;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pColors[hData->Img[index]].r) << 16) |  //0x00rr0000
						((UCHAR)(pColors[hData->Img[index]].g) << 8) | //0x0000gg00
						((UCHAR)(pColors[hData->Img[index]].b));      //0x000000bb
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
					index = (x+xPict) + (y+yPict) * hData->Width;

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(pColors[hData->Img[index]].r) << 16) |  //0x00rr0000
						((UCHAR)(pColors[hData->Img[index]].g) << 8) | //0x0000gg00
						((UCHAR)(pColors[hData->Img[index]].b));      //0x000000bb
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
