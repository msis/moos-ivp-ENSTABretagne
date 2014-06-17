/***************************************************************************************************************:')

WebcamMgr.h

Webcam handling.

Fabrice Le Bars

Created : 2009-06-24

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef WEBCAMMGR_H
#define WEBCAMMGR_H

#include "OSThread.h"
#include "OSCriticalSection.h"
#include "WebcamCfg.h"

/*
Debug macros specific to WebcamMgr.
*/
#ifdef _DEBUG_MESSAGES_WEBCAMUTILS
#	define _DEBUG_MESSAGES_WEBCAMMGR
#endif // _DEBUG_MESSAGES_WEBCAMUTILS

#ifdef _DEBUG_WARNINGS_WEBCAMUTILS
#	define _DEBUG_WARNINGS_WEBCAMMGR
#endif // _DEBUG_WARNINGS_WEBCAMUTILS

#ifdef _DEBUG_ERRORS_WEBCAMUTILS
#	define _DEBUG_ERRORS_WEBCAMMGR
#endif // _DEBUG_ERRORS_WEBCAMUTILS

#ifdef _DEBUG_MESSAGES_WEBCAMMGR
#	define PRINT_DEBUG_MESSAGE_WEBCAMMGR(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_WEBCAMMGR(params)
#endif // _DEBUG_MESSAGES_WEBCAMMGR

#ifdef _DEBUG_WARNINGS_WEBCAMMGR
#	define PRINT_DEBUG_WARNING_WEBCAMMGR(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_WEBCAMMGR(params)
#endif // _DEBUG_WARNINGS_WEBCAMMGR

#ifdef _DEBUG_ERRORS_WEBCAMMGR
#	define PRINT_DEBUG_ERROR_WEBCAMMGR(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_WEBCAMMGR(params)
#endif // _DEBUG_ERRORS_WEBCAMMGR

#define IMG_POLLING_MODE_WEBCAM 0x00000001

struct WEBCAMMGR
{
	HWEBCAM hWebcam;
	UINT dev;
	UINT width;
	UINT height;
	THREAD_IDENTIFIER ThreadId;
	int PollingTimeInterval;
	int PollingMode;
	BOOL bRunThread;
	BOOL bConnected;
	CRITICAL_SECTION WebcamCS;
	CRITICAL_SECTION pImgCS;
	UINT imgcounter;
	IplImage* pImg;
	BOOL bWaitForFirstData;
};
typedef struct WEBCAMMGR* HWEBCAMMGR;

#define INVALID_HWEBCAMMGR_VALUE NULL

/*
Private function.
Thread function that handles the polling and automatic reconnection of a Webcam.

LPVOID lpParam : (INOUT) Parameter of the thread.

Return : 0.
*/
EXTERN_C THREAD_PROC_RETURN_VALUE WebcamMgrThreadProc(void* lpParam);

