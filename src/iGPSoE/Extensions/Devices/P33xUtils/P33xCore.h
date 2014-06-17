/***************************************************************************************************************:')

P33xCore.h

P33x pressure sensor handling.

Fabrice Le Bars
CalcCRC16() is based on the code provided in the documentation of the pressure sensor

Created : 2009-03-28

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef P33XCORE_H
#define P33XCORE_H

#include "OSMisc.h"
#include "OSRS232Port.h"

/*
Debug macros specific to P33xUtils.
*/
#ifdef _DEBUG_MESSAGES
#	define _DEBUG_MESSAGES_P33XUTILS
#endif // _DEBUG_MESSAGES

#ifdef _DEBUG_WARNINGS
#	define _DEBUG_WARNINGS_P33XUTILS
#endif // _DEBUG_WARNINGS

#ifdef _DEBUG_ERRORS
#	define _DEBUG_ERRORS_P33XUTILS
#endif // _DEBUG_ERRORS

#ifdef _DEBUG_MESSAGES_P33XUTILS
#	define PRINT_DEBUG_MESSAGE_P33XUTILS(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_P33XUTILS(params)
#endif // _DEBUG_MESSAGES_P33XUTILS

#ifdef _DEBUG_WARNINGS_P33XUTILS
#	define PRINT_DEBUG_WARNING_P33XUTILS(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_P33XUTILS(params)
#endif // _DEBUG_WARNINGS_P33XUTILS

#ifdef _DEBUG_ERRORS_P33XUTILS
#	define PRINT_DEBUG_ERROR_P33XUTILS(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_P33XUTILS(params)
#endif // _DEBUG_ERRORS_P33XUTILS

/*
Debug macros specific to P33xCore.
*/
#ifdef _DEBUG_MESSAGES_P33XUTILS
#	define _DEBUG_MESSAGES_P33XCORE
#endif // _DEBUG_MESSAGES_P33XUTILS

#ifdef _DEBUG_WARNINGS_P33XUTILS
#	define _DEBUG_WARNINGS_P33XCORE
#endif // _DEBUG_WARNINGS_P33XUTILS

#ifdef _DEBUG_ERRORS_P33XUTILS
#	define _DEBUG_ERRORS_P33XCORE
#endif // _DEBUG_ERRORS_P33XUTILS

#ifdef _DEBUG_MESSAGES_P33XCORE
#	define PRINT_DEBUG_MESSAGE_P33XCORE(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_P33XCORE(params)
#endif // _DEBUG_MESSAGES_P33XCORE

#ifdef _DEBUG_WARNINGS_P33XCORE
#	define PRINT_DEBUG_WARNING_P33XCORE(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_P33XCORE(params)
#endif // _DEBUG_WARNINGS_P33XCORE

#ifdef _DEBUG_ERRORS_P33XCORE
#	define PRINT_DEBUG_ERROR_P33XCORE(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_P33XCORE(params)
#endif // _DEBUG_ERRORS_P33XCORE

/*
Get the depth from the pressure, assuming that the pressure at the surface is 1 bar.

double pressure : (IN) Pressure in bar.

Return : The depth in m.
*/
inline double Pressure2Height(double pressure)
{
	return -(pressure-1)*1e5/(1000.0*9.8);
}

