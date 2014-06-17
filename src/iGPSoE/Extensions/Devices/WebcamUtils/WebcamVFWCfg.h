#pragma once

#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
#include <stdio.h>
#include <string.h>
#include <Vfw.h>
#include <Shellapi.h>


LRESULT CALLBACK capVideoStreamCallback(HWND hWnd, LPVIDEOHDR lpVHdr);

LRESULT CALLBACK capYieldCallback(HWND hWnd);

int VFWInit();

int VFWStop();