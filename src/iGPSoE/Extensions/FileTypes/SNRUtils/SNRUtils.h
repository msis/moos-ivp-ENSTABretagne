/***************************************************************************************************************:')

SNRUtils.h

SNR files handling. These files contain SONAR data. Each line corresponds to the data of a scanline, beginning with
its angle.

Fabrice Le Bars

Created : 2009-06-28

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef SNRUTILS_H
#define SNRUTILS_H

#include "OSCore.h"

struct SNRDATA
{
	double* angles;
	UCHAR* scanlines;
	UINT NBins;
	UINT NScanlines;
	UINT RangeScale;
};
typedef struct SNRDATA SNRDATA;




int fread_snr(unsigned char** data, double** angle, int* NBins, int* NbLines, FILE* fp);
int fwrite_snr(unsigned char** data, double** angle, int NBins, int NbLines, FILE* fp);
int LoadFromSNRFile(char *filename, unsigned char** data, double** angle, int* NBins, int* NbLines);
int SaveToSNRFile(char *filename, unsigned char** data, double** angle, int NBins, int NbLines);

#endif // SNRUTILS_H




