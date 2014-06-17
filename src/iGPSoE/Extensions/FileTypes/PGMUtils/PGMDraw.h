/***************************************************************************************************************:')

PGMDraw.h

Drawing functions for PGM data.

Fabrice Le Bars

Created : 2008-12-01

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef PGMDRAW_H
#define PGMDRAW_H

#include "PGMCore.h"

/*
Debug macros specific to PGMDraw.
*/
#ifdef _DEBUG_MESSAGES_PGMUTILS
#	define _DEBUG_MESSAGES_PGMDRAW
#endif // _DEBUG_MESSAGES_PGMUTILS

#ifdef _DEBUG_WARNINGS_PGMUTILS
#	define _DEBUG_WARNINGS_PGMDRAW
#endif // _DEBUG_WARNINGS_PGMUTILS

#ifdef _DEBUG_ERRORS_PGMUTILS
#	define _DEBUG_ERRORS_PGMDRAW
#endif // _DEBUG_ERRORS_PGMUTILS

#ifdef _DEBUG_MESSAGES_PGMDRAW
#	define PRINT_DEBUG_MESSAGE_PGMDRAW(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_PGMDRAW(params)
#endif // _DEBUG_MESSAGES_PGMDRAW

#ifdef _DEBUG_WARNINGS_PGMDRAW
#	define PRINT_DEBUG_WARNING_PGMDRAW(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_PGMDRAW(params)
#endif // _DEBUG_WARNINGS_PGMDRAW

#ifdef _DEBUG_ERRORS_PGMDRAW
#	define PRINT_DEBUG_ERROR_PGMDRAW(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_PGMDRAW(params)
#endif // _DEBUG_ERRORS_PGMDRAW

/*
Clear a PGM picture (set all the pixels to the same color). 
The function does not check any parameter.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
UCHAR value : (IN) Gray value of the picture.

Return : Nothing.
*/
inline void ClearPGMImg(HPGMDATA hData, UCHAR value)
{
	memset(hData->Img, value, hData->Size);
}

