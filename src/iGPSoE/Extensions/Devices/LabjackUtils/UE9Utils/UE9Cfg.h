/***************************************************************************************************************:')

UE9Cfg.h

Labjack UE9 handling.

Fabrice Le Bars
Emilie Jude
Nassima Bouchelit
Some functions and parts of functions are from the examples provided by www.labjack.com

Created : 2009-03-28

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef UE9CFG_H
#define UE9CFG_H

#include "OSMisc.h" 
#include "UE9Core.h" 

/*
Debug macros specific to UE9Cfg.
*/
#ifdef _DEBUG_MESSAGES_UE9UTILS
#	define _DEBUG_MESSAGES_UE9CFG
#endif // _DEBUG_MESSAGES_UE9UTILS

#ifdef _DEBUG_WARNINGS_UE9UTILS
#	define _DEBUG_WARNINGS_UE9CFG
#endif // _DEBUG_WARNINGS_UE9UTILS

#ifdef _DEBUG_ERRORS_UE9UTILS
#	define _DEBUG_ERRORS_UE9CFG
#endif // _DEBUG_ERRORS_UE9UTILS

#ifdef _DEBUG_MESSAGES_UE9CFG
#	define PRINT_DEBUG_MESSAGE_UE9CFG(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_UE9CFG(params)
#endif // _DEBUG_MESSAGES_UE9CFG

#ifdef _DEBUG_WARNINGS_UE9CFG
#	define PRINT_DEBUG_WARNING_UE9CFG(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_UE9CFG(params)
#endif // _DEBUG_WARNINGS_UE9CFG

#ifdef _DEBUG_ERRORS_UE9CFG
#	define PRINT_DEBUG_ERROR_UE9CFG(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_UE9CFG(params)
#endif // _DEBUG_ERRORS_UE9CFG

struct UE9
{
#ifdef _WIN32
	LJ_HANDLE hDev;
#else
	HANDLE hDev;
	ue9CalibrationInfo caliInfo;
#endif // _WIN32
	double adblTimerValues[MAX_NB_TIMERS_UE9]; // Timers duty cycles.
};
typedef struct UE9* HUE9;

#define INVALID_HUE9_VALUE NULL

