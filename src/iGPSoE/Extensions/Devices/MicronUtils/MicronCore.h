/***************************************************************************************************************:')

MicronCore.h

Micron sonar handling.

Fabrice Le Bars

Created : 2012-07-07

Version status: Not finished

***************************************************************************************************************:)*/

#ifndef MICRONCORE_H
#define MICRONCORE_H

#include "OSRS232Port.h"

/*
Debug macros specific to MicronUtils.
*/
#ifdef _DEBUG_MESSAGES
#	define _DEBUG_MESSAGES_MICRONUTILS
#endif // _DEBUG_MESSAGES

#ifdef _DEBUG_WARNINGS
#	define _DEBUG_WARNINGS_MICRONUTILS
#endif // _DEBUG_WARNINGS

#ifdef _DEBUG_ERRORS
#	define _DEBUG_ERRORS_MICRONUTILS
#endif // _DEBUG_ERRORS

#ifdef _DEBUG_MESSAGES_MICRONUTILS
#	define PRINT_DEBUG_MESSAGE_MICRONUTILS(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MICRONUTILS(params)
#endif // _DEBUG_MESSAGES_MICRONUTILS

#ifdef _DEBUG_WARNINGS_MICRONUTILS
#	define PRINT_DEBUG_WARNING_MICRONUTILS(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MICRONUTILS(params)
#endif // _DEBUG_WARNINGS_MICRONUTILS

#ifdef _DEBUG_ERRORS_MICRONUTILS
#	define PRINT_DEBUG_ERROR_MICRONUTILS(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MICRONUTILS(params)
#endif // _DEBUG_ERRORS_MICRONUTILS

/*
Debug macros specific to MicronCore.
*/
#ifdef _DEBUG_MESSAGES_MICRONUTILS
#	define _DEBUG_MESSAGES_MICRONCORE
#endif // _DEBUG_MESSAGES_MICRONUTILS

#ifdef _DEBUG_WARNINGS_MICRONUTILS
#	define _DEBUG_WARNINGS_MICRONCORE
#endif // _DEBUG_WARNINGS_MICRONUTILS

#ifdef _DEBUG_ERRORS_MICRONUTILS
#	define _DEBUG_ERRORS_MICRONCORE
#endif // _DEBUG_ERRORS_MICRONUTILS

#ifdef _DEBUG_MESSAGES_MICRONCORE
#	define PRINT_DEBUG_MESSAGE_MICRONCORE(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_MICRONCORE(params)
#endif // _DEBUG_MESSAGES_MICRONCORE

#ifdef _DEBUG_WARNINGS_MICRONCORE
#	define PRINT_DEBUG_WARNING_MICRONCORE(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_MICRONCORE(params)
#endif // _DEBUG_WARNINGS_MICRONCORE

#ifdef _DEBUG_ERRORS_MICRONCORE
#	define PRINT_DEBUG_ERROR_MICRONCORE(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_MICRONCORE(params)
#endif // _DEBUG_ERRORS_MICRONCORE

// Motor Step Angle Size : scanning motor step angle between pings in 1/16 Gradian units
// Low Resolution = 32 (= 2 Gradians = 1.8 °)
#define LOW_RESOLUTION_MICRON 32
// Medium Resolution = 16 (= 1 Gradian = 0.9°)
#define MEDIUM_RESOLUTION_MICRON 16
// Mgr Resolution = 8 (= 0.5 Gradian = 0.45°)
#define MGR_RESOLUTION_MICRON 8
// Ultimate Resolution = 4 (= 0.25 Gradian = 0.225°)
#define ULTIMATE_RESOLUTION_MICRON 4

#define RESOLUTION2STEP_ANGLE_SIZE_IN_DEGREES(res) ((double)res * 0.05625)

#define RESOLUTION2NUMBER_OF_STEPS(res) (360.0/RESOLUTION2STEP_ANGLE_SIZE_IN_DEGREES(res))

// Parameters of the device
struct _SNRPARAMS	{
	int adc8on; // The head will return 4-bit packed echo data (0..15) representing the amplitude
	// of received echoes in a databin if it is set to 0. Otherwise, it will be in 8 bits (0..255)
	int cont; // Scanning will be restricted to a sector defined by the direction LeftAngleLimit 
	// and RightAngleLimit if it is set to 0. Otherwise, it will be a continuous rotation and 
	// LeftAngleLimit and RightAngleLimit will be ignored
	int invert; // Allow the rotation direction to be reversed if the sonar head is mounted inverted, 
	// i.e. when the sonar transducer boot is pointing downward rather than up (Default = 0 = Sonar 
	// mounted upright, transducer boot pointing up)
	double LeftAngleLimit; // For a sector scan (cont = 0), in degrees
	double RightAngleLimit; // For a sector scan (cont = 0), in degrees
	int VelocityOfSound; // In m/s
	int RangeScale; // In meters
	double StepAngleSize; // In degrees: Ultimate Resolution (0.225°), High Resolution (0.45°), Medium Resolution (0.9°), Low Resolution (1.8°)
	// StepAngleSize = (scanWidth/NSteps)
	int NBins; // Number of bins generated after a ping
	int IGain; // Initial Gain of the receiver (in units 0..210 = 0..+80dB = 0..100%)
};
typedef struct _SNRPARAMS SNRPARAMS;

