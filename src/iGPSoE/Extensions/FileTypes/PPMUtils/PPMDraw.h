/***************************************************************************************************************:')

PPMDraw.h

Drawing functions for PPM data.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef PPMDRAW_H
#define PPMDRAW_H

#include "PPMCore.h"

/*
Debug macros specific to PPMDraw.
*/
#ifdef _DEBUG_MESSAGES_PPMUTILS
#	define _DEBUG_MESSAGES_PPMDRAW
#endif // _DEBUG_MESSAGES_PPMUTILS

#ifdef _DEBUG_WARNINGS_PPMUTILS
#	define _DEBUG_WARNINGS_PPMDRAW
#endif // _DEBUG_WARNINGS_PPMUTILS

#ifdef _DEBUG_ERRORS_PPMUTILS
#	define _DEBUG_ERRORS_PPMDRAW
#endif // _DEBUG_ERRORS_PPMUTILS

#ifdef _DEBUG_MESSAGES_PPMDRAW
#	define PRINT_DEBUG_MESSAGE_PPMDRAW(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_PPMDRAW(params)
#endif // _DEBUG_MESSAGES_PPMDRAW

#ifdef _DEBUG_WARNINGS_PPMDRAW
#	define PRINT_DEBUG_WARNING_PPMDRAW(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_PPMDRAW(params)
#endif // _DEBUG_WARNINGS_PPMDRAW

#ifdef _DEBUG_ERRORS_PPMDRAW
#	define PRINT_DEBUG_ERROR_PPMDRAW(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_PPMDRAW(params)
#endif // _DEBUG_ERRORS_PPMDRAW

/*
Clear a PPM picture (set all the pixels to the same gray value). 
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UCHAR value : (IN) Gray value of the picture.

Return : Nothing.
*/
inline void ClearPPMImgGray(HPPMDATA hData, UCHAR value)
{
	memset(hData->Img, value, hData->Size);
}

/*
Clear a PPM picture (set all the pixels to the same color). 
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UCHAR red : (IN) Red value of the color of the picture (between 0 and 255).
UCHAR green : (IN) Green value of the color of the picture (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the picture (between 0 and 255).

Return : Nothing.
*/
inline void ClearPPMImg(HPPMDATA hData, UCHAR red, UCHAR green, UCHAR blue)
{
	int i, j;
	UINT index;

	for (i = hData->Height-1; i >= 0; i--)
	{
		for (j = hData->Width-1; j >= 0; j--)
		{	
			index = 3*(j+hData->Width*i);

			hData->Img[index] = red;
			hData->Img[index+1] = green;
			hData->Img[index+2] = blue;
		}
	}
}