/*
Open a Labjack UE9 and set 6 PWM outputs from FIO0 to FIO5, 2 analog inputs on
AIN0 and AIN1 and 2 analog outputs on DAC0 and DAC1.

HUE9* phUE9 : (INOUT) Valid pointer that will receive an identifier of the
UE9 opened.
char* szDevice : (IN) Path to the device. For example, pass "1" to the function
to open the first.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int OpenUE9(HUE9* phUE9, char* szDevice)
{
	long alngEnableTimers[MAX_NB_TIMERS_UE9]; // Specify how many timers are enabled.
	long alngEnableCounters[MAX_NB_TIMERS_UE9]; // Specify how many counters are enabled.
	long alngTimerModes[MAX_NB_TIMERS_UE9]; // Timers modes.
	long lngTimerClockBaseIndex = 0; // Timer base clock.
	int i = 0;

	*phUE9 = (HUE9)calloc(1, sizeof(struct UE9));

	if (*phUE9 == NULL)
	{
		PRINT_DEBUG_ERROR_UE9CFG(("OpenUE9 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	// Open a Labjack UE9.
#ifdef _WIN32
	if (OpenLabJack(LJ_dtUE9, LJ_ctUSB, szDevice, 0, &(*phUE9)->hDev) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_UE9CFG(("OpenUE9 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot open the device. ", 
			szDevice));
		free(*phUE9);
		return EXIT_FAILURE;
	}
#else
	if (OpenUE9USB(szDevice, &(*phUE9)->hDev, &(*phUE9)->caliInfo) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_UE9CFG(("OpenUE9 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot open the device. ", 
			szDevice));
		free(*phUE9);
		return EXIT_FAILURE;
	}
#endif // _WIN32

	// Problem under Linux, due to getCalibration() in OpenUE9USB().
//	ResetUE9USB((*phUE9)->hDev, FALSE);

	// Configure FIO0, 1, 2, 3 as PWM outputs with a frequency next to 50 Hz and 
	// a pulse width of 1.5 ms (neutral position) in order to use them to control
	// servomotors.

	// Indicate that all the timers of the Labjack UE9 are enabled, and the 2 
	// counters are disabled.
	for (i = 0; i < MAX_NB_TIMERS_UE9; i++)
	{
		alngEnableTimers[i] = 1;
	}
	alngEnableCounters[0] = 0;
	alngEnableCounters[1] = 0;

	// All output timers use the same timer clock, configured here. The
	// base clock is set to the system clock (48 MHz).
	lngTimerClockBaseIndex = LJ_tcSYS;

	// All the timers will have their duty cycles coded on 16 bits and
	// the duty cycles will correspond to a pulse width of 1.5 ms.
	for (i = 0; i < MAX_NB_TIMERS_UE9; i++)
	{
		alngTimerModes[i] = LJ_tmPWM16;
		(*phUE9)->adblTimerValues[i] = PWM_DUTY_CYCLE_DEFAULT_UE9;
	}

	if (eTCConfig((*phUE9)->hDev, // Handle to the Labjack.
		alngEnableTimers, // Specify how many timers are enabled.
		alngEnableCounters,  // Specify how many counters are enabled.
		0, // Indicate that the first timer will be on FIO0 (offset=0).
		lngTimerClockBaseIndex, // Timer base clock.
		TIMER_CLOCK_DIVISOR_UE9, // The PWM frequency, which depends on TIMER_CLOCK_DIVISOR_UE9 is set to 48.83 Hz. 
		// See the macro PWM_FREQUENCY_UE9.
		alngTimerModes, // Timers modes (PWM with duty cycle coded on 16 bit).
		(*phUE9)->adblTimerValues, // Timers duty cycles. 
		0, 0 // Reserved.
		) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_UE9CFG(("OpenUE9 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot configure timers. ", 
			szDevice));
		ResetUE9USB((*phUE9)->hDev, FALSE);
		free(*phUE9);
		return EXIT_FAILURE;
	}

	// Set DAC0 and DAC1 to 0 V.
	if ((eDAC((*phUE9)->hDev, 
#ifndef _WIN32
		&(*phUE9)->caliInfo, 
#endif // _WIN32
		DAC0, 0, 0, 0, 0) != LJE_NOERROR) || 
		(eDAC((*phUE9)->hDev, 
#ifndef _WIN32
		&(*phUE9)->caliInfo, 
#endif // _WIN32
		DAC1, 0, 0, 0, 0) != LJE_NOERROR))
	{
		PRINT_DEBUG_ERROR_UE9CFG(("OpenUE9 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error setting a voltage. ", 
			szDevice));
		ResetUE9USB((*phUE9)->hDev, FALSE);
		free(*phUE9);
		return EXIT_FAILURE;
	}

	// The configuration of AIN0 and AIN1 as analog inputs
	// will be done when calling GetVoltageUE9().

	return EXIT_SUCCESS;
}

/*
Change the frequency of all PWM of a Labjack UE9.

HUE9 hUE9 : (IN) Identifier of the Labjack UE9.
double freq : (IN) Desired frequency in Hz. The resulting frequency will be 
very approximative.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetAllPWMFreqUE9(HUE9 hUE9, double freq)
{
	long alngEnableTimers[MAX_NB_TIMERS_UE9]; // Specify how many timers are enabled.
	long alngEnableCounters[MAX_NB_TIMERS_UE9]; // Specify how many counters are enabled.
	long alngTimerModes[MAX_NB_TIMERS_UE9]; // Timers modes.
	long lngTimerClockBaseIndex = 0; // Timer base clock.
	long lngTimerClockDivisor = (long)((double)FREQUENCY_UE9/(65536.0*freq));
	int i = 0;

	// Indicate that all the timers of the Labjack UE9 are enabled, and the 2 
	// counters are disabled.
	for (i = 0; i < MAX_NB_TIMERS_UE9; i++)
	{
		alngEnableTimers[i] = 1;
	}
	alngEnableCounters[0] = 0;
	alngEnableCounters[1] = 0;

	// All output timers use the same timer clock, configured here. The
	// base clock is set to the system clock (48 MHz).
	lngTimerClockBaseIndex = LJ_tcSYS;

	// All the timers will have their duty cycles coded on 16 bits.
	for (i = 0; i < MAX_NB_TIMERS_UE9; i++)
	{
		alngTimerModes[i] = LJ_tmPWM16;
			// Keep adblTimerValues.
	}

	if (eTCConfig(hUE9->hDev, // Handle to the Labjack.
		alngEnableTimers, // Specify how many timers are enabled.
		alngEnableCounters,  // Specify how many counters are enabled.
		0, // Indicate that the first timer will be on FIO0 (offset=0).
		lngTimerClockBaseIndex, // Timer base clock.
		lngTimerClockDivisor, // The PWM frequency depends on lngTimerClockDivisor. 
		// See the macro PWM_FREQUENCY_UE9.
		alngTimerModes, // Timers modes (PWM with duty cycle coded on 16 bit).
		hUE9->adblTimerValues, // Timers duty cycles. 
		0, 0 // Reserved.
		) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_UE9CFG(("SetAllPWMFreqUE9 error (%s) : %s"
			"(hUE9=%#x, freq=%f)\n", 
			strtime_m(), 
			"Error setting the timer clock divisor. ", 
			hUE9, freq));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Modify all PWM pulse width of a Labjack UE9.

HUE9 hUE9 : (IN) Identifier of the Labjack UE9.
double pulseWidths[MAX_NB_TIMERS_UE9] : (IN) Desired pulse width in ms. For example, if a
servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
or 2 in the other side.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetAllPWMPulseWidthUE9(HUE9 hUE9, double pulseWidths[MAX_NB_TIMERS_UE9])
{
	long alngReadTimers[MAX_NB_TIMERS_UE9]; 
	long alngUpdateResetTimers[MAX_NB_TIMERS_UE9]; 
	long alngReadCounters[MAX_NB_TIMERS_UE9]; 
	long alngResetCounters[MAX_NB_TIMERS_UE9];
	double adblCounterValues[MAX_NB_TIMERS_UE9];
	int i = 0;

	for (i = 0; i < MAX_NB_TIMERS_UE9; i++)
	{
		alngReadTimers[i] = 0;
		alngUpdateResetTimers[i] = 1; // Indicate to update the timer.
		alngReadCounters[i] = 0;
		alngResetCounters[i] = 0;
		hUE9->adblTimerValues[i] = PULSE_WIDTH2TIMER_DUTY_CYCLE_UE9(pulseWidths[i]); // New duty cycle.
		adblCounterValues[i] = 0;
	}

	if (eTCValues(hUE9->hDev, alngReadTimers, alngUpdateResetTimers, alngReadCounters, 
		alngResetCounters, hUE9->adblTimerValues, adblCounterValues, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_UE9CFG(("SetAllPWMPulseWidthUE9 error (%s) : %s"
			"(hUE9=%#x, pulseWidths=%#x)\n", 
			strtime_m(), 
			"Error setting a PWM pulse width. ", 
			hUE9, pulseWidths));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Modify a PWM pulse width of a Labjack UE9.

HUE9 hUE9 : (IN) Identifier of the Labjack UE9.
double pulseWidth : (IN) Desired pulse width in ms. For example, if a
servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
or 2 in the other side.
long pin : (IN) Should be set to FIO0, 1, 2, 3, 4 or FIO5.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetPWMPulseWidthUE9(HUE9 hUE9, double pulseWidth, long pin)
{
	long alngReadTimers[MAX_NB_TIMERS_UE9]; 
	long alngUpdateResetTimers[MAX_NB_TIMERS_UE9]; 
	long alngReadCounters[MAX_NB_TIMERS_UE9]; 
	long alngResetCounters[MAX_NB_TIMERS_UE9];
	double adblCounterValues[MAX_NB_TIMERS_UE9];
	int i = 0;

	for (i = 0; i < MAX_NB_TIMERS_UE9; i++)
	{
		if (i == pin)
		{
			alngReadTimers[i] = 0;
			alngUpdateResetTimers[i] = 1; // Indicate to update the timer.
			alngReadCounters[i] = 0;
			alngResetCounters[i] = 0;
			hUE9->adblTimerValues[i] = PULSE_WIDTH2TIMER_DUTY_CYCLE_UE9(pulseWidth); // New duty cycle.
			adblCounterValues[i] = 0;
		}
		else
		{
			alngReadTimers[i] = 0;
			alngUpdateResetTimers[i] = 0;
			alngReadCounters[i] = 0;
			alngResetCounters[i] = 0;
			// Keep adblTimerValues.
			adblCounterValues[i] = 0;
		}
	}

	if (eTCValues(hUE9->hDev, alngReadTimers, alngUpdateResetTimers, alngReadCounters, 
		alngResetCounters, hUE9->adblTimerValues, adblCounterValues, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_UE9CFG(("SetPWMPulseWidthUE9 error (%s) : %s"
			"(hUE9=%#x, pulseWidth=%f, pin=%d)\n", 
			strtime_m(), 
			"Error setting a PWM pulse width. ", 
			hUE9, pulseWidth, (int)pin));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Output a voltage from a Labjack UE9.

HUE9 hUE9 : (IN) Identifier of the Labjack UE9.
double voltage : (IN) Desired voltage in V. 
long pin : (IN) Should be set to DAC0 or DAC1.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetVoltageUE9(HUE9 hUE9, double voltage, long pin)
{
	if (eDAC(hUE9->hDev, 
#ifndef _WIN32
		&hUE9->caliInfo, 
#endif // _WIN32
		pin, voltage, 0, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_UE9CFG(("SetVoltageUE9 error (%s) : %s"
			"(hUE9=%#x, voltage=%f, pin=%d)\n", 
			strtime_m(), 
			"Error setting a voltage. ", 
			hUE9, voltage, (int)pin));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Read a voltage from a Labjack UE9.

HUE9 hUE9 : (IN) Identifier of the Labjack UE9.
double* pVoltage : (INOUT) Valid pointer receiving the voltage in V. For 
example, declare voltage as a double and pass &voltage to the function.
long pin : (IN) Should be set to AIN0 or AIN1.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetVoltageUE9(HUE9 hUE9, double* pVoltage, long pin)
{
	if (eAIN(hUE9->hDev, 
#ifndef _WIN32
		&hUE9->caliInfo, 
#endif // _WIN32
		pin, 0, pVoltage, LJ_rgUNI5V, 12, 0, 0, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_UE9CFG(("GetVoltageUE9 error (%s) : %s"
			"(hUE9=%#x, pin=%d)\n", 
			strtime_m(), 
			"Error reading a voltage. ", 
			hUE9, (int)pin));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Get the temperature in a Labjack UE9.

HUE9 hUE9 : (IN) Identifier of the Labjack UE9.
double* pTemperature : (INOUT) Valid pointer receiving the temperature in 
Celsius degrees. For example, declare temperature as a double and pass  
&temperature to the function.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetTemperatureUE9(HUE9 hUE9, double* pTemperature)
{
	if (eAIN(hUE9->hDev, 
#ifndef _WIN32
		&hUE9->caliInfo, 
#endif // _WIN32
		TEMPERATURE_SENSOR_CHANNEL, 0, pTemperature, LJ_rgUNI5V, 12, 0, 0, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_UE9CFG(("GetTemperatureUE9 error (%s) : %s"
			"(hUE9=%#x)\n", 
			strtime_m(), 
			"Error getting the temperature. ", 
			hUE9));
		return EXIT_FAILURE;
	}

	// Converts from Kelvin to Celsius.
	*pTemperature = KELVIN2CELSIUS(*pTemperature);

	return EXIT_SUCCESS;
}

/*
Software reset of a Labjack UE9.

HUE9 hUE9 : (IN) Identifier of the Labjack UE9.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ResetUE9(HUE9 hUE9)
{
	if (ResetUE9USB(hUE9->hDev, FALSE) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_UE9CFG(("ResetUE9 error (%s) : %s"
			"(hUE9=%#x)\n", 
			strtime_m(), 
			"Error resetting the device. ", 
			hUE9));
		return EXIT_FAILURE;
	}

	mSleep(4000);

	return EXIT_SUCCESS;
}

/*
Close a Labjack UE9. 
Note that on Windows the device is really closed when the LabJackUD DLL
is unloaded or the application exits.

HUE9* phUE9 : (INOUT) Valid pointer to the identifier of the Labjack UE9.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CloseUE9(HUE9* phUE9)
{
	long alngEnableTimers[MAX_NB_TIMERS_UE9]; // Specify how many timers are enabled.
	long alngEnableCounters[MAX_NB_TIMERS_UE9]; // Specify how many counters are enabled.
	long alngTimerModes[MAX_NB_TIMERS_UE9]; // Timers modes.
	long lngTimerClockBaseIndex = 0; // Timer base clock.
	int i = 0;
	BOOL bError = FALSE;

	// Disable all timers.
	for (i = 0; i < MAX_NB_TIMERS_UE9; i++)
	{
		alngEnableTimers[i] = 0;
	}
	alngEnableCounters[0] = 0;
	alngEnableCounters[1] = 0;

	// All output timers use the same timer clock, configured here. The
	// base clock is set to the system clock (48 MHz).
	lngTimerClockBaseIndex = LJ_tcSYS;

	// All the timers will have their duty cycles coded on 16 bits and
	// the duty cycles will correspond to a pulse width of 1.5 ms.
	for (i = 0; i < MAX_NB_TIMERS_UE9; i++)
	{
		alngTimerModes[i] = LJ_tmPWM16;
		(*phUE9)->adblTimerValues[i] = PWM_DUTY_CYCLE_DEFAULT_UE9;
	}

	if (eTCConfig((*phUE9)->hDev, // Handle to the Labjack.
		alngEnableTimers, // Specify how many timers are enabled.
		alngEnableCounters,  // Specify how many counters are enabled.
		0, // Indicate that the first timer will be on FIO0 (offset=0).
		lngTimerClockBaseIndex, // Timer base clock.
		TIMER_CLOCK_DIVISOR_UE9, // The PWM frequency, which depends on TIMER_CLOCK_DIVISOR_UE9 is set to 48.83 Hz. 
		// See the macro PWM_FREQUENCY_UE9.
		alngTimerModes, // Timers modes (PWM with duty cycle coded on 16 bit).
		(*phUE9)->adblTimerValues, // Timers duty cycles. 
		0, 0 // Reserved.
		) != LJE_NOERROR)
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_UE9CFG(("CloseUE9 error (%s) : %s"
			"(*phUE9=%#x)\n", 
			strtime_m(), 
			"Cannot disable timers. ", 
			*phUE9));
	}

	// Set DAC0 and DAC1 to 0 V.
	if ((eDAC((*phUE9)->hDev, 
#ifndef _WIN32
		&(*phUE9)->caliInfo, 
#endif // _WIN32
		DAC0, 0, 0, 0, 0) != LJE_NOERROR) || 
		(eDAC((*phUE9)->hDev, 
#ifndef _WIN32
		&(*phUE9)->caliInfo, 
#endif // _WIN32
		DAC1, 0, 0, 0, 0) != LJE_NOERROR))
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_UE9CFG(("CloseUE9 error (%s) : %s"
			"(*phUE9=%#x)\n", 
			strtime_m(), 
			"Error setting a voltage. ", 
			*phUE9));
	}

#ifdef _WIN32
	// The device is closed when the LabJackUD DLL
	// is unloaded or the application exits.
#else
	if (CloseUE9USB((*phUE9)->hDev) != LJE_NOERROR)
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_UE9CFG(("CloseUE9 error (%s) : %s"
			"(*phUE9=%#x)\n", 
			strtime_m(), 
			"Error closing the device. ", 
			*phUE9));
	}
#endif // _WIN32

	free(*phUE9);

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // UE9CFG_H
