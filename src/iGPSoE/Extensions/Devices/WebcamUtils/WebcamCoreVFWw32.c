/***************************************************************************************************************:')

WebcamCore.c

Webcam handling.

Fabrice Le Bars

Created: 2009-06-17

Version status: Not finished

***************************************************************************************************************:)*/

#include "WebcamCoreVFWw32.h"

LRESULT CALLBACK capFrameCallback(HWND hWnd, LPVIDEOHDR lpVHdr)
{
/*	int i = 0, j = 0;
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
*/
	capSetUserData(hWnd, (LPARAM)lpVHdr->lpData);

	return (LRESULT)TRUE; 
} 
