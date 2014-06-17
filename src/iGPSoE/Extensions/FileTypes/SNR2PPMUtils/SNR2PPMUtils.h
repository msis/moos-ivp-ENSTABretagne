/***************************************************************************************************************:')

SNR2PPMUtils.h

Conversions of the SONAR data into PPM pictures.

Fabrice Le Bars

Created : 2009-06-28

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef SNR2PPMUTILS_H
#define SNR2PPMUTILS_H

#include "PPMUtils.h"
#include "SNRUtils.h"

int FirstObstacleDist2PPM(unsigned char* img, double dist, double angle, int NBins, int RangeScale);
int Scanline2PPM(unsigned char* img, unsigned char* scanline, double angle, int NBins);
int Scanline2PPM_gs(unsigned char* img, unsigned char* scanline, double angle, int NBins);
int Scanlines2PPM(unsigned char* img, unsigned char* scanlines, int NBins, int NbLines);
int Scanlines2PPM2(unsigned char* img, unsigned char* scanlines, double* angles, int NBins, int NSteps);
int Scanlines2PPM2_gs(unsigned char* img, unsigned char* scanlines, double* angles, int NBins, int NSteps);
int Scanlines2PPMFile(char *ppm_filename, unsigned char* scanlines, int NBins, int NbLines);
int Scanlines2PPMFile2(char *ppm_filename, unsigned char* scanlines, double* angles, int NBins, int NSteps);
int Scanlines2PPMFile2_gs(char *ppm_filename, unsigned char* scanlines, double* angles, int NBins, int NSteps);
int SNR2PPM(char *snr_filename, char *ppm_filename)	;
int SNR2PPM2(char *snr_filename, char *ppm_filename);
int SNR2PPM2_gs(char *snr_filename, char *ppm_filename);

#endif // SNR2PPMUTILS_H
