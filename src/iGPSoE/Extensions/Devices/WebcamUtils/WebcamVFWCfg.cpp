#include "WebcamVFWCfg.h"


HWND hWndC;
UCHAR* pImg = (UCHAR*)23;



LRESULT CALLBACK capVideoStreamCallback(HWND hWnd, LPVIDEOHDR lpVHdr)	{ 

	int i = 0, j = 0;
	int nbBytes = (int)((lpVHdr->dwBufferLength/4)*3);
	char r, g, b, a;
	char *p = (char*)malloc(nbBytes);

	while (j < (int)(lpVHdr->dwBufferLength)) {
		b = (lpVHdr->lpData)[j+0];
		g = (lpVHdr->lpData)[j+1];
		r = (lpVHdr->lpData)[j+2];
		a = (lpVHdr->lpData)[j+3];
		p[i+0] = (char)b;
		p[i+1] = (char)g;
		p[i+2] = (char)r;
		i+=3;
		j+=4;
	}

HWND hWndParent = GetParent(hWnd);
 int thing = GetWindowLongPtr(hWndParent, GWLP_USERDATA);
	LONG_PTR ret = SetWindowLongPtr(hWndParent, GWLP_USERDATA, (LONG_PTR)lpVHdr->lpData);


	//SendTCP(ClientSocket, (char*)p, (int)nbBytes);
	//SendTCP(ClientSocket, (char*)(lpVHdr->lpData), (int)(lpVHdr->dwBufferLength));

	return (LRESULT) TRUE ; 
} 

LRESULT CALLBACK capYieldCallback(HWND hWnd){ 

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	int bRet = 0;
	if (bRet = GetMessage(&msg, NULL, 0, 0) != 0)	{
		if (bRet == -1)	{
			return (LRESULT) FALSE;
		}
		else	{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (LRESULT) TRUE; 
} 

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )	{

	PAINTSTRUCT ps;
	HDC hdc;

	switch( msg )
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN: 
		switch (wParam) 
		{ 
		case VK_ESCAPE: 
			PostQuitMessage(0);
			return 0;
		default: 
			return DefWindowProc(hWnd, msg, wParam, lParam); 
		} 
		break; 
	case WM_MOUSEMOVE: 

		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps); 
		break;
	default:
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


int VFWInit()	{

	// Register the window class
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, sizeof(void*),
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"VFWSrv", NULL };
	RegisterClassEx( &wc );

	HWND hWnd = CreateWindow( "VFWSrv", "VFWSrv",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 1, 1,
		NULL, NULL, wc.hInstance, NULL );

	LONG_PTR ret = SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pImg);

	hWndC = capCreateCaptureWindow (
		(LPSTR) "Capture Window", // window name if pop-up 
		WS_CHILD | WS_VISIBLE,       // window style 
		0, 0, 320, 240,         // window position and dimensions
		(HWND) hWnd, 
		(int) 0						// child ID
		);

	// connect to the MSVIDEO driver: 
	capDriverConnect(hWndC, 0); 

	CAPDRIVERCAPS CapDrvCaps; 

	if(!capDriverGetCaps(hWndC, &CapDrvCaps, sizeof (CAPDRIVERCAPS)))	{
		MessageBox(NULL, "Unable to get driver capabilities", "ERROR", MB_OK);
		return EXIT_FAILURE;
	}

	if (CapDrvCaps.fHasOverlay) 
		capOverlay(hWndC, TRUE);

	CAPSTATUS CapStatus;

	if(!capGetStatus(hWndC, &CapStatus, sizeof (CAPSTATUS)))	{
		MessageBox(NULL, "Unable to get capture status", "ERROR", MB_OK);
		return EXIT_FAILURE;
	}

	SetWindowPos(hWndC, NULL, 0, 0, CapStatus.uiImageWidth, 
		CapStatus.uiImageHeight, SWP_NOZORDER | SWP_NOMOVE); 

	LPBITMAPINFO lpbi;
	DWORD dwSize;

	dwSize = capGetVideoFormatSize(hWndC);
	lpbi = (LPBITMAPINFO)GlobalAlloc (GHND, dwSize);
	capGetVideoFormat(hWndC, lpbi, dwSize); 
	lpbi->bmiHeader.biSize = 40;
	lpbi->bmiHeader.biWidth = 320;
	lpbi->bmiHeader.biHeight = 240;
	lpbi->bmiHeader.biPlanes = 1; 
	lpbi->bmiHeader.biBitCount = 24; 
	lpbi->bmiHeader.biCompression = BI_RGB; 
	lpbi->bmiHeader.biSizeImage = lpbi->bmiHeader.biWidth * lpbi->bmiHeader.biHeight * 3; 
	lpbi->bmiHeader.biClrImportant = 0; 
	lpbi->bmiHeader.biClrUsed = 0; 
	lpbi->bmiHeader.biXPelsPerMeter = 0; 
	lpbi->bmiHeader.biYPelsPerMeter = 0; 
	if(!capSetVideoFormat(hWndC, lpbi, dwSize))	{
		GlobalFree(lpbi);
		MessageBox(NULL, "Unsupported video format", "ERROR", MB_OK);
		return EXIT_FAILURE;
	}
	GlobalFree(lpbi);

	CAPTUREPARMS CapParams;
	capCaptureGetSetup(hWndC, &CapParams, sizeof(CAPTUREPARMS));
	CapParams.fCaptureAudio = FALSE;
	CapParams.fLimitEnabled = FALSE;
	CapParams.fYield = FALSE;
	CapParams.fMakeUserHitOKToCapture = TRUE;
	CapParams.fAbortLeftMouse = FALSE;
	CapParams.fAbortRightMouse = FALSE;
	CapParams.vKeyAbort = VK_ESCAPE;
	capCaptureSetSetup(hWndC, &CapParams, sizeof(CAPTUREPARMS));

	// Register the video-stream callback function using the
	// capSetCallbackOnVideoStream macro. 
	capSetCallbackOnVideoStream(hWndC, capVideoStreamCallback); 

	// Register the frame callback function using the
	// capSetCallbackOnFrame macro. 
	//capSetCallbackOnFrame(hWndC, capFrameCallback); 

	capSetCallbackOnYield(hWndC, capYieldCallback); 

	// Access the video format and then free the allocated memory.

	capPreviewRate(hWndC, 1);     // rate, in milliseconds
	capPreview(hWndC, TRUE);       // starts preview 

	// Preview

	// Set up the capture operation.
	capCaptureSequenceNoFile(hWndC); 

	// Capture.

	return EXIT_SUCCESS;
}

int VFWGetPict()
{
	HWND hWndParent = GetParent(hWndC);

void* pict = GetWindowLongPtr(hWndParent, GWLP_USERDATA);

	return EXIT_SUCCESS;
}

int VFWStop()	{

	//capSetCallbackOnVideoStream(hWndC, NULL); 
	capCaptureStop(hWndC);
	capPreview(hWndC, FALSE);
	//capOverlay(hWndC, FALSE);
	capDriverDisconnect(hWndC);

	return EXIT_SUCCESS;
}
