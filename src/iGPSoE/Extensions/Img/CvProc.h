/***************************************************************************************************************:')

CvProc.h

IplImage data processing.

Fabrice Le Bars

Created : 2009-03-27

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef CVPROC_H
#define CVPROC_H

#include "CvCore.h"

/*
Returns the estimated position and dimensions of an orange ball in a picture.

uIplImage* pImg : (IN) pointer to the data of the picture
int *X : (OUT) pointer to the estimated position of the ball (x-axis, between 0 and width)
int *Y : (OUT) pointer to the estimated position of the ball (y-axis, between 0 and height)
int *W : (OUT) pointer to the estimated width
int *H : (OUT) pointer to the estimated height

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
inline int orange_ball_detection(IplImage* pImg, int *X, int *Y, int *W, int *H)	{

	char *p = pImg->imageData;
	int i = 0, j = 0, r = 0, g = 0, b = 0;
	int foundflag = 0;
	int objflag = 0, objwidth = 0, objheight = 0;
	int objX = 0, objY = 0;//center of the object
	int x0 = 0, x1 = 0, y0 = 0, y1 = 0;

	for (i=0;i<pImg->height;i++)	{
		for (j=0;j<pImg->width;j++)	{
			r = (unsigned char)p[0+3*j+3*pImg->width*i];
			g = (unsigned char)p[1+3*j+3*pImg->width*i];
			b = (unsigned char)p[2+3*j+3*pImg->width*i];
			if ( (r>200) && (g<150) && (b<150) )	{//we are on the object
				if (!objflag)	{//it's the beginning of the object at the current line, we were not on it before
					objflag = 1;//we are entering in the object
					foundflag = 1;//says that we have found something in the picture
					x0 = j;//position of the beginning of the object at the current line
				}//else we are somewhere on the object
				else if (j == pImg->width-1)	{//we are on the object but we are at the end of the line, so we do as if we were at the end of the object
					objflag = 0;//we are leaving the object
					x1 = j;//position of the end of the object at the current line
					if ( x1-x0 > objwidth )	{//when objwidth is max, we are at the center of the object
						objwidth = x1-x0;
						objX = x0+objwidth/2;
						if (objY == 0)	{//we are at the beginning of the object on the Y axis
							y0 = i;
						}
						y1 = i;
						objheight = 2*(y1-y0);//*2 because the last time we will be here, we will be on the center
						objY = i;			//(y0 is the beginning of the object and the last value of y1 will be the center)
					}
				}
			}
			else	{
				if (objflag)	{//we were on the object just before, now we are not on it any more
					objflag = 0;//we are leaving the object
					x1 = j;//position of the end of the object at the current line
					if ( x1-x0 > objwidth )	{//when objwidth is max, we are at the center of the object
						objwidth = x1-x0;
						objX = x0+objwidth/2;
						if (objY == 0)	{//we are at the beginning of the object on the Y axis
							y0 = i;
						}
						y1 = i;
						objheight = 2*(y1-y0);//*2 because the last time we will be here, we will be on the center
						objY = i;			//(y0 is the beginning of the object and the last value of y1 will be the center)
					}
				}
			}
		}
	}

	if (foundflag)	{
		//Now we have the center of the object, at (objX,objY), assuming that (0,0) is in the top-left corner
		*X = objX;
		*Y = objY;
		*W = objwidth;
		*H = objheight;
	}
	else	{
		//We did not see any orange ball
		*X = -1;
		*Y = -1;
		*W = -1;
		*H = -1;
	}

	return EXIT_SUCCESS;
}

#endif // CVPROC_H
