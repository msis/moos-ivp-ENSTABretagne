/***************************************************************************************************************:')

U3Cfg.h

Labjack U3 handling.

Fabrice Le Bars

Created : 2009-03-28

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef U3CFG_H
#define U3CFG_H

#include "OSMisc.h" 
#include "U3Core.h" 

/*
Debug macros specific to U3Cfg.
*/
#ifdef _DEBUG_MESSAGES_U3UTILS
#	define _DEBUG_MESSAGES_U3CFG
#endif // _DEBUG_MESSAGES_U3UTILS

#ifdef _DEBUG_WARNINGS_U3UTILS
#	define _DEBUG_WARNINGS_U3CFG
#endif // _DEBUG_WARNINGS_U3UTILS

#ifdef _DEBUG_ERRORS_U3UTILS
#	define _DEBUG_ERRORS_U3CFG
#endif // _DEBUG_ERRORS_U3UTILS

#ifdef _DEBUG_MESSAGES_U3CFG
#	define PRINT_DEBUG_MESSAGE_U3CFG(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_U3CFG(params)
#endif // _DEBUG_MESSAGES_U3CFG

#ifdef _DEBUG_WARNINGS_U3CFG
#	define PRINT_DEBUG_WARNING_U3CFG(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_U3CFG(params)
#endif // _DEBUG_WARNINGS_U3CFG

#ifdef _DEBUG_ERRORS_U3CFG
#	define PRINT_DEBUG_ERROR_U3CFG(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_U3CFG(params)
#endif // _DEBUG_ERRORS_U3CFG

struct U3
{
#ifdef _WIN32
	LJ_HANDLE hDev;
#else
	HANDLE hDev;
	u3CalibrationInfo caliInfo;
#endif // _WIN32
	double adblTimerValues[MAX_NB_TIMERS_U3]; // Timers duty cycles.
};
typedef struct U3* HU3;

#define INVALID_HU3_VALUE NULL