/*
Return the distance to the first obstacle from a sonar scanline.
The function does not check any argument.

uint8* scanline : (IN) Pointer to the sonar scanline to process..
uint8 threshold : (IN) Threshold that indicates an obstacle (from 0 to 255).
double minDist : (IN) Distance from which we begin the search of the first 
obstacle (in m).
double maxDist : (IN) Distance to which we stop the search of the first 
obstacle (in m).
int NBins : (IN) Number of bins per scanline.
int RangeScale : (IN) Sonar range scale.
double* pDist : (INOUT) Valid pointer that will receive the distance to the 
first obstacle in m. Return -1 if all the bins are under the threshold (i.e. 
no obstacle).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetFirstObstacleDist(uint8* scanline, uint8 threshold, double minDist, double maxDist, 
						 int NBins, int RangeScale, double* pDist)
{
	int minBin = max(0, (int)(minDist*(double)NBins/(double)RangeScale));
	int maxBin = min(NBins, (int)(maxDist*(double)NBins/(double)RangeScale));
	int i = 0;

	i = minBin;
	while ((i < maxBin) && (scanline[i] < threshold))
	{
		i++;
	}

	if (i >= maxBin)
	{ 
		*pDist = -1;
	}
	else	
	{
		*pDist = (double)i*(double)RangeScale/(double)NBins; // Convert in m.
	}

	return EXIT_SUCCESS;
}

/*
Read a message from a Micron.

HANDLE hDev : (IN) Identifier of the device serial port.
uint8* readbuf : (INOUT) Valid pointer that will receive the data read.
UINT nb_bytes : (IN) Number of bytes of the message data.
uint8 cmd_byte : (IN) Message number (see the Micron documentation).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int ReadMsgMicron(HANDLE hDev, uint8* readbuf, UINT nb_bytes, uint8 cmd_byte)
{
	UINT j = 0;
	UINT BytesRead = 0;

	if (nb_bytes <= 10)
	{
		PRINT_DEBUG_ERROR_MICRONCORE(("ReadMsgMicron error (%s) : %s"
			"(hDev=%#x, nb_bytes=%u, cmd_byte=%#x)\n", 
			strtime_m(), 
			"Invalid number of bytes to read. ", 
			hDev, nb_bytes, (int)cmd_byte));
		return EXIT_FAILURE;
	}

	// Read nb_bytes bytes.
	if (ReadAllRS232Port(hDev, readbuf, nb_bytes) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("ReadMsgMicron error (%s) : %s"
			"(hDev=%#x, nb_bytes=%u, cmd_byte=%#x)\n", 
			strtime_m(), 
			"Error reading data from the serial port. ", 
			hDev, nb_bytes, (int)cmd_byte));
		return EXIT_FAILURE;
	}

	BytesRead = nb_bytes;
	while (
		(readbuf[0]			!= 0x40)		||	// Message Header = @
		(readbuf[10]		!= cmd_byte)	||	// Command Byte
		(readbuf[nb_bytes-1]!= 0x0A)			// Message Terminator = Line Feed
		)	
	{
		PRINT_DEBUG_WARNING_MICRONCORE(("ReadMsgMicron warning (%s) : %s"
			"(hDev=%#x, nb_bytes=%u, cmd_byte=%#x)\n", 
			strtime_m(), 
			"Unexpected data from the serial port. ", 
			hDev, nb_bytes, (int)cmd_byte));

		// Maximum number of retries until it is considered as failed.
		// 2 * 22 -> 2 mtAlive message.
		if (BytesRead > nb_bytes + 2 * 22)
		{ 
			PRINT_DEBUG_ERROR_MICRONCORE(("ReadMsgMicron error (%s) : %s"
				"(hDev=%#x, nb_bytes=%u, cmd_byte=%#x)\n", 
				strtime_m(), 
				"Invalid data from the serial port. ", 
				hDev, nb_bytes, (int)cmd_byte));
			return EXIT_FAILURE;	
		}

		// Look if there is a message header somewhere in readbuf.
		j = 1;
		while ((j < nb_bytes) && (readbuf[j] != 0x40))
		{ 
			j++;
		}

		// Make a shift of j bytes in readbuf.
		memmove(readbuf, readbuf+j, nb_bytes-j); 

		// Read j bytes.
		if (ReadAllRS232Port(hDev, readbuf+nb_bytes-j, j) != EXIT_SUCCESS)
		{ 
			PRINT_DEBUG_ERROR_MICRONCORE(("ReadMsgMicron error (%s) : %s"
				"(hDev=%#x, nb_bytes=%u, cmd_byte=%#x)\n", 
				strtime_m(), 
				"Error reading data from the serial port. ", 
				hDev, nb_bytes, (int)cmd_byte));
			return EXIT_FAILURE;
		}

		BytesRead += j;
	}

	return EXIT_SUCCESS;
}

/*
Wait for a mtAlive message from a Micron.

HANDLE hDev : (IN) Identifier of the device serial port.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int WaitForAliveMsgMicron(HANDLE hDev)
{
	uint8 readbuf[22];

	// Wait for a mtAlive message. It should come every 1 second.
	if (ReadMsgMicron(hDev, readbuf, 22, 0x04) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("WaitForAliveMsgMicron error (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"The device is not responding correctly. ", 
			hDev));
		return EXIT_FAILURE;
	}

	PRINT_DEBUG_MESSAGE_MICRONCORE(("mtAlive message readbuf[20]=%#x\n", readbuf[20]));

	return EXIT_SUCCESS;
}

/*
Get the version of a Micron. This message must be sent to be able to use the 
device although we do not try to interprete the data received.

HANDLE hDev : (IN) Identifier of the device serial port.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetVersionMicron(HANDLE hDev)
{
	uint8 readbuf[25];
	uint8 writebuf[14] = {0x40,0x30,0x30,0x30,0x38,0x08,0x00,0xFF,0x02,0x03,0x17,0x80,0x02,0x0A};

	if (WriteAllRS232Port(hDev, writebuf, 14) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("GetVersionMicron error (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Error writing data to the serial port. ", 
			hDev));
		return EXIT_FAILURE;
	}

	// Wait for an mtVersionData message.
	if (ReadMsgMicron(hDev, readbuf, 25, 0x01) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("GetVersionMicron error (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"The device is not responding correctly. ", 
			hDev));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Get various settings of a Micron. This message must sometimes be sent to be 
able to use the device although we do not try to interprete the data received.

HANDLE hDev : (IN) Identifier of the device serial port.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int GetUserMicron(HANDLE hDev)
{
	uint8 readbuf[264];
	uint8 writebuf[14] = {0x40,0x30,0x30,0x30,0x38,0x08,0x00,0xFF,0x02,0x03,0x18,0x80,0x02,0x0A};

	if (WriteAllRS232Port(hDev, writebuf, 14) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("GetUserMicron error (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Error writing data to the serial port. ", 
			hDev));
		return EXIT_FAILURE;
	}

	// Wait for an mtBBUserData message.
	if (ReadMsgMicron(hDev, readbuf, 264, 0x06) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("GetUserMicron error (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"The device is not responding correctly. ", 
			hDev));
		return EXIT_FAILURE;
	}


	
		// Wait for an unknown message.
		if (ReadMsgMicron(hDev, readbuf, 80, 0x3F) != EXIT_SUCCESS)
		{ 
			PRINT_DEBUG_ERROR_MICRONCORE(("SetParamMicron error (%s) : %s"
				"(hDev=%#x)\n", 
				strtime_m(), 
				"The device is not responding correctly. ", 
				hDev));
			return EXIT_FAILURE;
		}

		// Wait for an unknown message.
		if (ReadMsgMicron(hDev, readbuf, 28, 0x39) != EXIT_SUCCESS)
		{ 
			PRINT_DEBUG_ERROR_MICRONCORE(("SetParamMicron error (%s) : %s"
				"(hDev=%#x)\n", 
				strtime_m(), 
				"The device is not responding correctly. ", 
				hDev));
			return EXIT_FAILURE;
		}


	return EXIT_SUCCESS;
}

/*
Reboot a Micron. This should take several seconds. After it is rebooted, 
the mtHeadCommand message should be sent.

HANDLE hDev : (IN) Identifier of the device serial port.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int RebootMicron(HANDLE hDev)
{
	uint8 readbuf[22];
	uint8 writebuf[14] = {0x40,0x30,0x30,0x30,0x38,0x08,0x00,0xFF,0x02,0x03,0x10,0x80,0x02,0x0A};
	CHRONO chrono;
	double duration = 0;

	if (WriteAllRS232Port(hDev, writebuf, 14) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("RebootMicron error (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Error writing data to the serial port. ", 
			hDev));
		return EXIT_FAILURE;
	}

	// Wait for the device to stop.
	mSleep(500);

	if (PurgeRS232Port(hDev) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("RebootMicron error (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Serial port error. ", 
			hDev));
		return EXIT_FAILURE;
	}

	// Wait for the device to reboot.
	mSleep(2000);

	// On power-up of the sonar, the 'HeadInf' byte will change in value to 
	// reflect the different states that the sonar is going through during initialisation
	// i) Power applied to sonar
	// 1st Alive: HeadInf = '5D' (Head is in Re-Centre operation and has No Params). Byte 14 = 80h.
	// 2nd Alive: HeadInf = '4D' (No 'Dir' so Transducer is back at centre)
	// 3rd Alive: HeadInf = '4A' (Transducer is now centred and Not Motoring)
	// ii) 'mtHeadCommand' sent to Sonar
	// 4th Alive: HeadInf = 'CA' (Has been 'Sent Cfg'. Acknowledgement of 'ParamsCmd')
	// 5th Alive: HeadInf = '8A' (Has Params. Parameters have been validated. Ready for 'GetData').
	// Byte 14 now set to 00h

	StartChrono(&chrono);

	readbuf[20] = 0;

	while ((readbuf[20] != 0x4A)&&(readbuf[20] != 0x5A))
	{
		GetTimeElapsedChrono(&chrono, &duration);
		if (duration > 30)
		{
			PRINT_DEBUG_ERROR_MICRONCORE(("RebootMicron error (%s) : %s"
				"(hDev=%#x)\n", 
				strtime_m(), 
				"The operation timed out. ", 
				hDev));
			return EXIT_FAILURE;
		}

		// Wait for an mtAlive message. It should come every 1 second.
		if (ReadMsgMicron(hDev, readbuf, 22, 0x04) != EXIT_SUCCESS)
		{ 
			PRINT_DEBUG_ERROR_MICRONCORE(("RebootMicron error (%s) : %s"
				"(hDev=%#x)\n", 
				strtime_m(), 
				"The device is not responding correctly. ", 
				hDev));
			return EXIT_FAILURE;
		}

		PRINT_DEBUG_MESSAGE_MICRONCORE(("mtAlive message readbuf[20]=%#x\n", readbuf[20]));
	}

	return EXIT_SUCCESS;
}

/*
Set various parameters in a Micron. This message is important to be able to 
interprete the sonar data.

HANDLE hDev : (IN) Identifier of the device serial port.
SNRPARAMS params : (IN) Structure containing various parameters.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetParamMicron(HANDLE hDev, SNRPARAMS params)
{
	struct HdCtrlStruct
	{
		unsigned int adc8on : 1;
		unsigned int cont : 1;
		unsigned int scanright : 1;
		unsigned int invert : 1;
		unsigned int motoff : 1;
		unsigned int txoff : 1;
		unsigned int spare : 1;
		unsigned int chan2 : 1;
		unsigned int raw : 1;
		unsigned int hasmot : 1;
		unsigned int applyoffset : 1;
		unsigned int pingpong : 1;
		unsigned int stareLLim : 1;
		unsigned int ReplyASL : 1;
		unsigned int ReplyThr : 1;
		unsigned int IgnoreSensor : 1;
		unsigned int padding : 16;
	};
	union uHdCtrlUnion
	{
		struct HdCtrlStruct bits;  
		uint8 c[4];
	} HdCtrl;
	CHRONO chrono;
	double duration = 0;
	uint8 readbuf[1024];
	uint8 writebuf[82];
//	uint8 buf = 0;
	uShort word;
	double d = 0;
	int adc8on = params.adc8on; // The head will return 4-bit packed echo data (0..15) representing the amplitude
	// of received echoes in a databin if it is set to 0. Otherwise, it will be in 8 bits (0..255)
	int cont = params.cont; // Scanning will be restricted to a sector defined by the directions LeftAngleLimit 
	// and RightAngleLimit if it is set to 0. Otherwise, it will be a continuous rotation and 
	// LeftAngleLimit and RightAngleLimit will be ignored
	double LeftAngleLimit = params.LeftAngleLimit; // For a sector scan (cont = 0), in degrees
	double RightAngleLimit = params.RightAngleLimit; // For a sector scan (cont = 0), in degrees
	int VelocityOfSound = params.VelocityOfSound; // In m/s
	int RangeScale = params.RangeScale; // In meters
	double StepAngleSize = params.StepAngleSize; // In degrees: Ultimate Resolution (0.225°), Mgr Resolution (0.45°), Medium Resolution (0.9°), Low Resolution (1.8°)
	int NBins = params.NBins; // Number of bins generated after a ping
	int IGain = params.IGain; // Initial Gain of the receiver (in units 0..210 = 0..+80dB = 0..100%)

	writebuf[0] = (uint8)0x40; // Message Header = @
	writebuf[1] = (uint8)0x30; // Hex Length of whole binary packet excluding LF Terminator in ASCII = '0'
	writebuf[2] = (uint8)0x30; // Hex Length of whole binary packet excluding LF Terminator in ASCII = '0'
	writebuf[3] = (uint8)0x33; // Hex Length of whole binary packet excluding LF Terminator in ASCII = '3'
	writebuf[4] = (uint8)0x43; // Hex Length of whole binary packet excluding LF Terminator in ASCII = 'C'
	writebuf[5] = (uint8)0x3C; // Binary Word of above Hex Length (LSB), Hex Length = 60
	writebuf[6] = (uint8)0x00; // Binary Word of above Hex Length (MSB), Hex Length = 60
	writebuf[7] = (uint8)0xFF; // Packet Source Identification (Tx Node number 0 - 255)
	writebuf[8] = (uint8)0x02; // Packet Destination Identification (Rx Node number 0 - 255)
	writebuf[9] = (uint8)0x37; // Byte Count of attached message that follows this byte (Set to 0 (zero) in ‘mtHeadData’
	// reply to indicate Multi-packet mode NOT used by device) = 55
	writebuf[10] = (uint8)0x13; // Command / Reply Message = mtHeadCommand
	writebuf[11] = (uint8)0x80; // Message Sequence, always = 0x80
	writebuf[12] = (uint8)0x02; // Node number, copy of 8


	/////////////////

	writebuf[13] = (uint8)0x01; // If the device is Dual Channel (i.e. SeaKing SONAR) then a 16-byte V3B Gain Parameter block
	// is appended at the end and this byte must be set to 0x1D to indicate this. Else, for Single 
	// channel devices such as SeaPrince and Micron, the V3B block is not appended and this byte
	// is set to 0x01 to indicate this
//	buf = 0x0F;
//	if (!adc8on)	{
//		buf = buf & 0xFE; // adc8on = 0
//	}
//	if (!cont)	{
//		buf = buf & 0xFD; // cont = 0
//	}
//	writebuf[14] = (uint8)buf; // HdCtrl (LSB)
//	writebuf[15] = (uint8)0x23; // HdCtrl (MSB)

    HdCtrl.bits.adc8on = adc8on;
    HdCtrl.bits.cont = cont;
    HdCtrl.bits.scanright = 1;
    HdCtrl.bits.invert = 1;
    HdCtrl.bits.motoff = 0;
    HdCtrl.bits.txoff = 0;
    HdCtrl.bits.spare = 0;
    HdCtrl.bits.chan2 = 0;
    HdCtrl.bits.raw = 1;
    HdCtrl.bits.hasmot = 1;
    HdCtrl.bits.applyoffset = 0;
    HdCtrl.bits.pingpong = 0;
    HdCtrl.bits.stareLLim = 0;
    HdCtrl.bits.ReplyASL = 1;
    HdCtrl.bits.ReplyThr = 0;
    HdCtrl.bits.IgnoreSensor = 0;

	writebuf[14] = (uint8)HdCtrl.c[0]; // HdCtrl (LSB)
	writebuf[15] = (uint8)HdCtrl.c[1]; // HdCtrl (MSB)
	// Bit 0 : adc8on (0=4bit DataBins, 1=8Bit) = 1
	// Bit 1 : cont (0=SectorScan, 1=Continuous) = 1
	// Bit 2 : scanright (ScanDirection 0=Left, 1=Right) = 1
	// Bit 3 : invert (0=Upright, 1=Inverted Orientation) = 1
	// Bit 4 : motoff (0=MotorOn, 1=MotorOff) = 0
	// Bit 5 : txoff (0=Tx on, 1=Tx off. For Test) = 0
	// Bit 6 : spare (0=Normal by default) = 0
	// Bit 7 : chan2 (hSON 0=Use Chan1, 1=Use Chan2) = 0
	// Bit 8 : raw (0=CookedADCmode, 1=RawADC) = 1
	// Bit 9 : hasmot (0=NoMotor, 1=HasMotor) = 1
	// Bit 10 : applyoffset (1=Applied Hdgoffset, 0=Ignore) = 0
	// Bit 11 : pingpong (1=pingpong Chan1/Chan2 e.g. hSSS) = 0
	// Bit 12 : stareLLim (1=Don't Scan, Point at LeftLim) = 0
	// Bit 13 : ReplyASL (1=ASLin ReplyRec, 0=NotIn) = 1
	// Bit 14 : ReplyThr (1=hThrRec Requested) = 0
	// Bit 15 : IgnoreSensor (1=Ignore the Centre Sensor) 0
	writebuf[16] = (uint8)0x02; // Device Type (0x02 = Imaging SONAR)
	writebuf[17] = (uint8)0x66; // Transmitter numbers for channel 1
	writebuf[18] = (uint8)0x66; // Transmitter numbers for channel 1
	writebuf[19] = (uint8)0x66; // Transmitter numbers for channel 1
	writebuf[20] = (uint8)0x05; // Transmitter numbers for channel 1
	writebuf[21] = (uint8)0x66; // Transmitter numbers for channel 2
	writebuf[22] = (uint8)0x66; // Transmitter numbers for channel 2
	writebuf[23] = (uint8)0x66; // Transmitter numbers for channel 2
	writebuf[24] = (uint8)0x05; // Transmitter numbers for channel 2
	// F * 2^32 / 32e6 with F = Transmitter Frequency in Hertz (675 kHz)
	// This sonar has only 1 channel so channel 1 = channel 2
	writebuf[25] = (uint8)0x70; // Receiver numbers for channel 1
	writebuf[26] = (uint8)0x3D; // Receiver numbers for channel 1
	writebuf[27] = (uint8)0x0A; // Receiver numbers for channel 1
	writebuf[28] = (uint8)0x09; // Receiver numbers for channel 1
	writebuf[29] = (uint8)0x70; // Receiver numbers for channel 2
	writebuf[30] = (uint8)0x3D; // Receiver numbers for channel 2
	writebuf[31] = (uint8)0x0A; // Receiver numbers for channel 2
	writebuf[32] = (uint8)0x09; // Receiver numbers for channel 2
	// (F + 455000) * 2^32 / 32e6 with F = Transmitter Frequency in Hertz (675 kHz)
	// This sonar has only 1 channel so channel 1 = channel 2
	if (RangeScale < 0)	{
		RangeScale = 30;
		PRINT_DEBUG_WARNING_MICRONCORE(("SetParamMicron warning (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Negative RangeScale value. Default value of 30 m used. ", 
			hDev));
	}
	d = (double)((RangeScale + 10.0) * 25.0 / 10.0);
	word.v = (uint16)d;
	if (word.v > 350)	{
		word.v = 350;
		PRINT_DEBUG_WARNING_MICRONCORE(("SetParamMicron warning (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Too high RangeScale value. ", 
			hDev));
	}
	if (word.v < 37)	{
		word.v = 37;
		PRINT_DEBUG_WARNING_MICRONCORE(("SetParamMicron warning (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Too low RangeScale value. ", 
			hDev));
	}
	writebuf[33] = (uint8)word.c[0]; // Transmitter Pulse Length in microseconds units (LSB)
	writebuf[34] = (uint8)word.c[1]; // Transmitter Pulse Length in microseconds units (MSB)
	// TxPulseLen = [RangeScale(m) + Ofs] * Mul / 10 (Use defaults; Ofs = 10, Mul =25)
	// Should be constrained to between 50 .. 350
	// microseconds. A typical value is 100 microseconds
	word.v = (uint16)(RangeScale * 10);
	writebuf[35] = (uint8)word.c[0]; // Range Scale setting in decimetre units (LSB)
	writebuf[36] = (uint8)word.c[1]; // Range Scale setting in decimetre units (MSB)
	// The low order 14 bits are set to a value of Rangescale * 10 units.
	// Bit 6, Bit 7 of the MSB are used as a
	// code (0..3) for the Range Scale units : 
	// 0 = Metres, 1 = Feet, 2 = Fathoms, 3 = Yards
	// For example, Rangescale = 30 m
	d = (LeftAngleLimit * 10.0 / 9.0) * 16.0;
	word.v = ((uint16)d)%6400;
/*	if (word.v > 6399)
	{
		word.v = 6399;
		PRINT_DEBUG_WARNING_MICRONCORE(("SetParamMicron warning (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Too high LeftAngleLimit value. ", 
			hDev));
	}
*/	writebuf[37] = (uint8)word.c[0]; // Left Angle Limit in 1/16 Gradian units (LSB) (overridden if bit cont of byte HdCtrl is set)
	writebuf[38] = (uint8)word.c[1]; // Left Angle Limit in 1/16 Gradian units (MSB) (overridden if bit cont of byte HdCtrl is set)
	d = (RightAngleLimit * 10.0 / 9.0) * 16.0;
	word.v = ((uint16)d)%6400;
