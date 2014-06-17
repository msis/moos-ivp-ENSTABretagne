/***************************************************************************************************************:')

SNR2PPMUtils.c

Conversions of the SONAR data into PPM pictures.

Fabrice Le Bars

Created : 2009-06-28

Version status : Not finished

***************************************************************************************************************:)*/

#include "SNR2PPMUtils.h"

/*
Modifies a picture representing the position of the first obstacle detected on a SONAR scanline at a specific
angle.

unsigned char* img : (IN,OUT) picture
double dist : (IN) distance to the first obstacle in m
double angle : (IN) angle (in degrees) of the scanline
int NBins : (IN) number of bins per scanline
int RangeScale : (IN) SONAR range scale

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int FirstObstacleDist2PPM(unsigned char* img, double dist, double angle, int NBins, int RangeScale)	{

	//unsigned char* scanline = (unsigned char*)calloc(NBins, sizeof(unsigned char));
	int j = 0, i0 = 0, i1 = 0;
	double d = 0;
	int width = NBins*2;
	int height = NBins*2;

	d = (double)(dist*(double)NBins/(double)RangeScale);
	j = (int)d;

	if (j >= NBins)	{
		return EXIT_FAILURE;
	}

	i0 = height-(int)(j*sin(-angle*2.0*M_PI/360.0)+NBins);
	i1 = (int)(j*cos(-angle*2.0*M_PI/360.0)+NBins);
	img[0+3*i1+3*width*i0] = (unsigned char)255; // R
	img[1+3*i1+3*width*i0] = (unsigned char)0; // G
	img[2+3*i1+3*width*i0] = (unsigned char)0; // B

	//free(scanline);scanline = NULL;

	return EXIT_SUCCESS;
}

/*
Modifies a picture representing a SONAR scanline at a specific angle.

unsigned char* img : (IN,OUT) picture
unsigned char* scanline : (IN) pointer to the SONAR scanline
double angle : (IN) angle (in degrees) of the scanline
int NBins : (IN) number of bins per scanline

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int Scanline2PPM(unsigned char* img, unsigned char* scanline, double angle, int NBins)	{

	int j = 0, i0 = 0, i1 = 0;
	double h = 0.0;
	double s = 1.0;
	double l = 0.6;
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	int width = NBins*2;
	int height = NBins*2;

	for (j=0; j<NBins; j++)	{
		h = *(scanline+j*sizeof(uint8))/255.0+0.66;
		HSL2RGB(h,s,l,&r,&g,&b);
		i0 = height-(int)(j*sin(-angle*2.0*M_PI/360.0)+NBins);
		i1 = (int)(j*cos(-angle*2.0*M_PI/360.0)+NBins);
		img[0+3*i1+3*width*i0] = (unsigned char)r; // R
		img[1+3*i1+3*width*i0] = (unsigned char)g; // G
		img[2+3*i1+3*width*i0] = (unsigned char)b; // B
	}

	return EXIT_SUCCESS;
}


/*
Modifies a picture representing a SONAR scanline at a specific angle (in gray scale).

unsigned char* img : (IN,OUT) picture
unsigned char* scanline : (IN) pointer to the SONAR scanline
double angle : (IN) angle (in degrees) of the scanline
int NBins : (IN) number of bins per scanline

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int Scanline2PPM_gs(unsigned char* img, unsigned char* scanline, double angle, int NBins)	{

	int j = 0, i0 = 0, i1 = 0;
	unsigned char g = 0;
	int width = NBins*2;
	int height = NBins*2;

	for (j=0; j<NBins; j++)	{
		g = *(scanline+j*sizeof(uint8));
		i0 = height-(int)(j*sin(-angle*2.0*M_PI/360.0)+NBins);
		i1 = (int)(j*cos(-angle*2.0*M_PI/360.0)+NBins);
		img[0+3*i1+3*width*i0] = (unsigned char)g; // R
		img[1+3*i1+3*width*i0] = (unsigned char)g; // G
		img[2+3*i1+3*width*i0] = (unsigned char)g; // B
	}

	return EXIT_SUCCESS;
}

/*
Generates a picture representing a SONAR image.
Can be used directly after a call to LoadFromSNRFile().

unsigned char* img : (OUT) picture
unsigned char* scanlines : (IN) pointer to the table of SONAR scanlines
int NBins : (IN) number of bins per scanline
int NbLines : (IN) number of scanlines in the table

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int Scanlines2PPM(unsigned char* img, unsigned char* scanlines, int NBins, int NbLines)	{

	int width = 0, height = 0, i = 0, j = 0, i0 = 0;

	height = NbLines;
	width = NBins;

	i0 = 0;
	for (i=0; i<height; i++)	{
		for (j=0; j<width; j++)	{
			img[i0+0] = (unsigned char)*(scanlines+i*NBins*sizeof(unsigned char)+j*sizeof(unsigned char)); // R
			img[i0+1] = (unsigned char)*(scanlines+i*NBins*sizeof(unsigned char)+j*sizeof(unsigned char)); // G
			img[i0+2] = (unsigned char)*(scanlines+i*NBins*sizeof(unsigned char)+j*sizeof(unsigned char)); // B
			i0+=3;
		}
	}

	return EXIT_SUCCESS;
}

/*
Generates a picture representing a SONAR image. This functions uses another way than Scanlines2PPM() to display
a SONAR image. If there are data corresponding to several SONAR loops in the parameters, only the last loop is
displayed.
Can be used directly after a call to LoadFromSNRFile().

unsigned char* img : (OUT) picture
unsigned char* scanlines : (IN) pointer to the table of SONAR scanlines
double* angles :(IN) table of the angles (in degrees) corresponding to the scanlines
int NBins : (IN) number of bins per scanline
int NSteps : (IN) number of scanlines in the table

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int Scanlines2PPM2(unsigned char* img, unsigned char* scanlines, double* angles, int NBins, int NSteps)	{

	int i = 0, j = 0, i0 = 0, i1 = 0;
	double h = 0.0;
	double s = 1.0;
	double l = 0.6;
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	int width = NBins*2;
	int height = NBins*2;

	for (i=0; i<NSteps; i++)	{
		for (j=0; j<NBins; j++)	{

			h = *(scanlines+i*NBins*sizeof(uint8)+j*sizeof(uint8))/255.0+0.66;
			HSL2RGB(h,s,l,&r,&g,&b);
			i0 = height-(int)(j*sin(-angles[i]*2.0*M_PI/360.0)+NBins);
			i1 = (int)(j*cos(-angles[i]*2.0*M_PI/360.0)+NBins);
			img[0+3*i1+3*width*i0] = (unsigned char)r; // R
			img[1+3*i1+3*width*i0] = (unsigned char)g; // G
			img[2+3*i1+3*width*i0] = (unsigned char)b; // B
		}
	}

	return EXIT_SUCCESS;
}

/*
Generates a picture representing a SONAR image like Scanlines2PPM2(), but in gray scale.
Can be used directly after a call to LoadFromSNRFile().

unsigned char* img : (OUT) picture
unsigned char* scanlines : (IN) pointer to the table of SONAR scanlines
double* angles :(IN) table of the angles (in degrees) corresponding to the scanlines
int NBins : (IN) number of bins per scanline
int NSteps : (IN) number of scanlines in the table

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int Scanlines2PPM2_gs(unsigned char* img, unsigned char* scanlines, double* angles, int NBins, int NSteps)	{

	int i = 0, j = 0, i0 = 0, i1 = 0;
	double g = 0;
	int width = NBins*2;
	int height = NBins*2;

	for (i=0; i<NSteps; i++)	{
		for (j=0; j<NBins; j++)	{

			g = *(scanlines+i*NBins*sizeof(uint8)+j*sizeof(uint8));
			i0 = height-(int)(j*sin(-angles[i]*2.0*M_PI/360.0)+NBins);
			i1 = (int)(j*cos(-angles[i]*2.0*M_PI/360.0)+NBins);
			img[0+3*i1+3*width*i0] = (unsigned char)g; // R
			img[1+3*i1+3*width*i0] = (unsigned char)g; // G
			img[2+3*i1+3*width*i0] = (unsigned char)g; // B
		}
	}

	return EXIT_SUCCESS;
}

/*
Generates a picture representing a SONAR image.
Can be used directly after a call to LoadFromSNRFile().
Uses Scanlines2PPM().

char *ppm_filename : (IN) PPM file where to save the picture
unsigned char* scanlines : (IN) pointer to the table of SONAR scanlines
int NBins : (IN) number of bins per scanline
int NbLines : (IN) number of scanlines in the table

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int Scanlines2PPMFile(char *ppm_filename, unsigned char* scanlines, int NBins, int NbLines)
{
	HPPMDATA hPPMData;

	if (CreatePPMData(&hPPMData, NBins, NbLines) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	if (Scanlines2PPM(hPPMData->Img, scanlines, NBins, NbLines) != EXIT_SUCCESS)
	{
		DestroyPPMData(&hPPMData);
		return EXIT_FAILURE;
	}

	if (SavePPMDataToFile(hPPMData, ppm_filename) != EXIT_SUCCESS)
	{
		DestroyPPMData(&hPPMData);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}	

/*
Generates a picture representing a SONAR image.
Can be used directly after a call to LoadFromSNRFile().
Uses Scanlines2PPM2().

char *ppm_filename : (IN) PPM file where to save the picture
unsigned char* scanlines : (IN) pointer to the table of SONAR scanlines
double* angles :(IN) table of the angles (in degrees) corresponding to the scanlines
int NBins : (IN) number of bins per scanline
int NSteps : (IN) number of scanlines in the table

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int Scanlines2PPMFile2(char *ppm_filename, unsigned char* scanlines, double* angles, int NBins, int NSteps)
{
	HPPMDATA hPPMData;

	if (CreatePPMData(&hPPMData, 2*NBins, 2*NBins) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	if (Scanlines2PPM2(hPPMData->Img, scanlines, angles, NBins, NSteps) != EXIT_SUCCESS)
	{
		DestroyPPMData(&hPPMData);
		return EXIT_FAILURE;
	}

	if (SavePPMDataToFile(hPPMData, ppm_filename) != EXIT_SUCCESS)
	{
		DestroyPPMData(&hPPMData);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}	

/*
Generates a picture representing a SONAR image.
Can be used directly after a call to LoadFromSNRFile().
Uses Scanlines2PPM2_gs().

char *ppm_filename : (IN) PPM file where to save the picture
unsigned char* scanlines : (IN) pointer to the table of SONAR scanlines
double* angles :(IN) table of the angles (in degrees) corresponding to the scanlines
int NBins : (IN) number of bins per scanline
int NSteps : (IN) number of scanlines in the table

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int Scanlines2PPMFile2_gs(char *ppm_filename, unsigned char* scanlines, double* angles, int NBins, int NSteps)	
{
	HPPMDATA hPPMData;

	if (CreatePPMData(&hPPMData, 2*NBins, 2*NBins) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	if (Scanlines2PPM2_gs(hPPMData->Img, scanlines, angles, NBins, NSteps) != EXIT_SUCCESS)
	{
		DestroyPPMData(&hPPMData);
		return EXIT_FAILURE;
	}

	if (SavePPMDataToFile(hPPMData, ppm_filename) != EXIT_SUCCESS)
	{
		DestroyPPMData(&hPPMData);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}	

/*
Converts a SNR file into a PPM file using Scanlines2PPMFile().

char *snr_filename : (IN) SNR file to convert
char *ppm_filename : (IN) PPM file where to save the picture

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int SNR2PPM(char *snr_filename, char *ppm_filename)	{

	int NBins = 0;
	int NbLines = 0;
	unsigned char* data = NULL;
	double* angle = NULL;

	if (LoadFromSNRFile(snr_filename, &data, &angle, &NBins, &NbLines) != EXIT_SUCCESS)	{
		return EXIT_FAILURE;
	}

	if (Scanlines2PPMFile(ppm_filename, data, NBins, NbLines) != EXIT_SUCCESS)	{
		free(data);data = NULL;
		free(angle);angle = NULL;
		return EXIT_FAILURE;
	}

	free(data);data = NULL;
	free(angle);angle = NULL;

	return EXIT_SUCCESS;
}

/*
Converts a SNR file into a PPM file using Scanlines2PPMFile2().

char *snr_filename : (IN) SNR file to convert
char *ppm_filename : (IN) PPM file where to save the picture

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int SNR2PPM2(char *snr_filename, char *ppm_filename)	{

	int NBins = 0;
	int NbLines = 0;
	unsigned char* data = NULL;
	double* angle = NULL;

	if (LoadFromSNRFile(snr_filename, &data, &angle, &NBins, &NbLines) != EXIT_SUCCESS)	{
		return EXIT_FAILURE;
	}

	if (Scanlines2PPMFile2(ppm_filename, data, angle, NBins, NbLines) != EXIT_SUCCESS)	{
		free(data);data = NULL;
		free(angle);angle = NULL;
		return EXIT_FAILURE;
	}

	free(data);data = NULL;
	free(angle);angle = NULL;

	return EXIT_SUCCESS;
}

/*
Converts a SNR file into a PPM file using Scanlines2PPMFile2_gs().

char *snr_filename : (IN) SNR file to convert
char *ppm_filename : (IN) PPM file where to save the picture

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int SNR2PPM2_gs(char *snr_filename, char *ppm_filename)	{

	int NBins = 0;
	int NbLines = 0;
	unsigned char* data = NULL;
	double* angle = NULL;

	if (LoadFromSNRFile(snr_filename, &data, &angle, &NBins, &NbLines) != EXIT_SUCCESS)	{
		return EXIT_FAILURE;
	}

	if (Scanlines2PPMFile2_gs(ppm_filename, data, angle, NBins, NbLines) != EXIT_SUCCESS)	{
		free(data);data = NULL;
		free(angle);angle = NULL;
		return EXIT_FAILURE;
	}

	free(data);data = NULL;
	free(angle);angle = NULL;

	return EXIT_SUCCESS;
}

