/***************************************************************************************************************:')

SNRUtils.c

SNR files handling. These files contain SONAR data. Each line corresponds to the data of a scanline, beginning with
its angle.

Fabrice Le Bars

Created : 2009-06-28

Version status : Not finished

***************************************************************************************************************:)*/

#include "SNRUtils.h"

/*
Reads a snr file (raw SONAR data) and returns the number of bins per scanline, the number of scanlines in the file,
the scanlines and the corresponding angles (in degrees).
data is a table of NbLines scanlines, where each scanline has NBins values.
angle is a table of NbLines values, corresponding to the angle (in degrees) of the scanlines.
All the memory needed is allocated (by realloc()) so don't forget to free it at the end.
If there is an error, NBins and NbLines are left unchanged.

unsigned char** data : (OUT) table of scanlines
double** angle : (OUT) table of the angles (in degrees) corresponding to the scanlines
int* NBins : (OUT) pointer receiving the number of bins per scanline
int* NbLines : (IN,OUT) pointer receiving the number of scanlines read in the file, and the number of lines to read if >0
FILE* fp : (IN,OUT) file to read

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int fread_snr(unsigned char** data, double** angle, int* NBins, int* NbLines, FILE* fp)	{

	int i = 0, j = 0, k = 0;
	unsigned int x = 0, y = 0;
	char buf[4];
	union	{
		unsigned short v;  
		unsigned char c[2];
	} transducer_bearing;

	if (fp == NULL)	{
		return EXIT_FAILURE;
	}

	for (;;)	{

		if ((*NbLines > 0)&&(i >= *NbLines))	{ // We have read all the lines wanted, so we stop here
			*NBins = j;
			*NbLines = i;
			break;
		}

		// Reading the transducer bearing on the two first bytes of the current line
		if ((fscanf(fp, "%2x ", &x) == 1) && (fscanf(fp, "%2x ", &y) == 1))	{
			transducer_bearing.c[0] = (unsigned char)x; // LSB
			transducer_bearing.c[1] = (unsigned char)y; // MSB
			*angle = (double*)realloc(*angle, (i+1)*sizeof(double));
			(*angle)[i] = (double)(transducer_bearing.v * 0.05625); // Angle of the transducer in degrees (0.05625 = (1/16)*(9/10))
			i++;
			j = 0;
		}
		else	{
			if (feof(fp))	{ // We are at the end of the file and there is no more to read
				if (i <= 0)	{ // No lines were read
					fprintf(stderr, "Error reading the file, end of file reached\n");
					free(*data);*data = NULL;
					free(*angle);*angle = NULL;
					return EXIT_FAILURE;
				}
				else	{
					*NBins = j;
					*NbLines = i;
					break;				
				}
			}
			else	{			
				fprintf(stderr, "Error reading the file\n");
				free(*data);*data = NULL;
				free(*angle);*angle = NULL;
				return EXIT_FAILURE;
			}
		}

		for (;;)	{

			buf[0] = 0;
			buf[1] = 0;
			buf[2] = 0;
			buf[3] = 0;
			if (fread(buf, 1, 1, fp) != 1)	{				
				fprintf(stderr, "Error reading the file\n");
				free(*data);*data = NULL;
				free(*angle);*angle = NULL;
				return EXIT_FAILURE;
			}
			if (buf[0] == '\n')	{ // We are at the end of the line
				break;
			}
			if (fread(buf+1, 1, 2, fp) != 2)	{				
				fprintf(stderr, "Error reading the file\n");
				free(*data);*data = NULL;
				free(*angle);*angle = NULL;
				return EXIT_FAILURE;
			}
			if (sscanf(buf, "%2x ", &y) == 1)	{ // The other NBins bytes are the data of the scanline
				*data = (unsigned char*)realloc((unsigned char*)*data, (k+1)*sizeof(unsigned char));
				*(*data+k*sizeof(unsigned char)) = (unsigned char)y;
				k++;
				j++;
			}
			else	{				
				fprintf(stderr, "Error reading the file\n");
				free(*data);*data = NULL;
				free(*angle);*angle = NULL;
				return EXIT_FAILURE;
			}
		}
	}

	return EXIT_SUCCESS;
}

/*
Writes the SONAR data to a snr file.
data should be a table of NbLines scanlines, where each scanline has NBins values.
angle should be a table of NbLines values, corresponding to the angle (in degrees) of the scanlines.

unsigned char** data : (IN) table of scanlines
double** angle : (IN) table of the angles (in degrees) corresponding to the scanlines
int* NBins : (IN) number of bins per scanline
int* NbLines : (IN) number of scanlines in the file
FILE* fp : (IN,OUT) file to write

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int fwrite_snr(unsigned char** data, double** angle, int NBins, int NbLines, FILE* fp)	{

	int i = 0, j = 0;
	double d = 0;
	union	{
		unsigned short v;  
		unsigned char c[2];
	} transducer_bearing;

	if (fp == NULL)	{
		return EXIT_FAILURE;
	}

	for (i=0;i<NbLines;i++){

		d = (*angle)[i] * 16.0 * 10.0 / 9.0; // Converts from degrees to 1/16 Gradian units
		transducer_bearing.v = (unsigned short)d;

		// On a line, the two first bytes are the transducer bearing
		if (fprintf(fp, "%.2x %.2x ", transducer_bearing.c[0], transducer_bearing.c[1]) != 6)	{
			fprintf(stderr, "Error writing to the file\n");
			return EXIT_FAILURE;
		}

		// The other NBins bytes are the data of the scanline
		for (j=0;j<NBins;j++){
			if (fprintf(fp, "%.2x ", (unsigned char)*(*data+i*NBins*sizeof(unsigned char)+j*sizeof(unsigned char))) != 3)	{
				fprintf(stderr, "Error writing to the file\n");
				return EXIT_FAILURE;
			}
		}

		if (fprintf(fp, "\n") != 1)	{
			fprintf(stderr, "Error writing to the file\n");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

/*
Opens, reads, and then close a snr file (raw SONAR data) and returns the number of bins per scanline, 
the number of scanlines in the file,
the scanlines and the corresponding angles (in degrees). Uses fread_snr().
data is a table of NbLines scanlines, where each scanline has NBins values.
angle is a table of NbLines values, corresponding to the angle (in degrees) of the scanlines.
All the memory needed is allocated (by realloc()) so don't forget to free it at the end.
If there is an error, NBins and NbLines are left unchanged.

char *filename : (IN) name of the file to read
unsigned char** data : (OUT) table of scanlines
double** angle : (OUT) table of the angles (in degrees) corresponding to the scanlines
int* NBins : (OUT) pointer receiving the number of bins per scanline
int* NbLines : (OUT) pointer receiving the number of scanlines in the file

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int LoadFromSNRFile(char *filename, unsigned char** data, double** angle, int* NBins, int* NbLines)	{

	FILE *fp = fopen(filename, "r");
	if (fp == NULL)	{
		fprintf(stderr, "Error opening the file\n");
		return EXIT_FAILURE;
	}

	*NbLines = 0; // To specify to read all the file
	if (fread_snr(data, angle, NBins, NbLines, fp) != EXIT_SUCCESS)	{
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (fclose(fp) != EXIT_SUCCESS)	{
		return EXIT_FAILURE;
	}
	fp = NULL;

	return EXIT_SUCCESS;
}

/*
Writes the SONAR data to a snr file. Uses fwrite_snr().
data should be a table of NbLines scanlines, where each scanline has NBins values.
angle should be a table of NbLines values, corresponding to the angle (in degrees) of the scanlines.

char *filename : (IN) name of the file to write
unsigned char** data : (IN) table of scanlines
double** angle :(IN) table of the angles (in degrees) corresponding to the scanlines
int NBins : (IN) number of bins per scanline
int NbLines : (IN) number of scanlines in the file

Returns : EXIT_SUCCESS or EXIT_FAILURE if there is an error
*/
int SaveToSNRFile(char *filename, unsigned char** data, double** angle, int NBins, int NbLines)	{

	FILE *fp = fopen(filename, "w");
	if (fp == NULL)	{
		fprintf(stderr, "Error opening the file\n");
		return EXIT_FAILURE;
	}

	if (fwrite_snr(data, angle, NBins, NbLines, fp) != EXIT_SUCCESS)	{
		fclose(fp);fp = NULL;
		return EXIT_FAILURE;
	}

	if (fclose(fp) != EXIT_SUCCESS)	{
		return EXIT_FAILURE;
	}
	fp = NULL;

	return EXIT_SUCCESS;
}

