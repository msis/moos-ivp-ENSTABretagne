/***************************************************************************************************************:')

UE9CfgUDw32.h

Labjack UE9 handling. It uses the high level DLL provided with the Windows driver. Therefore, it works only under 
Windows.

Fabrice Le Bars
Some functions and parts of functions are from the examples provided by www.labjack.com

Created : 2009-03-28

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef UE9CFGUDW32_H
#define UE9CFGUDW32_H

// C RunTime Header Files.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Windows header file.
#include <windows.h>

// Labjack high level Windows driver header.
#include "LabJackUD.h"

// Type representing an identifier of the device.
typedef LJ_HANDLE HUE9;

// Kelvin to Celsius degrees conversions.
#define KELVIN2CELSIUS(temperature) ((temperature)-273.15)
#define CELSIUS2KELVIN(temperature) ((temperature)+273.15)

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

// The PWM have a frequency that depends on the base internal frequency
// of the timers (UE9_FREQUENCY) and UE9_TIMER_CLOCK_DIVISOR as in the following 
// formula. The PWM frequency chosen will be of 45.78 Hz.
#define PWM_FREQUENCY_UE9 ((double)FREQUENCY_UE9/(65536.0*TIMER_CLOCK_DIVISOR_UE9))

// Macros used to set the desired PWM pulse width.
#define PULSE_WIDTH2TIMER_DUTY_CYCLE_UE9(pulse_width) ((int)(65536.0-((pulse_width)*PWM_FREQUENCY_UE9*65.536)))
#define TIMER_DUTY_CYCLE2PULSE_WIDTH_UE9(timer_duty_cycle) ((65536.0-(timer_duty_cycle))/(65536.0*PWM_FREQUENCY_UE9))

// Timer value corresponding to a pulse width of 1.5 ms (neutral position for
// a servomotor).
#define PWM_DUTY_CYCLE_DEFAULT_UE9 (PULSE_WIDTH2TIMER_DUTY_CYCLE_UE9(1.5))

// Base internal frequency for all timers in the device.
#define FREQUENCY_UE9 48000000
#define TIMER_CLOCK_DIVISOR_UE9 16

int OpenUE9(HUE9* phUE9, char* szDevice);
/*int OpenUE9Ex(HUE9* phUE9, char* szDevice, int nbTimers, int nbVoltageIn, int nbVoltageOut, BOOL bEnableI2C);*/
int TestUE9(HUE9 hUE9);
int SetPWMFreqUE9(HUE9 hUE9, double freq, long pin);
int SetPWMPulseWidthUE9(HUE9 hUE9, double pulseWidth, long pin);
int SetVoltageUE9(HUE9 hUE9, double voltage, long pin);
int GetVoltageUE9(HUE9 hUE9, double* pVoltage, long pin);
int GetTemperatureUE9(HUE9 hUE9, double* pTemperature);
/*
int ConfigI2CUE9(int address, BOOL bResetAtStart, int speed);
int WriteI2CUE9(char* buf, int nbBytes);
int ReadI2CUE9(char** pBuf, int nbBytes);
*/
int CloseUE9(HUE9* phUE9);

#endif // UE9CFGUDW32_H
