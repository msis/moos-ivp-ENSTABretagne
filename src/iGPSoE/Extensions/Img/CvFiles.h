/***************************************************************************************************************:')

CvFiles.h

IplImage and picture files handling.
2 types of IplImage format should be used in the functions :
_ a default RGB color format than can be created using CreateDefaultColorCvImg() or cvLoadImage("file.ppm", 1).
_ a default gray format than can be created using CreateDefaultGrayCvImg() or cvLoadImage("file.pgm", 0).
Note that you can use most of the OpenCV functions with such IplImage formats.

Fabrice Le Bars

Created : 2009-03-27

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef CVFILES_H
#define CVFILES_H

#include "CvCore.h"

/*
Open, read and then close a picture file and return an IplImage with a simple 
default color format. 
Use DestroyDefaultColorCvImg() or cvReleaseImage() to free the picture data at 
the end.
See the documentation of OpenCV for more information.

IplImage** ppImg : (INOUT) Valid pointer to the pointer to the structure.
char* filename : (IN) Path to the file to read.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CreateDefaultColorCvImgFromFile(IplImage** ppImg, char* filename)
{
	*ppImg = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
	
	if (*ppImg == NULL)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Save an IplImage to a file. 
Note that it is only a call to cvSaveImage().
See the documentation of OpenCV for more information.

IplImage* pImg : (IN) Pointer to the structure.
char* filename : (IN) Path to the file to write.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SaveDefaultColorCvImgToFile(IplImage* pImg, char* filename)
{
#if !defined(OPENCV220) && !defined(OPENCV230) && !defined(OPENCV231) && !defined(OPENCV242) && !defined(OPENCV243)
	if (!cvSaveImage(filename, pImg))
#else
	if (!cvSaveImage(filename, pImg, 0))
#endif // !defined(OPENCV220) && !defined(OPENCV230) && !defined(OPENCV231) && !defined(OPENCV242) && !defined(OPENCV243)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Open, read and then close a picture file and return an IplImage with a simple 
default gray format. 
Use DestroyDefaultGrayCvImg() or cvReleaseImage() to free the picture data at 
the end.
See the documentation of OpenCV for more information.

IplImage** ppImg : (INOUT) Valid pointer to the pointer to the structure.
char* filename : (IN) Path to the file to read.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CreateDefaultGrayCvImgFromFile(IplImage** ppImg, char* filename)
{
	*ppImg = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
	
	if (*ppImg == NULL)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Save an IplImage to a file. 
Note that it is only a call to cvSaveImage().
See the documentation of OpenCV for more information.

IplImage* pImg : (IN) Pointer to the structure.
char* filename : (IN) Path to the file to write.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SaveDefaultGrayCvImgToFile(IplImage* pImg, char* filename)
{
#if !defined(OPENCV220) && !defined(OPENCV230) && !defined(OPENCV231) && !defined(OPENCV242) && !defined(OPENCV243)
	if (!cvSaveImage(filename, pImg))
#else
	if (!cvSaveImage(filename, pImg, 0))
#endif // !defined(OPENCV220) && !defined(OPENCV230) && !defined(OPENCV231) && !defined(OPENCV242) && !defined(OPENCV243)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // CVFILES_H
