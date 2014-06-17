/***************************************************************************************************************:')

CvDraw.h

Drawing functions for IplImage.
2 types of IplImage format should be used in the functions :
_ a default RGB color format than can be created using CreateDefaultColorCvImg() or cvLoadImage("file.ppm", 1).
_ a default gray format than can be created using CreateDefaultGrayCvImg() or cvLoadImage("file.pgm", 0).
Note that you can use most of the OpenCV functions with such IplImage formats.

Fabrice Le Bars

Created : 2009-03-27

Version status : To test and validate

***************************************************************************************************************:)*/

#ifndef CVDRAW_H
#define CVDRAW_H

#include "CvCore.h"

/*
Clear an IplImage (set all the pixels to the same color). 
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UCHAR red : (IN) Red value of the color of the picture (between 0 and 255).
UCHAR green : (IN) Green value of the color of the picture (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the picture (between 0 and 255).

Return : Nothing.
*/
inline void ClearDefaultColorCvImg(IplImage* pImg, UCHAR red, UCHAR green, UCHAR blue)
{
	cvSet(pImg, CV_RGB(red, green, blue), NULL);
}

/*
Draw a rectangle in an IplImage.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
int i0 : (IN) Line of the up-left pixel of the rectangle.
int j0 : (IN) Row of the up-left pixel of the rectangle.
int i1 : (IN) Line of the down-right pixel of the rectangle.
int j1 : (IN) Row of the down-right pixel of the rectangle.
UCHAR red : (IN) Red value of the color of the rectangle (between 0 and 255).
UCHAR green : (IN) Green value of the color of the rectangle (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void DrawRectangleDefaultColorCvImg(IplImage* pImg, int i0, int j0, int i1, int j1, UCHAR red, UCHAR green, UCHAR blue)
{
	CvPoint pt1 = cvPoint(j0, i0);
	CvPoint pt2 = cvPoint(j1, i1);

	cvRectangle(pImg, pt1, pt2, CV_RGB(red, green, blue), 1, 8, 0);
}

/*
Draw a rectangle in an IplImage and fill it.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
int i0 : (IN) Line of the up-left pixel of the rectangle.
int j0 : (IN) Row of the up-left pixel of the rectangle.
int i1 : (IN) Line of the down-right pixel of the rectangle.
int j1 : (IN) Row of the down-right pixel of the rectangle.
UCHAR red : (IN) Red value of the color of the rectangle (between 0 and 255).
UCHAR green : (IN) Green value of the color of the rectangle (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void FillRectangleDefaultColorCvImg(IplImage* pImg, int i0, int j0, int i1, int j1, UCHAR red, UCHAR green, UCHAR blue)
{
	CvPoint pt1 = cvPoint(j0, i0);
	CvPoint pt2 = cvPoint(j1, i1);

	cvRectangle(pImg, pt1, pt2, CV_RGB(red, green, blue), CV_FILLED, 8, 0);
}

/*
Draw a line in an IplImage.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
int i0 : (IN) Line of the first pixel of the line.
int j0 : (IN) Row of the first pixel of the line.
int i1 : (IN) Line of the last pixel of the line.
int j1 : (IN) Row of the last pixel of the line.
UCHAR red : (IN) Red value of the color of the line (between 0 and 255).
UCHAR green : (IN) Green value of the color of the line (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawLineDefaultColorCvImg(IplImage* pImg, int i0, int j0, int i1, int j1, UCHAR red, UCHAR green, UCHAR blue)
{
	CvPoint pt1 = cvPoint(j0, i0);
	CvPoint pt2 = cvPoint(j1, i1);

	cvLine(pImg, pt1, pt2, CV_RGB(red, green, blue), 1, 8, 0);
}

/*
Draw a circle in an IplImage.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
int i : (IN) Line of the center of the circle.
int j : (IN) Row of the center of the circle.
int radius : (IN) Radius of the circle.
UCHAR red : (IN) Red value of the color of the rectangle (between 0 and 255).
UCHAR green : (IN) Green value of the color of the rectangle (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void DrawCircleDefaultColorCvImg(IplImage* pImg, int i, int j, int radius, UCHAR red, UCHAR green, UCHAR blue)
{
	CvPoint center = cvPoint(j, i);

	cvCircle(pImg, center, radius, CV_RGB(red, green, blue), 1, 8, 0);
}

/*
Draw a circle in an IplImage and fill it.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
int i : (IN) Line of the center of the circle.
int j : (IN) Row of the center of the circle.
int radius : (IN) Radius of the circle.
UCHAR red : (IN) Red value of the color of the rectangle (between 0 and 255).
UCHAR green : (IN) Green value of the color of the rectangle (between 0 and 255).
UCHAR blue : (IN) Blue value of the color of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void FillCircleDefaultColorCvImg(IplImage* pImg, int i, int j, int radius, UCHAR red, UCHAR green, UCHAR blue)
{
	CvPoint center = cvPoint(j, i);

	cvCircle(pImg, center, radius, CV_RGB(red, green, blue), CV_FILLED, 8, 0);
}

/*
Clear an IplImage (set all the pixels to the same gray value). 
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
UCHAR value : (IN) Gray value of the picture (between 0 and 255).

Return : Nothing.
*/
inline void ClearDefaultGrayCvImg(IplImage* pImg, UCHAR value)
{
	cvSet(pImg, cvRealScalar(value), NULL);
}

