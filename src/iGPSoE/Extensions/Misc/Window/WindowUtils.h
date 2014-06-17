/***************************************************************************************************************:')

Window_Utils.h

Graphical user interface functions.

Fabrice Le Bars

2006-2007

0.2

To do : 
To give up...

***************************************************************************************************************:)*/

#ifndef WINDOWUTILS_H
#define WINDOWUTILS_H

#include "OSCore.h"

#ifdef _WIN32
#else 
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef _WIN32
HWND CreateDefaultWindow(int x, int y, int width, int height, char* title, WNDPROC MsgProc);
#else
Window CreateDefaultWindow(Display* pDisplay, int x, int y, int width, int height, char* title);
#endif

#endif // WINDOWUTILS_H
