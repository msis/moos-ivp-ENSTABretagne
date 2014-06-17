/***************************************************************************************************************:')

UE9CfgUDw32.c

Labjack UE9 handling. It uses the high level DLL provided with the Windows driver. Therefore, it works only under 
Windows.

Fabrice Le Bars
Some functions and parts of functions are from the examples provided by www.labjack.com

Created : 2009-03-28

Version status : Not finished

***************************************************************************************************************:)*/

#include "UE9CfgUDw32.h"

#define MAX_NB_TIMERS_UE9 6

/*
Error handling macro that is called after every Labjack function call.
If there was an error, a message is displayed and the calling function
will return EXIT_FAILURE.

LJ_ERROR lngErrorcode : (IN) Error code returned by a Labjack function.
*/
#define ErrorHandlerUE9(lngErrorcode) \
	if (lngErrorcode != LJE_NOERROR)\
{\
	char szErrMsg[256];\
	ErrorToString(lngErrorcode, szErrMsg);\
	printf("%s\n", szErrMsg);\
	return EXIT_FAILURE;\
}

/*
Open a Labjack UE9 and set 6 PWM outputs from FIO0 to FIO5, 2 analog inputs on
FIO6 and FIO7 and 2 analog outputs on DAC0 and DAC1.

HUE9* phUE9 : (INOUT) Pointer that will receive an identifier representing the
device opened. For example, declare hUE9 as a HUE9 and pass &hUE9 to the function.
char* szDevice : (IN) Path to the device. For example, pass "0" to the function.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int OpenUE9(HUE9* phUE9, char* szDevice)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.
	long aEnableTimers[MAX_NB_TIMERS_UE9]; // Specify how many timers are enabled.
	long aEnableCounters[MAX_NB_TIMERS_UE9]; // Specify how many counters are enabled.
	long aTimerModes[MAX_NB_TIMERS_UE9]; // Timers modes.
	double aTimerValues[MAX_NB_TIMERS_UE9]; // Timers duty cycles.
	long TimerClockBaseIndex = 0; // Timer base clock.
	int i = 0;

	// Open a Labjack UE9.
	lngErrorcode = OpenLabJack(LJ_dtUE9, LJ_ctUSB, szDevice, 0, phUE9);
	ErrorHandlerUE9(lngErrorcode);

	// Set the pins to the factory default condition.
	lngErrorcode = ResetLabJack(*phUE9); 
	ErrorHandlerUE9(lngErrorcode);

	// Configure FIO0, 1, 2, 3 as PWM outputs with a frequency next to 50 Hz and 
	// a pulse width of 1.5 ms (neutral position) in order to use them to control
	// servomotors.

	// Indicate that all the timers of the Labjack UE9 are enabled, and the 2 
	// counters are disabled.
	for (i = 0; i < MAX_NB_TIMERS_UE9; i++)
	{
		aEnableTimers[i] = 1;
	}
	aEnableCounters[0] = 0;
	aEnableCounters[1] = 0;

	// All output timers use the same timer clock, configured here. The
	// base clock is set to 48MHZ_DIV, meaning that the clock divisor
	// is supported and Counter0 is not available.
	TimerClockBaseIndex = LJ_tc48MHZ_DIV;

	// All the timers will have their duty cycles coded on 16 bits and
	// the duty cycles will correspond to a pulse width of 1.5 ms.
	for (i = 0; i < MAX_NB_TIMERS_UE9; i++)
	{
		aTimerModes[i] = LJ_tmPWM16;
		aTimerValues[i] = PWM_DUTY_CYCLE_DEFAULT_UE9;
	}

	lngErrorcode = eTCConfig(*phUE9, // Handle to the Labjack.
		aEnableTimers, // Specify how many timers are enabled.
		aEnableCounters,  // Specify how many counters are enabled.
		0, // Indicate that the first timer will be on FIO0 (offset=0).
		TimerClockBaseIndex, // Timer base clock.
		TIMER_CLOCK_DIVISOR_UE9, // The PWM frequency, which depends on TIMER_CLOCK_DIVISOR_UE9 is set to 45,78 Hz. 
		// See the macro PWM_FREQUENCY_UE9.
		aTimerModes, // Timers modes.
		aTimerValues, // Timers duty cycles. 
		0, 0 // Reserved.
		);
	ErrorHandlerUE9(lngErrorcode);

	// Configure FIO6 and FIO7 as analog inputs.
	lngErrorcode = ePut(*phUE9, LJ_ioPUT_ANALOG_ENABLE_BIT, FIO6, 1, 0);
	ErrorHandlerUE9(lngErrorcode);
	lngErrorcode = ePut(*phUE9, LJ_ioPUT_ANALOG_ENABLE_BIT, FIO7, 1, 0);
	ErrorHandlerUE9(lngErrorcode);

	// Configure DAC0 and DAC1 to output 0 V.
	lngErrorcode = eDAC(*phUE9, DAC0, 0, 0, 0, 0);
	ErrorHandlerUE9(lngErrorcode);
	lngErrorcode = eDAC(*phUE9, DAC1, 0, 0, 0, 0);
	ErrorHandlerUE9(lngErrorcode);

	return EXIT_SUCCESS;
}

/*
Change the frequency of a PWM.

HUE9 hUE9 : (IN) Identifier representing the device.
double freq : (IN) Desired frequency in Hz. The resulting frequency will be 
 very approximative.
long pin : (IN) Should be set to FIO0, 1, 2, 3, 4 or FIO5.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int SetPWMFreqUE9(HUE9 hUE9, double freq, long pin)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.
	int timer_clock_divisor = (int)((double)FREQUENCY_UE9/(65536.0*freq));

	// Change the timer clock divisor at the specified pin.
	lngErrorcode = ePut(hUE9, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_DIVISOR, timer_clock_divisor, pin);
	ErrorHandlerUE9(lngErrorcode);

	return EXIT_SUCCESS;
}

/*
Modify a PWM pulse width.

HUE9 hUE9 : (IN) Identifier representing the device.
double pulseWidth : (IN) Desired pulse width in ms. For example, if a
 servomotor is connected, pass 1.5 to put it at a neutral state, 1 in one side
 or 2 in the other side.
long pin : (IN) Should be set to FIO0, 1, 2, 3, 4 or FIO5.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int SetPWMPulseWidthUE9(HUE9 hUE9, double pulseWidth, long pin)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.

	lngErrorcode = ePut(hUE9, LJ_ioPUT_TIMER_VALUE, pin, PULSE_WIDTH2TIMER_DUTY_CYCLE_UE9(pulseWidth), 0);
	ErrorHandlerUE9(lngErrorcode);

	return EXIT_SUCCESS;
}

/*
Output a voltage.

HUE9 hUE9 : (IN) Identifier representing the device.
double voltage : (IN) Desired voltage in V. 
long pin : (IN) Should be set to DAC0 or DAC1.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int SetVoltageUE9(HUE9 hUE9, double voltage, long pin)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.

	lngErrorcode = ePut(hUE9, LJ_ioPUT_DAC, pin, voltage, 0);
	ErrorHandlerUE9(lngErrorcode);

	return EXIT_SUCCESS;
}

/*
Read a voltage.

HUE9 hUE9 : (IN) Identifier representing the device.
double* pVoltage : (INOUT) Pointer receiving the voltage in V. For example,
 declare voltage as a double and pass &voltage to the function.
long pin : (IN) Should be set to FIO6 or FIO7.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int GetVoltageUE9(HUE9 hUE9, double* pVoltage, long pin)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.

	lngErrorcode = eGet(hUE9, LJ_ioGET_AIN, pin, pVoltage, 0);
	ErrorHandlerUE9(lngErrorcode);

	return EXIT_SUCCESS;
}

/*
Get the temperature in the device.

HUE9 hUE9 : (IN) Identifier representing the device.
double* pTemperature : (INOUT) Pointer receiving the temperature in Kelvin. For
 example, declare temperature as a double and pass &temperature to the function.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int GetTemperatureUE9(HUE9 hUE9, double* pTemperature)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.

	lngErrorcode = eGet(hUE9, LJ_ioGET_AIN, 30, pTemperature, 0);
	ErrorHandlerUE9(lngErrorcode);

	return EXIT_SUCCESS;
}
/*
int ConfigI2CUE9(int address, BOOL bResetAtStart, int speed)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.

	//Configure the I2C communication.
	lngErrorcode = AddRequest(hUE9, LJ_ioPUT_CONFIG, LJ_chI2C_ADDRESS, address, 0, 0);
	ErrorHandlerUE9(lngErrorcode);

	//SCL is FIO8
	lngErrorcode = AddRequest(hUE9, LJ_ioPUT_CONFIG, LJ_chI2C_SCL_PIN_NUM, 8, 0, 0);
	ErrorHandlerUE9(lngErrorcode);

	//SDA is FIO9
	lngErrorcode = AddRequest(hUE9, LJ_ioPUT_CONFIG, LJ_chI2C_SDA_PIN_NUM, 9, 0, 0);
	ErrorHandlerUE9(lngErrorcode);

	//See description of low-level I2C function.
	lngErrorcode = AddRequest(hUE9, LJ_ioPUT_CONFIG, LJ_chI2C_OPTIONS, bResetAtStart, 0, 0);
	ErrorHandlerUE9(lngErrorcode);

	//See description of low-level I2C function. 0 is max speed of about 130 kHz.
	lngErrorcode = AddRequest(hUE9, LJ_ioPUT_CONFIG, LJ_chI2C_SPEED_ADJUST, 0, 0, 0);
	ErrorHandlerUE9(lngErrorcode);
	
	//Execute the requests on a single LabJack.
	lngErrorcode = GoOne(hUE9);
	ErrorHandlerUE9(lngErrorcode);

	//Get all the results just to check for errors.
	lngErrorcode = GetFirstResult(hUE9, &lngIOType, &lngChannel, &dblValue, 0, 0);
	ErrorHandlerUE9(lngErrorcode);
	lngGetNextIteration=0;	//Used by the error handling function.
	while (lngErrorcode < LJE_MIN_GROUP_ERROR)
	{
		lngErrorcode = GetNextResult(hUE9, &lngIOType, &lngChannel, &dblValue, 0, 0);
		if (lngErrorcode != LJE_NO_MORE_DATA_AVAILABLE)
		{
			ErrorHandler(lngErrorcode, __LINE__, lngGetNextIteration);
		}

		lngGetNextIteration++;
	}

	return EXIT_SUCCESS;
}

int WriteI2CUE9(char* buf, int nbBytes)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.

	lngErrorcode = eGet(hUE9, LJ_ioGET_AIN, 30, pTemperature, 0);
	ErrorHandlerUE9(lngErrorcode);

	return EXIT_SUCCESS;
}

int ReadI2CUE9(char** pBuf, int nbBytes)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.

	lngErrorcode = eGet(hUE9, LJ_ioGET_AIN, 30, pTemperature, 0);
	ErrorHandlerUE9(lngErrorcode);

	return EXIT_SUCCESS;
}
*/
/*
Close the device. Note that the device is really closed when the LabJackUD DLL
is unloaded or the application exits.

HUE9* phUE9 : (INOUT) Pointer to the identifier representing the device.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int CloseUE9(HUE9* phUE9)
{
	LJ_ERROR lngErrorcode; // Used to store the error code.

	// Set the pins to the factory default condition.
	lngErrorcode = ResetLabJack(*phUE9); 
	ErrorHandlerUE9(lngErrorcode);

	return EXIT_SUCCESS;
}
