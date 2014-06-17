/***************************************************************************************************************:')

U3Core.h

Labjack U3 handling.

Fabrice Le Bars

Created: 2009-03-28

Version status: Not finished

***************************************************************************************************************:)*/

#ifndef U3CORE_H
#define U3CORE_H

#include "OSCore.h" 

// Labjack U3 headers.
#ifdef _WIN32
/*
There is a problem with LabJackUD.h in C mode,
compile in C++ mode...
*/
#include "LabJackUD.h"
#else
#include "u3.h"
#endif // _WIN32

/*
Debug macros specific to U3Utils.
*/
#ifdef _DEBUG_MESSAGES
#	define _DEBUG_MESSAGES_U3UTILS
#endif // _DEBUG_MESSAGES

#ifdef _DEBUG_WARNINGS
#	define _DEBUG_WARNINGS_U3UTILS
#endif // _DEBUG_WARNINGS

#ifdef _DEBUG_ERRORS
#	define _DEBUG_ERRORS_U3UTILS
#endif // _DEBUG_ERRORS

#ifdef _DEBUG_MESSAGES_U3UTILS
#	define PRINT_DEBUG_MESSAGE_U3UTILS(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_U3UTILS(params)
#endif // _DEBUG_MESSAGES_U3UTILS

#ifdef _DEBUG_WARNINGS_U3UTILS
#	define PRINT_DEBUG_WARNING_U3UTILS(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_U3UTILS(params)
#endif // _DEBUG_WARNINGS_U3UTILS

#ifdef _DEBUG_ERRORS_U3UTILS
#	define PRINT_DEBUG_ERROR_U3UTILS(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_U3UTILS(params)
#endif // _DEBUG_ERRORS_U3UTILS

/*
Debug macros specific to U3Core.
*/
#ifdef _DEBUG_MESSAGES_U3UTILS
#	define _DEBUG_MESSAGES_U3CORE
#endif // _DEBUG_MESSAGES_U3UTILS

#ifdef _DEBUG_WARNINGS_U3UTILS
#	define _DEBUG_WARNINGS_U3CORE
#endif // _DEBUG_WARNINGS_U3UTILS

#ifdef _DEBUG_ERRORS_U3UTILS
#	define _DEBUG_ERRORS_U3CORE
#endif // _DEBUG_ERRORS_U3UTILS

#ifdef _DEBUG_MESSAGES_U3CORE
#	define PRINT_DEBUG_MESSAGE_U3CORE(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_U3CORE(params)
#endif // _DEBUG_MESSAGES_U3CORE

#ifdef _DEBUG_WARNINGS_U3CORE
#	define PRINT_DEBUG_WARNING_U3CORE(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_U3CORE(params)
#endif // _DEBUG_WARNINGS_U3CORE

#ifdef _DEBUG_ERRORS_U3CORE
#	define PRINT_DEBUG_ERROR_U3CORE(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_U3CORE(params)
#endif // _DEBUG_ERRORS_U3CORE

#define MAX_NB_TIMERS_U3 2

// I/O identifiers. 
#define FIO0 0
#define FIO1 1
#define FIO2 2
#define FIO3 3
#define FIO4 4
#define FIO5 5
#define FIO6 6
#define FIO7 7
#define DAC0 0
#define DAC1 1
#define AIN0 0
#define AIN1 1
#define AIN2 2
#define AIN3 3
#define AIN4 4
#define AIN5 5
#define AIN6 6
#define AIN7 7

// Temperature sensor channel.
#define TEMPERATURE_SENSOR_CHANNEL 30

// Negative AIN channel for single-ended channels.
#define NEGATIVE_CHANNEL_FOR_SINGLE_ENDED 31

// If the device has a hardware version number less than 1.21, define 
// OLD_HARDWARE_VERSION. The version of the driver installed must also support
// the hardware version used.
#ifdef OLD_HARDWARE_VERSION
// Base internal frequency for all timers in the device.
#define FREQUENCY_U3 24000000
#define TIMER_CLOCK_DIVISOR_U3 8
#else
// Base internal frequency for all timers in the device.
#define FREQUENCY_U3 48000000
#define TIMER_CLOCK_DIVISOR_U3 15
#endif // OLD_HARDWARE_VERSION

