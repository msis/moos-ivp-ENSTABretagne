/***************************************************************************************************************:')

U3CfgUDw32.h

Labjack U3 handling. It uses the high level DLL provided with the Windows driver. Therefore, it works only under 
Windows.

Fabrice Le Bars
Stéphane Bazeille
Arunas Mazeika
Some functions and parts of functions are from the examples provided by www.labjack.com

Created : 2009-03-28

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef U3CFGUDW32_H
#define U3CFGUDW32_H

// C RunTime Header Files.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Windows header file.
#include <windows.h>

// Labjack high level Windows driver header.
#include "LabJackUD.h"

// Kelvin to Celsius degrees conversions.
#define KELVIN2CELSIUS(temperature) ((temperature)-273.15)
#define CELSIUS2KELVIN(temperature) ((temperature)+273.15)

// Type representing an identifier of the device.
typedef LJ_HANDLE HU3;

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

// If the device has a hardware version number less than 1.21, define 
// OLD_HARDWARE_VERSION. The version of the driver installed must also support
// the hardware version used.
#ifdef OLD_HARDWARE_VERSION
// Base internal frequency for all timers in the device.
#define U3_FREQUENCY 24000000
#define U3_TIMER_CLOCK_DIVISOR 8
#else
// Base internal frequency for all timers in the device.
#define U3_FREQUENCY 48000000
#define U3_TIMER_CLOCK_DIVISOR 16
#endif // OLD_HARDWARE_VERSION

// The PWM have a frequency that depends on the base internal frequency
// of the timers (U3_FREQUENCY) and U3_TIMER_CLOCK_DIVISOR as in the following 
// formula. The PWM frequency chosen will be of 45.78 Hz.
#define U3_PWM_FREQUENCY ((double)U3_FREQUENCY/(65536.0*U3_TIMER_CLOCK_DIVISOR))

// Macros used to set the desired PWM pulse width.
#define U3_PULSE_WIDTH2TIMER_DUTY_CYCLE(pulse_width) ((int)(65536.0-((pulse_width)*U3_PWM_FREQUENCY*65.536)))
#define U3_TIMER_DUTY_CYCLE2PULSE_WIDTH(timer_duty_cycle) ((65536.0-(timer_duty_cycle))/(65536.0*U3_PWM_FREQUENCY))

// Timer value corresponding to a pulse width of 1.5 ms (neutral position for
// a servomotor).
#define U3_PWM_DUTY_CYCLE_DEFAULT (U3_PULSE_WIDTH2TIMER_DUTY_CYCLE(1.5))

int U3Open(HU3* phU3, char* szDevice);
int U3Test(HU3 hU3);
int U3SetPWMFreq(HU3 hU3, double freq, long pin);
int U3SetPWMPulseWidth(HU3 hU3, double pulseWidth, long pin);
int U3SetVoltage(HU3 hU3, double voltage, long pin);
int U3GetVoltage(HU3 hU3, double* pVoltage, long pin);
int U3GetTemperature(HU3 hU3, double* pTemperature);
int U3Close(HU3* phU3);

#endif // U3CFGUDW32_H
