/***************************************************************************************************************:')

U6Cfg.h

Labjack U6 handling.

Fabrice Le Bars

Created : 2012-04-03

Version status : Not finished

***************************************************************************************************************:)*/

#ifndef U6CFG_H
#define U6CFG_H

#include "OSMisc.h" 
#include "U6Core.h" 

/*
Debug macros specific to U6Cfg.
*/
#ifdef _DEBUG_MESSAGES_U6UTILS
#	define _DEBUG_MESSAGES_U6CFG
#endif // _DEBUG_MESSAGES_U6UTILS

#ifdef _DEBUG_WARNINGS_U6UTILS
#	define _DEBUG_WARNINGS_U6CFG
#endif // _DEBUG_WARNINGS_U6UTILS

#ifdef _DEBUG_ERRORS_U6UTILS
#	define _DEBUG_ERRORS_U6CFG
#endif // _DEBUG_ERRORS_U6UTILS

#ifdef _DEBUG_MESSAGES_U6CFG
#	define PRINT_DEBUG_MESSAGE_U6CFG(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_U6CFG(params)
#endif // _DEBUG_MESSAGES_U6CFG

#ifdef _DEBUG_WARNINGS_U6CFG
#	define PRINT_DEBUG_WARNING_U6CFG(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_U6CFG(params)
#endif // _DEBUG_WARNINGS_U6CFG

#ifdef _DEBUG_ERRORS_U6CFG
#	define PRINT_DEBUG_ERROR_U6CFG(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_U6CFG(params)
#endif // _DEBUG_ERRORS_U6CFG

struct U6
{
#ifdef _WIN32
	LJ_HANDLE hDev;
#else
	HANDLE hDev;
	u6CalibrationInfo caliInfo;
#endif // _WIN32
	double adblTimerValues[MAX_NB_TIMERS_U6]; // Timers duty cycles.
};
typedef struct U6* HU6;

#define INVALID_HU6_VALUE NULL

