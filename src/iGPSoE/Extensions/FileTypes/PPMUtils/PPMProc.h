/***************************************************************************************************************:')

PPMProc.h

PPM data processing.

Fabrice Le Bars

Created : 2009-02-12

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef PPMPROC_H
#define PPMPROC_H

#include "PPMCore.h"

/*
Debug macros specific to PPMProc.
*/
#ifdef _DEBUG_MESSAGES_PPMUTILS
#	define _DEBUG_MESSAGES_PPMPROC
#endif // _DEBUG_MESSAGES_PPMUTILS

#ifdef _DEBUG_WARNINGS_PPMUTILS
#	define _DEBUG_WARNINGS_PPMPROC
#endif // _DEBUG_WARNINGS_PPMUTILS

#ifdef _DEBUG_ERRORS_PPMUTILS
#	define _DEBUG_ERRORS_PPMPROC
#endif // _DEBUG_ERRORS_PPMUTILS

#ifdef _DEBUG_MESSAGES_PPMPROC
#	define PRINT_DEBUG_MESSAGE_PPMPROC(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_PPMPROC(params)
#endif // _DEBUG_MESSAGES_PPMPROC

#ifdef _DEBUG_WARNINGS_PPMPROC
#	define PRINT_DEBUG_WARNING_PPMPROC(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_PPMPROC(params)
#endif // _DEBUG_WARNINGS_PPMPROC

#ifdef _DEBUG_ERRORS_PPMPROC
#	define PRINT_DEBUG_ERROR_PPMPROC(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_PPMPROC(params)
#endif // _DEBUG_ERRORS_PPMPROC

/*
Smooth the pixel at line i and row j of a PPM picture using the values of 4 
neighbors. 
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (IN) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR* pRed : (INOUT) Valid pointer to the new red value of the pixel.
UCHAR* pGreen : (INOUT) Valid pointer to the new green value of the pixel.
UCHAR* pBlue : (INOUT) Valid pointer to the new blue value of the pixel.

Return : Nothing.
*/
inline void Smooth4PPMImgPixel(HPPMDATA hData, UINT i, UINT j, UCHAR* pRed, UCHAR* pGreen, UCHAR* pBlue)
{
	// Color values of the current pixel.
	int index = 3*(j+hData->Width*i);
	double red = (double)hData->Img[index];
	double green = (double)hData->Img[index + 1];
	double blue = (double)hData->Img[index + 2];

	int n = 1; // Number of pixels used to smooth.

	if (i > 0) 
	{ 
		index = 3*(j+hData->Width*(i-1));
		red += (double)hData->Img[index]; 
		green += (double)hData->Img[index + 1]; 
		blue += (double)hData->Img[index + 2]; 
		n++;
	}

	if (i < hData->Height-1)
	{
		index = 3*(j+hData->Width*(i+1));
		red += (double)hData->Img[index]; 
		green += (double)hData->Img[index + 1]; 
		blue += (double)hData->Img[index + 2]; 
		n++;
	}

	if (j > 0) 
	{ 
		index = 3*(j-1+hData->Width*i);
		red += (double)hData->Img[index]; 
		green += (double)hData->Img[index + 1]; 
		blue += (double)hData->Img[index + 2]; 
		n++;
	}

	if (j < hData->Width-1)
	{
		index = 3*(j+1+hData->Width*i);
		red += (double)hData->Img[index]; 
		green += (double)hData->Img[index + 1]; 
		blue += (double)hData->Img[index + 2]; 
		n++;
	}

	red /= (double)n;
	green /= (double)n;
	blue /= (double)n;

	*pRed = (UCHAR)red;
	*pGreen = (UCHAR)green;
	*pBlue = (UCHAR)blue;
}

/*
Smooth the pixel at line i and row j of a PPM picture using the values of 8 
neighbors. 
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (IN) Identifier of the structure.
UINT i : (IN) Line of the pixel.
UINT j : (IN) Row of the pixel.
UCHAR* pRed : (INOUT) Valid pointer to the new red value of the pixel.
UCHAR* pGreen : (INOUT) Valid pointer to the new green value of the pixel.
UCHAR* pBlue : (INOUT) Valid pointer to the new blue value of the pixel.

Return : Nothing.
*/
//inline void Smooth8PPMImgPixel(HPPMDATA hData, UINT i, UINT j, UCHAR* pRed, UCHAR* pGreen, UCHAR* pBlue)
//{
// To be implemented.
//}