/*
Open a Labjack U3 and set 2 PWM outputs from FIO4 to FIO5, 2 analog inputs on
AIN0 and AIN1 and 2 analog outputs on DAC0 and DAC1.

HU3* phU3 : (INOUT) Valid pointer that will receive an identifier of the
U3 opened.
char* szDevice : (IN) Path to the device. For example, pass "1" to the function
to open the first.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int OpenU3(HU3* phU3, char* szDevice)
{
	long alngEnableTimers[MAX_NB_TIMERS_U3]; // Specify how many timers are enabled.
	long alngEnableCounters[MAX_NB_TIMERS_U3]; // Specify how many counters are enabled.
	long alngTimerModes[MAX_NB_TIMERS_U3]; // Timers modes.
	long lngTimerClockBaseIndex = 0; // Timer base clock.
	int i = 0;

	*phU3 = (HU3)calloc(1, sizeof(struct U3));

	if (*phU3 == NULL)
	{
		PRINT_DEBUG_ERROR_U3CFG(("OpenU3 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	// Open a Labjack U3.
#ifdef _WIN32
	if (OpenLabJack(LJ_dtU3, LJ_ctUSB, szDevice, 0, &(*phU3)->hDev) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U3CFG(("OpenU3 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot open the device. ", 
			szDevice));
		free(*phU3);
		return EXIT_FAILURE;
	}
#else
	if (OpenU3USB(szDevice, &(*phU3)->hDev, &(*phU3)->caliInfo) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U3CFG(("OpenU3 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot open the device. ", 
			szDevice));
		free(*phU3);
		return EXIT_FAILURE;
	}
#endif // _WIN32

	// Problem under Linux, due to getCalibration() in OpenU3USB().
//	ResetU3USB((*phU3)->hDev, FALSE);

	// Configure FIO4 and FIO5 as PWM outputs with a frequency next to 50 Hz and 
	// a pulse width of 1.5 ms (neutral position) in order to use them to control
	// servomotors.

	// Indicate that all the timers of the Labjack U3 are enabled, and the 2 
	// counters are disabled.
	for (i = 0; i < MAX_NB_TIMERS_U3; i++)
	{
		alngEnableTimers[i] = 1;
	}
	alngEnableCounters[0] = 0;
	alngEnableCounters[1] = 0;

#ifdef OLD_HARDWARE_VERSION_U3
	// All output timers use the same timer clock, configured here. The
	// base clock is set to the system clock (24 MHz) with a divisor.
	lngTimerClockBaseIndex = LJ_tc24MHZ_DIV;
#else
	// All output timers use the same timer clock, configured here. The
	// base clock is set to the system clock (48 MHz) with a divisor.
	lngTimerClockBaseIndex = LJ_tc48MHZ_DIV;
#endif // OLD_HARDWARE_VERSION_U3

	// All the timers will have their duty cycles coded on 16 bits and
	// the duty cycles will correspond to a pulse width of 1.5 ms.
	for (i = 0; i < MAX_NB_TIMERS_U3; i++)
	{
		alngTimerModes[i] = LJ_tmPWM16;
		(*phU3)->adblTimerValues[i] = PWM_DUTY_CYCLE_DEFAULT_U3;
	}

	if (eTCConfig((*phU3)->hDev, // Handle to the Labjack.
		alngEnableTimers, // Specify how many timers are enabled.
		alngEnableCounters,  // Specify how many counters are enabled.
		4, // Indicate that the first timer will be on FIO4 (offset=4).
		lngTimerClockBaseIndex, // Timer base clock.
		TIMER_CLOCK_DIVISOR_U3, // The PWM frequency, which depends on TIMER_CLOCK_DIVISOR_U3 is set to 48.83 Hz. 
		// See the macro PWM_FREQUENCY_U3.
		alngTimerModes, // Timers modes (PWM with duty cycle coded on 16 bit).
		(*phU3)->adblTimerValues, // Timers duty cycles. 
		0, 0 // Reserved.
		) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U3CFG(("OpenU3 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot configure timers. ", 
			szDevice));
		ResetU3USB((*phU3)->hDev, FALSE);
		free(*phU3);
		return EXIT_FAILURE;
	}
/*
	Setting DAC1 to 0 V using eDAC() cause problems when reading the temperature...

	// Set DAC0 and DAC1 to 0 V.
	if ((eDAC((*phU3)->hDev, 
#ifndef _WIN32
		&(*phU3)->caliInfo, 
		1, 
#endif // _WIN32
		DAC0, 0, 0, 0, 0) != LJE_NOERROR) || 
		(eDAC((*phU3)->hDev, 
#ifndef _WIN32
		&(*phU3)->caliInfo, 
		1, 
#endif // _WIN32
		DAC1, 0, 0, 0, 0) != LJE_NOERROR))
	{
		PRINT_DEBUG_ERROR_U3CFG(("OpenU3 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error setting a voltage. ", 
			szDevice));
		ResetU3USB((*phU3)->hDev, FALSE);
		free(*phU3);
		return EXIT_FAILURE;
	}
*/
	// The configuration of AIN0 and AIN1 as analog inputs
	// will be done when calling GetVoltageU3().

	return EXIT_SUCCESS;
}

