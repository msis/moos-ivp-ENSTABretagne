/***************************************************************************************************************:')

MDMCfg.h

Tritech Micron data modem handling.

Fabrice Le Bars

Created : 2012-07-01

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef MDMCFG_H
#define MDMCFG_H

#include "OSMisc.h"
#include "MDMCore.h"

/*
Debug macros specific to MDMCfg.
*/
#ifdef _DEBUG_MESSAGES_MDMUTILS
#	define _DEBUG_MESSAGES_MDMCFG
#endif // _DEBUG_MESSAGES_MDMUTILS

#ifdef _DEBUG_WARNINGS_MDMUTILS
#	define _DEBUG_WARNINGS_MDMCFG
#endif // _DEBUG_WARNINGS_MDMUTILS

#ifdef _DEBUG_ERRORS_MDMUTILS
#	define _DEBUG_ERRORS_MDMCFG
#endif // _DEBUG_ERRORS_MDMUTILS

#ifdef _DEBUG_MESSAGES_MDMCFG
#	define PRINT_DEBUG_MESSAGE_MDMCFG(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MDMCFG(params)
#endif // _DEBUG_MESSAGES_MDMCFG

#ifdef _DEBUG_WARNINGS_MDMCFG
#	define PRINT_DEBUG_WARNING_MDMCFG(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MDMCFG(params)
#endif // _DEBUG_WARNINGS_MDMCFG

#ifdef _DEBUG_ERRORS_MDMCFG
#	define PRINT_DEBUG_ERROR_MDMCFG(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MDMCFG(params)
#endif // _DEBUG_ERRORS_MDMCFG

typedef HANDLE HMDM;

#define INVALID_HMDM_VALUE INVALID_HANDLE_VALUE

