/***************************************************************************************************************:')

UE9Core.h

Labjack UE9 handling.

Fabrice Le Bars
Emilie Jude
Nassima Bouchelit
Some functions and parts of functions are from the examples provided by www.labjack.com

Created : 2009-03-28

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef UE9CORE_H
#define UE9CORE_H

#include "OSCore.h" 

// Labjack UE9 headers.
#ifdef _WIN32
/*
There is a problem with LabJackUD.h in C mode,
compile in C++ mode...
*/
#include "LabJackUD.h"
#else
#include "ue9.h"
#endif // _WIN32

/*
Debug macros specific to UE9Utils.
*/
#ifdef _DEBUG_MESSAGES
#	define _DEBUG_MESSAGES_UE9UTILS
#endif // _DEBUG_MESSAGES

#ifdef _DEBUG_WARNINGS
#	define _DEBUG_WARNINGS_UE9UTILS
#endif // _DEBUG_WARNINGS

#ifdef _DEBUG_ERRORS
#	define _DEBUG_ERRORS_UE9UTILS
#endif // _DEBUG_ERRORS

#ifdef _DEBUG_MESSAGES_UE9UTILS
#	define PRINT_DEBUG_MESSAGE_UE9UTILS(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_UE9UTILS(params)
#endif // _DEBUG_MESSAGES_UE9UTILS

#ifdef _DEBUG_WARNINGS_UE9UTILS
#	define PRINT_DEBUG_WARNING_UE9UTILS(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_UE9UTILS(params)
#endif // _DEBUG_WARNINGS_UE9UTILS

#ifdef _DEBUG_ERRORS_UE9UTILS
#	define PRINT_DEBUG_ERROR_UE9UTILS(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_UE9UTILS(params)
#endif // _DEBUG_ERRORS_UE9UTILS

/*
Debug macros specific to UE9Core.
*/
#ifdef _DEBUG_MESSAGES_UE9UTILS
#	define _DEBUG_MESSAGES_UE9CORE
#endif // _DEBUG_MESSAGES_UE9UTILS

#ifdef _DEBUG_WARNINGS_UE9UTILS
#	define _DEBUG_WARNINGS_UE9CORE
#endif // _DEBUG_WARNINGS_UE9UTILS

#ifdef _DEBUG_ERRORS_UE9UTILS
#	define _DEBUG_ERRORS_UE9CORE
#endif // _DEBUG_ERRORS_UE9UTILS

#ifdef _DEBUG_MESSAGES_UE9CORE
#	define PRINT_DEBUG_MESSAGE_UE9CORE(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_UE9CORE(params)
#endif // _DEBUG_MESSAGES_UE9CORE

#ifdef _DEBUG_WARNINGS_UE9CORE
#	define PRINT_DEBUG_WARNING_UE9CORE(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_UE9CORE(params)
#endif // _DEBUG_WARNINGS_UE9CORE

#ifdef _DEBUG_ERRORS_UE9CORE
#	define PRINT_DEBUG_ERROR_UE9CORE(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_UE9CORE(params)
#endif // _DEBUG_ERRORS_UE9CORE

#define MAX_NB_TIMERS_UE9 6

// I/O identifiers. 
#define FIO0 0
#define FIO1 1
#define FIO2 2
#define FIO3 3
//#define FIO4 4
//#define FIO5 5
//#define FIO6 6
//#define FIO7 7
#define DAC0 0
#define DAC1 1
#define AIN0 0
#define AIN1 1
#define AIN2 2
#define AIN3 3

// Temperature sensor channel.
#define TEMPERATURE_SENSOR_CHANNEL 133

// Base internal frequency for all timers in the device.
#define FREQUENCY_UE9 48000000
#define TIMER_CLOCK_DIVISOR_UE9 15

// The PWM have a frequency that depends on the base internal frequency
// of the timers (UE9_FREQUENCY) and UE9_TIMER_CLOCK_DIVISOR as in the following 
// formula. The PWM frequency chosen will be of 48.83 Hz.
#define PWM_FREQUENCY_UE9 ((double)FREQUENCY_UE9/(65536.0*TIMER_CLOCK_DIVISOR_UE9))

// Macros used to set the desired PWM pulse width.
#define PULSE_WIDTH2TIMER_DUTY_CYCLE_UE9(pulse_width) ((int)(65536.0-((pulse_width)*PWM_FREQUENCY_UE9*65.536)))
#define TIMER_DUTY_CYCLE2PULSE_WIDTH_UE9(timer_duty_cycle) ((65536.0-(timer_duty_cycle))/(65536.0*PWM_FREQUENCY_UE9))

// Timer value corresponding to a pulse width of 1.5 ms (neutral position for
// a servomotor).
#define PWM_DUTY_CYCLE_DEFAULT_UE9 (PULSE_WIDTH2TIMER_DUTY_CYCLE_UE9(1.5))

#ifdef _WIN32
inline long ResetUE9USB(long hDevice, BOOL bHardReset)
{
	if (bHardReset)
	{
		if (ResetLabJack(hDevice) != LJE_NOERROR)
		{
			return 1;
		}
	}
	else
	{
		if (ePut(hDevice, LJ_ioPIN_CONFIGURATION_RESET, 0, 0, 0) != LJE_NOERROR)
		{
			return 1;
		}
	}

	return LJE_NOERROR;
}

