/***************************************************************************************************************:')

WebcamMgr.c

Webcam handling.

Fabrice Le Bars

Created : 2009-06-24

Version status : Not finished

***************************************************************************************************************:)*/

#include "WebcamMgr.h"

THREAD_PROC_RETURN_VALUE WebcamMgrThreadProc(void* lpParam)	
{
	HWEBCAMMGR hWebcamMgr = (HWEBCAMMGR)lpParam;
	IplImage* pImg = NULL;

	do
	{
		mSleep(hWebcamMgr->PollingTimeInterval);

		if (!hWebcamMgr->bConnected)
		{ 
			// The device is not connected, trying to connect or reconnect.
			ConnectToWebcam(hWebcamMgr);
		}
		else	
		{
			// Wait for new data.
			if (hWebcamMgr->PollingMode & IMG_POLLING_MODE_WEBCAM)
			{
				EnterCriticalSection(&hWebcamMgr->WebcamCS);

				if (!hWebcamMgr->bConnected)
				{ 
					LeaveCriticalSection(&hWebcamMgr->WebcamCS);
					continue;
				}

				if (GetPictureWebcam(hWebcamMgr->hWebcam, &pImg) != EXIT_SUCCESS)
				{
					PRINT_DEBUG_MESSAGE_WEBCAMMGR(("Connection to a Webcam lost\n"));

					// The device was supposed to be connected before.
					// Notify the thread to try to reconnect the device.
					hWebcamMgr->bConnected = FALSE; 
					CloseWebcam(&hWebcamMgr->hWebcam);

					LeaveCriticalSection(&hWebcamMgr->WebcamCS);
					continue;
				}

				LeaveCriticalSection(&hWebcamMgr->WebcamCS);

				EnterCriticalSection(&hWebcamMgr->pImgCS);
				hWebcamMgr->imgcounter++;
				CopyDefaultColorCvImg(&hWebcamMgr->pImg, pImg, TRUE);
				LeaveCriticalSection(&hWebcamMgr->pImgCS);
			}
		}
	} 
	while (hWebcamMgr->bRunThread);

	EnterCriticalSection(&hWebcamMgr->WebcamCS);

	if (hWebcamMgr->bConnected)
	{ 
		hWebcamMgr->bConnected = FALSE; 

		if (CloseWebcam(&hWebcamMgr->hWebcam) != EXIT_SUCCESS)
		{
			PRINT_DEBUG_ERROR_WEBCAMMGR(("WebcamMgrThreadProc error (%s) : %s"
				"\n", 
				strtime_m(), 
				". "));
		}
	}
	
	LeaveCriticalSection(&hWebcamMgr->WebcamCS);

	return 0; 
} 