/*
Draw a rectangle in a PPM picture and fill it.
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i0 : (IN) Line of the up-left pixel of the rectangle.
UINT j0 : (IN) Row of the up-left pixel of the rectangle.
UINT i1 : (IN) Line of the down-right pixel of the rectangle.
UINT j1 : (IN) Row of the down-right pixel of the rectangle.
UCHAR red : (IN) Red value of the color of the rectangle (between 0 and 255).
UCHAR green : (IN) Green value of the color of the rectangle (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void FillRectanglePPMQuick(HPPMDATA hData, UINT i0, UINT j0, UINT i1, UINT j1, UCHAR red, UCHAR green, UCHAR blue)
{
	UINT i, j, index;

	for (i = i0; i <= i1; i++)
	{
		for (j = j0; j <= j1; j++)
		{
			index = 3*(j+hData->Width*i);

			hData->Img[index] = red;
			hData->Img[index+1] = green;
			hData->Img[index+2] = blue;
		}
	}
}

/*
Draw a rectangle in a PPM picture and fill it.
The function handles automatically any out of bounds index.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the up-left pixel of the rectangle.
int j0 : (IN) Row of the up-left pixel of the rectangle.
int i1 : (IN) Line of the down-right pixel of the rectangle.
int j1 : (IN) Row of the down-right pixel of the rectangle.
UCHAR red : (IN) Red value of the color of the rectangle (between 0 and 255).
UCHAR green : (IN) Green value of the color of the rectangle (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void FillRectanglePPM(HPPMDATA hData, int i0, int j0, int i1, int j1, UCHAR red, UCHAR green, UCHAR blue)
{
	int index;
	int i;
	int j;

	if (i0 > i1)
	{
		i = i0;
		i0 = i1;
		i1 = i;
	}

	if (j0 > j1)
	{
		j = j0;
		j0 = j1;
		j1 = j;
	}

	if (i1 < 0) return;
	if (j1 < 0) return;
	if (i0 >= (int)hData->Height) return;
	if (j0 >= (int)hData->Width) return;

	if (i1 >= (int)hData->Height) i1 = (int)hData->Height-1;
	if (j1 >= (int)hData->Width) j1 = (int)hData->Width-1;
	if (i0 < 0) i0 = 0;
	if (j0 < 0) j0 = 0;

	for (i = i0; i <= i1; i++)
	{
		for (j = j0; j <= j1; j++)
		{
			index = 3*(j+hData->Width*i);

			hData->Img[index] = red;
			hData->Img[index+1] = green;
			hData->Img[index+2] = blue;
		}
	}
}

/*
Draw a rectangle in a PPM picture.
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i0 : (IN) Line of the up-left pixel of the rectangle.
UINT j0 : (IN) Row of the up-left pixel of the rectangle.
UINT i1 : (IN) Line of the down-right pixel of the rectangle.
UINT j1 : (IN) Row of the down-right pixel of the rectangle.
UCHAR red : (IN) Red value of the color of the rectangle (between 0 and 255).
UCHAR green : (IN) Green value of the color of the rectangle (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void DrawRectanglePPMQuick(HPPMDATA hData, UINT i0, UINT j0, UINT i1, UINT j1, UCHAR red, UCHAR green, UCHAR blue)
{
	UINT index;
	UINT i, j;

	// Upper and lower bound.
	for (j = j0; j <= j1; j++)
	{
		index = 3*(j+hData->Width*i0);

		hData->Img[index] = red;
		hData->Img[index+1] = green;
		hData->Img[index+2] = blue;

		index = 3*(j+hData->Width*i1);

		hData->Img[index] = red;
		hData->Img[index+1] = green;
		hData->Img[index+2] = blue;
	}

	// Left and right bounds.
	for (i = i0; i <= i1; i++)
	{
		index = 3*(j0+hData->Width*i);

		hData->Img[index] = red;
		hData->Img[index+1] = green;
		hData->Img[index+2] = blue;

		index = 3*(j1+hData->Width*i);

		hData->Img[index] = red;
		hData->Img[index+1] = green;
		hData->Img[index+2] = blue;
	}
}

/*
Draw a rectangle in a PPM picture.
The function handles automatically any out of bounds index.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the up-left pixel of the rectangle.
int j0 : (IN) Row of the up-left pixel of the rectangle.
int i1 : (IN) Line of the down-right pixel of the rectangle.
int j1 : (IN) Row of the down-right pixel of the rectangle.
UCHAR red : (IN) Red value of the color of the rectangle (between 0 and 255).
UCHAR green : (IN) Green value of the color of the rectangle (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void DrawRectanglePPM(HPPMDATA hData, int i0, int j0, int i1, int j1, UCHAR red, UCHAR green, UCHAR blue)
{
	int index;
	int i;
	int j;
	BOOL bUpper = FALSE;
	BOOL bLower = FALSE;
	BOOL bLeft = FALSE;
	BOOL bRight = FALSE;

	if (i0 > i1)
	{
		i = i0;
		i0 = i1;
		i1 = i;
	}

	if (j0 > j1)
	{
		j = j0;
		j0 = j1;
		j1 = j;
	}

	if (i1 < 0) return;
	if (j1 < 0) return;
	if (i0 >= (int)hData->Height) return;
	if (j0 >= (int)hData->Width) return;

	if (i1 >= (int)hData->Height) i1 = (int)hData->Height-1; else bLower = TRUE;
	if (j1 >= (int)hData->Width) j1 = (int)hData->Width-1; else bRight = TRUE;
	if (i0 < 0) i0 = 0; else bUpper = TRUE;
	if (j0 < 0) j0 = 0; else bLeft = TRUE;

	if (bLower)
	{
		// Lower bound.
		for (j = j0; j <= j1; j++)
		{
			index = 3*(j+hData->Width*i1);

			hData->Img[index] = red;
			hData->Img[index+1] = green;
			hData->Img[index+2] = blue;
		}
	}
	if (bRight)
	{
		// Right bound.
		for (i = i0; i <= i1; i++)
		{
			index = 3*(j1+hData->Width*i);

			hData->Img[index] = red;
			hData->Img[index+1] = green;
			hData->Img[index+2] = blue;
		}
	}
	if (bUpper)
	{
		// Upper bound.
		for (j = j0; j <= j1; j++)
		{
			index = 3*(j+hData->Width*i0);

			hData->Img[index] = red;
			hData->Img[index+1] = green;
			hData->Img[index+2] = blue;
		}
	}
	if (bLeft)
	{
		// Left bound.
		for (i = i0; i <= i1; i++)
		{
			index = 3*(j0+hData->Width*i);

			hData->Img[index] = red;
			hData->Img[index+1] = green;
			hData->Img[index+2] = blue;
		}
	}
}

/*
Draw a vertical line in a PPM picture.
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i0 : (IN) Line of the upper pixel of the line.
UINT i1 : (IN) Line of the downer pixel of the line.
UINT j : (IN) Row of the line.
UCHAR red : (IN) Red value of the color of the line (between 0 and 255).
UCHAR green : (IN) Green value of the color of the line (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawVerticalLinePPMQuick(HPPMDATA hData, UINT i0, UINT i1, UINT j, UCHAR red, UCHAR green, UCHAR blue)
{
	UINT index, i;

	for (i = i0; i <= i1; i++)
	{
		index = 3*(j+hData->Width*i);

		hData->Img[index] = red;
		hData->Img[index+1] = green;
		hData->Img[index+2] = blue;
	}
}

/*
Draw a vertical line in a PPM picture.
The function handles automatically any out of bounds index.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the upper pixel of the line.
int i1 : (IN) Line of the downer pixel of the line.
int j : (IN) Row of the line.
UCHAR red : (IN) Red value of the color of the line (between 0 and 255).
UCHAR green : (IN) Green value of the color of the line (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawVerticalLinePPM(HPPMDATA hData, int i0, int i1, int j, UCHAR red, UCHAR green, UCHAR blue)
{
	int index, i;

	if (i0 > i1)
	{
		i = i0;
		i0 = i1;
		i1 = i;
	}

	if (i1 < 0) return;
	if (j < 0) return;
	if (i0 >= (int)hData->Height) return;
	if (j >= (int)hData->Width) return;

	if (i1 >= (int)hData->Height) i1 = (int)hData->Height-1;
	if (i0 < 0) i0 = 0;

	for (i = i0; i <= i1; i++)
	{
		index = 3*(j+hData->Width*i);

		hData->Img[index] = red;
		hData->Img[index+1] = green;
		hData->Img[index+2] = blue;
	}
}

/*
Draw a horizontal line in a PPM picture.
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the line.
UINT j0 : (IN) Row of the left pixel of the line.
UINT j1 : (IN) Row of the right pixel of the line.
UCHAR red : (IN) Red value of the color of the line (between 0 and 255).
UCHAR green : (IN) Green value of the color of the line (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawHorizontalLinePPMQuick(HPPMDATA hData, UINT i, UINT j0, UINT j1, UCHAR red, UCHAR green, UCHAR blue)
{
	UINT index, j;

	for (j = j0; j <= j1; j++)
	{
		index = 3*(j+hData->Width*i);

		hData->Img[index] = red;
		hData->Img[index+1] = green;
		hData->Img[index+2] = blue;
	}
}

/*
Draw a horizontal line in a PPM picture.
The function handles automatically any out of bounds index.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
int i : (IN) Line of the line.
int j0 : (IN) Row of the left pixel of the line.
int j1 : (IN) Row of the right pixel of the line.
UCHAR red : (IN) Red value of the color of the line (between 0 and 255).
UCHAR green : (IN) Green value of the color of the line (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawHorizontalLinePPM(HPPMDATA hData, int i, int j0, int j1, UCHAR red, UCHAR green, UCHAR blue)
{
	int index, j;

	if (j0 > j1)
	{
		j = j0;
		j0 = j1;
		j1 = j;
	}

	if (i < 0) return;
	if (j1 < 0) return;
	if (i >= (int)hData->Height) return;
	if (j0 >= (int)hData->Width) return;

	if (j1 >= (int)hData->Width) j1 = (int)hData->Width-1;
	if (j0 < 0) j0 = 0;

	for (j = j0; j <= j1; j++)
	{
		index = 3*(j+hData->Width*i);

		hData->Img[index] = red;
		hData->Img[index+1] = green;
		hData->Img[index+2] = blue;
	}
}

/*
Draw a strictly diagonal line in a PPM picture.
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i0 : (IN) Line of the first pixel of the line.
UINT j0 : (IN) Row of the first pixel of the line.
UINT i1 : (IN) Line of the last pixel of the line.
UINT j1 : (IN) Row of the last pixel of the line.
UCHAR red : (IN) Red value of the color of the line (between 0 and 255).
UCHAR green : (IN) Green value of the color of the line (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawDiagonalLinePPMQuick(HPPMDATA hData, UINT i0, UINT j0, UINT i1, UINT j1, UCHAR red, UCHAR green, UCHAR blue)
{
	UINT i, j, index;
	int di = i1 - i0;
	int dj = j1 - j0;

	if (i1 > i0)
	{
		if (j1 > j0)
		{
			if (di > dj)
			{
				for (i = i0; i <= i1; i++)
				{
					j = (UINT)(j0 + (i - i0)*(double)dj/(double)di);
					index = 3*(j+hData->Width*i);

					hData->Img[index] = red;
					hData->Img[index+1] = green;
					hData->Img[index+2] = blue;
				}
			} // (di > dj)
			else
			{
				for (j = j0; j <= j1; j++)
				{
					i = (UINT)(i0 + (j - j0)*(double)di/(double)dj);
					index = 3*(j+hData->Width*i);

					hData->Img[index] = red;
					hData->Img[index+1] = green;
					hData->Img[index+2] = blue;
				}
			} // !(di > dj)
		} // (j1 > j0)
		else
		{
			if (di > -dj)
			{
				for (i = i0; i <= i1; i++)
				{
					j = (UINT)(j0 + (i - i0)*(double)dj/(double)di);
					index = 3*(j+hData->Width*i);

					hData->Img[index] = red;
					hData->Img[index+1] = green;
					hData->Img[index+2] = blue;
				}
			} // (di > -dj)
			else
			{
				for (j = j1; j <= j0; j++)
				{
					i = (UINT)(i0 + (j - j0)*(double)di/(double)dj);
					index = 3*(j+hData->Width*i);

					hData->Img[index] = red;
					hData->Img[index+1] = green;
					hData->Img[index+2] = blue;
				}
			} // !(di > -dj)
		} // !(j1 > j0)
	} // (i1 > i0)
	else
	{
		if (j1 > j0)
		{
			if (-di > dj)
			{
				for (i = i1; i <= i0; i++)
				{
					j = (UINT)(j0 + (i - i0)*(double)dj/(double)di);
					index = 3*(j+hData->Width*i);

					hData->Img[index] = red;
					hData->Img[index+1] = green;
					hData->Img[index+2] = blue;
				}
			} // (-di > dj)
			else
			{
				for (j = j0; j <= j1; j++)
				{
					i = (UINT)(i0 + (j - j0)*(double)di/(double)dj);
					index = 3*(j+hData->Width*i);

					hData->Img[index] = red;
					hData->Img[index+1] = green;
					hData->Img[index+2] = blue;
				}
			} // !(-di > dj)
		} // (j1 > j0)
		else
		{
			if (di < dj)
			{
				for (i = i1; i <= i0; i++)
				{
					j = (UINT)(j0 + (i - i0)*(double)dj/(double)di);
					index = 3*(j+hData->Width*i);

					hData->Img[index] = red;
					hData->Img[index+1] = green;
					hData->Img[index+2] = blue;
				}
			} // (di < dj)
			else
			{
				for (j = j1; j <= j0; j++)
				{
					i = (UINT)(i0 + (j - j0)*(double)di/(double)dj);
					index = 3*(j+hData->Width*i);

					hData->Img[index] = red;
					hData->Img[index+1] = green;
					hData->Img[index+2] = blue;
				}
			} // !(di < dj)
		} // !(j1 > j0)
	} // !(i1 > i0)
}

/*
Draw a strictly diagonal line in a PPM picture.
The function handles automatically any out of bounds index.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the first pixel of the line.
int j0 : (IN) Row of the first pixel of the line.
int i1 : (IN) Line of the last pixel of the line.
int j1 : (IN) Row of the last pixel of the line.
UCHAR red : (IN) Red value of the color of the line (between 0 and 255).
UCHAR green : (IN) Green value of the color of the line (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawDiagonalLinePPM(HPPMDATA hData, int i0, int j0, int i1, int j1, UCHAR red, UCHAR green, UCHAR blue)
{
	int i, j, index;
	int di = i1 - i0;
	int dj = j1 - j0;
	int i0new, i1new, j0new, j1new;

	if (i1 > i0)
	{
		if (j1 > j0)
		{
			if (i1 < 0) return;
			if (j1 < 0) return;
			if (i0 >= (int)hData->Height) return;
			if (j0 >= (int)hData->Width) return;

			if (i1 >= (int)hData->Height) i1new = (int)hData->Height-1; else i1new = i1;
			if (j1 >= (int)hData->Width) j1new = (int)hData->Width-1; else j1new = j1;
			if (i0 < 0) i0new = 0; else i0new = i0;
			if (j0 < 0) j0new = 0; else j0new = j0;

			if (di > dj)
			{
				for (i = i0new; i <= i1new; i++)
				{
					j = (int)(j0 + (i - i0)*(double)dj/(double)di);
					if ((j >= 0)&&(j < (int)hData->Width))
					{
						index = 3*(j+hData->Width*i);

						hData->Img[index] = red;
						hData->Img[index+1] = green;
						hData->Img[index+2] = blue;
					}
				}
			} // (di > dj)
			else
			{
				for (j = j0new; j <= j1new; j++)
				{
					i = (int)(i0 + (j - j0)*(double)di/(double)dj);
					if ((i >= 0)&&(i < (int)hData->Height))
					{
						index = 3*(j+hData->Width*i);

						hData->Img[index] = red;
						hData->Img[index+1] = green;
						hData->Img[index+2] = blue;
					}
				}
			} // !(di > dj)
		} // (j1 > j0)
		else
		{
			if (i1 < 0) return;
			if (j0 < 0) return;
			if (i0 >= (int)hData->Height) return;
			if (j1 >= (int)hData->Width) return;

			if (i1 >= (int)hData->Height) i1new = (int)hData->Height-1; else i1new = i1;
			if (j0 >= (int)hData->Width) j0new = (int)hData->Width-1; else j0new = j0;
			if (i0 < 0) i0new = 0; else i0new = i0;
			if (j1 < 0) j1new = 0; else j1new = j1;

			if (di > -dj)
			{
				for (i = i0new; i <= i1new; i++)
				{
					j = (int)(j0 + (i - i0)*(double)dj/(double)di);
					if ((j >= 0)&&(j < (int)hData->Width))
					{
						index = 3*(j+hData->Width*i);

						hData->Img[index] = red;
						hData->Img[index+1] = green;
						hData->Img[index+2] = blue;
					}
				}
			} // (di > -dj)
			else
			{
				for (j = j1new; j <= j0new; j++)
				{
					i = (int)(i0 + (j - j0)*(double)di/(double)dj);
					if ((i >= 0)&&(i < (int)hData->Height))
					{
						index = 3*(j+hData->Width*i);

						hData->Img[index] = red;
						hData->Img[index+1] = green;
						hData->Img[index+2] = blue;
					}
				}
			} // !(di > -dj)
		} // !(j1 > j0)
	} // (i1 > i0)
	else
	{
		if (j1 > j0)
		{
			if (i0 < 0) return;
			if (j1 < 0) return;
			if (i1 >= (int)hData->Height) return;
			if (j0 >= (int)hData->Width) return;

			if (i0 >= (int)hData->Height) i0new = (int)hData->Height-1; else i0new = i0;
			if (j1 >= (int)hData->Width) j1new = (int)hData->Width-1; else j1new = j1;
			if (i1 < 0) i1new = 0; else i1new = i1;
			if (j0 < 0) j0new = 0; else j0new = j0;

			if (-di > dj)
			{
				for (i = i1new; i <= i0new; i++)
				{
					j = (int)(j0 + (i - i0)*(double)dj/(double)di);
					if ((j >= 0)&&(j < (int)hData->Width))
					{
						index = 3*(j+hData->Width*i);

						hData->Img[index] = red;
						hData->Img[index+1] = green;
						hData->Img[index+2] = blue;
					}
				}
			} // (-di > dj)
			else
			{
				for (j = j0new; j <= j1new; j++)
				{
					i = (int)(i0 + (j - j0)*(double)di/(double)dj);
					if ((i >= 0)&&(i < (int)hData->Height))
					{
						index = 3*(j+hData->Width*i);

						hData->Img[index] = red;
						hData->Img[index+1] = green;
						hData->Img[index+2] = blue;
					}
				}
			} // !(-di > dj)
		} // (j1 > j0)
		else
		{
			if (i0 < 0) return;
			if (j0 < 0) return;
			if (i1 >= (int)hData->Height) return;
			if (j1 >= (int)hData->Width) return;

			if (i0 >= (int)hData->Height) i0new = (int)hData->Height-1; else i0new = i0;
			if (j0 >= (int)hData->Width) j0new = (int)hData->Width-1; else j0new = j0;
			if (i1 < 0) i1new = 0; else i1new = i1;
			if (j1 < 0) j1new = 0; else j1new = j1;

			if (di < dj)
			{
				for (i = i1new; i <= i0new; i++)
				{
					j = (int)(j0 + (i - i0)*(double)dj/(double)di);
					if ((j >= 0)&&(j < (int)hData->Width))
					{
						index = 3*(j+hData->Width*i);

						hData->Img[index] = red;
						hData->Img[index+1] = green;
						hData->Img[index+2] = blue;
					}
				}
			} // (di < dj)
			else
			{
				for (j = j1new; j <= j0new; j++)
				{
					i = (int)(i0 + (j - j0)*(double)di/(double)dj);
					if ((i >= 0)&&(i < (int)hData->Height))
					{
						index = 3*(j+hData->Width*i);

						hData->Img[index] = red;
						hData->Img[index+1] = green;
						hData->Img[index+2] = blue;
					}
				}
			} // !(di < dj)
		} // !(j1 > j0)
	} // !(i1 > i0)
}

/*
Draw a line in a PPM picture.
The function does not check any parameter.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
UINT i0 : (IN) Line of the first pixel of the line.
UINT j0 : (IN) Row of the first pixel of the line.
UINT i1 : (IN) Line of the last pixel of the line.
UINT j1 : (IN) Row of the last pixel of the line.
UCHAR red : (IN) Red value of the color of the line (between 0 and 255).
UCHAR green : (IN) Green value of the color of the line (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawLinePPMQuick(HPPMDATA hData, UINT i0, UINT j0, UINT i1, UINT j1, UCHAR red, UCHAR green, UCHAR blue)
{
	if (i1 == i0)
	{	
		if (j1 == j0)
		{
			SetPPMImgPixelQuick(hData, i0, j0, red, green, blue);
		}
		else
		{
			if (j0 < j1)
			{
				DrawHorizontalLinePPMQuick(hData, i0, j0, j1, red, green, blue);
			}
			else
			{
				DrawHorizontalLinePPMQuick(hData, i0, j1, j0, red, green, blue);
			}
		}
	}
	else
	{
		if (j1 == j0)
		{
			if (i0 < i1)
			{
				DrawVerticalLinePPMQuick(hData, i0, i1, j0, red, green, blue);
			}
			else
			{
				DrawVerticalLinePPMQuick(hData, i1, i0, j0, red, green, blue);
			}
		}
		else
		{
			DrawDiagonalLinePPMQuick(hData, i0, j0, i1, j1, red, green, blue);
		}
	}
}

/*
Draw a line in a PPM picture.
The function handles automatically any out of bounds index.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the first pixel of the line.
int j0 : (IN) Row of the first pixel of the line.
int i1 : (IN) Line of the last pixel of the line.
int j1 : (IN) Row of the last pixel of the line.
UCHAR red : (IN) Red value of the color of the line (between 0 and 255).
UCHAR green : (IN) Green value of the color of the line (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawLinePPM(HPPMDATA hData, int i0, int j0, int i1, int j1, UCHAR red, UCHAR green, UCHAR blue)
{
	if (i1 == i0)
	{	
		if (j1 == j0)
		{
			SetPPMImgPixel(hData, i0, j0, red, green, blue);
		}
		else
		{
			DrawHorizontalLinePPM(hData, i0, j0, j1, red, green, blue);
		}
	}
	else
	{
		if (j1 == j0)
		{
			DrawVerticalLinePPM(hData, i0, i1, j0, red, green, blue);
		}
		else
		{
			DrawDiagonalLinePPM(hData, i0, j0, i1, j1, red, green, blue);
		}
	}
}

/*
Draw a circle in a PPM picture.
The function handles automatically any out of bounds index.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the center.
int j0 : (IN) Row of the center.
int radius : (IN) Radius.
UCHAR red : (IN) Red value of the color of the circle (between 0 and 255).
UCHAR green : (IN) Green value of the color of the circle (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the circle (between 0 and 255).

Return : Nothing.
*/
inline void DrawCirclePPM(HPPMDATA hData, int i0, int j0, int radius, UCHAR red, UCHAR green, UCHAR blue)
{
	double NAngle = 200; // Precision of drawing.
	int a = 0, j = 0, i = 0;

	radius = abs(radius);
	NAngle = 18*radius;
		
	for (a = 0; a < NAngle; a++)
	{
		j = (int)(radius*cos(2.0*a*M_PI/NAngle)+j0);
		i = (int)(radius*sin(2.0*a*M_PI/NAngle)+i0);		
		
		SetPPMImgPixel(hData, i, j, red, green, blue);
	}

	// To check and optimize...
}

/*
Draw a circle in a PPM picture and fill it.
The function handles automatically any out of bounds index.
See the description of the PPMDATA structure for more information. 

HPPMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the center.
int j0 : (IN) Row of the center.
int radius : (IN) Radius.
UCHAR red : (IN) Red value of the color of the circle (between 0 and 255).
UCHAR green : (IN) Green value of the color of the circle (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the circle (between 0 and 255).

Return : Nothing.
*/
inline void FillCirclePPM(HPPMDATA hData, int i0, int j0, int radius, UCHAR red, UCHAR green, UCHAR blue)
{
	double NAngle = 200; // Precision of drawing.
	int a = 0, j = 0, i = 0;

	radius = abs(radius);
	NAngle = 18*radius;
		
	for (a = 0; a < NAngle; a++)
	{
		j = (int)(radius*cos(2.0*a*M_PI/NAngle)+j0);
		i = (int)(radius*sin(2.0*a*M_PI/NAngle)+i0);		
		
		DrawLinePPM(hData, i0, j0, i, j, red, green, blue);
	}

	// To check and optimize...
}

#endif // PPMDRAW_H
