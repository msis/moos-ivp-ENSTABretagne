/***************************************************************************************************************:')

PPMDisp.c

Functions to display PPM picture files in windows.

Fabrice Le Bars

Created : 2009-02-12

Version status : Not finished

***************************************************************************************************************:)*/

#include "PPMDisp.h"

#ifdef __BCPLUSPLUS__
void DispPPMDataExQuick(HPPMDATA hData, 
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

	// Fill the bitmap with the PPM data with symetries if needed.
	if (bFlipV)
	{
		if (bFlipH)
		{
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = 3 * ((hData->Width-1-x-xPict) + (hData->Height-1-y-yPict) * hData->Width);

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(hData->Img[index + 0]) << 16) |  //0x00rr0000
						((UCHAR)(hData->Img[index + 1]) << 8) | //0x0000gg00
						((UCHAR)(hData->Img[index + 2]));      //0x000000bb
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
					index = 3 * ((x+xPict) + (hData->Height-1-y-yPict) * hData->Width);

					pPixels[x] = 
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
			for (y = pBitmap->Height-1; y >= 0; y--)
			{
				pPixels = (UINT32*)pBitmap->ScanLine[y];
				for (x = pBitmap->Width-1; x >= 0; x--)
				{
					index = 3 * ((hData->Width-1-x-xPict) + (y+yPict) * hData->Width);

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(hData->Img[index + 0]) << 16) |  //0x00rr0000
						((UCHAR)(hData->Img[index + 1]) << 8) | //0x0000gg00
						((UCHAR)(hData->Img[index + 2]));      //0x000000bb
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
					index = 3 * ((x+xPict) + (y+yPict) * hData->Width);

					pPixels[x] = 
						((UCHAR)(0) << 24) |       //0xaa000000
						((UCHAR)(hData->Img[index + 0]) << 16) |  //0x00rr0000
						((UCHAR)(hData->Img[index + 1]) << 8) | //0x0000gg00
						((UCHAR)(hData->Img[index + 2]));      //0x000000bb
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
void DispPPMData(HPPMDATA hData, Graphics^ g, int xCtrl, int yCtrl, UINT width, UINT height)
{
	try 
	{
		int x = 0, y = 0, index = 0; // Stepping variables.
		Bitmap^ bmp = gcnew Bitmap(hData->Width, hData->Height);

		// Fill the bitmap with the PPM data.
		for (y = bmp->Height-1; y >= 0; y--)
		{
			for (x = bmp->Width-1; x >= 0; x--)
			{
				index = 3 * (x + y * hData->Width);

				bmp->SetPixel(x, y, Color::FromArgb(
					hData->Img[index + 0], 
					hData->Img[index + 1], 
					hData->Img[index + 2])
					);
			}
		}

		// Create rectangle for displaying image.
		System::Drawing::Rectangle destRect = System::Drawing::Rectangle::Rectangle(
			xCtrl,  // x-coord of destination upper-left corner
			yCtrl,  // y-coord of destination upper-left corner
			(int)width,  // width of destination rectangle
			(int)height // height of destination rectangle
			);

		// Create rectangle for source image.
		System::Drawing::Rectangle srcRect = System::Drawing::Rectangle::Rectangle(
			0,   // x-coordinate of source upper-left corner
			0,   // y-coordinate of source upper-left corner
			bmp->Width,  // width of source rectangle
			bmp->Height // height of source rectangle
			);

		GraphicsUnit units = GraphicsUnit::Pixel;

		g->DrawImage(bmp, destRect, srcRect, units);

		// Do cleanup.
		delete bmp;    
	}
	catch (...) 
	{  
		// Handle all exceptions.
	}
}

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
						BOOL bStretch, BOOL bFlipV, BOOL bFlipH)
{
	try 
	{
		int x = 0, y = 0, index = 0; // Stepping variables.
		Bitmap^ bmp = gcnew Bitmap(widthSrc, heightSrc);

		// Fill the bitmap with the PPM data with flips if needed.
		if (bFlipV)
		{
			if (bFlipH)
			{
				for (y = bmp->Height-1; y >= 0; y--)
				{
					for (x = bmp->Width-1; x >= 0; x--)
					{
						index = 3 * ((hData->Width-1-(x+xPict)) + (hData->Height-1-(y+yPict)) * hData->Width);

						bmp->SetPixel(x, y, Color::FromArgb(
							hData->Img[index + 0], 
							hData->Img[index + 1], 
							hData->Img[index + 2])
							);
					}
				}
			}
			else
			{
				for (y = bmp->Height-1; y >= 0; y--)
				{
					for (x = bmp->Width-1; x >= 0; x--)
					{
						index = 3 * ((x+xPict) + (hData->Height-1-(y+yPict)) * hData->Width);

						bmp->SetPixel(x, y, Color::FromArgb(
							hData->Img[index + 0], 
							hData->Img[index + 1], 
							hData->Img[index + 2])
							);
					}
				}
			}
		}
		else
		{
			if (bFlipH)
			{
				for (y = bmp->Height-1; y >= 0; y--)
				{
					for (x = bmp->Width-1; x >= 0; x--)
					{
						index = 3 * ((hData->Width-1-(x+xPict)) + (y+yPict) * hData->Width);

						bmp->SetPixel(x, y, Color::FromArgb(
							hData->Img[index + 0], 
							hData->Img[index + 1], 
							hData->Img[index + 2])
							);
					}
				}
			}
			else
			{
				for (y = bmp->Height-1; y >= 0; y--)
				{
					for (x = bmp->Width-1; x >= 0; x--)
					{
						index = 3 * ((x+xPict) + (y+yPict) * hData->Width);

						bmp->SetPixel(x, y, Color::FromArgb(
							hData->Img[index + 0], 
							hData->Img[index + 1], 
							hData->Img[index + 2])
							);
					}
				}
			}
		}

		if (bStretch)
		{
			// Create rectangle for displaying image.
			System::Drawing::Rectangle destRect = System::Drawing::Rectangle::Rectangle(
				xCtrl,  // x-coord of destination upper-left corner
				yCtrl,  // y-coord of destination upper-left corner
				(int)widthDest,  // width of destination rectangle
				(int)heightDest // height of destination rectangle
				);

			// Create rectangle for source image.
			System::Drawing::Rectangle srcRect = System::Drawing::Rectangle::Rectangle(
				0,   // x-coordinate of source upper-left corner
				0,   // y-coordinate of source upper-left corner
				(int)widthSrc,  // width of source rectangle
				(int)heightSrc // height of source rectangle
				);

			GraphicsUnit units = GraphicsUnit::Pixel;

			g->DrawImage(bmp, destRect, srcRect, units);
		}
		else
		{
			g->DrawImage(bmp,
				xCtrl,  // x-coord of destination upper-left corner
				yCtrl  // y-coord of destination upper-left corner
				);
		}

		// Do cleanup.
		delete bmp;    
	}
	catch (...) 
	{  
		// Handle all exceptions.
	}
}
#endif // __cplusplus_cli 