inline long ConfigWatchdogUE9(long hDevice, BOOL bEnable, SHORT timeout)
{
	if (bEnable)
	{
		// Specify that the comm processor should be reset on timeout.
		//if (ePut(hDevice, LJ_ioPUT_CONFIG, LJ_chSWDT_RESET_COMM, 1, 0) != LJE_NOERROR)
		//{
		//	return 1;
		//}

		// Specify that the control processor should be reset on timeout.
		if (ePut(hDevice, LJ_ioPUT_CONFIG, LJ_chSWDT_RESET_CONTROL, 1, 0) != LJE_NOERROR)
		{
			return 1;
		}

		// Enable the watchdog with a timeout (in s). 
		if (ePut(hDevice, LJ_ioSWDT_CONFIG, LJ_chSWDT_ENABLE, timeout, 0) != LJE_NOERROR)
		{
			return 1;
		}
	}
	else
	{
		// Disable the watchdog. 
		if (ePut(hDevice, LJ_ioSWDT_CONFIG, LJ_chSWDT_DISABLE, 0, 0) != LJE_NOERROR)
		{
			return 1;
		}
	}

	return LJE_NOERROR;
}
#else
EXTERN_C const long LJE_NOERROR;

inline long OpenUE9USB(char* address, HANDLE* phDevice, ue9CalibrationInfo* pCaliInfo)
{
	*phDevice = openUSBConnection(atoi(address));
	if (*phDevice == NULL)
	{
		return 1;
	}

	if (getCalibrationInfo(*phDevice, pCaliInfo) != 0)
	{
		return 1;
	}

	return LJE_NOERROR;
}

inline long ResetUE9USB(HANDLE hDevice, BOOL bHardReset)
{
  uint8 sendBuff[4], recBuff[4];
  int sendChars, recChars;

  sendBuff[1] = 0x99; // Command byte.
  sendBuff[2] = bHardReset; // ResetOptions Bit 1: Hard Reset Bit 0: Soft Reset.
  sendBuff[3] = 0; // Reserved.
  sendBuff[0] = normalChecksum8(sendBuff, 4);

  // Sending command to UE9.
  sendChars = LJUSB_BulkWrite(hDevice, UE9_PIPE_EP1_OUT, sendBuff, 4);
  if (sendChars < 4)
  {
    if (sendChars == 0)
      printf("ResetUE9USB error : write failed\n");
    else
      printf("ResetUE9USB error : did not write all of the buffer\n");
    return -1;
  }

  // Reading response from UE9.
  recChars = LJUSB_BulkRead(hDevice, UE9_PIPE_EP1_IN, recBuff, 4);
  if (recChars < 4)
  {
    if (recChars == 0)
      printf("ResetUE9USB error : read failed\n");
    else
      printf("ResetUE9USB error : did not read all of the buffer\n");
      return -1;
  }

  if ((uint8)(normalChecksum8(recBuff, 4)) != recBuff[0])
  {
    printf("ResetUE9USB error : read buffer has bad checksum\n");
    return -1;
  }

  if (recBuff[1] != (uint8)(0x99))
  {
    printf("ResetUE9USB error : read buffer has wrong command byte\n");
    return -1;
  }

  if (recBuff[2] != (uint8)(0x00))
  {
    printf("ResetUE9USB error : wrong response\n");
    return -1;
  }

  if (recBuff[3] != (uint8)(0x00))
  {
    printf("ResetUE9USB error : the command failed\n");
    return -1;
  }

	return 0;
}

inline long ConfigWatchdogUE9(HANDLE hDevice, BOOL bEnable, SHORT timeout)
{
  uint8 sendBuff[16], recBuff[8];
  int sendChars, recChars;
  uShort word;

  sendBuff[1] = 0xF8; // Command byte.
  sendBuff[2] = 0x05; // Number of data words.
  sendBuff[3] = 0x09; // Extended command number.
  sendBuff[6] = 0;

  if (bEnable)
  {
//	  sendBuff[7] = 64+32; // WatchdogOptions (the comm and control processors should be reset on timeout).
	  sendBuff[7] = 32; // WatchdogOptions (the control processor should be reset on timeout).
  }
  else
  {
	  sendBuff[7] = 0; // WatchdogOptions (watchdog disabled).
  }

  word.v = timeout;
  sendBuff[8] = word.c[0]; // TimeoutPeriod (LSB).
  sendBuff[9] = word.c[1]; // TimeoutPeriod (MSB).
  sendBuff[10] = 0; // DIOConfigA.
  sendBuff[11] = 0; // DIOConfigB.
  sendBuff[12] = 0; // DAC0.
  sendBuff[13] = 0; // DAC0.
  sendBuff[14] = 0; // DAC1.
  sendBuff[15] = 0; // DAC1.

  extendedChecksum(sendBuff, 16);

  // Sending command to UE9.
  sendChars = LJUSB_BulkWrite(hDevice, UE9_PIPE_EP1_OUT, sendBuff, 16);
  if (sendChars < 16)
  {
    if (sendChars == 0)
      printf("EnableWatchdogUE9USB error : write failed\n");
    else
      printf("EnableWatchdogUE9USB error : did not write all of the buffer\n");
    return -1;
  }

  // Reading response from UE9.
  recChars = LJUSB_BulkRead(hDevice, UE9_PIPE_EP1_IN, recBuff, 8);
  if (recChars < 8)
  {
    if (recChars == 0)
      printf("EnableWatchdogUE9USB error : read failed\n");
    else
      printf("EnableWatchdogUE9USB error : did not read all of the buffer\n");
      return -1;
  }

	return 0;
}

inline long CloseUE9USB(HANDLE hDevice)
{
	closeUSBConnection(hDevice);

	return LJE_NOERROR;
}
#endif // _WIN32

#endif // UE9CORE_H
