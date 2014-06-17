/***************************************************************************************************************:')

MTCfg.h

MT inertial measurement unit handling.

Fabrice Le Bars

Created : 2009-06-18

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef MTCFG_H
#define MTCFG_H

#include "MTCore.h"

/*
Debug macros specific to MTCfg.
*/
#ifdef _DEBUG_MESSAGES_MTUTILS
#	define _DEBUG_MESSAGES_MTCFG
#endif // _DEBUG_MESSAGES_MTUTILS

#ifdef _DEBUG_WARNINGS_MTUTILS
#	define _DEBUG_WARNINGS_MTCFG
#endif // _DEBUG_WARNINGS_MTUTILS

#ifdef _DEBUG_ERRORS_MTUTILS
#	define _DEBUG_ERRORS_MTCFG
#endif // _DEBUG_ERRORS_MTUTILS

#ifdef _DEBUG_MESSAGES_MTCFG
#	define PRINT_DEBUG_MESSAGE_MTCFG(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MTCFG(params)
#endif // _DEBUG_MESSAGES_MTCFG

#ifdef _DEBUG_WARNINGS_MTCFG
#	define PRINT_DEBUG_WARNING_MTCFG(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MTCFG(params)
#endif // _DEBUG_WARNINGS_MTCFG

#ifdef _DEBUG_ERRORS_MTCFG
#	define PRINT_DEBUG_ERROR_MTCFG(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MTCFG(params)
#endif // _DEBUG_ERRORS_MTCFG

struct MT
{
	CXbus xbus;
	int outputMode;
	int outputSettings;
	int orientationOffset;
};
typedef struct MT* HMT;

#define INVALID_HMT_VALUE NULL