/*
Open a Labjack U6 and set 4 PWM outputs from FIO0 to FIO3, 2 analog inputs on
AIN0 and AIN1 and 2 analog outputs on DAC0 and DAC1.

HU6* phU6 : (INOUT) Valid pointer that will receive an identifier of the
U6 opened.
char* szDevice : (IN) Path to the device. For example, pass "1" to the function
to open the first.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int OpenU6(HU6* phU6, char* szDevice)
{
	long alngEnableTimers[MAX_NB_TIMERS_U6]; // Specify how many timers are enabled.
	long alngEnableCounters[MAX_NB_TIMERS_U6]; // Specify how many counters are enabled.
	long alngTimerModes[MAX_NB_TIMERS_U6]; // Timers modes.
	long lngTimerClockBaseIndex = 0; // Timer base clock.
	int i = 0;

	*phU6 = (HU6)calloc(1, sizeof(struct U6));

	if (*phU6 == NULL)
	{
		PRINT_DEBUG_ERROR_U6CFG(("OpenU6 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Out of memory. ", 
			szDevice));
		return EXIT_FAILURE;
	}

	// Open a Labjack U6.
#ifdef _WIN32
	if (OpenLabJack(LJ_dtU6, LJ_ctUSB, szDevice, 0, &(*phU6)->hDev) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U6CFG(("OpenU6 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot open the device. ", 
			szDevice));
		free(*phU6);
		return EXIT_FAILURE;
	}
#else
	if (OpenU6USB(szDevice, &(*phU6)->hDev, &(*phU6)->caliInfo) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U6CFG(("OpenU6 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot open the device. ", 
			szDevice));
		free(*phU6);
		return EXIT_FAILURE;
	}
#endif // _WIN32

	// Problem under Linux, due to getCalibration() in OpenU6USB().
//	ResetU6USB((*phU6)->hDev, FALSE);

	// Configure FIO0, FIO1, FIO2 and FIO3 as PWM outputs with a frequency next to 50 Hz and 
	// a pulse width of 1.5 ms (neutral position) in order to use them to control
	// servomotors.

	// Indicate that all the timers of the Labjack U6 are enabled, and the 2 
	// counters are disabled.
	for (i = 0; i < MAX_NB_TIMERS_U6; i++)
	{
		alngEnableTimers[i] = 1;
	}
	alngEnableCounters[0] = 0;
	alngEnableCounters[1] = 0;

	// All output timers use the same timer clock, configured here. The
	// base clock is set to the system clock (48 MHz) with a divisor.
	lngTimerClockBaseIndex = LJ_tc48MHZ_DIV;

	// All the timers will have their duty cycles coded on 16 bits and
	// the duty cycles will correspond to a pulse width of 1.5 ms.
	for (i = 0; i < MAX_NB_TIMERS_U6; i++)
	{
		alngTimerModes[i] = LJ_tmPWM16;
		(*phU6)->adblTimerValues[i] = PWM_DUTY_CYCLE_DEFAULT_U6;
	}

	if (eTCConfig((*phU6)->hDev, // Handle to the Labjack.
		alngEnableTimers, // Specify how many timers are enabled.
		alngEnableCounters,  // Specify how many counters are enabled.
		0, // Indicate that the first timer will be on FIO0 (offset=0).
		lngTimerClockBaseIndex, // Timer base clock.
		TIMER_CLOCK_DIVISOR_U6, // The PWM frequency, which depends on TIMER_CLOCK_DIVISOR_U6 is set to 48.83 Hz. 
		// See the macro PWM_FREQUENCY_U6.
		alngTimerModes, // Timers modes (PWM with duty cycle coded on 16 bit).
		(*phU6)->adblTimerValues, // Timers duty cycles. 
		0, 0 // Reserved.
		) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U6CFG(("OpenU6 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Cannot configure timers. ", 
			szDevice));
		ResetU6USB((*phU6)->hDev, FALSE);
		free(*phU6);
		return EXIT_FAILURE;
	}

	// Set DAC0 and DAC1 to 0 V.
	if ((eDAC((*phU6)->hDev, 
#ifndef _WIN32
		&(*phU6)->caliInfo, 
		1, 
#endif // _WIN32
		DAC0, 0, 0, 0, 0) != LJE_NOERROR) || 
		(eDAC((*phU6)->hDev, 
#ifndef _WIN32
		&(*phU6)->caliInfo, 
		1, 
#endif // _WIN32
		DAC1, 0, 0, 0, 0) != LJE_NOERROR))
	{
		PRINT_DEBUG_ERROR_U6CFG(("OpenU6 error (%s) : %s"
			"(szDevice=%s)\n", 
			strtime_m(), 
			"Error setting a voltage. ", 
			szDevice));
		ResetU6USB((*phU6)->hDev, FALSE);
		free(*phU6);
		return EXIT_FAILURE;
	}

	// The configuration of AIN0 and AIN1 as analog inputs
	// will be done when calling GetVoltageU6().

	return EXIT_SUCCESS;
}

/*
Change the frequency of all PWM of a Labjack U6.

HU6 hU6 : (IN) Identifier of the Labjack U6.
double freq : (IN) Desired frequency in Hz. The resulting frequency will be 
very approximative.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetAllPWMFreqU6(HU6 hU6, double freq)
{
	long alngEnableTimers[MAX_NB_TIMERS_U6]; // Specify how many timers are enabled.
	long alngEnableCounters[MAX_NB_TIMERS_U6]; // Specify how many counters are enabled.
	long alngTimerModes[MAX_NB_TIMERS_U6]; // Timers modes.
	long lngTimerClockBaseIndex = 0; // Timer base clock.
	long lngTimerClockDivisor = (long)((double)FREQUENCY_U6/(65536.0*freq));
	int i = 0;

	// Indicate that all the timers of the Labjack U6 are enabled, and the 2 
	// counters are disabled.
	for (i = 0; i < MAX_NB_TIMERS_U6; i++)
	{
		alngEnableTimers[i] = 1;
	}
	alngEnableCounters[0] = 0;
	alngEnableCounters[1] = 0;

	// All output timers use the same timer clock, configured here. The
	// base clock is set to the system clock (48 MHz) with a divisor.
	lngTimerClockBaseIndex = LJ_tc48MHZ_DIV;

	// All the timers will have their duty cycles coded on 16 bits.
	for (i = 0; i < MAX_NB_TIMERS_U6; i++)
	{
		alngTimerModes[i] = LJ_tmPWM16;
		// Keep adblTimerValues.
	}

	if (eTCConfig(hU6->hDev, // Handle to the Labjack.
		alngEnableTimers, // Specify how many timers are enabled.
		alngEnableCounters,  // Specify how many counters are enabled.
		0, // Indicate that the first timer will be on FIO0 (offset=0).
		lngTimerClockBaseIndex, // Timer base clock.
		lngTimerClockDivisor, // The PWM frequency depends on lngTimerClockDivisor. 
		// See the macro PWM_FREQUENCY_U6.
		alngTimerModes, // Timers modes (PWM with duty cycle coded on 16 bit).
		hU6->adblTimerValues, // Timers duty cycles. 
		0, 0 // Reserved.
		) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U6CFG(("SetAllPWMFreqU6 error (%s) : %s"
			"(hU6=%#x, freq=%f)\n", 
			strtime_m(), 
			"Error setting the timer clock divisor. ", 
			hU6, freq));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Modify all PWM pulse width of a Labjack U6.

HU6 hU6 : (IN) Identifier of the Labjack U6.
double pulseWidths[MAX_NB_TIMERS_U6] : (IN) Desired pulse width in ms. For example, if a
servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
or 2 in the other side.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetAllPWMPulseWidthU6(HU6 hU6, double pulseWidths[MAX_NB_TIMERS_U6])
{
	long alngReadTimers[MAX_NB_TIMERS_U6]; 
	long alngUpdateResetTimers[MAX_NB_TIMERS_U6]; 
	long alngReadCounters[MAX_NB_TIMERS_U6]; 
	long alngResetCounters[MAX_NB_TIMERS_U6];
	double adblCounterValues[MAX_NB_TIMERS_U6];
	int i = 0;

	for (i = 0; i < MAX_NB_TIMERS_U6; i++)
	{
		alngReadTimers[i] = 0;
		alngUpdateResetTimers[i] = 1; // Indicate to update the timer.
		alngReadCounters[i] = 0;
		alngResetCounters[i] = 0;
		hU6->adblTimerValues[i] = PULSE_WIDTH2TIMER_DUTY_CYCLE_U6(pulseWidths[i]); // New duty cycle.
		adblCounterValues[i] = 0;
	}

	if (eTCValues(hU6->hDev, alngReadTimers, alngUpdateResetTimers, alngReadCounters, 
		alngResetCounters, hU6->adblTimerValues, adblCounterValues, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U6CFG(("SetAllPWMPulseWidthU6 error (%s) : %s"
			"(hU6=%#x, pulseWidths=%#x)\n", 
			strtime_m(), 
			"Error setting a PWM pulse width. ", 
			hU6, pulseWidths));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Modify a PWM pulse width of a Labjack U6.

HU6 hU6 : (IN) Identifier of the Labjack U6.
double pulseWidth : (IN) Desired pulse width in ms. For example, if a
servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
or 2 in the other side.
long pin : (IN) Should be set to FIO0, FIO1, FIO2 or FIO3.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetPWMPulseWidthU6(HU6 hU6, double pulseWidth, long pin)
{
	long alngReadTimers[MAX_NB_TIMERS_U6]; 
	long alngUpdateResetTimers[MAX_NB_TIMERS_U6]; 
	long alngReadCounters[MAX_NB_TIMERS_U6]; 
	long alngResetCounters[MAX_NB_TIMERS_U6];
	double adblCounterValues[MAX_NB_TIMERS_U6];
	int i = 0;

	for (i = 0; i < MAX_NB_TIMERS_U6; i++)
	{
		if (i == pin)
		{
			alngReadTimers[i] = 0;
			alngUpdateResetTimers[i] = 1; // Indicate to update the timer.
			alngReadCounters[i] = 0;
			alngResetCounters[i] = 0;
			hU6->adblTimerValues[i] = PULSE_WIDTH2TIMER_DUTY_CYCLE_U6(pulseWidth); // New duty cycle.
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

	if (eTCValues(hU6->hDev, alngReadTimers, alngUpdateResetTimers, alngReadCounters, 
		alngResetCounters, hU6->adblTimerValues, adblCounterValues, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U6CFG(("SetPWMPulseWidthU6 error (%s) : %s"
			"(hU6=%#x, pulseWidth=%f, pin=%d)\n", 
			strtime_m(), 
			"Error setting a PWM pulse width. ", 
			hU6, pulseWidth, (int)pin));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Output a voltage from a Labjack U6.

HU6 hU6 : (IN) Identifier of the Labjack U6.
double voltage : (IN) Desired voltage in V. 
long pin : (IN) Should be set to DAC0 or DAC1.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetVoltageU6(HU6 hU6, double voltage, long pin)
{
	if (eDAC(hU6->hDev, 
#ifndef _WIN32
		&hU6->caliInfo, 
		1, 
#endif // _WIN32
		pin, voltage, 0, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U6CFG(("SetVoltageU6 error (%s) : %s"
			"(hU6=%#x, voltage=%f, pin=%d)\n", 
			strtime_m(), 
			"Error setting a voltage. ", 
			hU6, voltage, (int)pin));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Read a voltage from a Labjack U6.

HU6 hU6 : (IN) Identifier of the Labjack U6.
double* pVoltage : (INOUT) Valid pointer receiving the voltage in V. For 
example, declare voltage as a double and pass &voltage to the function.
long pin : (IN) Should be set to AIN0 or AIN1.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetVoltageU6(HU6 hU6, double* pVoltage, long pin)
{
#ifndef _WIN32
	long DAC1Enable = 0;
#endif // _WIN32

	if (eAIN(hU6->hDev, 
#ifndef _WIN32
		&hU6->caliInfo, 
		1, 
		&DAC1Enable, 
#endif // _WIN32
		pin, NEGATIVE_CHANNEL_FOR_SINGLE_ENDED_U6, pVoltage, 0, 0, 0, 0, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U6CFG(("GetVoltageU6 error (%s) : %s"
			"(hU6=%#x, pin=%d)\n", 
			strtime_m(), 
			"Error reading a voltage. ", 
			hU6, (int)pin));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Get the temperature in a Labjack U6.

HU6 hU6 : (IN) Identifier of the Labjack U6.
double* pTemperature : (INOUT) Valid pointer receiving the temperature in 
Celsius degrees. For example, declare temperature as a double and pass  
&temperature to the function.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetTemperatureU6(HU6 hU6, double* pTemperature)
{
#ifndef _WIN32
	long DAC1Enable = 0;
#endif // _WIN32

	if (eAIN(hU6->hDev, 
#ifndef _WIN32
		&hU6->caliInfo, 
		1, 
		&DAC1Enable, 
#endif // _WIN32
		TEMPERATURE_SENSOR_CHANNEL_U6, NEGATIVE_CHANNEL_FOR_SINGLE_ENDED_U6, pTemperature, 0, 0, 0, 0, 0, 0) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U6CFG(("GetTemperatureU6 error (%s) : %s"
			"(hU6=%#x)\n", 
			strtime_m(), 
			"Error getting the temperature. ", 
			hU6));
		return EXIT_FAILURE;
	}

	// Converts from Kelvin to Celsius.
	*pTemperature = KELVIN2CELSIUS(*pTemperature);

	return EXIT_SUCCESS;
}

/*
Software reset of a Labjack U6.

HU6 hU6 : (IN) Identifier of the Labjack U6.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ResetU6(HU6 hU6)
{
	if (ResetU6USB(hU6->hDev, FALSE) != LJE_NOERROR)
	{
		PRINT_DEBUG_ERROR_U6CFG(("ResetU6 error (%s) : %s"
			"(hU6=%#x)\n", 
			strtime_m(), 
			"Error resetting the device. ", 
			hU6));
		return EXIT_FAILURE;
	}

	mSleep(4000);

	return EXIT_SUCCESS;
}

/*
Close a Labjack U6. 
Note that on Windows the device is really closed when the LabJackUD DLL
is unloaded or the application exits.

HU6* phU6 : (INOUT) Valid pointer to the identifier of the Labjack U6.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CloseU6(HU6* phU6)
{
	long alngEnableTimers[MAX_NB_TIMERS_U6]; // Specify how many timers are enabled.
	long alngEnableCounters[MAX_NB_TIMERS_U6]; // Specify how many counters are enabled.
	long alngTimerModes[MAX_NB_TIMERS_U6]; // Timers modes.
	long lngTimerClockBaseIndex = 0; // Timer base clock.
	int i = 0;
	BOOL bError = FALSE;

	// Disable all timers.
	for (i = 0; i < MAX_NB_TIMERS_U6; i++)
	{
		alngEnableTimers[i] = 0;
	}
	alngEnableCounters[0] = 0;
	alngEnableCounters[1] = 0;

	// All output timers use the same timer clock, configured here. The
	// base clock is set to the system clock (48 MHz) with a divisor.
	lngTimerClockBaseIndex = LJ_tc48MHZ_DIV;

	// All the timers will have their duty cycles coded on 16 bits and
	// the duty cycles will correspond to a pulse width of 1.5 ms.
	for (i = 0; i < MAX_NB_TIMERS_U6; i++)
	{
		alngTimerModes[i] = LJ_tmPWM16;
		(*phU6)->adblTimerValues[i] = PWM_DUTY_CYCLE_DEFAULT_U6;
	}

	if (eTCConfig((*phU6)->hDev, // Handle to the Labjack.
		alngEnableTimers, // Specify how many timers are enabled.
		alngEnableCounters,  // Specify how many counters are enabled.
		0, // Indicate that the first timer will be on FIO0 (offset=0).
		lngTimerClockBaseIndex, // Timer base clock.
		TIMER_CLOCK_DIVISOR_U6, // The PWM frequency, which depends on TIMER_CLOCK_DIVISOR_U6 is set to 48.83 Hz. 
		// See the macro PWM_FREQUENCY_U6.
		alngTimerModes, // Timers modes (PWM with duty cycle coded on 16 bit).
		(*phU6)->adblTimerValues, // Timers duty cycles. 
		0, 0 // Reserved.
		) != LJE_NOERROR)
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_U6CFG(("CloseU6 error (%s) : %s"
			"(*phU6=%#x)\n", 
			strtime_m(), 
			"Cannot disable timers. ", 
			*phU6));
	}

	// Set DAC0 and DAC1 to 0 V.
	if ((eDAC((*phU6)->hDev, 
#ifndef _WIN32
		&(*phU6)->caliInfo, 
		1, 
#endif // _WIN32
		DAC0, 0, 0, 0, 0) != LJE_NOERROR) || 
		(eDAC((*phU6)->hDev, 
#ifndef _WIN32
		&(*phU6)->caliInfo, 
		1, 
#endif // _WIN32
		DAC1, 0, 0, 0, 0) != LJE_NOERROR))
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_U6CFG(("CloseU6 error (%s) : %s"
			"(*phU6=%#x)\n", 
			strtime_m(), 
			"Error setting a voltage. ", 
			*phU6));
	}

#ifdef _WIN32
	// The device is closed when the LabJackUD DLL
	// is unloaded or the application exits.
#else
	if (CloseU6USB((*phU6)->hDev) != LJE_NOERROR)
	{
		bError = TRUE;
		PRINT_DEBUG_ERROR_U6CFG(("CloseU6 error (%s) : %s"
			"(*phU6=%#x)\n", 
			strtime_m(), 
			"Error closing the device. ", 
			*phU6));
	}
#endif // _WIN32

	free(*phU6);

	if (bError)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // U6CFG_H
