/***************************************************************************************************************:')

WebcamCfg.h

Webcam handling.

Fabrice Le Bars

Created: 2009-06-17

Version status: Not finished

***************************************************************************************************************:)*/

#ifndef WEBCAMCFG_H
#define WEBCAMCFG_H

#include "WebcamCoreVFWw32.h"

/*
Debug macros specific to WebcamCfg.
*/
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

struct WEBCAM
{
	HWND hWndC;
	PPMDATA img;
};
typedef struct WEBCAM* HWEBCAM;


//typedef struct HWND HWEBCAM;
//#define INVALID_HWEBCAM_VALUE INVALID_HANDLE_VALUE

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
__inline int OpenWebcam(HWEBCAM* phWebcam, UINT dev, UINT width, UINT height)
{
	CAPDRIVERCAPS CapDrvCaps; 
	CAPSTATUS CapStatus;
	CAPTUREPARMS CapParams;
	LPBITMAPINFO lpbi;
	DWORD dwSize;

	memset(&CapDrvCaps, 0, sizeof(CAPDRIVERCAPS));
	memset(&CapStatus, 0, sizeof(CAPSTATUS));
	memset(&CapParams, 0, sizeof(CAPTUREPARMS));

	*phWebcam = (HWEBCAM)calloc(1, sizeof(struct WEBCAM));

	if (*phWebcam == NULL)
	{
		PRINT_DEBUG_ERROR_WEBCAMCFG(("OpenWebcam error (%s) : %s"
			"(dev=%u, width=%u,  height=%u)\n", 
			strtime_m(), 
			"Out of memory. ", 
			dev, width, height));
		return EXIT_FAILURE;
	}

	// Create the capture window.
	(*phWebcam)->hWndC = capCreateCaptureWindow(
		(LPSTR)"Capture Window", // window name if pop-up 
		WS_CHILD | WS_POPUP,       // window style 
		0, 0, width, height,         // window position and dimensions
		0, // no parent window
		(int)0						// child ID
		); 

	// Connect to the webcam. 
	if (!capDriverConnect((*phWebcam)->hWndC, dev))
	{
		DestroyWindow((*phWebcam)->hWndC);
		free(*phWebcam);
		PRINT_DEBUG_ERROR_WEBCAMCFG(("OpenWebcam error (%s) : %s"
			"(dev=%u, width=%u,  height=%u)\n", 
			strtime_m(), 
			"Cannot open the webcam. ", 
			dev, width, height));
		return EXIT_FAILURE;
	}

	if (!capDriverGetCaps((*phWebcam)->hWndC, &CapDrvCaps, sizeof (CAPDRIVERCAPS)))
	{
		DestroyWindow((*phWebcam)->hWndC);
		free(*phWebcam);
		PRINT_DEBUG_ERROR_WEBCAMCFG(("OpenWebcam error (%s) : %s"
			"(dev=%u, width=%u,  height=%u)\n", 
			strtime_m(), 
			"Unable to get driver capabilities. ", 
			dev, width, height));
		return EXIT_FAILURE;
	}

	if (CapDrvCaps.fHasOverlay) 
	{
		capOverlay((*phWebcam)->hWndC, TRUE);
	}

	if (!capGetStatus((*phWebcam)->hWndC, &CapStatus, sizeof (CAPSTATUS)))
	{
		DestroyWindow((*phWebcam)->hWndC);
		free(*phWebcam);
		PRINT_DEBUG_ERROR_WEBCAMCFG(("OpenWebcam error (%s) : %s"
			"(dev=%u, width=%u,  height=%u)\n", 
			strtime_m(), 
			"Unable to get capture status. ", 
			dev, width, height));
		return EXIT_FAILURE;
	}

	PRINT_DEBUG_MESSAGE_WEBCAMCFG(("Initial resolution : width=%u,  height=%u\n", 
							  CapStatus.uiImageWidth, 
							  CapStatus.uiImageHeight));

	(*phWebcam)->img.Width = width;
	(*phWebcam)->img.Height = height;
	(*phWebcam)->img.NbPixels = width*height;
	(*phWebcam)->img.NbElements = 3*(*phWebcam)->img.NbPixels;
	(*phWebcam)->img.Size = (*phWebcam)->img.NbElements*sizeof(UCHAR);
	(*phWebcam)->img.ColorDepth = 255;

	SetWindowPos((*phWebcam)->hWndC, NULL, 0, 0, CapStatus.uiImageWidth, 
		CapStatus.uiImageHeight, SWP_NOZORDER | SWP_NOMOVE); 

	dwSize = capGetVideoFormatSize((*phWebcam)->hWndC);
	lpbi = (LPBITMAPINFO)GlobalAlloc (GHND, dwSize);
	capGetVideoFormat((*phWebcam)->hWndC, lpbi, dwSize); 
	lpbi->bmiHeader.biSize = 40;
	lpbi->bmiHeader.biWidth = width;
	lpbi->bmiHeader.biHeight = height;
	lpbi->bmiHeader.biPlanes = 1; 
	lpbi->bmiHeader.biBitCount = 24; 
	lpbi->bmiHeader.biCompression = BI_RGB; 
	lpbi->bmiHeader.biSizeImage = lpbi->bmiHeader.biWidth * lpbi->bmiHeader.biHeight * 3; 
	lpbi->bmiHeader.biClrImportant = 0; 
	lpbi->bmiHeader.biClrUsed = 0; 
	lpbi->bmiHeader.biXPelsPerMeter = 0; 
	lpbi->bmiHeader.biYPelsPerMeter = 0; 

	if (!capSetVideoFormat((*phWebcam)->hWndC, lpbi, dwSize))
	{
		GlobalFree(lpbi);
		DestroyWindow((*phWebcam)->hWndC);
		free(*phWebcam);
		PRINT_DEBUG_ERROR_WEBCAMCFG(("OpenWebcam error (%s) : %s"
			"(dev=%u, width=%u,  height=%u)\n", 
			strtime_m(), 
			"Unsupported video format. ", 
			dev, width, height));
		return EXIT_FAILURE;
	}

	GlobalFree(lpbi);

	capCaptureGetSetup((*phWebcam)->hWndC, &CapParams, sizeof(CAPTUREPARMS));
	CapParams.fCaptureAudio = FALSE;
	CapParams.fLimitEnabled = FALSE;
	CapParams.fYield = FALSE;
	CapParams.fMakeUserHitOKToCapture = FALSE;
	CapParams.fAbortLeftMouse = FALSE;
	CapParams.fAbortRightMouse = FALSE;
	CapParams.vKeyAbort = VK_ESCAPE;
	capCaptureSetSetup((*phWebcam)->hWndC, &CapParams, sizeof(CAPTUREPARMS));

	// Register the frame callback function using the
	// capSetCallbackOnFrame macro. 
	capSetCallbackOnFrame((*phWebcam)->hWndC, capFrameCallback); 

	//capPreviewRate((*phWebcam)->hWndC, 1);     // rate, in milliseconds
	//capPreview((*phWebcam)->hWndC, TRUE);       // starts preview 

	return EXIT_SUCCESS;
}