/*	if (word.v > 6399)
	{
		word.v = 6399;
		PRINT_DEBUG_WARNING_MICRONCORE(("SetParamMicron warning (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Too high RightAngleLimit value. ", 
			hDev));
	}
*/	writebuf[39] = (uint8)word.c[0]; // Right Angle Limit in 1/16 Gradian units (LSB) (overridden if bit cont of byte HdCtrl is set)
	writebuf[40] = (uint8)word.c[1]; // Right Angle Limit in 1/16 Gradian units (MSB) (overridden if bit cont of byte HdCtrl is set)
	writebuf[41] = (uint8)0x4D; // ADSpan
	writebuf[42] = (uint8)0x28; // ADLow
	// The SONAR receiver has an 80dB dynamic range, and signal 
	// levels are processed internally by the SONAR head such that
	// 0 .. 80dB = 0 .. 255
	// If adc8on in HdCtrl = 0, then the 80dB receiver signal is mapped to 4-bit, 16 level reply data
	// values, to a display dynamic range defined by ADSpan and ADLow such that:
	// ADSpan = 255 * Span(dB) / 80
	// ADLow = 255 * Low(dB) / 80
	// For example, ADSpan = 38 (12dB), ADLow = 40 (13dB)
	// 4-bit Data Values 0..15 = Signal Amplitudes 0 = 13dB, 15 = 25dB
	// ADSpan = 77 (24 dB) and ADLow = 40 (13dB) are typical values
	// If adc8on = 1 then the full 8-bit, 80dB dynamic range data bin amplitudes are returned to
	// the user:
	// 8-bit data Values 0..255 = Signal Amplitudes 0 = 0dB, 255 = 80dB
	if (IGain > 210)
	{
		IGain = 210;
		PRINT_DEBUG_WARNING_MICRONCORE(("SetParamMicron warning (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Too high Gain value. ", 
			hDev));
	}
	if (IGain < 0)
	{
		IGain = 0;
		PRINT_DEBUG_WARNING_MICRONCORE(("SetParamMicron warning (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Negative Gain value. ", 
			hDev));
	}
	writebuf[43] = (uint8)IGain; // Initial Gain of the receiver for channel 1 in units 0..210 = 0..+80dB = 0..100% (default = 0x49)
	writebuf[44] = (uint8)IGain; // Initial Gain of the receiver for channel 2 in units 0..210 = 0..+80dB = 0..100% (default = 0x49)
	writebuf[45] = (uint8)0x7D; // Slope setting for channel 1 in 1/255 units (LSB)
	writebuf[46] = (uint8)0x00; // Slope setting for channel 1 in 1/255 units (MSB)
	writebuf[47] = (uint8)0x7D; // Slope setting for channel 2 in 1/255 units (LSB)
	writebuf[48] = (uint8)0x00; // Slope setting for channel 2 in 1/255 units (MSB)
	// This sonar has only 1 channel so channel 1 = channel 2
	// For a channel at 675 kHz, Default Slope = 125
	writebuf[49] = (uint8)0x19; // Motor Step Delay Time : high speed limit of the scanning motor in units of 10 microseconds, typically = 25
	d = (double)((StepAngleSize * 10.0 / 9.0) * 16.0);
	writebuf[50] = (uint8)d; // Motor Step Angle Size : scanning motor step angle between pings in 1/16 Gradian units
	// Low Resolution = 32 (= 2 Gradians = 1.8 °)
	// Medium Resolution = 16 (= 1 Gradian = 0.9°)
	// High Resolution = 8 (= 0.5 Gradian = 0.45°)
	// Ultimate Resolution = 4 (= 0.25 Gradian = 0.225°)
	d = (double)(((RangeScale * 2.0 / NBins) / VelocityOfSound) / 0.000000640);
	word.v = (uint16)d;
	if (word.v < 5)	{
		word.v = 5;
		PRINT_DEBUG_WARNING_MICRONCORE(("SetParamMicron warning (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Too low ADInterval value. Invalid RangeScale, NBins or VelocityOfSound values. ", 
			hDev));
	}
	writebuf[51] = (uint8)word.c[0]; // AD Interval in units of 640 nanoseconds (LSB)
	writebuf[52] = (uint8)word.c[1]; // AD Interval in units of 640 nanoseconds (MSB)
	// Sampling Interval(s) = (RangeScale(m) * 2 / Number of Bins) / VOS (i.e. use Range *2 for Return Path)
	// with VOS = 1500 m/sec (Velocity Of Sound)
	// ADInterval = Sampling interval in units of 640 nanoseconds = Sampling Interval(s) / 640e-9
	// A practical minimum for ADInterval is about 5 (approximatively 3 microseconds)
	word.v = (uint16)NBins;
	if (word.v > 800)	{
		word.v = 800;
		PRINT_DEBUG_WARNING_MICRONCORE(("SetParamMicron warning (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Too high NBins value. ", 
			hDev));
	}
	if (word.v < 5)	{
		word.v = 5;
		PRINT_DEBUG_WARNING_MICRONCORE(("SetParamMicron warning (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Too low NBins value. ", 
			hDev));
	}
	writebuf[53] = (uint8)word.c[0]; // Number of sample bins over scan-line (LSB)
	writebuf[54] = (uint8)word.c[1]; // Number of sample bins over scan-line (MSB)
	// The maximum value of NBins is limited to 800
	writebuf[55] = (uint8)0xE8; // MaxADbuf, default = 500, limit = 1000 (LSB)
	writebuf[56] = (uint8)0x03; // MaxADbuf, default = 500, limit = 1000 (MSB)
	writebuf[57] = (uint8)0x64; // Lockout period in microsecond units, default = 100 (LSB)
	writebuf[58] = (uint8)0x00; // Lockout period in microsecond units, default = 100 (MSB)
	writebuf[59] = (uint8)0x40; // Minor Axis of dual-axis device in 1/16 Gradian units (LSB)
	writebuf[60] = (uint8)0x06; // Minor Axis of dual-axis device in 1/16 Gradian units (MSB)
	// For the standard (Single Axis) devices they should be fixed at 1600
	writebuf[61] = (uint8)0x01; // Major Axis in 1/16 Gradian units. Always 1 for SONAR
	writebuf[62] = (uint8)0x00; // Ctl2, extra SONAR Control Functions to be implemented for operating and test purposes
	writebuf[63] = (uint8)0x00; // ScanZ, for Special devices and should both be left at default values of 0 (LSB)
	writebuf[64] = (uint8)0x00; // ScanZ, for Special devices and should both be left at default values of 0 (MSB)