/*
Convert a top-down picture to a bottom-up picture (or the other way round).

HPPMDATA hData : (INOUT) Identifier of the structure.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int TopDown2BottomUpPPM(HPPMDATA hData)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	UCHAR* p1 = (UCHAR*)malloc(hData->Size);

	if (p1 == NULL)
	{
		return EXIT_FAILURE;
	}

	memcpy(p1, hData->Img, hData->Size);

	for (i = hData->Height-1; i >= 0; i--)
	{
		for (j = hData->Width-1; j >= 0; j--)
		{
			k = 3*(j+hData->Width*i);
			l = 3*(j+hData->Width*(hData->Height-1-i));
			hData->Img[k] = p1[l];
			hData->Img[k+1] = p1[l+1];
			hData->Img[k+2] = p1[l+2];
		}
	}

	free(p1);p1 = NULL;

	return EXIT_SUCCESS;
}

/*
Convert a BGR picture to RGB (or the other way round).

HPPMDATA hData : (INOUT) Identifier of the structure.

Return : Nothing.
*/
inline void SwapRedBluePPM(HPPMDATA hData)
{
	UCHAR c = 0;
	UINT i = 0;

	while (i <= hData->Size-3)
	{
		c = hData->Img[i]; hData->Img[i] = hData->Img[i+2]; hData->Img[i+2] = c;
		i += 3;
	}
}