/*
Change the frequency of all PWM of a Labjack U3.

HU3 hU3 : (IN) Identifier of the Labjack U3.
double freq : (IN) Desired frequency in Hz. The resulting frequency will be 
very approximative.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetAllPWMFreqU3(HU3 hU3, double freq)
{
	long alngEnableTimers[MAX_NB_TIMERS_U3]; // Specify how many timers are enabled.
	long alngEnableCounters[MAX_NB_TIMERS_U3]; // Specify how many counters are enabled.
	long alngTimerModes[MAX_NB_TIMERS_U3]; // Timers modes.
	long lngTimerClockBaseIndex = 0; // Timer base clock.
	long lngTimerClockDivisor = (long)((double)FREQUENCY_U3/(65536.0*freq));
	int i = 0;

	// Indicate that all the timers of the Labjack U3 are enabled, and the 2 
	// counters are disabled.
	for (i = 0; i < MAX_NB_TIMERS_U3; i++)
	{
		alngEnableTimers[i] = 1;
	}
	alngEnableCounters[0] = 0;
	alngEnableCounters[1] = 0;

#ifdef OLD_HARDWARE_VERSION_U3
	// All output timers use the same timer clock, configured here. The
	// base clock is set to the system clock (24 MHz) with a divisor.
	lngTimerClockBaseIndex = LJ_tc24MHZ_DIV;
#else
	// All output timers use the same timer clock, configured here. The
	// base clock is set to the system clock (48 MHz) with a divisor.
	lngTimerClockBaseIndex = LJ_tc48MHZ_DIV;
#endif // OLD_HARDWARE_VERSION_U3

	// All the timers will have their duty cycles coded on 16 bits.
	for (i = 0; i < MAX_NB_TIMERS_U3; i++)
	{
		alngTimerModes[i] = LJ_tmPWM16;
		// Keep adblTimerValues.
	}

	if (eTCConfig(hU3->hDev, // Handle to the Labjack.
		alngEnableTimers, // Specify how many timers are enabled.
		alngEnableCounters,  // Specify how many counters are enabled.
		4, // Indicate that the first timer will be on FIO4 (offset=4).
		lngTimerClockBaseIndex, // Timer base clock.
		lngTimerClockDivisor, // The PWM frequency depends on lngTimerClockDivisor. 
		// See the macro PWM_FREQUENCY_U3.
		alngTimerModes, // Timers modes (PWM with duty cycle coded on 16 bit).
		hU3->adblTimerValues, // Timers duty cycles. 
		0, 0 // Reserved.
		) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U3CFG(("SetAllPWMFreqU3 error (%s) : %s"
			"(hU3=%#x, freq=%f)\n", 
			strtime_m(), 
			"Error setting the timer clock divisor. ", 
			hU3, freq));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Modify all PWM pulse width of a Labjack U3.

HU3 hU3 : (IN) Identifier of the Labjack U3.
double pulseWidths[MAX_NB_TIMERS_U3] : (IN) Desired pulse width in ms. For example, if a
servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
or 2 in the other side.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetAllPWMPulseWidthU3(HU3 hU3, double pulseWidths[MAX_NB_TIMERS_U3])
{
	long alngReadTimers[MAX_NB_TIMERS_U3]; 
	long alngUpdateResetTimers[MAX_NB_TIMERS_U3]; 
	long alngReadCounters[MAX_NB_TIMERS_U3]; 
	long alngResetCounters[MAX_NB_TIMERS_U3];
	double adblCounterValues[MAX_NB_TIMERS_U3];
	int i = 0;

	for (i = 0; i < MAX_NB_TIMERS_U3; i++)
	{
		alngReadTimers[i] = 0;
		alngUpdateResetTimers[i] = 1; // Indicate to update the timer.
		alngReadCounters[i] = 0;
		alngResetCounters[i] = 0;
		hU3->adblTimerValues[i] = PULSE_WIDTH2TIMER_DUTY_CYCLE_U3(pulseWidths[i]); // New duty cycle.
		adblCounterValues[i] = 0;
	}

	if (eTCValues(hU3->hDev, alngReadTimers, alngUpdateResetTimers, alngReadCounters, 
		alngResetCounters, hU3->adblTimerValues, adblCounterValues, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U3CFG(("SetAllPWMPulseWidthU3 error (%s) : %s"
			"(hU3=%#x, pulseWidths=%#x)\n", 
			strtime_m(), 
			"Error setting a PWM pulse width. ", 
			hU3, pulseWidths));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Modify a PWM pulse width of a Labjack U3.

HU3 hU3 : (IN) Identifier of the Labjack U3.
double pulseWidth : (IN) Desired pulse width in ms. For example, if a
servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
or 2 in the other side.
long pin : (IN) Should be set to FIO4 or FIO5.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetPWMPulseWidthU3(HU3 hU3, double pulseWidth, long pin)
{
	long alngReadTimers[MAX_NB_TIMERS_U3]; 
	long alngUpdateResetTimers[MAX_NB_TIMERS_U3]; 
	long alngReadCounters[MAX_NB_TIMERS_U3]; 
	long alngResetCounters[MAX_NB_TIMERS_U3];
	double adblCounterValues[MAX_NB_TIMERS_U3];
	int i = 0;

	for (i = 0; i < MAX_NB_TIMERS_U3; i++)
	{
		if (i == pin)
		{
			alngReadTimers[i] = 0;
			alngUpdateResetTimers[i] = 1; // Indicate to update the timer.
			alngReadCounters[i] = 0;
			alngResetCounters[i] = 0;
			hU3->adblTimerValues[i] = PULSE_WIDTH2TIMER_DUTY_CYCLE_U3(pulseWidth); // New duty cycle.
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

	if (eTCValues(hU3->hDev, alngReadTimers, alngUpdateResetTimers, alngReadCounters, 
		alngResetCounters, hU3->adblTimerValues, adblCounterValues, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U3CFG(("SetPWMPulseWidthU3 error (%s) : %s"
			"(hU3=%#x, pulseWidth=%f, pin=%d)\n", 
			strtime_m(), 
			"Error setting a PWM pulse width. ", 
			hU3, pulseWidth, (int)pin));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Output a voltage from a Labjack U3.

HU3 hU3 : (IN) Identifier of the Labjack U3.
double voltage : (IN) Desired voltage in V. 
long pin : (IN) Should be set to DAC0 or DAC1 (DAC1 cause sometimes some problems).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetVoltageU3(HU3 hU3, double voltage, long pin)
{
	if (eDAC(hU3->hDev, 
#ifndef _WIN32
		&hU3->caliInfo, 
		1, 
#endif // _WIN32
		pin, voltage, 0, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U3CFG(("SetVoltageU3 error (%s) : %s"
			"(hU3=%#x, voltage=%f, pin=%d)\n", 
			strtime_m(), 
			"Error setting a voltage. ", 
			hU3, voltage, (int)pin));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Read a voltage from a Labjack U3.

HU3 hU3 : (IN) Identifier of the Labjack U3.
double* pVoltage : (INOUT) Valid pointer receiving the voltage in V. For 
example, declare voltage as a double and pass &voltage to the function.
long pin : (IN) Should be set to AIN0 or AIN1.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetVoltageU3(HU3 hU3, double* pVoltage, long pin)
{
#ifndef _WIN32
	long DAC1Enable = 0;
#endif // _WIN32

	if (eAIN(hU3->hDev, 
#ifndef _WIN32
		&hU3->caliInfo, 
		1, 
		&DAC1Enable, 
#endif // _WIN32
		pin, NEGATIVE_CHANNEL_FOR_SINGLE_ENDED_U3, pVoltage, 0, 0, 0, 0, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U3CFG(("GetVoltageU3 error (%s) : %s"
			"(hU3=%#x, pin=%d)\n", 
			strtime_m(), 
			"Error reading a voltage. ", 
			hU3, (int)pin));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Get the temperature in a Labjack U3.

HU3 hU3 : (IN) Identifier of the Labjack U3.
double* pTemperature : (INOUT) Valid pointer receiving the temperature in 
Celsius degrees. For example, declare temperature as a double and pass  
&temperature to the function.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetTemperatureU3(HU3 hU3, double* pTemperature)
{
#ifndef _WIN32
	long DAC1Enable = 0;
#endif // _WIN32

	if (eAIN(hU3->hDev, 
#ifndef _WIN32
		&hU3->caliInfo, 
		1, 
		&DAC1Enable, 
#endif // _WIN32
		TEMPERATURE_SENSOR_CHANNEL_U3, NEGATIVE_CHANNEL_FOR_SINGLE_ENDED_U3, pTemperature, 0, 0, 0, 0, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U3CFG(("GetTemperatureU3 error (%s) : %s"
			"(hU3=%#x)\n", 
			strtime_m(), 
			"Error getting the temperature. ", 
			hU3));
		return EXIT_FAILURE;
	}

	// Converts from Kelvin to Celsius.
	*pTemperature = KELVIN2CELSIUS(*pTemperature);

	return EXIT_SUCCESS;
}

/*
Software reset of a Labjack U3.

HU3 hU3 : (IN) Identifier of the Labjack U3.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ResetU3(HU3 hU3)
{
	if (ResetU3USB(hU3->hDev, FALSE) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U3CFG(("ResetU3 error (%s) : %s"
			"(hU3=%#x)\n", 
			strtime_m(), 
			"Error resetting the device. ", 
			hU3));
		return EXIT_FAILURE;
	}

	mSleep(4000);

	return EXIT_SUCCESS;
}

/*
Close a Labjack U3. 
Note that on Windows the device is really closed when the LabJackUD DLL
is unloaded or the application exits.

HU3* phU3 : (INOUT) Valid pointer to the identifier of the Labjack U3.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CloseU3(HU3* phU3)
{
	long alngEnableTimers[MAX_NB_TIMERS_U3]; // Specify how many timers are enabled.
	long alngEnableCounters[MAX_NB_TIMERS_U3]; // Specify how many counters are enabled.
	long alngTimerModes[MAX_NB_TIMERS_U3]; // Timers modes.
	long lngTimerClockBaseIndex = 0; // Timer base clock.
	int i = 0;
	BOOL bError = FALSE;

	// Disable all timers.
	for (i = 0; i < MAX_NB_TIMERS_U3; i++)
	{
		alngEnableTimers[i] = 0;
	}
	alngEnableCounters[0] = 0;
	alngEnableCounters[1] = 0;

#ifdef OLD_HARDWARE_VERSION_U3
	// All output timers use the same timer clock, configured here. The
	// base clock is set to the system clock (24 MHz) with a divisor.
	lngTimerClockBaseIndex = LJ_tc24MHZ_DIV;
#else
	// All output timers use the same timer clock, configured here. The
	// base clock is set to the system clock (48 MHz) with a divisor.
	lngTimerClockBaseIndex = LJ_tc48MHZ_DIV;
#endif // OLD_HARDWARE_VERSION_U3

	// All the timers will have their duty cycles coded on 16 bits and
	// the duty cycles will correspond to a pulse width of 1.5 ms.
	for (i = 0; i < MAX_NB_TIMERS_U3; i++)
	{
		alngTimerModes[i] = LJ_tmPWM16;
		(*phU3)->adblTimerValues[i] = PWM_DUTY_CYCLE_DEFAULT_U3;
	}

	if (eTCConfig((*phU3)->hDev, // Handle to the Labjack.
		alngEnableTimers, // Specify how many timers are enabled.
		alngEnableCounters,  // Specify how many counters are enabled.
		4, // Indicate that the first timer will be on FIO4 (offset=4).
		lngTimerClockBaseIndex, // Timer base clock.
		TIMER_CLOCK_DIVISOR_U3, // The PWM frequency, which depends on TIMER_CLOCK_DIVISOR_U3 is set to 48.83 Hz. 
		// See the macro PWM_FREQUENCY_U3.
		alngTimerModes, // Timers modes (PWM with duty cycle coded on 16 bit).
		(*phU3)->adblTimerValues, // Timers duty cycles. 
		0, 0 // Reserved.
		) != LJE_NOERROR)
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_U3CFG(("CloseU3 error (%s) : %s"
			"(*phU3=%#x)\n", 
			strtime_m(), 
			"Cannot disable timers. ", 
			*phU3));
	}

	// Set DAC0 and DAC1 to 0 V.
	if ((eDAC((*phU3)->hDev, 
#ifndef _WIN32
		&(*phU3)->caliInfo, 
		1, 
#endif // _WIN32
		DAC0, 0, 0, 0, 0) != LJE_NOERROR) || 
		(eDAC((*phU3)->hDev, 
#ifndef _WIN32
		&(*phU3)->caliInfo, 
		1, 
#endif // _WIN32
		DAC1, 0, 0, 0, 0) != LJE_NOERROR))
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_U3CFG(("CloseU3 error (%s) : %s"
			"(*phU3=%#x)\n", 
			strtime_m(), 
			"Error setting a voltage. ", 
			*phU3));
	}

#ifdef _WIN32
	// The device is closed when the LabJackUD DLL
	// is unloaded or the application exits.
#else
	if (CloseU3USB((*phU3)->hDev) != LJE_NOERROR)
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_U3CFG(("CloseU3 error (%s) : %s"
			"(*phU3=%#x)\n", 
			strtime_m(), 
			"Error closing the device. ", 
			*phU3));
	}
#endif // _WIN32

	free(*phU3);

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // U3CFG_H
