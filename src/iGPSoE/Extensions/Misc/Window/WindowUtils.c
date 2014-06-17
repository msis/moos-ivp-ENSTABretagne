/***************************************************************************************************************:')

Window_Utils.c

Graphical user interface functions.

Fabrice Le Bars

2006-2007

0.2

To do : 
To give up...

***************************************************************************************************************:)*/

#include "WindowUtils.h"

#ifdef _WIN32
/*
Creates a window.

int x : (IN) position of the window on the screen (x-axis)
int y : (IN) position of the window on the screen (y-axis)
int width : (IN) width of the window
int height : (IN) height of the window
char* title : (IN) title of the window
WNDPROC MsgProc : (IN) callback function to handle the messages sent to the window

Returns : the handle of the created window
*/
HWND CreateDefaultWindow(int x, int y, int width, int height, char* title, WNDPROC MsgProc)	{

	HWND hWnd;
	WNDCLASSEX wc;

	// Register the window class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = MsgProc;
	wc.cbClsExtra = 0L;
	wc.cbWndExtra = 0L;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = title;
	wc.hIconSm = NULL;
	RegisterClassEx(&wc);

	// Create the application's window
	hWnd = CreateWindow(title, title,
		WS_OVERLAPPEDWINDOW, x, y, width, height+25,
		NULL, NULL, wc.hInstance, NULL);

	// Show the window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);

	return hWnd;
}
#else
/*
Creates a window.

Display* pDisplay : (IN) display of the window
int x : (IN) position of the window on the screen (x-axis)
int y : (IN) position of the window on the screen (y-axis)
int width : (IN) width of the window
int height : (IN) height of the window
char* title : (IN) title of the window

Returns : the identifier of the created window
*/
Window CreateDefaultWindow(Display* pDisplay, int x, int y, int width, int height, char* title)	{

	Window win;
	XSetWindowAttributes attr;

	attr.background_pixel = WhitePixel(pDisplay, DefaultScreen(pDisplay)); // Value of a white pixel on this screen.
	attr.border_pixel = BlackPixel(pDisplay, DefaultScreen(pDisplay)); // Value of a black pixel on this screen.
	attr.colormap = DefaultColormap(pDisplay, DefaultScreen(pDisplay));

	// this variable will store the ID of the newly created window
	win = XCreateWindow (
		pDisplay, 
		RootWindow(pDisplay, DefaultScreen(pDisplay)), // ID of the root window of our screen. Each screen always has a root window that covers the whole screen, and always exists.                 
		x, // Horizontal window's location
		y, // Vertical window's location
		width, // Window's width
		height, // Window's height
		2, // Border width
		DisplayPlanes(pDisplay, DefaultScreen(pDisplay)), 
		CopyFromParent, 
		DefaultVisual(pDisplay, DefaultScreen(pDisplay)), 
		CWBackPixel | CWBorderPixel | CWColormap, 
		&attr
		);

	XMapWindow(pDisplay, win);
	XStoreName(pDisplay, win, title);
	XSelectInput(pDisplay, win, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask | PointerMotionHintMask | Button1MotionMask | Button2MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask | ButtonMotionMask | KeymapStateMask | VisibilityChangeMask | StructureNotifyMask | ResizeRedirectMask/* | SubstructureNotifyMask*/ |SubstructureRedirectMask | FocusChangeMask | PropertyChangeMask | ColormapChangeMask | OwnerGrabButtonMask);

	return win;
}
#endif