/*
Return if the main colour in the picture is the colour in parameters.

HPPMDATA hData : (IN) Identifier of the structure.
UCHAR r : (IN) Red value of the colour (between 0 and 255).
UCHAR g : (IN) Green value of the colour (between 0 and 255).
UCHAR b : (IN) Blue value of the colour (between 0 and 255).
BOOL* bFound : (INOUT) TRUE if it is the main colour, FALSE otherwise.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int UniformColorDetectionPPM(HPPMDATA hData, UCHAR r, UCHAR g, UCHAR b, BOOL* bFound)
{
	int i = 0, j = 0, k = 0;
	double r0 = 0, g0 = 0, b0 = 0;

	for (i = hData->Height-1; i >= 0; i--)
	{
		for (j = hData->Width-1; j >= 0; j--)
		{
			k = 3*(j+hData->Width*i);
			r0 += (double)hData->Img[k];
			g0 += (double)hData->Img[k+1];
			b0 += (double)hData->Img[k+2];
		}
	}
	r0 /= (double)(hData->Width*hData->Height);
	g0 /= (double)(hData->Width*hData->Height);
	b0 /= (double)(hData->Width*hData->Height);

	if (
		(r0 <= (double)(r+50))&&(r0 >= (double)(r-50))&&	
		(g0 <= (double)(g+50))&&(g0 >= (double)(g-50))&&
		(b0 <= (double)(b+50))&&(b0 >= (double)(b-50))
		)	
	{
		// It is the right color.
		*bFound = TRUE;
	}
	else	
	{
		*bFound = FALSE;
	}

	return EXIT_SUCCESS;
}

/*
Optimize the brightness in a picture.

HPPMDATA hData : (INOUT) Identifier of the structure.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int OptimizeBrightnessPPM(HPPMDATA hData)
{
	UINT i = 0, r = 0, g = 0, b = 0;
	UCHAR Maxr = 0, Maxg = 0, Maxb = 0;
	UCHAR Minr = (UCHAR)hData->ColorDepth, Ming = (UCHAR)hData->ColorDepth, Minb = (UCHAR)hData->ColorDepth; // ColorDepth = 255 in general

	// Look for the max and the min for R,G,B.
	while (i <= hData->Size-3)
	{
		Maxr = max(hData->Img[i+0], Maxr);
		Maxg = max(hData->Img[i+1], Maxg);
		Maxb = max(hData->Img[i+2], Maxb);

		Minr = min(hData->Img[i+0], Minr);
		Ming = min(hData->Img[i+1], Ming);
		Minb = min(hData->Img[i+2], Minb);

		i += 3;
	}

	if ( 
		((Maxr-Minr) == 0)|| 
		((Maxg-Ming) == 0)|| 
		((Maxb-Minb) == 0)
		)	
	{
		return EXIT_FAILURE;
	}

	// Change R,G,B so that the min be 0 and the max 255.
	i = 0;
	while (i <= hData->Size-3)
	{
		r = (hData->Img[i+0]-Minr)*hData->ColorDepth/(Maxr-Minr);
		g = (hData->Img[i+1]-Ming)*hData->ColorDepth/(Maxg-Ming);
		b = (hData->Img[i+2]-Minb)*hData->ColorDepth/(Maxb-Minb);
		hData->Img[i+0] = (UCHAR)r;
		hData->Img[i+1] = (UCHAR)g;
		hData->Img[i+2] = (UCHAR)b;
		i += 3;
	}

	return EXIT_SUCCESS;
}

/*
Return the estimated position and dimensions of an orange ball in a picture using RGB.

HPPMDATA hData : (IN) Identifier of the structure.
int* pX : (INOUT) Valid pointer to the estimated position of the ball (x-axis, between 0 and width).
int* pY : (INOUT) Valid pointer to the estimated position of the ball (y-axis, between 0 and height).
int* pW : (INOUT) Valid pointer to the estimated width.
int* pH : (INOUT) Valid pointer to the estimated height.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int OrangeBallDetectionRGBPPM(HPPMDATA hData, int* pX, int* pY, int* pW, int* pH)
{
	UINT i = 0, j = 0, k = 0, r = 0, g = 0, b = 0;
	BOOL bFound = FALSE, bObj = FALSE;
	int objwidth = 0, objheight = 0;
	int objX = 0, objY = 0; // Center of the object.
	int x0 = 0, x1 = 0, y0 = 0, y1 = 0;

	for (i = 0; i < hData->Height; i++)
	{
		for (j = 0; j < hData->Width; j++)
		{
			k = 3*(j+hData->Width*i);
			r = hData->Img[k];
			g = hData->Img[k+1];
			b = hData->Img[k+2];

			if ((r > 200)&&(g < 150)&&(b < 150))
			{
				// We are on the object.
				if (!bObj)
				{
					// It is the beginning of the object at the current line, we were not on it before.
					bObj = TRUE; // We are entering in the object.
					bFound = TRUE; // Indicate that we have found something in the picture.
					x0 = j; // Position of the beginning of the object at the current line.
				} // Else we are somewhere on the object.
				else 
				{
					if (j == hData->Width-1)
					{ 
						// We are on the object but we are at the end of the line, so we do as if we were at the end of the object.
						bObj = FALSE; // We are leaving the object.
						x1 = j; // Position of the end of the object at the current line.
						if (x1-x0 > objwidth)
						{
							// When objwidth is max, we are at the center of the object.
							objwidth = x1-x0;
							objX = x0+objwidth/2;
							if (objY == 0)
							{
								// We are at the beginning of the object on the Y axis.
								y0 = i;
							}
							y1 = i;
							objheight = 2*(y1-y0); // *2 because the last time we will be here, we will be on the center
							objY = i; // (y0 is the beginning of the object and the last value of y1 will be the center).
						}
					}
				}
			}
			else
			{
				if (bObj)
				{
					// We were on the object just before, now we are not on it any more.
					bObj = FALSE; // We are leaving the object.
					x1 = j; // Position of the end of the object at the current line.
					if (x1-x0 > objwidth)
					{
						// When objwidth is max, we are at the center of the object.
						objwidth = x1-x0;
						objX = x0+objwidth/2;
						if (objY == 0)
						{
							// We are at the beginning of the object on the Y axis.
							y0 = i;
						}
						y1 = i;
						objheight = 2*(y1-y0); // *2 because the last time we will be here, we will be on the center
						objY = i; // (y0 is the beginning of the object and the last value of y1 will be the center).
					}
				}
			}
		}
	}

	if (bFound)
	{
		// Now we have the center of the object at (objX,objY), assuming that (0,0) is in the top-left corner.
		*pX = objX;
		*pY = objY;
		*pW = objwidth;
		*pH = objheight;
	}
	else
	{
		// We did not see any orange ball.
		*pX = -1;
		*pY = -1;
		*pW = -1;
		*pH = -1;
	}

	return EXIT_SUCCESS;
}

/*
Reduce the number of different coulours used in a picture.
Warning : this does not change the format or the size of the picture data.

HPPMDATA hData : (INOUT) Identifier of the structure.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReduceColoursPPM(HPPMDATA hData)
{
	UINT i = 0, n = 128, a = 0, b = 0, c = 0;

	while (i <= hData->Size-3)
	{
		a = (hData->Img[i+0]+(hData->ColorDepth+1)/2)/n;
		b = (hData->Img[i+1]+(hData->ColorDepth+1)/2)/n;
		c = (hData->Img[i+2]+(hData->ColorDepth+1)/2)/n;
		a = a*n;
		b = b*n;
		c = c*n;
		hData->Img[i+0] = (UCHAR)(a-(hData->ColorDepth+1)/2);
		hData->Img[i+1] = (UCHAR)(b-(hData->ColorDepth+1)/2);
		hData->Img[i+2] = (UCHAR)(c-(hData->ColorDepth+1)/2);
		i += 3;
	}

	return EXIT_SUCCESS;
}

/*
Convert a coloured picture in gray scale.
Warning : this does not change the format or the size of the picture data.

HPPMDATA hData : (INOUT) Identifier of the structure.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GrayScalePPM(HPPMDATA hData)
{
	UINT i = 0, r = 0, g = 0, b = 0, gray = 0;

	while (i <= hData->Size-3)
	{
		r = hData->Img[i+0];
		g = hData->Img[i+1];
		b = hData->Img[i+2];
		gray = (r+g+b)/3;
		hData->Img[i+0] = (unsigned char)gray;
		hData->Img[i+1] = (unsigned char)gray;
		hData->Img[i+2] = (unsigned char)gray;
		i += 3;
	}

	return EXIT_SUCCESS;
}

#endif // PPMPROC_H