// The PWM have a frequency that depends on the base internal frequency
// of the timers (U3_FREQUENCY) and U3_TIMER_CLOCK_DIVISOR as in the following 
// formula. The PWM frequency chosen will be of 48.83 Hz.
#define PWM_FREQUENCY_U3 ((double)FREQUENCY_U3/(65536.0*TIMER_CLOCK_DIVISOR_U3))

// Macros used to set the desired PWM pulse width.
#define PULSE_WIDTH2TIMER_DUTY_CYCLE_U3(pulse_width) ((int)(65536.0-((pulse_width)*PWM_FREQUENCY_U3*65.536)))
#define TIMER_DUTY_CYCLE2PULSE_WIDTH_U3(timer_duty_cycle) ((65536.0-(timer_duty_cycle))/(65536.0*PWM_FREQUENCY_U3))

// Timer value corresponding to a pulse width of 1.5 ms (neutral position for
// a servomotor).
#define PWM_DUTY_CYCLE_DEFAULT_U3 (PULSE_WIDTH2TIMER_DUTY_CYCLE_U3(1.5))

// Kelvin to Celsius degrees conversions.
#define KELVIN2CELSIUS(temperature) ((temperature)-273.15)
#define CELSIUS2KELVIN(temperature) ((temperature)+273.15)

#ifdef _WIN32
__inline long ResetU3USB(long hDevice, BOOL bHardReset)
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

__inline long ConfigWatchdogU3(long hDevice, BOOL bEnable, SHORT timeout)
{
	if (bEnable)
	{
		// Specify that the device should be reset on timeout (hard reset).
		if (ePut(hDevice, LJ_ioPUT_CONFIG, LJ_chSWDT_RESET_DEVICE, 1, 0) != LJE_NOERROR)
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

__inline long OpenU3USB(char* address, HANDLE* phDevice, u3CalibrationInfo* pCaliInfo)
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

__inline long ResetU3USB(HANDLE hDevice, BOOL bHardReset)
{
  uint8 sendBuff[4], recBuff[4];
  int sendChars, recChars;

  sendBuff[1] = 0x99; // Command byte.
  sendBuff[2] = bHardReset; // ResetOptions Bit 1: Hard Reset Bit 0: Soft Reset.
  sendBuff[3] = 0; // Reserved.
  sendBuff[0] = normalChecksum8(sendBuff, 4);

  // Sending command to U3.
  sendChars = LJUSB_BulkWrite(hDevice, U3_PIPE_EP1_OUT, sendBuff, 4);
  if (sendChars < 4)
  {
    if (sendChars == 0)
      printf("ResetU3USB error : write failed\n");
    else
      printf("ResetU3USB error : did not write all of the buffer\n");
    return -1;
  }

  // Reading response from U3.
  recChars = LJUSB_BulkRead(hDevice, U3_PIPE_EP2_IN, recBuff, 4);
  if (recChars < 4)
  {
    if (recChars == 0)
      printf("ResetU3USB error : read failed\n");
    else
      printf("ResetU3USB error : did not read all of the buffer\n");
      return -1;
  }

  if ((uint8)(normalChecksum8(recBuff, 4)) != recBuff[0])
  {
    printf("ResetU3USB error : read buffer has bad checksum\n");
    return -1;
  }

  if (recBuff[1] != (uint8)(0x99))
  {
    printf("ResetU3USB error : read buffer has wrong command byte\n");
    return -1;
  }

  if (recBuff[2] != (uint8)(0x00))
  {
    printf("ResetU3USB error : wrong response\n");
    return -1;
  }

  if (recBuff[3] != (uint8)(0x00))
  {
    printf("ResetU3USB error : the command failed\n");
    return -1;
  }

	return LJE_NOERROR;
}

__inline long ConfigWatchdogU3(HANDLE hDevice, BOOL bEnable, SHORT timeout)
{
  uint8 sendBuff[16], recBuff[8];
  int sendChars, recChars;
  uShort word;

  sendBuff[1] = 0xF8; // Command byte.
  sendBuff[2] = 0x05; // Number of data words.
  sendBuff[3] = 0x09; // Extended command number.
  sendBuff[6] = 1; // ?????????????????????????????????????????????????????????????????????

  if (bEnable)
  {
	  sendBuff[7] = 32; // WatchdogOptions (the device should be reset on timeout).
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

	return LJE_NOERROR;
}

__inline long CloseU3USB(HANDLE hDevice)
{
	closeUSBConnection(hDevice);

	return LJE_NOERROR;
}
#endif // _WIN32

#endif // U3CORE_H
