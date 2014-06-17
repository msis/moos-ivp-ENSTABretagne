/***************************************************************************************************************:')

WebcamCfg.h

Webcam handling.

Fabrice Le Bars

Created : 2009-06-17

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef WEBCAMCFG_H
#define WEBCAMCFG_H

#include "WebcamCore.h"

/*
Debug macros specific to WebcamCfg.
*/
#ifdef _DEBUG_MESSAGES_WEBCAMUTILS
#	define _DEBUG_MESSAGES_WEBCAMCFG
#endif // _DEBUG_MESSAGES_WEBCAMUTILS

#ifdef _DEBUG_WARNINGS_WEBCAMUTILS
#	define _DEBUG_WARNINGS_WEBCAMCFG
#endif // _DEBUG_WARNINGS_WEBCAMUTILS

#ifdef _DEBUG_ERRORS_WEBCAMUTILS
#	define _DEBUG_ERRORS_WEBCAMCFG
#endif // _DEBUG_ERRORS_WEBCAMUTILS

#ifdef _DEBUG_MESSAGES_WEBCAMCFG
#	define PRINT_DEBUG_MESSAGE_WEBCAMCFG(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_WEBCAMCFG(params)
#endif // _DEBUG_MESSAGES_WEBCAMCFG

#ifdef _DEBUG_WARNINGS_WEBCAMCFG
#	define PRINT_DEBUG_WARNING_WEBCAMCFG(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_WEBCAMCFG(params)
#endif // _DEBUG_WARNINGS_WEBCAMCFG

#ifdef _DEBUG_ERRORS_WEBCAMCFG
#	define PRINT_DEBUG_ERROR_WEBCAMCFG(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_WEBCAMCFG(params)
#endif // _DEBUG_ERRORS_WEBCAMCFG

typedef CvCapture* HWEBCAM;

#define INVALID_HWEBCAM_VALUE NULL

/*
Open a Webcam.

HWEBCAM* phWebcam : (INOUT) Valid pointer that will receive an identifier of the
Webcam opened.
UINT dev : (IN) Webcam number to open (0 for the first connected, 1, 2...).
UINT width : (IN) Width of the pictures to capture (in pixels).
UINT height : (IN) Height of the pictures to capture (in pixels).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int OpenWebcam(HWEBCAM* phWebcam, UINT dev, UINT width, UINT height)
{
	*phWebcam = cvCaptureFromCAM(dev);

	if (*phWebcam == NULL)
	{
		PRINT_DEBUG_ERROR_WEBCAMCFG(("OpenWebcam error (%s) : %s"
			"(dev=%u, width=%u,  height=%u)\n", 
			strtime_m(), 
			"Cannot open the webcam. ", 
			dev, width, height));
		return EXIT_FAILURE;
	}

	PRINT_DEBUG_MESSAGE_WEBCAMCFG(("Initial resolution : width=%u,  height=%u\n", 
							  (UINT)cvGetCaptureProperty(*phWebcam, CV_CAP_PROP_FRAME_WIDTH), 
							  (UINT)cvGetCaptureProperty(*phWebcam, CV_CAP_PROP_FRAME_HEIGHT)));

	cvSetCaptureProperty(*phWebcam, CV_CAP_PROP_FRAME_WIDTH, (double)width);
	//PRINT_DEBUG_MESSAGE_WEBCAMCFG(("%s\n", cvErrorStr(cvGetErrStatus())));

	cvSetCaptureProperty(*phWebcam, CV_CAP_PROP_FRAME_HEIGHT, (double)height);
	//PRINT_DEBUG_MESSAGE_WEBCAMCFG(("%s\n", cvErrorStr(cvGetErrStatus())));

#ifndef DISABLE_CAMERA_IMAGE_RESIZING_CHECK
	if (((UINT)cvGetCaptureProperty(*phWebcam, CV_CAP_PROP_FRAME_WIDTH) != width)||
		((UINT)cvGetCaptureProperty(*phWebcam, CV_CAP_PROP_FRAME_HEIGHT) != height))
	{
		PRINT_DEBUG_ERROR_WEBCAMCFG(("OpenWebcam error (%s) : %s"
			"(dev=%u, width=%u,  height=%u)\n", 
			strtime_m(), 
			"Cannot set the size of the capture window. ", 
			dev, width, height));
		cvReleaseCapture(phWebcam);
		return EXIT_FAILURE;
	}
#endif // DISABLE_CAMERA_IMAGE_RESIZING_CHECK

	PRINT_DEBUG_MESSAGE_WEBCAMCFG(("Final resolution : width=%u,  height=%u\n", 
							  (UINT)cvGetCaptureProperty(*phWebcam, CV_CAP_PROP_FRAME_WIDTH), 
							  (UINT)cvGetCaptureProperty(*phWebcam, CV_CAP_PROP_FRAME_HEIGHT)));

	return EXIT_SUCCESS;
}

/*
Get a picture from a Webcam.
Do not try to release *ppImg in any way, it is handled internally.

HWEBCAM hWebcam : (IN) Identifier of the Webcam.
IplImage** ppImg : (INOUT) Valid pointer that will receive the image.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetPictureWebcam(HWEBCAM hWebcam, IplImage** ppImg)
{
	*ppImg = cvQueryFrame(hWebcam);
	//PRINT_DEBUG_MESSAGE_WEBCAMCFG(("%s\n", cvErrorStr(cvGetErrStatus())));

	if (*ppImg == NULL)
	{
		PRINT_DEBUG_ERROR_WEBCAMCFG(("GetPictureWebcam error (%s) : %s"
			"(hWebcam=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hWebcam));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Close a Webcam.

HWEBCAM* phWebcam : (INOUT) Valid pointer to the identifier of the Webcam.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CloseWebcam(HWEBCAM* phWebcam)
{
	cvReleaseCapture(phWebcam);

	*phWebcam = INVALID_HWEBCAM_VALUE;

	return EXIT_SUCCESS;
}

#endif // WEBCAMCFG_H