/*
Draw a rectangle in a PGM picture and fill it.
The function does not check any parameter.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
UINT i0 : (IN) Line of the up-left pixel of the rectangle.
UINT j0 : (IN) Row of the up-left pixel of the rectangle.
UINT i1 : (IN) Line of the down-right pixel of the rectangle.
UINT j1 : (IN) Row of the down-right pixel of the rectangle.
UCHAR value : (IN) Gray value of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void FillRectanglePGMQuick(HPGMDATA hData, UINT i0, UINT j0, UINT i1, UINT j1, UCHAR value)
{
	UINT i;

	for (i = i0; i <= i1; i++)
	{
		memset(
			&hData->Img[j0+i*hData->Width], 
			value, 
			j1-j0+1
			);
	}
}

/*
Draw a rectangle in a PGM picture and fill it.
The function handles automatically any out of bounds index.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the up-left pixel of the rectangle.
int j0 : (IN) Row of the up-left pixel of the rectangle.
int i1 : (IN) Line of the down-right pixel of the rectangle.
int j1 : (IN) Row of the down-right pixel of the rectangle.
UCHAR value : (IN) Gray value of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void FillRectanglePGM(HPGMDATA hData, int i0, int j0, int i1, int j1, UCHAR value)
{
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
		memset(
			&hData->Img[j0+i*hData->Width], 
			value, 
			j1-j0+1
			);
	}
}

/*
Draw a rectangle in a PGM picture.
The function does not check any parameter.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
UINT i0 : (IN) Line of the up-left pixel of the rectangle.
UINT j0 : (IN) Row of the up-left pixel of the rectangle.
UINT i1 : (IN) Line of the down-right pixel of the rectangle.
UINT j1 : (IN) Row of the down-right pixel of the rectangle.
UCHAR value : (IN) Gray value of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void DrawRectanglePGMQuick(HPGMDATA hData, UINT i0, UINT j0, UINT i1, UINT j1, UCHAR value)
{
	UINT tmp;
	UINT i;

	// Upper bound.
	memset(
		&hData->Img[j0+i0*hData->Width], 
		value, 
		j1-j0+1
		);

	// Lower bound.
	memset(
		&hData->Img[j0+i1*hData->Width], 
		value, 
		j1-j0+1
		);

	// Left and right bounds.
	for (i = i0; i <= i1; i++)
	{
		tmp = i*hData->Width;
		hData->Img[j0+tmp] = value;
		hData->Img[j1+tmp] = value;
	}
}

/*
Draw a rectangle in a PGM picture.
The function handles automatically any out of bounds index.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the up-left pixel of the rectangle.
int j0 : (IN) Row of the up-left pixel of the rectangle.
int i1 : (IN) Line of the down-right pixel of the rectangle.
int j1 : (IN) Row of the down-right pixel of the rectangle.
UCHAR value : (IN) Gray value of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void DrawRectanglePGM(HPGMDATA hData, int i0, int j0, int i1, int j1, UCHAR value)
{
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
		memset(
			&hData->Img[j0+i1*hData->Width], 
			value, 
			j1-j0+1
			);
	}
	if (bRight)
	{
		// Right bound.
		for (i = i0; i <= i1; i++)
		{
			hData->Img[j1+i*hData->Width] = value;
		}
	}
	if (bUpper)
	{
		// Upper bound.
		memset(
			&hData->Img[j0+i0*hData->Width], 
			value, 
			j1-j0+1
			);
	}
	if (bLeft)
	{
		// Left bound.
		for (i = i0; i <= i1; i++)
		{
			hData->Img[j0+i*hData->Width] = value;
		}
	}
}

/*
Draw a vertical line in a PGM picture.
The function does not check any parameter.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
UINT i0 : (IN) Line of the upper pixel of the line.
UINT i1 : (IN) Line of the downer pixel of the line.
UINT j : (IN) Row of the line.
UCHAR value : (IN) Gray value of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawVerticalLinePGMQuick(HPGMDATA hData, UINT i0, UINT i1, UINT j, UCHAR value)
{
	UINT i;

	for (i = i0; i <= i1; i++)
	{
		hData->Img[j+i*hData->Width] = value;
	}
}

/*
Draw a vertical line in a PGM picture.
The function handles automatically any out of bounds index.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the upper pixel of the line.
int i1 : (IN) Line of the downer pixel of the line.
int j : (IN) Row of the line.
UCHAR value : (IN) Gray value of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawVerticalLinePGM(HPGMDATA hData, int i0, int i1, int j, UCHAR value)
{
	int i;

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
	if (i0 <= 0) i0 = 0;

	for (i = i0; i <= i1; i++)
	{
		hData->Img[j+i*hData->Width] = value;
	}
}

/*
Draw a horizontal line in a PGM picture.
The function does not check any parameter.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
UINT i : (IN) Line of the line.
UINT j0 : (IN) Row of the left pixel of the line.
UINT j1 : (IN) Row of the right pixel of the line.
UCHAR value : (IN) Gray value of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawHorizontalLinePGMQuick(HPGMDATA hData, UINT i, UINT j0, UINT j1, UCHAR value)
{
	memset(
		&hData->Img[j0+i*hData->Width], 
		value, 
		j1-j0+1
		);
}

/*
Draw a horizontal line in a PGM picture.
The function handles automatically any out of bounds index.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
int i : (IN) Line of the line.
int j0 : (IN) Row of the left pixel of the line.
int j1 : (IN) Row of the right pixel of the line.
UCHAR value : (IN) Gray value of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawHorizontalLinePGM(HPGMDATA hData, int i, int j0, int j1, UCHAR value)
{
	int j;

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

	memset(
		&hData->Img[j0+i*hData->Width], 
		value, 
		j1-j0+1
		);
}

/*
Draw a strictly diagonal line in a PGM picture.
The function does not check any parameter.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
UINT i0 : (IN) Line of the first pixel of the line.
UINT j0 : (IN) Row of the first pixel of the line.
UINT i1 : (IN) Line of the last pixel of the line.
UINT j1 : (IN) Row of the last pixel of the line.
UCHAR value : (IN) Gray value of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawDiagonalLinePGMQuick(HPGMDATA hData, UINT i0, UINT j0, UINT i1, UINT j1, UCHAR value)
{
	UINT i, j;
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
					hData->Img[j+i*hData->Width] = value;
				}
			} // (di > dj)
			else
			{
				for (j = j0; j <= j1; j++)
				{
					i = (UINT)(i0 + (j - j0)*(double)di/(double)dj);
					hData->Img[j+i*hData->Width] = value;
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
					hData->Img[j+i*hData->Width] = value;
				}
			} // (di > -dj)
			else
			{
				for (j = j1; j <= j0; j++)
				{
					i = (UINT)(i0 + (j - j0)*(double)di/(double)dj);
					hData->Img[j+i*hData->Width] = value;
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
					hData->Img[j+i*hData->Width] = value;
				}
			} // (-di > dj)
			else
			{
				for (j = j0; j <= j1; j++)
				{
					i = (UINT)(i0 + (j - j0)*(double)di/(double)dj);
					hData->Img[j+i*hData->Width] = value;
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
					hData->Img[j+i*hData->Width] = value;
				}
			} // (di < dj)
			else
			{
				for (j = j1; j <= j0; j++)
				{
					i = (UINT)(i0 + (j - j0)*(double)di/(double)dj);
					hData->Img[j+i*hData->Width] = value;
				}
			} // !(di < dj)
		} // !(j1 > j0)
	} // !(i1 > i0)
}

/*
Draw a strictly diagonal line in a PGM picture.
The function handles automatically any out of bounds index.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the first pixel of the line.
int j0 : (IN) Row of the first pixel of the line.
int i1 : (IN) Line of the last pixel of the line.
int j1 : (IN) Row of the last pixel of the line.
UCHAR value : (IN) Gray value of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawDiagonalLinePGM(HPGMDATA hData, int i0, int j0, int i1, int j1, UCHAR value)
{
	int i, j;
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
						hData->Img[j+i*hData->Width] = value;
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
						hData->Img[j+i*hData->Width] = value;
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
						hData->Img[j+i*hData->Width] = value;
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
						hData->Img[j+i*hData->Width] = value;
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
						hData->Img[j+i*hData->Width] = value;
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
						hData->Img[j+i*hData->Width] = value;
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
						hData->Img[j+i*hData->Width] = value;
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
						hData->Img[j+i*hData->Width] = value;
					}
				}
			} // !(di < dj)
		} // !(j1 > j0)
	} // !(i1 > i0)
}

/*
Draw a line in a PGM picture.
The function does not check any parameter.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
UINT i0 : (IN) Line of the first pixel of the line.
UINT j0 : (IN) Row of the first pixel of the line.
UINT i1 : (IN) Line of the last pixel of the line.
UINT j1 : (IN) Row of the last pixel of the line.
UCHAR value : (IN) Gray value of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawLinePGMQuick(HPGMDATA hData, UINT i0, UINT j0, UINT i1, UINT j1, UCHAR value)
{
	if (i1 == i0)
	{	
		if (j1 == j0)
		{
			SetPGMImgPixelQuick(hData, i0, j0, value);
		}
		else
		{
			if (j0 < j1)
			{
				DrawHorizontalLinePGMQuick(hData, i0, j0, j1, value);
			}
			else
			{
				DrawHorizontalLinePGMQuick(hData, i0, j1, j0, value);
			}
		}
	}
	else
	{
		if (j1 == j0)
		{
			if (i0 < i1)
			{
				DrawVerticalLinePGMQuick(hData, i0, i1, j0, value);
			}
			else
			{
				DrawVerticalLinePGMQuick(hData, i1, i0, j0, value);
			}
		}
		else
		{
			DrawDiagonalLinePGMQuick(hData, i0, j0, i1, j1, value);
		}
	}
}

/*
Draw a line in a PGM picture.
The function handles automatically any out of bounds index.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the first pixel of the line.
int j0 : (IN) Row of the first pixel of the line.
int i1 : (IN) Line of the last pixel of the line.
int j1 : (IN) Row of the last pixel of the line.
UCHAR value : (IN) Gray value of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawLinePGM(HPGMDATA hData, int i0, int j0, int i1, int j1, UCHAR value)
{
	if (i1 == i0)
	{	
		if (j1 == j0)
		{
			SetPGMImgPixel(hData, i0, j0, value);
		}
		else
		{
			DrawHorizontalLinePGM(hData, i0, j0, j1, value);
		}
	}
	else
	{
		if (j1 == j0)
		{
			DrawVerticalLinePGM(hData, i0, i1, j0, value);
		}
		else
		{
			DrawDiagonalLinePGM(hData, i0, j0, i1, j1, value);
		}
	}
}

/*
Draw a circle in a PGM picture.
The function handles automatically any out of bounds index.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the center.
int j0 : (IN) Row of the center.
int radius : (IN) Radius.
UCHAR value : (IN) Gray value of the circle (between 0 and 255).

Return : Nothing.
*/
inline void DrawCirclePGM(HPGMDATA hData, int i0, int j0, int radius, UCHAR value)
{
	double NAngle = 200; // Precision of drawing.
	int a = 0, j = 0, i = 0;

	radius = abs(radius);
	NAngle = 18*radius;
		
	for (a = 0; a < NAngle; a++)
	{
		j = (int)(radius*cos(2.0*a*M_PI/NAngle)+j0);
		i = (int)(radius*sin(2.0*a*M_PI/NAngle)+i0);		
		
		SetPGMImgPixel(hData, i, j, value);
	}

	// To check and optimize...
}

/*
Draw a circle in a PGM picture and fill it.
The function handles automatically any out of bounds index.
See the description of the PGMDATA structure for more information. 

HPGMDATA hData : (INOUT) Identifier of the structure.
int i0 : (IN) Line of the center.
int j0 : (IN) Row of the center.
int radius : (IN) Radius.
UCHAR value : (IN) Gray value of the circle (between 0 and 255).

Return : Nothing.
*/
inline void FillCirclePGM(HPGMDATA hData, int i0, int j0, int radius, UCHAR value)
{
	double NAngle = 200; // Precision of drawing.
	int a = 0, j = 0, i = 0;

	radius = abs(radius);
	NAngle = 18*radius;
		
	for (a = 0; a < NAngle; a++)
	{
		j = (int)(radius*cos(2.0*a*M_PI/NAngle)+j0);
		i = (int)(radius*sin(2.0*a*M_PI/NAngle)+i0);		
		
		DrawLinePGM(hData, i0, j0, i, j, value);
	}

	// To check and optimize...
}

#endif // PGMDRAW_H
