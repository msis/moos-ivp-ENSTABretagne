/***************************************************************************************************************:')

U3CfgUDw32.c

Labjack U3 handling. It uses the high level DLL provided with the Windows driver. Therefore, it works only under 
Windows.

Fabrice Le Bars
Stéphane Bazeille
Arunas Mazeika
Some functions and parts of functions are from the examples provided by www.labjack.com

Created : 2009-03-28

Version status : Not finished

***************************************************************************************************************:)*/

#include "U3CfgUDw32.h"

#define MAX_NB_TIMERS_U3 2

/*
Error handling macro that is called after every Labjack function call.
If there was an error, a message is displayed and the calling function
will return EXIT_FAILURE.

LJ_ERROR lngErrorcode : (IN) Error code returned by a Labjack function.
*/
#define U3ErrorHandler(lngErrorcode) \
	if (lngErrorcode != LJE_NOERROR)\
{\
	char szErrMsg[256];\
	ErrorToString(lngErrorcode, szErrMsg);\
	printf("%s\n", szErrMsg);\
	return EXIT_FAILURE;\
}

/*
Open a Labjack U3 and set 2 PWM outputs on FIO0 and FIO1, 2 analog inputs on
FIO2 and FIO3 and 2 analog outputs on DAC0 and DAC1.

HU3* phU3 : (INOUT) Pointer that will receive an identifier representing the
device opened. For example, declare hU3 as a HU3 and pass &hU3 to the function.
char* szDevice : (IN) Path to the device. For example, pass "0" to the function.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int U3Open(HU3* phU3, char* szDevice)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.
	long aEnableTimers[MAX_NB_TIMERS_U3]; // Specify how many timers are enabled.
	long aEnableCounters[MAX_NB_TIMERS_U3]; // Specify how many counters are enabled.
	long aTimerModes[MAX_NB_TIMERS_U3]; // Timers modes.
	double aTimerValues[MAX_NB_TIMERS_U3]; // Timers duty cycles.
	long TimerClockBaseIndex = 0; // Timer base clock.
	int i = 0;

	// Open a Labjack U3;
	lngErrorcode = OpenLabJack(LJ_dtU3, LJ_ctUSB, szDevice, 0, phU3);
	U3ErrorHandler(lngErrorcode);

	// Set the pins to the factory default condition.
	lngErrorcode = ResetLabJack(*phU3); 
	U3ErrorHandler(lngErrorcode);

	// Configure FIO0 and FIO1 as PWM outputs with a frequency next to 50 Hz and 
	// a pulse width of 1.5 ms (neutral position) in order to use them to control
	// servomotors.

	// Indicate that all the timers of the Labjack U3 are enabled, and the 2 
	// counters are disabled.
	for (i = 0; i < MAX_NB_TIMERS_U3; i++)
	{
		aEnableTimers[i] = 1;
	}
	aEnableCounters[0] = 0;
	aEnableCounters[1] = 0;
#ifdef OLD_HARDWARE_VERSION
	// All output timers use the same timer clock, configured here. The
	// base clock is set to 24MHZ_DIV, meaning that the clock divisor
	// is supported and Counter0 is not available.
	TimerClockBaseIndex = LJ_tc24MHZ_DIV;
#else
	// All output timers use the same timer clock, configured here. The
	// base clock is set to 48MHZ_DIV, meaning that the clock divisor
	// is supported and Counter0 is not available.
	TimerClockBaseIndex = LJ_tc48MHZ_DIV;
#endif // OLD_HARDWARE_VERSION
	// All the timers will have their duty cycles coded on 16 bits and
	// the duty cycles will correspond to a pulse width of 1.5 ms.
	for (i = 0; i < MAX_NB_TIMERS_U3; i++)
	{
		aTimerModes[i] = LJ_tmPWM16;
		aTimerValues[i] = U3_PWM_DUTY_CYCLE_DEFAULT;
	}

	lngErrorcode = eTCConfig(*phU3, // Handle to the Labjack.
		aEnableTimers, // Specify how many timers are enabled.
		aEnableCounters,  // Specify how many counters are enabled.
		0, // Indicate that the first timer will be on FIO0 (offset=0).
		TimerClockBaseIndex, // Timer base clock.
		U3_TIMER_CLOCK_DIVISOR, // The PWM frequency, which depends on U3_TIMER_CLOCK_DIVISOR is set to 45,78 Hz. 
		// See the macro U3_PWM_FREQUENCY.
		aTimerModes, // Timers modes.
		aTimerValues, // Timers duty cycles. 
		0, 0 // Reserved.
		);
	U3ErrorHandler(lngErrorcode);

	// Configure FIO2 and FIO3 as analog inputs.
	lngErrorcode = ePut(*phU3, LJ_ioPUT_ANALOG_ENABLE_BIT, FIO2, 1, 0);
	U3ErrorHandler(lngErrorcode);
	lngErrorcode = ePut(*phU3, LJ_ioPUT_ANALOG_ENABLE_BIT, FIO3, 1, 0);
	U3ErrorHandler(lngErrorcode);

	// Configure DAC0 and DAC1 to output 0 V.
	lngErrorcode = eDAC(*phU3, DAC0, 0, 0, 0, 0);
	U3ErrorHandler(lngErrorcode);
	lngErrorcode = eDAC(*phU3, DAC1, 0, 0, 0, 0);
	U3ErrorHandler(lngErrorcode);

	return EXIT_SUCCESS;
}

/*
Change the frequency of a PWM.

HU3 hU3 : (IN) Identifier representing the device.
double freq : (IN) Desired frequency in Hz. The resulting frequency will be 
 very approximative.
long pin : (IN) Should be set to FIO0 (first PWM) or FIO1 (second PWM).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int U3SetPWMFreq(HU3 hU3, double freq, long pin)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.
	int timer_clock_divisor = (int)((double)U3_FREQUENCY/(65536.0*freq));

	// Change the timer clock divisor at the specified pin.
	lngErrorcode = ePut(hU3, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_DIVISOR, timer_clock_divisor, pin);
	U3ErrorHandler(lngErrorcode);

	return EXIT_SUCCESS;
}

/*
Modify a PWM pulse width.

HU3 hU3 : (IN) Identifier representing the device.
double pulseWidth : (IN) Desired pulse width in ms. For example, if a
 servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
 or 2 in the other side.
long pin : (IN) Should be set to FIO0 (first PWM) or FIO1 (second PWM).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int U3SetPWMPulseWidth(HU3 hU3, double pulseWidth, long pin)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.

	lngErrorcode = ePut(hU3, LJ_ioPUT_TIMER_VALUE, pin, U3_PULSE_WIDTH2TIMER_DUTY_CYCLE(pulseWidth), 0);
	U3ErrorHandler(lngErrorcode);

	return EXIT_SUCCESS;
}

/*
Output a voltage.

HU3 hU3 : (IN) Identifier representing the device.
double voltage : (IN) Desired voltage in V. 
long pin : (IN) Should be set to DAC0 or DAC1.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int U3SetVoltage(HU3 hU3, double voltage, long pin)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.

	lngErrorcode = ePut(hU3, LJ_ioPUT_DAC, pin, voltage, 0);
	U3ErrorHandler(lngErrorcode);

	return EXIT_SUCCESS;
}

/*
Read a voltage.

HU3 hU3 : (IN) Identifier representing the device.
double* pVoltage : (INOUT) Pointer receiving the voltage in V. For example,
 declare voltage as a double and pass &voltage to the function.
long pin : (IN) Should be set to FIO2 or FIO3.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int U3GetVoltage(HU3 hU3, double* pVoltage, long pin)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.

	lngErrorcode = eGet(hU3, LJ_ioGET_AIN, pin, pVoltage, 0);
	U3ErrorHandler(lngErrorcode);

	return EXIT_SUCCESS;
}

/*
Get the temperature in the device.

HU3 hU3 : (IN) Identifier representing the device.
double* pTemperature : (INOUT) Pointer receiving the temperature in Kelvin. For
 example, declare temperature as a double and pass &temperature to the function.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int U3GetTemperature(HU3 hU3, double* pTemperature)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.

	lngErrorcode = eGet(hU3, LJ_ioGET_AIN, 30, pTemperature, 0);
	U3ErrorHandler(lngErrorcode);

	return EXIT_SUCCESS;
}

/*
Close the device. Note that the device is really closed when the LabJackUD DLL
is unloaded or the application exits.

HU3* phU3 : (INOUT) Pointer to the identifier representing the device.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int U3Close(HU3* phU3)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.

	// Set the pins to the factory default condition.
	lngErrorcode = ResetLabJack(*phU3); 
	U3ErrorHandler(lngErrorcode);

	return EXIT_SUCCESS;
}