/*
Open a MT.

HMT* phMT : (INOUT) Valid pointer that will receive an identifier of the
MT opened.
char* szDevice : (IN) MT serial port to open.
double MagWeightFactor : (IN) Magnetometer weighting factor (Rho) (valid range 
is [0.0,10.0[, default 1.0).
double Gain : (IN) Filter gain (valid range is ]0.01,50.0], default 1.0).
BOOL bAdaptToMagneticDisturbances : (IN) Adapt to magnetic disturbances 
(experimental).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int OpenMT(HMT* phMT, char* szDevice, 
					 double MagWeightFactor, double Gain, 
					 BOOL bAdaptToMagneticDisturbances)
{
#ifdef DONT_KEEP_LAST_SETTINGS_MT
#ifdef SET_PERIOD_AND_OUTPUTSKIPFACTOR_MT
	long OutputSkipFactor = 0;
	long MTPeriod = 0;
	long MTDataFreq = 0;
#endif // SET_PERIOD_AND_OUTPUTSKIPFACTOR_MT
#else
	UNREFERENCED_PARAMETER(MagWeightFactor);
	UNREFERENCED_PARAMETER(Gain);
	UNREFERENCED_PARAMETER(bAdaptToMagneticDisturbances);
#endif // DONT_KEEP_LAST_SETTINGS_MT

	*phMT = (HMT)calloc(1, sizeof(struct MT));

	if (*phMT == NULL)
	{
		PRINT_DEBUG_ERROR_MTCFG(("OpenMT error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	(*phMT)->xbus = CXbus();

	// szDevice+3 to delete "COM" (for example if szDevice="COM9", we get 9).
	if (((*phMT)->xbus).initSerialPort(atoi(szDevice+3), 115200) != X_XOK)
	{
		PRINT_DEBUG_ERROR_MTCFG(("OpenMT error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot open the serial port. ", 
			szDevice));
		free(*phMT);
		return EXIT_FAILURE;
	}

	((*phMT)->xbus).setTimeOut(4000);

	if (((*phMT)->xbus).mtSendMessage(MID_GOTOCONFIG) != X_XOK)
	{
		PRINT_DEBUG_ERROR_MTCFG(("OpenMT error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"MID_GOTOCONFIG error. ", 
			szDevice));
		((*phMT)->xbus).closeSerialPort();
		free(*phMT);
		return EXIT_FAILURE;
	}

#ifdef DONT_KEEP_LAST_SETTINGS_MT
	// There is no ack for this message.
	((*phMT)->xbus).mtSendMessage(MID_RESTOREFACTORYDEF);
#endif // DONT_KEEP_LAST_SETTINGS_MT

	(*phMT)->outputMode = OUTPUTMODE_CALIB | OUTPUTMODE_ORIENT;
	(*phMT)->outputSettings = OUTPUTSETTINGS_EULER; // | OUTPUTSETTINGS_SAMPLECNT;

	// If calibrated data (9 floats) present, 
	// orientation data starts after calibrated data.
	(*phMT)->orientationOffset = 0;
	if ((*phMT)->outputMode & OUTPUTMODE_CALIB)
	{
		(*phMT)->orientationOffset = LEN_CALIBDATA / LEN_FLOAT;
	}

	if (((*phMT)->xbus).mtSetSetting(MID_SETOUTPUTMODE, 
		(*phMT)->outputMode, LEN_OUTPUTMODE) != X_XOK)
	{
		PRINT_DEBUG_ERROR_MTCFG(("OpenMT error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"MID_SETOUTPUTMODE error. ", 
			szDevice));
		((*phMT)->xbus).closeSerialPort();
		free(*phMT);
		return EXIT_FAILURE;
	}

	if (((*phMT)->xbus).mtSetSetting(MID_SETOUTPUTSETTINGS, 
		(*phMT)->outputSettings, LEN_OUTPUTSETTINGS) != X_XOK)
	{
		PRINT_DEBUG_ERROR_MTCFG(("OpenMT error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"MID_SETOUTPUTSETTINGS error. ", 
			szDevice));
		((*phMT)->xbus).closeSerialPort();
		free(*phMT);
		return EXIT_FAILURE;
	}

#ifdef DONT_KEEP_LAST_SETTINGS_MT
#ifdef SET_PERIOD_AND_OUTPUTSKIPFACTOR_MT
	// Set the MT to send data every 10 ms.
	// It seems we cannot enable to request data at any time...

	//MTDataFreq = (long)(115200.0/((double)MTPeriod*((double)OutputSkipFactor+1)));
	//MTDataFreq = 10;
	//MTPeriod = (long)(115200.0/((double)MTDataFreq*((double)OutputSkipFactor+1)));
	MTPeriod = 1152;
	if (((*phMT)->xbus).mtSetSetting(MID_SETPERIOD, MTPeriod, LEN_PERIOD) != X_XOK)
	{
		PRINT_DEBUG_ERROR_MTCFG(("OpenMT error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"MID_SETPERIOD error. ", 
			szDevice));
		((*phMT)->xbus).closeSerialPort();
		free(*phMT);
		return EXIT_FAILURE;
	}

	// Set the MT to send data every 10 ms.
	// It seems we cannot enable to request data at any time...
	OutputSkipFactor = 0;
	if (((*phMT)->xbus).mtSetSetting(MID_SETOUTPUTSKIPFACTOR, OutputSkipFactor, LEN_OUTPUTSKIPFACTOR) != X_XOK)
	{
		PRINT_DEBUG_ERROR_MTCFG(("OpenMT error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"MID_SETOUTPUTSKIPFACTOR error. ", 
			szDevice));
		((*phMT)->xbus).closeSerialPort();
		free(*phMT);
		return EXIT_FAILURE;
	}
#endif // SET_PERIOD_AND_OUTPUTSKIPFACTOR_MT

	// Filter gain.
	// Valid range is ]0.01,50.0], default 1.0.
	if (((*phMT)->xbus).mtSetSetting(MID_SETFILTERSETTINGS, PARAM_FILTER_GAIN, 
		(float)Gain, STORE, 255) != X_XOK)
	{
		PRINT_DEBUG_ERROR_MTCFG(("OpenMT error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"MID_SETFILTERSETTINGS PARAM_FILTER_GAIN error. ", 
			szDevice));
		((*phMT)->xbus).closeSerialPort();
		free(*phMT);
		return EXIT_FAILURE;
	}

	// Magnetometer weighting factor (Rho).
	// Valid range is [0.0,10.0[, default 1.0.
	if (((*phMT)->xbus).mtSetSetting(MID_SETFILTERSETTINGS, PARAM_FILTER_RHO, 
		(float)MagWeightFactor, STORE, 255) != X_XOK)
	{
		PRINT_DEBUG_ERROR_MTCFG(("OpenMT error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"MID_SETFILTERSETTINGS PARAM_FILTER_RHO error. ", 
			szDevice));
		((*phMT)->xbus).closeSerialPort();
		free(*phMT);
		return EXIT_FAILURE;
	}

	if (bAdaptToMagneticDisturbances)
	{
		// Adapt to magnetic disturbances.
		if (((*phMT)->xbus).mtSetSetting(MID_SETAMD, AMDSETTING_ENABLED, LEN_AMD) != X_XOK)
		{
			PRINT_DEBUG_ERROR_MTCFG(("OpenMT error (%s) : %s"
				"(szDevice=%s)\n", 
				strtime_m(), 
				"MID_SETAMD error. ", 
				szDevice));
			((*phMT)->xbus).closeSerialPort();
			free(*phMT);
			return EXIT_FAILURE;
		}
	}
#endif // DONT_KEEP_LAST_SETTINGS_MT

	// There is no ack for this message.
	((*phMT)->xbus).mtSendMessage(MID_GOTOMEASUREMENT);

	return EXIT_SUCCESS;
}

/*
Discard old data from a MT.
Because the MT sends data every 10 ms, you must delete old data if you
did not read it during some time (> 10 ms).
Note that a call to this function can last about 100 ms.

HMT hMT : (IN) Identifier of the MT.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int DiscardDataMT(HMT hMT)
{
	hMT->xbus.flushSerialPort();

	return EXIT_SUCCESS;
}

/*
Read data from a MT.

HMT hMT : (IN) Identifier of the MT.
MTDATA* pData : (INOUT) Valid pointer that will receive the data.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReadDataMT(HMT hMT, MTDATA* pData)
{
	int numFloatBytes = 0;
	unsigned short samplecounter = 0;
	unsigned char data[MAXMSGLEN];
	short datalen = 0;
	float fdata[18] = {0};
	int i = 0;

	//	hMT->xbus.mtSendMessage(MID_REQDATA);

	if (hMT->xbus.mtReadDataMessage(data, datalen) != X_XOK)
	{
		PRINT_DEBUG_ERROR_MTCFG(("GetDataMT error (%s) : %s"
			"(hMT=%#x)\n", 
			strtime_m(), 
			"Error getting data from the device. ", 
			hMT));
		return EXIT_FAILURE;
	}

	if (hMT->outputSettings & OUTPUTSETTINGS_SAMPLECNT)
	{
		// Number of data bytes without sample counter.
		// Sample counter 2 bytes.
		numFloatBytes = (datalen - LEN_SAMPLECNT);
	}
	else
	{
		numFloatBytes = datalen;
	}

	// Swap endian from big to little endian and copy data directly into floating point array.
	// Each float consists of 4 bytes, big endian, pc memory (windows) is little endian.
	for (i = 0; i < (numFloatBytes/LEN_FLOAT); i++) 
	{
		hMT->xbus.swapEndian(&data[i * LEN_FLOAT], (unsigned char*)&fdata[i], LEN_FLOAT);
	}

	if (hMT->outputSettings & OUTPUTSETTINGS_SAMPLECNT)
	{
		// Get sample counter.
		samplecounter = data[numFloatBytes + IND_SAMPLECNTH] * 256 + data[numFloatBytes + IND_SAMPLECNTL];	
		PRINT_DEBUG_MESSAGE_MTCFG(("Sample Counter %05d\n", samplecounter));
	}

	if (hMT->outputMode & OUTPUTMODE_CALIB)
	{
		// Calibrated data.
		PRINT_DEBUG_MESSAGE_MTCFG(("%6.2f\t%6.2f\t%6.2f", 
			fdata[FLDNUM_CALIB_ACCX], 
			fdata[FLDNUM_CALIB_ACCY], 
			fdata[FLDNUM_CALIB_ACCZ]));
		PRINT_DEBUG_MESSAGE_MTCFG(("%6.2f\t%6.2f\t%6.2f", 
			fdata[FLDNUM_CALIB_GYRX], 
			fdata[FLDNUM_CALIB_GYRY], 
			fdata[FLDNUM_CALIB_GYRZ]));
		PRINT_DEBUG_MESSAGE_MTCFG(("%6.2f\t%6.2f\t%6.2f", 
			fdata[FLDNUM_CALIB_MAGX], 
			fdata[FLDNUM_CALIB_MAGY], 
			fdata[FLDNUM_CALIB_MAGZ]));

		// Acc X  Acc Y  Acc Z
		pData->AccX = (double)(fdata[FLDNUM_CALIB_ACCX]);
		pData->AccY = (double)(fdata[FLDNUM_CALIB_ACCY]);
		pData->AccZ = (double)(fdata[FLDNUM_CALIB_ACCZ]);
		// Gyr X  Gyr Y  Gyr Z
		pData->GyrX = (double)(fdata[FLDNUM_CALIB_GYRX]);
		pData->GyrY = (double)(fdata[FLDNUM_CALIB_GYRY]);
		pData->GyrZ = (double)(fdata[FLDNUM_CALIB_GYRZ]);
		// Mag X  Mag Y  Mag Z
		pData->MagX = (double)(fdata[FLDNUM_CALIB_MAGX]);
		pData->MagY = (double)(fdata[FLDNUM_CALIB_MAGY]);
		pData->MagZ = (double)(fdata[FLDNUM_CALIB_MAGZ]);
	}

	if (hMT->outputMode & OUTPUTMODE_ORIENT)
	{
		switch (hMT->outputSettings & OUTPUTSETTINGS_ORIENTMODE_MASK)
		{
		case OUTPUTSETTINGS_QUATERNION:
			// Output: quaternion.
			PRINT_DEBUG_MESSAGE_MTCFG(("%6.3f\t%6.3f\t%6.3f\t%6.3f\n",
				fdata[FLDNUM_ORIENT_Q0 + hMT->orientationOffset],
				fdata[FLDNUM_ORIENT_Q1 + hMT->orientationOffset], 
				fdata[FLDNUM_ORIENT_Q2 + hMT->orientationOffset], 
				fdata[FLDNUM_ORIENT_Q3 + hMT->orientationOffset]));
			break;
		case OUTPUTSETTINGS_EULER:
			// Output: Euler.
			PRINT_DEBUG_MESSAGE_MTCFG(("%6.1f\t%6.1f\t%6.1f\n",
				fdata[FLDNUM_ORIENT_ROLL + hMT->orientationOffset],
				fdata[FLDNUM_ORIENT_PITCH + hMT->orientationOffset], 
				fdata[FLDNUM_ORIENT_YAW + hMT->orientationOffset]));

			//  Roll  Pitch    Yaw
			pData->Roll = (double)(fdata[FLDNUM_ORIENT_ROLL + hMT->orientationOffset]);
			pData->Pitch = (double)(fdata[FLDNUM_ORIENT_PITCH + hMT->orientationOffset]);
			pData->Yaw = (double)(fdata[FLDNUM_ORIENT_YAW + hMT->orientationOffset]);
			break;
		case OUTPUTSETTINGS_MATRIX:
			// Output: Cosine Matrix.
			PRINT_DEBUG_MESSAGE_MTCFG(("%6.3f\t%6.3f\t%6.3f\n",
				fdata[FLDNUM_ORIENT_A + hMT->orientationOffset], 
				fdata[FLDNUM_ORIENT_B + hMT->orientationOffset], 
				fdata[FLDNUM_ORIENT_C + hMT->orientationOffset]));
			PRINT_DEBUG_MESSAGE_MTCFG(("%6.3f\t%6.3f\t%6.3f\n",
				fdata[FLDNUM_ORIENT_D + hMT->orientationOffset],
				fdata[FLDNUM_ORIENT_E + hMT->orientationOffset], 
				fdata[FLDNUM_ORIENT_F + hMT->orientationOffset]));
			PRINT_DEBUG_MESSAGE_MTCFG(("%6.3f\t%6.3f\t%6.3f\n",
				fdata[FLDNUM_ORIENT_G + hMT->orientationOffset], 
				fdata[FLDNUM_ORIENT_H + hMT->orientationOffset], 
				fdata[FLDNUM_ORIENT_I + hMT->orientationOffset]));
			break;
		default:
			;
		}
	}

	return EXIT_SUCCESS;
}

/*
Close a MT.

HMT* phMT : (INOUT) Valid pointer to the identifier of the MT.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CloseMT(HMT* phMT)
{
	((*phMT)->xbus).closeSerialPort();

	free(*phMT);

	*phMT = INVALID_HMT_VALUE;

	return EXIT_SUCCESS;
}

#endif // MTCFG_H
