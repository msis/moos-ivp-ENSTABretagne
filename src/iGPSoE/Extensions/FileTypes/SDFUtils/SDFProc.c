/***************************************************************************************************************:')

SDFProc.c

SDF data processing.

Fabrice Le Bars

Created : 2009-02-12

Version status : Not finished

***************************************************************************************************************:)*/

#include "SDFProc.h"

/*
Improve the contrast in sonar data corresponding to the SDF format. 
See the description of the SDFDATA structure for more information.

HSDFDATA hData : (IN) Identifier of the structure.
USHORT Threshold : (IN) Each sample will be divided by Threshold (between 0 and 65535). 
If it is 0, each sample will be divided by the maximum sample value.
Note that 1000 is often a good value.

Return : Nothing.
*/
void ContrastSDFData(HSDFDATA hData, USHORT Threshold)
{
	double value = 0.0; // Temporary value of a sample.
	double maxValue = 0.0; // Maximum sample value for all the Pings in the file 
	// (samples in the water column excluded).
	int i = 0; // For loop index for the Pings.
	int j = 0; // For loop index for the samples of a ping.
	int jWaterColumn = 0;

	//FILE* pfTest = fopen("0.txt", "w");

	if (Threshold != 0)
	{
		maxValue = Threshold;
	}
	else
	{
		// This loop is for each ping found in the file.
		// Its goal is to get the sample maximum value.
		for (i = hData->NumPings-1; i >= 0; i--)
		{
			// Handle empty parts in sonar data.
			if (hData->Headers[i].range != 0)
			{
				// The first samples have high values and they are immediately followed by low values,
				// what corresponds to the water column (water between the floor and the sonar).
				// These samples should not be used in the calculation of the maximum value among the samples.
				jWaterColumn = (UINT)(
					(double)((hData->Headers[i].altitude)*hData->NumSamples)/(double)hData->Headers[i].range
					);

				for (j = jWaterColumn; j < hData->NumSamples; j++)
				{
					// Maximum sample value for all the Pings in the file (samples in the water column excluded).
					maxValue = max(maxValue, (double)hData->Pings[i*hData->NumSamples+j]);
				}
			}
		}
	}

	if (maxValue != 0.0)
	{
		// This loop is for each ping found in the file.
		for (i = hData->NumPings-1; i >= 0; i--)
		{
			for (j = hData->NumSamples-1; j >= 0; j--)
			{
				//fprintf(pfTest, "%.5d ", hData->Pings[i*hData->NumSamples+j]);

				value = (double)hData->Pings[i*hData->NumSamples+j];

				// pow(value/maxValue, 0.5) can increase the contrast.
				hData->Pings[i*hData->NumSamples+j] = (USHORT)(
					65535.0*
					max(min(
					pow(value/maxValue, 0.5), 
					//value/maxValue, 
					1.0), 0.0)
					);
			}
			//fprintf(pfTest, "\n");
		}
	}

	//fclose(pfTest);
}