/*
Get a picture from a Webcam.
Do not try to release *ppImg in any way, it is handled internally.

HWEBCAM hWebcam : (IN) Identifier of the Webcam.
PPMDATA* pImg : (INOUT) Valid pointer that will receive the image.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
__inline int GetPictureWebcam(HWEBCAM hWebcam, PPMDATA* pImg)
{
	if (!capGrabFrameNoStop(hWebcam->hWndC))
	{
		PRINT_DEBUG_ERROR_WEBCAMCFG(("GetPictureWebcam error (%s) : %s"
			"(hWebcam=0x%x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hWebcam));
		return EXIT_FAILURE;
	}

	hWebcam->img.Img = (UCHAR*)capGetUserData(hWebcam->hWndC);

	if (hWebcam->img.Img == NULL)
	{
		PRINT_DEBUG_ERROR_WEBCAMCFG(("GetPictureWebcam error (%s) : %s"
			"(hWebcam=0x%x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hWebcam));
		return EXIT_FAILURE;
	}

	*pImg = hWebcam->img;

	return EXIT_SUCCESS;
}

/*
Close a Webcam.

HWEBCAM* phWebcam : (INOUT) Valid pointer to the identifier of the Webcam.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
__inline int CloseWebcam(HWEBCAM* phWebcam)
{
	BOOL bError = FALSE;

	if (!capSetCallbackOnFrame((*phWebcam)->hWndC, NULL))
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_WEBCAMCFG(("CloseWebcam error (%s) : %s"
			"(*phWebcam=0x%x)\n", 
			strtime_m(), 
			"Could not stop capture. ", 
			*phWebcam));
	}
	
	if (!capDriverDisconnect((*phWebcam)->hWndC))
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_WEBCAMCFG(("CloseWebcam error (%s) : %s"
			"(*phWebcam=0x%x)\n", 
			strtime_m(), 
			"Could not stop capture. ", 
			*phWebcam));
	}
	
	if (!DestroyWindow((*phWebcam)->hWndC))
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_WEBCAMCFG(("CloseWebcam error (%s) : %s"
			"(*phWebcam=0x%x)\n", 
			strtime_m(), 
			"DestroyWindow() failed. ", 
			*phWebcam));
	}

	free(*phWebcam);

	*phWebcam = INVALID_HWEBCAM_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // WEBCAMCFG_H