writebuf[65] = (uint8)0x50;
writebuf[66] = (uint8)0x51;
writebuf[67] = (uint8)0x09;
writebuf[68] = (uint8)0x08;
writebuf[69] = (uint8)0x54;
writebuf[70] = (uint8)0x54;
writebuf[71] = (uint8)0x00;
writebuf[72] = (uint8)0x00;
writebuf[73] = (uint8)0x5A;
writebuf[74] = (uint8)0x00;
writebuf[75] = (uint8)0x7D;
writebuf[76] = (uint8)0x00;
writebuf[77] = (uint8)0x00;
writebuf[78] = (uint8)0x00;
writebuf[79] = (uint8)0x00;
writebuf[80] = (uint8)0x00;



	writebuf[81] = (uint8)0x0A; // Message Terminator = Line Feed

	if (WriteAllRS232Port(hDev, writebuf, 82) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("SetParamMicron error (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Error writing data to the serial port. ", 
			hDev));
		return EXIT_FAILURE;
	}

	// On power-up of the sonar, the 'HeadInf' byte will change in value to 
	// reflect the different states that the sonar is going through during initialisation
	// i) Power applied to sonar
	// 1st Alive: HeadInf = '5D' (Head is in Re-Centre operation and has No Params). Byte 14 = 80h.
	// 2nd Alive: HeadInf = '4D' (No 'Dir' so Transducer is back at centre)
	// 3rd Alive: HeadInf = '4A' (Transducer is now centred and Not Motoring)
	// ii) 'mtHeadCommand' sent to Sonar
	// 4th Alive: HeadInf = 'CA' (Has been 'Sent Cfg'. Acknowledgement of 'ParamsCmd')
	// 5th Alive: HeadInf = '8A' (Has Params. Parameters have been validated. Ready for 'GetData').
	// Byte 14 now set to 00h




	StartChrono(&chrono);

	readbuf[20] = 0;

	while ((readbuf[20] != 0x8A)&&(readbuf[20] != 0x9A)&&(readbuf[20] != 0x1A)) //DA?
	{
		GetTimeElapsedChrono(&chrono, &duration);
		if (duration > 30)
		{
			PRINT_DEBUG_ERROR_MICRONCORE(("SetParamMicron error (%s) : %s"
				"(hDev=%#x)\n", 
				strtime_m(), 
				"The operation timed out. ", 
				hDev));
			return EXIT_FAILURE;
		}

		// Wait for an mtAlive message. It should come every 1 second.
		if (ReadMsgMicron(hDev, readbuf, 22, 0x04) != EXIT_SUCCESS)
		{ 
			PRINT_DEBUG_ERROR_MICRONCORE(("SetParamMicron error (%s) : %s"
				"(hDev=%#x)\n", 
				strtime_m(), 
				"The device is not responding correctly. ", 
				hDev));
			return EXIT_FAILURE;
		}

		PRINT_DEBUG_MESSAGE_MICRONCORE(("mtAlive message readbuf[20]=%#x\n", readbuf[20]));
	}

	return EXIT_SUCCESS;
}