/*
Initialize a P33x.

HANDLE hDev : (IN) Identifier of the device serial port.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitP33x(HANDLE hDev)
{
	uint8 readbuf[10];
	uint8 writebuf[4];
	uint8 crc_h = 0;
	uint8 crc_l = 0;
	int devclass = 0;
	int group = 0;
	int year = 0;
	int week = 0;
	int buf = 0;
	int stat = 0;

	writebuf[0] = (uint8)0xfa; // device address = 250
	writebuf[1] = (uint8)0x30; // function 48
	CalcCRC16(writebuf, 4-2, &(writebuf[2]), &(writebuf[3])); // CRC-16

	if (WriteAllRS232Port(hDev, writebuf, 4) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_P33XCORE(("InitP33x error (%s) : %s"
									"(hDev=%#x)\n", 
									strtime_m(), 
									"Error writing data to the serial port. ", 
									hDev));
		return EXIT_FAILURE;
	}

	// Read the echo.
	if (ReadAllRS232Port(hDev, readbuf, 4) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_P33XCORE(("InitP33x error (%s) : %s"
									"(hDev=%#x)\n", 
									strtime_m(), 
									"Error reading data from the serial port. ", 
									hDev));
		return EXIT_FAILURE;
	}

	// Read the data.
	if (ReadAllRS232Port(hDev, readbuf, 10) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_P33XCORE(("InitP33x error (%s) : %s"
									"(hDev=%#x)\n", 
									strtime_m(), 
									"Error reading data from the serial port. ", 
									hDev));
		return EXIT_FAILURE;
	}

	// Device address = 250.
	if (readbuf[0] != (uint8)0xfa)
	{ 
		PRINT_DEBUG_ERROR_P33XCORE(("InitP33x error (%s) : %s"
									"(hDev=%#x)\n", 
									strtime_m(), 
									"Bad device address. ", 
									hDev));
		return EXIT_FAILURE;	
	}

	// Function 48.
	if (readbuf[1] != (uint8)0x30)
	{ 
		PRINT_DEBUG_ERROR_P33XCORE(("InitP33x error (%s) : %s"
									"(hDev=%#x)\n", 
									strtime_m(), 
									"The device function failed. ", 
									hDev));
		return EXIT_FAILURE;	
	}

	CalcCRC16(readbuf, 10-2, &crc_h, &crc_l);

	// CRC-16.
	if ((readbuf[8] != crc_h)||(readbuf[9] != crc_l))
	{ 
		PRINT_DEBUG_ERROR_P33XCORE(("InitP33x error (%s) : %s"
									"(hDev=%#x)\n", 
									strtime_m(), 
									"Bad CRC-16. ", 
									hDev));
		return EXIT_FAILURE;	
	}

	devclass = readbuf[2];
	group = readbuf[3];
	year = readbuf[4];
	week = readbuf[5];
	buf = readbuf[6];
	stat = readbuf[7];

	PRINT_DEBUG_MESSAGE_P33XCORE(("Device ID : %d.%d\n", devclass, group));
	PRINT_DEBUG_MESSAGE_P33XCORE(("Firmware version : %d.%d\n", year, week));
	PRINT_DEBUG_MESSAGE_P33XCORE(("Length of the internal receive buffer : %d\n", buf));
	PRINT_DEBUG_MESSAGE_P33XCORE(("Status information : %d\n", stat));

	return EXIT_SUCCESS;
}

/*
Read the value at a specific channel of a P33x.

HANDLE hDev : (IN) Identifier of the device serial port.
uint8 Channel : (IN) Channel to read.
float* pValue : (INOUT) Valid pointer receiving the value.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReadChannelP33x(HANDLE hDev, uint8 Channel, float* pValue)
{
	uint8 readbuf[9];
	uint8 writebuf[5];
	uint8 crc_h = 0;
	uint8 crc_l = 0;
	uFloat value;
	int stat = 0;

	writebuf[0] = (uint8)0xfa; // device address = 250
	writebuf[1] = (uint8)0x49; // function 73
	writebuf[2] = (uint8)Channel; // Channel to read
	CalcCRC16(writebuf, 5-2, &(writebuf[3]), &(writebuf[4])); // CRC-16

	if (WriteAllRS232Port(hDev, writebuf, 5) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_P33XCORE(("ReadChannelP33x error (%s) : %s"
									"(hDev=%#x, Channel=%u)\n", 
									strtime_m(), 
									"Error writing data to the serial port. ", 
									hDev, (UINT)Channel));
		return EXIT_FAILURE;
	}

	// Read the echo.
	if (ReadAllRS232Port(hDev, readbuf, 5) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_P33XCORE(("ReadChannelP33x error (%s) : %s"
									"(hDev=%#x, Channel=%u)\n", 
									strtime_m(), 
									"Error reading data from the serial port. ", 
									hDev, (UINT)Channel));
		return EXIT_FAILURE;
	}

	// Read the data.
	if (ReadAllRS232Port(hDev, readbuf, 9) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_P33XCORE(("ReadChannelP33x error (%s) : %s"
									"(hDev=%#x, Channel=%u)\n", 
									strtime_m(), 
									"Error reading data from the serial port. ", 
									hDev, (UINT)Channel));
		return EXIT_FAILURE;
	}

	// Device address = 250.
	if (readbuf[0] != (uint8)0xfa)
	{ 
		PRINT_DEBUG_ERROR_P33XCORE(("ReadChannelP33x error (%s) : %s"
									"(hDev=%#x, Channel=%u)\n", 
									strtime_m(), 
									"Bad device address. ", 
									hDev, (UINT)Channel));
		return EXIT_FAILURE;	
	}

	// Function 73.
	if (readbuf[1] != (uint8)0x49)
	{
		PRINT_DEBUG_ERROR_P33XCORE(("ReadChannelP33x error (%s) : %s"
									"(hDev=%#x, Channel=%u)\n", 
									strtime_m(), 
									"The device function failed. ", 
									hDev, (UINT)Channel));
		return EXIT_FAILURE;	
	}

	CalcCRC16(readbuf, 9-2, &crc_h, &crc_l);

	// CRC-16.
	if ((readbuf[7] != crc_h)||(readbuf[8] != crc_l))
	{ 
		PRINT_DEBUG_ERROR_P33XCORE(("ReadChannelP33x error (%s) : %s"
									"(hDev=%#x, Channel=%u)\n", 
									strtime_m(), 
									"Bad CRC-16. ", 
									hDev, (UINT)Channel));
		return EXIT_FAILURE;	
	}

	value.c[3] = readbuf[2];
	value.c[2] = readbuf[3];
	value.c[1] = readbuf[4];
	value.c[0] = readbuf[5];
	stat = readbuf[6];

	*pValue = value.v;

	PRINT_DEBUG_MESSAGE_P33XCORE(("Channel value : %f\n", (double)*pValue));
	PRINT_DEBUG_MESSAGE_P33XCORE(("Status information : %d\n", stat));

	return EXIT_SUCCESS;
}

#endif // P33XCORE_H