/*
Open a Tritech Micron data modem.

HMDM* phMDM : (INOUT) Valid pointer that will receive an identifier of the
device opened.
char* szDevice : (IN) Device serial port to open.
UINT timeout : (IN) Time to wait to get at least 1 byte in ms (near 1000 ms for example, max is 
MAX_TIMEOUT_RS232PORT).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int OpenMDM(HMDM* phMDM, char* szDevice, UINT timeout)
{
	if (OpenRS232Port(phMDM, szDevice) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MDMCFG(("OpenMDM error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot open the serial port. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	if (SetOptionsRS232Port(*phMDM, 9600, NOPARITY, FALSE, 8, ONESTOPBIT, timeout) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MDMCFG(("OpenMDM error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot set the options of the serial port. ", 
			szDevice));
		CloseRS232Port(phMDM);
		return EXIT_FAILURE;
	}

	if (PurgeRS232Port(*phMDM) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MDMCFG(("OpenMDM error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot purge the serial port. ", 
			szDevice));
		CloseRS232Port(phMDM);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Discard old data from a Tritech Micron data modem.
Note that a call to this function can take some time.

HMDM hMDM : (IN) Identifier of the device.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int DiscardDataMDM(HMDM hMDM)
{
	if (PurgeRS232Port(hMDM) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MDMCFG(("DiscardDataMDM error (%s) : %s"
			"(hMDM=%#x)\n", 
			strtime_m(), 
			"Cannot purge the serial port. ", 
			hMDM));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Send data with a Tritech Micron data modem.
Should be used in combination with RecvDataMDM().

HMDM hMDM : (IN) Identifier of the device.
uint8* buf : (IN) Valid pointer to the data to send.
UINT buflen : (IN) Number of bytes to send.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SendDataMDM(HMDM hMDM, uint8* buf, UINT buflen)
{
#ifdef _DEBUG_MESSAGES_MDMCFG
	UINT i = 0;
#endif // _DEBUG_MESSAGES_MDMCFG

	if (buflen > INTERNAL_BUFFER_MDM)
	{
		PRINT_DEBUG_ERROR_MDMCFG(("SendDataMDM error (%s) : %s"
			"(hMDM=%#x)\n", 
			strtime_m(), 
			"Too many data to send at once. ", 
			hMDM));
		return EXIT_FAILURE;
	}

#ifdef _DEBUG_MESSAGES_MDMCFG
	for (i = 0; i < buflen; i++)
	{
		PRINT_DEBUG_MESSAGE_MDMCFG(("%.2x ", (int)buf[i]));
	}
	PRINT_DEBUG_MESSAGE_MDMCFG(("\n"));
#endif // _DEBUG_MESSAGES_MDMCFG

	if (WriteAllRS232Port(hMDM, buf, buflen) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MDMCFG(("SendDataMDM error (%s) : %s"
			"(hMDM=%#x)\n", 
			strtime_m(), 
			"Error sending data with the device. ", 
			hMDM));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Receive data with a Tritech Micron data modem.
Should be used in combination with SendDataMDM().

HMDM hMDM : (IN) Identifier of the device.
uint8* buf : (INOUT) Valid pointer that will receive the data received.
UINT buflen : (IN) Number of bytes to receive.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int RecvDataMDM(HMDM hMDM, uint8* buf, UINT buflen)
{
#ifdef _DEBUG_MESSAGES_MDMCFG
	UINT i = 0;
#endif // _DEBUG_MESSAGES_MDMCFG

	if (buflen > INTERNAL_BUFFER_MDM)
	{
		PRINT_DEBUG_ERROR_MDMCFG(("RecvDataMDM error (%s) : %s"
			"(hMDM=%#x)\n", 
			strtime_m(), 
			"Too many data to receive at once. ", 
			hMDM));
		return EXIT_FAILURE;
	}

	if (ReadAllRS232Port(hMDM, buf, buflen) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MDMCFG(("RecvDataMDM error (%s) : %s"
			"(hMDM=%#x)\n", 
			strtime_m(), 
			"Error receiving data with the device. ", 
			hMDM));
		return EXIT_FAILURE;
	}

#ifdef _DEBUG_MESSAGES_MDMCFG
	for (i = 0; i < buflen; i++)
	{
		PRINT_DEBUG_MESSAGE_MDMCFG(("%.2x ", (int)buf[i]));
	}
	PRINT_DEBUG_MESSAGE_MDMCFG(("\n"));
#endif // _DEBUG_MESSAGES_MDMCFG

	return EXIT_SUCCESS;
}

/*
Send data with a Tritech Micron data modem.
Append automatically a CRC-16 at the end of the data to enable error checking.
Should be used in combination with RecvDataCRC16MDM().

HMDM hMDM : (IN) Identifier of the device.
uint8* buf : (IN) Valid pointer to the data to send.
UINT buflen : (IN) Number of bytes to send.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SendDataCRC16MDM(HMDM hMDM, uint8* buf, UINT buflen)
{
	uint8 writebuf[INTERNAL_BUFFER_MDM];
#ifdef _DEBUG_MESSAGES_MDMCFG
	UINT i = 0;
#endif // _DEBUG_MESSAGES_MDMCFG

	if (buflen+2 > INTERNAL_BUFFER_MDM)
	{
		PRINT_DEBUG_ERROR_MDMCFG(("SendDataCRC16MDM error (%s) : %s"
			"(hMDM=%#x)\n", 
			strtime_m(), 
			"Too many data to send at once. ", 
			hMDM));
		return EXIT_FAILURE;
	}

	// Copy desired data.
	memcpy(writebuf, buf, buflen);
	// Append CRC-16.
	CalcCRC16(buf, buflen, &(writebuf[buflen]), &(writebuf[buflen+1]));

#ifdef _DEBUG_MESSAGES_MDMCFG
	for (i = 0; i < buflen+2; i++)
	{
		PRINT_DEBUG_MESSAGE_MDMCFG(("%.2x ", (int)writebuf[i]));
	}
	PRINT_DEBUG_MESSAGE_MDMCFG(("\n"));
#endif // _DEBUG_MESSAGES_MDMCFG

	if (WriteAllRS232Port(hMDM, writebuf, buflen+2) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MDMCFG(("SendDataCRC16MDM error (%s) : %s"
			"(hMDM=%#x)\n", 
			strtime_m(), 
			"Error sending data with the device. ", 
			hMDM));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Receive data with a Tritech Micron data modem.
Check the CRC-16 at the end of the data.
Should be used in combination with SendDataCRC16MDM().

HMDM hMDM : (IN) Identifier of the device.
uint8* buf : (INOUT) Valid pointer that will receive the data received.
UINT buflen : (IN) Number of bytes to receive.

Return : EXIT_SUCCESS, EXIT_INVALID_DATA if bad CRC-16 or EXIT_FAILURE if there is an error.
*/
inline int RecvDataCRC16MDM(HMDM hMDM, uint8* buf, UINT buflen)
{
	uint8 readbuf[INTERNAL_BUFFER_MDM];
	uint8 crc_h = 0;
	uint8 crc_l = 0;
#ifdef _DEBUG_MESSAGES_MDMCFG
	UINT i = 0;
#endif // _DEBUG_MESSAGES_MDMCFG

	if (buflen+2 > INTERNAL_BUFFER_MDM)
	{
		PRINT_DEBUG_ERROR_MDMCFG(("RecvDataCRC16MDM error (%s) : %s"
			"(hMDM=%#x)\n", 
			strtime_m(), 
			"Too many data to receive at once. ", 
			hMDM));
		return EXIT_FAILURE;
	}

	if (ReadAllRS232Port(hMDM, readbuf, buflen+2) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MDMCFG(("RecvDataCRC16MDM error (%s) : %s"
			"(hMDM=%#x)\n", 
			strtime_m(), 
			"Error receiving data with the device. ", 
			hMDM));
		return EXIT_FAILURE;
	}

#ifdef _DEBUG_MESSAGES_MDMCFG
	for (i = 0; i < buflen+2; i++)
	{
		PRINT_DEBUG_MESSAGE_MDMCFG(("%.2x ", (int)readbuf[i]));
	}
	PRINT_DEBUG_MESSAGE_MDMCFG(("\n"));
#endif // _DEBUG_MESSAGES_MDMCFG

	// Copy desired data.
	memcpy(buf, readbuf, buflen);
	// Compute CRC-16.
	CalcCRC16(buf, buflen, &crc_h, &crc_l);

	// Compare received CRC-16 with computed one.
	if ((readbuf[buflen] != crc_h)||(readbuf[buflen+1] != crc_l))
	{ 
		PRINT_DEBUG_ERROR_MDMCFG(("RecvDataCRC16MDM error (%s) : %s"
			"(hMDM=%#x)\n", 
			strtime_m(), 
			"Bad CRC-16. ", 
			hMDM));
		return EXIT_INVALID_DATA;	
	}

	return EXIT_SUCCESS;
}

// A simplistic example of forward error correction (FEC) is to transmit each data bit 3 times, 
// which is known as a (3,1) repetition code. This allows an error in any one of the three samples 
// to be corrected by "majority vote" or "democratic voting" (Wikipedia). 

/*
Close a Tritech Micron data modem.

HMDM* phMDM : (INOUT) Valid pointer to the identifier of the device.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CloseMDM(HMDM* phMDM)
{
	if (CloseRS232Port(phMDM) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_MDMCFG(("CloseMDM error (%s) : %s"
			"(*phMDM=%#x)\n", 
			strtime_m(), 
			"Error closing the serial port. ", 
			*phMDM));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // MDMCFG_H