/*
Draw a rectangle in an IplImage.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
int i0 : (IN) Line of the up-left pixel of the rectangle.
int j0 : (IN) Row of the up-left pixel of the rectangle.
int i1 : (IN) Line of the down-right pixel of the rectangle.
int j1 : (IN) Row of the down-right pixel of the rectangle.
UCHAR value : (IN) Gray value of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void DrawRectangleDefaultGrayCvImg(IplImage* pImg, int i0, int j0, int i1, int j1, UCHAR value)
{
	CvPoint pt1 = cvPoint(j0, i0);
	CvPoint pt2 = cvPoint(j1, i1);

	cvRectangle(pImg, pt1, pt2, cvRealScalar(value), 1, 8, 0);
}

/*
Draw a rectangle in an IplImage and fill it.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
int i0 : (IN) Line of the up-left pixel of the rectangle.
int j0 : (IN) Row of the up-left pixel of the rectangle.
int i1 : (IN) Line of the down-right pixel of the rectangle.
int j1 : (IN) Row of the down-right pixel of the rectangle.
UCHAR value : (IN) Gray value of the rectangle (between 0 and 255).

Return : Nothing.
*/
inline void FillRectangleDefaultGrayCvImg(IplImage* pImg, int i0, int j0, int i1, int j1, UCHAR value)
{
	CvPoint pt1 = cvPoint(j0, i0);
	CvPoint pt2 = cvPoint(j1, i1);

	cvRectangle(pImg, pt1, pt2, cvRealScalar(value), CV_FILLED, 8, 0);
}

/*
Draw a line in an IplImage.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
int i0 : (IN) Line of the first pixel of the line.
int j0 : (IN) Row of the first pixel of the line.
int i1 : (IN) Line of the last pixel of the line.
int j1 : (IN) Row of the last pixel of the line.
UCHAR value : (IN) Gray value of the line (between 0 and 255).

Return : Nothing.
*/
inline void DrawLineDefaultGrayCvImg(IplImage* pImg, int i0, int j0, int i1, int j1, UCHAR value)
{
	CvPoint pt1 = cvPoint(j0, i0);
	CvPoint pt2 = cvPoint(j1, i1);

	cvLine(pImg, pt1, pt2, cvRealScalar(value), 1, 8, 0);
}

/*
Draw a circle in an IplImage.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
int i : (IN) Line of the center of the circle.
int j : (IN) Row of the center of the circle.
int radius : (IN) Radius of the circle.
UCHAR value : (IN) Gray value of the circle (between 0 and 255).

Return : Nothing.
*/
inline void DrawCircleDefaultGrayCvImg(IplImage* pImg, int i, int j, int radius, UCHAR value)
{
	CvPoint center = cvPoint(j, i);

	cvCircle(pImg, center, radius, cvRealScalar(value), 1, 8, 0);
}

/*
Draw a circle in an IplImage and fill it.
See the documentation of OpenCV for more information. 

IplImage* pImg : (IN) Pointer to the structure.
int i : (IN) Line of the center of the circle.
int j : (IN) Row of the center of the circle.
int radius : (IN) Radius of the circle.
UCHAR value : (IN) Gray value of the circle (between 0 and 255).

Return : Nothing.
*/
inline void FillCircleDefaultGrayCvImg(IplImage* pImg, int i, int j, int radius, UCHAR value)
{
	CvPoint center = cvPoint(j, i);

	cvCircle(pImg, center, radius, cvRealScalar(value), CV_FILLED, 8, 0);
}

#endif // CVDRAW_H