/*
Request scanlines from a Micron. Use SendDataReplyMicron() to get the 
reply.

HANDLE hDev : (IN) Identifier of the device serial port.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SendDataRequestMicron(HANDLE hDev)
{
	time_t t;
	uLong dword;
	uint8 writebuf[18] = {0x40,0x30,0x30,0x30,0x43,0x0C,0x00,0xFF,0x02,0x07,0x19,0x80,0x02,0xCA,0x64,0xB0,0x03,0x0A};

	// We have to send the current time.
	time(&t);

	dword.v = (long)(t*1000); // Convert in ms.

	writebuf[13] = dword.c[0];
	writebuf[14] = dword.c[1];
	writebuf[15] = dword.c[2];
	writebuf[16] = dword.c[3];

	if (WriteAllRS232Port(hDev, writebuf, 18) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("SendDataRequestMicron error (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"Error writing data to the serial port. ", 
			hDev));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
Get 2 scanlines and the corresponding angles from a Micron.
Use it after a call to SendDataRequestMicron().
We have 2 scanlines because the Micron sends 2 replies for 1 request of data.

HANDLE hDev : (IN) Identifier of the device serial port.
SNRPARAMS params : (IN) Structure containing various parameters.
uint8* scanline1 : (INOUT) Valid pointer that will receive the first scanline.
double* pAngle1 : (INOUT) Valid pointer that will receive the angle of the 
first scanline (in degrees).
uint8* scanline2 : (INOUT) Valid pointer that will receive the second scanline.
double* pAngle2 : (INOUT) Valid pointer that will receive the angle of the 
second scanline (in degrees).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SendDataReplyMicron(HANDLE hDev, SNRPARAMS params,
							 uint8* scanline1, double* pAngle1, 
							 uint8* scanline2, double* pAngle2)
{
	uint8 readbuf1[1024];
	uint8 readbuf2[1024];
	int j = 0;
	uShort transducer_bearing;
	uShort Dbytes;

	if (!params.adc8on)
	{ 
		// If adc8on = 0, the scanlines data received are in 4 bit (1/2 byte).
		//Dbytes = params.NBins/2;
	}
	else	
	{ 
		// If adc8on = 1, the scanlines data received are in 8 bit (1 byte).
		//Dbytes = params.NBins;
	}

	// Wait for an mtHeadData message (first buffer).
	if (ReadAllRS232Port(hDev, readbuf1, 44) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("SendDataReplyMicron error (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"The device is not responding correctly. ", 
			hDev));
		return EXIT_FAILURE;
	}

	Dbytes.c[0] = readbuf1[42];
	Dbytes.c[1] = readbuf1[43];

	if (ReadAllRS232Port(hDev, readbuf1+44, Dbytes.v+1) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("SendDataReplyMicron error (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"The device is not responding correctly. ", 
			hDev));
		return EXIT_FAILURE;
	}

	// Wait for an mtHeadData message (second buffer).
	if (ReadAllRS232Port(hDev, readbuf2, 44) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("SendDataReplyMicron error (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"The device is not responding correctly. ", 
			hDev));
		return EXIT_FAILURE;
	}

	Dbytes.c[0] = readbuf2[42];
	Dbytes.c[1] = readbuf2[43];

	if (ReadAllRS232Port(hDev, readbuf2+44, Dbytes.v+1) != EXIT_SUCCESS)
	{ 
		PRINT_DEBUG_ERROR_MICRONCORE(("SendDataReplyMicron error (%s) : %s"
			"(hDev=%#x)\n", 
			strtime_m(), 
			"The device is not responding correctly. ", 
			hDev));
		return EXIT_FAILURE;
	}

	// First buffer

	// Transducer Bearing (Bytes 41,42). 
	// This is the position of the transducer for the current scanline (0..6399 in 1/16 Gradian units).
	transducer_bearing.c[0] = (uint8)readbuf1[40]; // LSB.
	transducer_bearing.c[1] = (uint8)readbuf1[41]; // MSB.
	*pAngle1 = (double)(transducer_bearing.v * 0.05625); // Angle of the transducer in degrees (0.05625 = (1/16)*(9/10)).

	// Retrieve the scanlines data from the data received.
	if (!params.adc8on)	
	{ 
		// If adc8on = 0, the scanlines data received are in 4 bit (1/2 byte).
		// The amplitude values are between 0 and 15 so we multiply by 16
		// to get values between 0 and 255.
		for (j = 44; j < Dbytes.v+44; j++)
		{
			scanline1[2*(j-44)+0] = (uint8)((readbuf1[j]>>4)*16);
			scanline1[2*(j-44)+1] = (uint8)(((readbuf1[j]<<4)>>4)*16);
		}
	}
	else	
	{
		// If adc8on = 1, the scanlines data received are in 8 bit (1 byte).
		// The amplitude values are between 0 and 255.
		memcpy(scanline1, readbuf1+44, Dbytes.v); // Copy the data received without the header (which was the 44 first bytes).
	}

	// Second buffer

	// Transducer Bearing (Bytes 41,42). 
	// This is the position of the transducer for the current scanline (0..6399 in 1/16 Gradian units).
	transducer_bearing.c[0] = (uint8)readbuf2[40]; // LSB.
	transducer_bearing.c[1] = (uint8)readbuf2[41]; // MSB.
	*pAngle2 = (double)(transducer_bearing.v * 0.05625); // Angle of the transducer in degrees (0.05625 = (1/16)*(9/10)).

	// Retrieve the scanlines data from the data received.	
	if (!params.adc8on)
	{ 
		// If adc8on = 0, the scanlines data received are in 4 bit (1/2 byte).
		// The amplitude values are between 0 and 15 so we multiply by 16
		// to get values between 0 and 255.
		for (j = 44; j < Dbytes.v+44; j++)
		{
			scanline2[2*(j-44)+0] = (uint8)((readbuf2[j]>>4)*16);
			scanline2[2*(j-44)+1] = (uint8)(((readbuf2[j]<<4)>>4)*16);
		}
	}
	else
	{
		// If adc8on = 1, the scanlines data received are in 8 bit (1 byte).
		// The amplitude values are between 0 and 255.
		memcpy(scanline2, readbuf2+44, Dbytes.v); // Copy the data received without the header (which was the 44 first bytes).
	}

	return EXIT_SUCCESS;
}

#endif // MICRONCORE_H