/*
Private function.
Open a Webcam and get data from it.

HWEBCAM hWebcam : (IN) Identifier of the Webcam.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ConnectToWebcam(HWEBCAMMGR hWebcamMgr)
{
	IplImage* pImg = NULL;

	// No critical section is needed here because the other functions 
	// should not try to access the device or data while the device is
	// not connected (bConnected = FALSE).

	if (OpenWebcam(&hWebcamMgr->hWebcam, hWebcamMgr->dev, 
		hWebcamMgr->width, hWebcamMgr->height) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_MESSAGE_WEBCAMMGR(("Unable to connect to a Webcam\n"));
		return EXIT_FAILURE;
	}

	// Get data before setting bConnected to TRUE to be sure that
	// the other functions will get valid data.
	if (hWebcamMgr->bWaitForFirstData)
	{
		if (hWebcamMgr->PollingMode & IMG_POLLING_MODE_WEBCAM)
		{
			if (GetPictureWebcam(hWebcamMgr->hWebcam, &pImg) != EXIT_SUCCESS)
			{
				PRINT_DEBUG_MESSAGE_WEBCAMMGR(("Unable to connect to a Webcam\n"));
				return EXIT_FAILURE;
			}
		}
	}

	CopyDefaultColorCvImg(&hWebcamMgr->pImg, pImg, TRUE);

	hWebcamMgr->bConnected = TRUE;
	PRINT_DEBUG_MESSAGE_WEBCAMMGR(("Webcam connected\n"));

	return EXIT_SUCCESS;
}

/*
Initialize a structure that manage the polling and the reconnection of a Webcam.

HWEBCAMMGR* phWebcamMgr : (INOUT) Valid pointer that will receive an identifier of
the structure.
UINT dev : (IN) Webcam number to open (0 for the first connected, 1, 2...).
UINT width : (IN) Width of the pictures to capture (in pixels).
UINT height : (IN) Height of the pictures to capture (in pixels).
int PollingTimeInterval : (IN) Time interval between 2 requests of data (in ms).
int PollingMode : (IN) Either IMG_POLLING_MODE_WEBCAM or 0 if you do not
want polling.
int Priority : (IN) Priority of the thread that managess the polling and the 
reconnection of the device. See SetThreadDefaultPriority().
BOOL bWaitForFirstData : (IN) TRUE to wait for the first entire data in 
InitWebcamMgr(), FALSE if it does not matter if the first data after the 
connection to the device is invalid during some seconds.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitWebcamMgr(HWEBCAMMGR* phWebcamMgr, UINT dev, 
						   UINT width, UINT height, 
						   int PollingTimeInterval,
						   int PollingMode,
						   int Priority,
						   BOOL bWaitForFirstData)
{
	*phWebcamMgr = (HWEBCAMMGR)calloc(1, sizeof(struct WEBCAMMGR));

	if (*phWebcamMgr == NULL)
	{
		PRINT_DEBUG_ERROR_WEBCAMMGR(("InitWebcamMgr error (%s) : %s"
			"(dev=%u)\n", 
			strtime_m(), 
			"Out of memory. ", 
			dev));
		return EXIT_FAILURE;
	}

	(*phWebcamMgr)->dev = dev;
	(*phWebcamMgr)->width = width;
	(*phWebcamMgr)->height = height;
	(*phWebcamMgr)->PollingTimeInterval = PollingTimeInterval;
	(*phWebcamMgr)->PollingMode = PollingMode;
	(*phWebcamMgr)->bConnected = FALSE;
	(*phWebcamMgr)->bRunThread = TRUE;
	(*phWebcamMgr)->imgcounter = 0;

	if (CreateDefaultColorCvImg(&(*phWebcamMgr)->pImg, width, height) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_WEBCAMMGR(("InitWebcamMgr error (%s) : %s"
			"(dev=%u)\n", 
			strtime_m(), 
			"Out of memory. ", 
			dev));
		free(*phWebcamMgr);
		return EXIT_FAILURE;
	}

	(*phWebcamMgr)->bWaitForFirstData = bWaitForFirstData;

	if (
		(InitCriticalSection(&(*phWebcamMgr)->WebcamCS) != EXIT_SUCCESS)||
		(InitCriticalSection(&(*phWebcamMgr)->pImgCS) != EXIT_SUCCESS)		
		)
	{
		PRINT_DEBUG_ERROR_WEBCAMMGR(("InitWebcamMgr error (%s) : %s"
			"(dev=%u)\n", 
			strtime_m(), 
			"Error creating a critical section. ", 
			dev));
		DestroyDefaultColorCvImg(&(*phWebcamMgr)->pImg);
		DeleteCriticalSection(&(*phWebcamMgr)->pImgCS);
		DeleteCriticalSection(&(*phWebcamMgr)->WebcamCS);
		free(*phWebcamMgr);
		return EXIT_FAILURE;
	}

	// First attempt to connect to the device.
	ConnectToWebcam(*phWebcamMgr);

	if (CreateDefaultThread(WebcamMgrThreadProc, (void*)*phWebcamMgr, &(*phWebcamMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_WEBCAMMGR(("InitWebcamMgr error (%s) : %s"
			"(dev=%u)\n", 
			strtime_m(), 
			"Error creating a thread. ", 
			dev));
		if ((*phWebcamMgr)->bConnected)
		{
			CloseWebcam(&(*phWebcamMgr)->hWebcam);
		}
		DestroyDefaultColorCvImg(&(*phWebcamMgr)->pImg);
		DeleteCriticalSection(&(*phWebcamMgr)->pImgCS);
		DeleteCriticalSection(&(*phWebcamMgr)->WebcamCS);
		free(*phWebcamMgr);
		return EXIT_FAILURE;
	}

#ifdef ENABLE_PRIORITY_HANDLING
	if (SetThreadDefaultPriority((*phWebcamMgr)->ThreadId, Priority) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_WEBCAMMGR(("InitWebcamMgr error (%s) : %s"
			"(dev=%u)\n", 
			strtime_m(), 
			"Error setting the priority of a thread. ", 
			dev));
	}
#else
	UNREFERENCED_PARAMETER(Priority);
#endif // ENABLE_PRIORITY_HANDLING

	return EXIT_SUCCESS;
}

/*
Get a picture from a Webcam (thread safe).
If PollingMode have the IMG_POLLING_MODE_WEBCAM flag set, get the last data 
polled in the last PollingTimeInterval, otherwise a direct thread safe request 
is made to the device.

HWEBCAMMGR hWebcamMgr : (IN) Identifier of the structure that manages the polling 
and the reconnection of a Webcam.
IplImage* pImg : (INOUT) Valid pointer receiving the picture.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetPictureWebcamMgr(HWEBCAMMGR hWebcamMgr, IplImage* pImg)
{
	IplImage* pImg0 = NULL;

	if (hWebcamMgr->PollingMode & IMG_POLLING_MODE_WEBCAM)
	{
		EnterCriticalSection(&hWebcamMgr->pImgCS);

		if (!hWebcamMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_WEBCAMMGR(("GetPictureWebcamMgr error (%s) : %s"
				"(hWebcamMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hWebcamMgr));

			LeaveCriticalSection(&hWebcamMgr->pImgCS);
			return EXIT_FAILURE;
		}

		// Get the last value stored.
		CopyDefaultColorCvImg(&pImg, hWebcamMgr->pImg, TRUE);

		LeaveCriticalSection(&hWebcamMgr->pImgCS);
	}
	else
	{
		EnterCriticalSection(&hWebcamMgr->WebcamCS);

		if (!hWebcamMgr->bConnected)
		{ 
			PRINT_DEBUG_ERROR_WEBCAMMGR(("GetPictureWebcamMgr error (%s) : %s"
				"(hWebcamMgr=%#x)\n", 
				strtime_m(), 
				"Device not currently connected. ", 
				hWebcamMgr));

			LeaveCriticalSection(&hWebcamMgr->WebcamCS);
			return EXIT_FAILURE;
		}

		// Get the value directly from the device.
		if (GetPictureWebcam(hWebcamMgr->hWebcam, &pImg0) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_MESSAGE_WEBCAMMGR(("Connection to a Webcam lost\n"));
			PRINT_DEBUG_ERROR_WEBCAMMGR(("GetPictureWebcamMgr error (%s) : %s"
				"(hWebcamMgr=%#x)\n", 
				strtime_m(), 
				"Error getting data from the device. ", 
				hWebcamMgr));

			// The device was supposed to be connected before.
			// Notify the thread to try to reconnect the device.
			hWebcamMgr->bConnected = FALSE; 
			CloseWebcam(&hWebcamMgr->hWebcam);

			LeaveCriticalSection(&hWebcamMgr->WebcamCS);
			return EXIT_FAILURE;
		}

		CopyDefaultColorCvImg(&pImg, pImg0, TRUE);

		LeaveCriticalSection(&hWebcamMgr->WebcamCS);
	}

	return EXIT_SUCCESS;
}

/*
Release a structure that manage the polling and the reconnection of a Webcam.

HWEBCAMMGR* phWebcamMgr : (INOUT) Valid pointer to the identifier of the structure  
that manages the polling and the reconnection of a Webcam.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReleaseWebcamMgr(HWEBCAMMGR* phWebcamMgr)
{
	BOOL bError = FALSE;

	// Indicate to the thread that it should terminate its loop.
	(*phWebcamMgr)->bRunThread = FALSE;

	if (WaitForThread((*phWebcamMgr)->ThreadId) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_WEBCAMMGR(("ReleaseWebcamMgr error (%s) : %s"
			"(*phWebcamMgr=%#x)\n", 
			strtime_m(), 
			"Error waiting for the thread. ", 
			*phWebcamMgr));
		bError = TRUE;
	}

	DestroyDefaultColorCvImg(&(*phWebcamMgr)->pImg);

	DeleteCriticalSection(&(*phWebcamMgr)->pImgCS);
	DeleteCriticalSection(&(*phWebcamMgr)->WebcamCS);

	free(*phWebcamMgr);

	*phWebcamMgr = INVALID_HWEBCAMMGR_VALUE;

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // WEBCAMMGR_H
