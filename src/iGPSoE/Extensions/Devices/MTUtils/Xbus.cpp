// Xbus.cpp: implementation of the CXbus class.
//
// Version 1.0.0 - Public release
//
// ----------------------------------------------------------------------------
//  This file is an Xsens Code Examples.
//
//  Copyright (C) Xsens Technologies B.V., 2004.  All rights reserved.
//
//  This source code is intended only as a example of the implementation
//	of the Xsens Xbus protocol.
//	It was written for cross platform capabilities, but has not been
//	tested on the Linux platform yet.
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//////////////////////////////////////////////////////////////////////



/*

Modified

*/


#ifdef _MSC_VER
// Disable some Visual Studio warnings that happen in Xbus.
#pragma warning(disable : 4244) 
#pragma warning(disable : 4127) 
#pragma warning(disable : 4996)
#endif // _MSC_VER




#include "Xbus.h"

#ifdef _DEBUG
#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXbus::CXbus()
{
	m_portOpen = false;
	m_errorCode = 0;		// No error
	m_retVal = X_XOK;
	m_timeOut = TO_DEFAULT;
}

CXbus::~CXbus()
{
	closeSerialPort();
}

////////////////////////////////////////////////////////////////////
// clockms
//
// Calculates the processor time used by the calling process.
//
// Output
//   returns processor time in milliseconds
//
clock_t CXbus::clockms()
{
	clock_t clk;

	clk = clock();		// Get current processor time

	clk /= (CLOCKS_PER_SEC / 1000);

	return clk;
}

////////////////////////////////////////////////////////////////////
// initSerialPort
//
// Retrieves handle to serial port and inits its properties
//
// Input
//   portNumber	 : number of serial port to open (1-99)
//   baudrate	 : baudrate value (115200, 230400, 460800, etc), default = 115200
//   inqueueSize : size of input queue, default = 4096
//   outqueueSize: size of output queue, default = 1024
//
// Output
//   returns X_XOK if serial port is successfully opened, else X_COMPORTNOTOPEN
//
short CXbus::initSerialPort(const int portNumber, const unsigned long baudrate, const unsigned long inqueueSize, const unsigned long outqueueSize)
{
#ifdef _WIN32	
	char pchFileName[10];
	
	sprintf(pchFileName,"\\\\.\\COM%d",portNumber);		// Create file name

	m_serialPort = CreateFile(pchFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (m_serialPort == INVALID_HANDLE_VALUE) {
		return (m_retVal = X_COMPORTNOTOPEN);
	}

	// Once here, port is open
	m_portOpen = true;

	//Get the current state & then change it
	DCB dcb;

	GetCommState(m_serialPort, &dcb);	// Get current state

	dcb.BaudRate = baudrate;			// Setup the baud rate
	dcb.Parity = NOPARITY;				// Setup the Parity
	dcb.ByteSize = 8;					// Setup the data bits
	dcb.StopBits = TWOSTOPBITS;			// Setup the stop bits
	dcb.fDsrSensitivity = FALSE;		// Setup the flow control 
	dcb.fOutxCtsFlow = FALSE;			// NoFlowControl:
	dcb.fOutxDsrFlow = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	SetCommState(m_serialPort, &dcb);	// Set new state

	// Set COM timeouts
	COMMTIMEOUTS CommTimeouts;

	GetCommTimeouts(m_serialPort,&CommTimeouts);	// Fill CommTimeouts structure
/*
	CommTimeouts.ReadTotalTimeoutConstant = 0;	
	CommTimeouts.ReadIntervalTimeout = MAXDWORD;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0; 
*/


	CommTimeouts.ReadIntervalTimeout = MAXDWORD ;
	CommTimeouts.ReadTotalTimeoutConstant = 4000;
	CommTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD ;
	CommTimeouts.WriteTotalTimeoutConstant = 4000;
	CommTimeouts.WriteTotalTimeoutMultiplier = MAXDWORD ;



	SetCommTimeouts(m_serialPort, &CommTimeouts);	// Set CommTimeouts structure

	// Other initialization functions
	EscapeCommFunction(m_serialPort, SETRTS);		// Enable RTS (for Xbus Master use)
	SetupComm(m_serialPort,inqueueSize,outqueueSize);	// Set queue size

	// Remove any 'old' data in buffer
	PurgeComm(m_serialPort, PURGE_TXCLEAR | PURGE_RXCLEAR);

	return (m_retVal = X_XOK);
#else	
	char chPort[15];
	struct termios options;

	/* Open port */
	sprintf(chPort,"/dev/ttyUSB%d",portNumber);
	
	m_serialPort = open(chPort, O_RDWR | O_NOCTTY);

	// O_RDWR: Read+Write
	// O_NOCTTY: Raw input, no "controlling terminal"
	// O_NDELAY: Don't care about DCD signal

	if (m_serialPort < 0) {
		// Port not open
		return (m_retVal = X_COMPORTNOTOPEN);
	}

	/* Start configuring of port for non-canonical transfer mode */
	// Get current options for the port
	tcgetattr(m_serialPort, &options);
	
	// Set baudrate. Must use the B**** define!!! Add your own when needed
	switch(baudrate) {
	case 57600:
		cfsetispeed(&options, B57600);
		cfsetospeed(&options, B57600);
		break;
	case 115200:
		cfsetispeed(&options, B115200);
		cfsetospeed(&options, B115200);
		break;
	case 230400:
		cfsetispeed(&options, B230400);
		cfsetospeed(&options, B230400);
		break;
	case 460800:
		cfsetispeed(&options, B460800);
		cfsetospeed(&options, B460800);
		break;
	default:
		;
	}	

	// Enable the receiver and set local mode
	options.c_cflag |= (CLOCAL | CREAD);
	// Set character size to data bits and set no parity Mask the characte size bits
	options.c_cflag &= ~(CSIZE|PARENB);
	options.c_cflag |= CS8;		// Select 8 data bits
	options.c_cflag |= CSTOPB;	// send 2 stop bits
	// Disable hardware flow control
	options.c_cflag &= ~CRTSCTS;
	options.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
	// Disable software flow control
	options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	// Set Raw output
	options.c_oflag &= ~OPOST;
	// Timeout 0.005 sec for first byte, read minimum of 0 bytes
	options.c_cc[VMIN]     = 0;
	options.c_cc[VTIME]    = 5;

	// Set the new options for the port
	tcsetattr(m_serialPort,TCSANOW, &options);
	
	tcflush(m_serialPort, TCIOFLUSH);

	return (m_retVal = X_XOK);
#endif	
}

////////////////////////////////////////////////////////////////////
// isPortOpen
//
// Return if serial port is open or not
//
bool CXbus::isPortOpen()
{
	return m_portOpen;
}

////////////////////////////////////////////////////////////////////
// writeSerialPort
//
// Writes characters to serial port
//
// Input
//   msgBuffer		: a pointer to a char buffer containing
//					  the characters to be written to serial port
//   nBytesToWrite	: number of buffer bytes to write to serial port
//
// Output
//   number of bytes actually written
int CXbus::writeSerialPort(const unsigned char* msgBuffer, const int nBytesToWrite)
{
#ifdef _WIN32
	DWORD nBytesWritten;
	WriteFile(m_serialPort, msgBuffer, nBytesToWrite, &nBytesWritten, NULL);
	return nBytesWritten;
#else
	return write(m_serialPort, msgBuffer, nBytesToWrite);
#endif
}

////////////////////////////////////////////////////////////////////
// readSerialPort
//
// Reads characters from serial port
//
// Input
//   msgBuffer		: pointer to buffer in which next string will be stored
//   nBytesToRead	: number of buffer bytes to read from serial port
//
// Output
//   number of bytes actually read
int CXbus::readSerialPort(unsigned char*  msgBuffer, const int nBytesToRead)
{
#ifdef _WIN32
	DWORD nBytesRead;
	(void)ReadFile(m_serialPort, msgBuffer, nBytesToRead, &nBytesRead, NULL);
	return nBytesRead;
#else
	// In Linux it is better to read per byte instead of a block of bytes at once
	int j = 0;	// Index in buffer for read data
	int k = 0;	// Timeout factor
	int nRead = 0;	// Bytes read from port, return by read function

	do {
		nRead = read(m_serialPort, &msgBuffer[j], 1);
		if (nRead == 1) {	// Byte read
			k = 0;
			j++;
		}
		else {
    		k++;
		}

		if (k == 3)	{ // Timeout, too long no data read from port
    		return nRead;
		}
	}
	while (j < nBytesToRead);
	
	return j;
#endif
}

////////////////////////////////////////////////////////////////////
// flushSerialPort
//
// Remove any 'old' data in COM port buffer
//
void CXbus::flushSerialPort()
{
	if (m_portOpen) {
#ifdef _WIN32
		// Remove any 'old' data in buffer
		PurgeComm(m_serialPort, PURGE_TXCLEAR | PURGE_RXCLEAR);
#else
		tcflush(m_serialPort, TCIOFLUSH);
#endif
	}		
}

////////////////////////////////////////////////////////////////////
// setSerialPortQueueSize
//
// Set input and output buffer size of serial port
//
void CXbus::setSerialPortQueueSize(const unsigned long inqueueSize /* = 4096 */, const unsigned long outqueueSize /* = 1024 */)
{
	if (m_portOpen) {
#ifdef _WIN32
		SetupComm(m_serialPort,inqueueSize,outqueueSize);	// Set queue size
#else
		// Not yet implemented
#endif
	}
}

////////////////////////////////////////////////////////////////////
// closeSerialPort
//
// Closes handle for serial port
//
void CXbus::closeSerialPort()
{
	if (m_portOpen) {
#ifdef _WIN32
		CloseHandle(m_serialPort);
#else
		close(m_serialPort);
#endif
	}
	m_portOpen = false;
}

// Basics

////////////////////////////////////////////////////////////////////
// mtSendMessage (optional: integer value)
//
// Sends a message. Return value states whether or not
//   an acknowledge has been received. Use this function for
//	 GotoConfig, Reset, ResetOrientation etc
//
// Input
//	 mid		  : MessageID of message to send
//	 dataValue	  : A integer value that will be included into the data message field
//				    can be a 1,2 or 4 bytes values
//	 dataValueLen : Size of dataValue in bytes
//   bid		  : BID or address to use in message to send (default = 0xFF)
//
// Output
//   = X_XOK if an Ack message received
//	 = X_RECVERRORMSG if an error message received
//	 = X_TIMEOUT if timeout occurred
//
short CXbus::mtSendMessage(const unsigned char mid, const unsigned long dataValue, 
						   const unsigned char dataValueLen, const unsigned char bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;

	buffer[IND_PREAMBLE] = PREAMBLE;
	buffer[IND_BID] = bid;
	buffer[IND_MID] = mid;
	buffer[IND_LEN] = dataValueLen;
	swapEndian((const unsigned char *)&dataValue,&buffer[IND_DATA0],dataValueLen);
	calcChecksum(buffer,LEN_MSGHEADER + dataValueLen);

	// Send message
	mtSendRawString(buffer, LEN_MSGHEADERCS + dataValueLen);

	// Keep reading until an Ack or Error message is received (or timeout)
	clock_t clkStart = clockms();
	short	defTimeOut = m_timeOut;
	do{
		if (mtReadMessageRaw(buffer, &msgLen) == X_XOK){
			// Message received
			if (buffer[IND_MID] == (mid+1)) {
				m_timeOut = defTimeOut;
				return (m_retVal = X_XOK);				// Acknowledge received
			}
			else if (buffer[IND_MID] == MID_ERROR){
				m_errorCode = buffer[IND_DATA0];
				m_timeOut = defTimeOut;
				return (m_retVal = X_RECVERRORMSG);	// Error message received
			}
		}
		// Calculate remaining time to check
		m_timeOut = m_timeOut - (clockms() - clkStart);
	}while (m_timeOut > 0);

	m_timeOut = defTimeOut;
	return (m_retVal = X_TIMEOUT);
}

////////////////////////////////////////////////////////////////////
// mtSendMessage (data array)
//
// Sends a message. Return value states whether or not
//   an acknowledge has been received. Use this function if you
//	 want to send a byte array to the device
//
// Input
//	 mid		: MessageID of message to send
//	 data	    : array pointer to data bytes
//	 dataLen    : number of bytes to include in message
//   bid		: BID or address to use in message to send (default = 0xFF)
//
// Output
//   = X_XOK if an Ack message received
//	 = X_RECVERRORMSG if an error message received
//	 = X_TIMEOUT if timeout occurred
//
short CXbus::mtSendMessage(const unsigned char mid, const unsigned char data[], 
						   const unsigned short &dataLen, const unsigned char bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;
	short headerLength;

	// Build message to send
	buffer[IND_PREAMBLE] = PREAMBLE;
	buffer[IND_BID] = bid;
	buffer[IND_MID] = mid;

	if (dataLen < EXTLENCODE) {
		buffer[IND_LEN] = (unsigned char)dataLen;
		headerLength = LEN_MSGHEADER;
	}
	else {
		buffer[IND_LEN] = EXTLENCODE;
		buffer[IND_LENEXTH] = (unsigned char)(dataLen >> 8);
		buffer[IND_LENEXTL] = (unsigned char)(dataLen & 0x00FF);
		headerLength = LEN_MSGEXTHEADER;
	}
	memcpy(&buffer[headerLength], data, dataLen);
	calcChecksum(buffer, headerLength + dataLen);

	// Send message
	mtSendRawString(buffer, headerLength + dataLen + LEN_CHECKSUM);

	// Keep reading until an Ack or Error message is received (or timeout)
	clock_t clkStart = clockms();
	short	defTimeOut = m_timeOut;
	do{
		if (mtReadMessageRaw(buffer, &msgLen) == X_XOK){
			// Message received
			if (buffer[IND_MID] == (mid+1)) {
				m_timeOut = defTimeOut;
				return (m_retVal = X_XOK);				// Acknowledge received
			}
			else if (buffer[IND_MID] == MID_ERROR){
				m_errorCode = buffer[IND_DATA0];
				m_timeOut = defTimeOut;
				return (m_retVal = X_RECVERRORMSG);	// Error message received
			}
		}
		// Calculate remaining time to check
		m_timeOut = m_timeOut - (clockms() - clkStart);
	}while (m_timeOut > 0);

	m_timeOut = defTimeOut;
	return (m_retVal = X_TIMEOUT);
}

////////////////////////////////////////////////////////////////////
// mtReqSetting (integer & no param variant)
//
// Request a integer setting from the device. This setting
// can be an unsigned 1,2 or 4 bytes setting
//
// Input
//	 mid		: Message ID of message to send
//   bid		: Bus ID of message to send (def 0xFF)
//
// Output
//   = X_XOK if an Ack message is received
//	 = X_RECVERRORMSG if an error message is received
//	 = X_TIMEOUT if timeout occurred
//
//	 value contains the integer value of the data field of the ack message
//
short CXbus::mtReqSetting(const unsigned char mid, unsigned long &value, const unsigned char bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;

	buffer[IND_PREAMBLE] = PREAMBLE;
	buffer[IND_BID] = bid;
	buffer[IND_MID] = mid;
	buffer[IND_LEN] = 0;
	calcChecksum(buffer,LEN_MSGHEADER + buffer[IND_LEN]);

	// Send message
	mtSendRawString(buffer, LEN_MSGHEADERCS + buffer[IND_LEN]);

	// Read next message or else timeout
	if (mtReadMessageRaw(buffer, &msgLen) == X_XOK){
		// Message received
		if (buffer[IND_MID] == (mid+1)) {
			// Acknowledge received
			value = 0;
			swapEndian(&buffer[IND_DATA0],(unsigned char *)&value, buffer[IND_LEN]);
			return (m_retVal = X_XOK);				
		}
		else if (buffer[IND_MID] == MID_ERROR){
			m_errorCode = buffer[IND_DATA0];
			return (m_retVal = X_RECVERRORMSG);	// Error message received
		}
		else{
			return (m_retVal = X_UNEXPECTEDMSG);// Unexpected message
		}
	}
	return m_retVal;
}

////////////////////////////////////////////////////////////////////
// mtReqSetting (integer & param variant)
//
// Request a integer setting from the device. This setting
// can be an unsigned 1,2 or 4 bytes setting
//
// Input
//	 mid		: Message ID of message to send
//	 param		: For messages that need a parameter
//   bid		: Bus ID of message to send (def 0xFF)
//
// Output
//   = X_XOK if an Ack message is received
//	 = X_RECVERRORMSG if an error message is received
//	 = X_TIMEOUT if timeout occurred
//
//	 value contains the integer value of the data field of the ack message
//
short CXbus::mtReqSetting(const unsigned char mid, const unsigned char param, unsigned long &value,  
						  const unsigned char bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;

	buffer[IND_PREAMBLE] = PREAMBLE;
	buffer[IND_BID] = bid;
	buffer[IND_MID] = mid;
	if (param != 0xFF) {
		buffer[IND_LEN] = 1;
		buffer[IND_DATA0] = param;
	}
	else{
		buffer[IND_LEN] = 0;
	}
	calcChecksum(buffer,LEN_MSGHEADER + buffer[IND_LEN]);

	// Send message
	mtSendRawString(buffer, LEN_MSGHEADERCS + buffer[IND_LEN]);

	// Read next message or else timeout
	if (mtReadMessageRaw(buffer, &msgLen) == X_XOK){
		// Message received
		if (buffer[IND_MID] == (mid+1)) {
			// Acknowledge received
			value = 0;
			if (param == 0xFF){
				swapEndian(&buffer[IND_DATA0],(unsigned char *)&value, buffer[IND_LEN]);
			}
			else{
				swapEndian(&buffer[IND_DATA0]+1,(unsigned char *)&value, buffer[IND_LEN]-1);
			}
			return (m_retVal = X_XOK);				
		}
		else if (buffer[IND_MID] == MID_ERROR){
			m_errorCode = buffer[IND_DATA0];
			return (m_retVal = X_RECVERRORMSG);	// Error message received
		}
		else{
			return (m_retVal = X_UNEXPECTEDMSG);// Unexpected message
		}
	}
	return m_retVal;
}

////////////////////////////////////////////////////////////////////
// mtReqSetting (float & no param variant)
//
// Request a float setting from the device. 
//
// Input
//	 mid		: Message ID of message to send
//   bid		: Bus ID of message to send (def 0xFF)
//
// Output
//   = X_XOK if an Ack message is received
//	 = X_RECVERRORMSG if an error message is received
//	 = X_TIMEOUT if timeout occurred
//
//	 value contains the float value of the acknowledge data field
//
short CXbus::mtReqSetting(const unsigned char mid, float &value, const unsigned char bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;

	buffer[IND_PREAMBLE] = PREAMBLE;
	buffer[IND_BID] = bid;
	buffer[IND_MID] = mid;
	buffer[IND_LEN] = 0;
	calcChecksum(buffer,LEN_MSGHEADER + buffer[IND_LEN]);

	// Send message
	mtSendRawString(buffer, LEN_MSGHEADERCS + buffer[IND_LEN]);

	// Read next message or else timeout
	if (mtReadMessageRaw(buffer, &msgLen) == X_XOK){
		// Message received
		if (buffer[IND_MID] == (mid+1)) {
			// Acknowledge received
			value = 0;
			swapEndian(&buffer[IND_DATA0],(unsigned char *)&value, buffer[IND_LEN]);
			return (m_retVal = X_XOK);				
		}
		else if (buffer[IND_MID] == MID_ERROR){
			m_errorCode = buffer[IND_DATA0];
			return (m_retVal = X_RECVERRORMSG);	// Error message received
		}
		else{
			return (m_retVal = X_UNEXPECTEDMSG);// Unexpected message
		}
	}
	return m_retVal;
}

////////////////////////////////////////////////////////////////////
// mtReqSetting (float & param variant)
//
// Request a float setting from the device. 
//
// Input
//	 mid		: Message ID of message to send
//	 param		: For messages that need a parameter (optional)
//   bid		: Bus ID of message to send (def 0xFF)
//
// Output
//   = X_XOK if an Ack message is received
//	 = X_RECVERRORMSG if an error message is received
//	 = X_TIMEOUT if timeout occurred
//
//	 value contains the float value of the acknowledge data field
//
short CXbus::mtReqSetting(const unsigned char mid, const unsigned char param, float &value,
						  const unsigned char bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;

	buffer[IND_PREAMBLE] = PREAMBLE;
	buffer[IND_BID] = bid;
	buffer[IND_MID] = mid;
	if (param != 0xFF) {
		buffer[IND_LEN] = 1;
		buffer[IND_DATA0] = param;
	}
	else{
		buffer[IND_LEN] = 0;
	}
	calcChecksum(buffer,LEN_MSGHEADER + buffer[IND_LEN]);

	// Send message
	mtSendRawString(buffer, LEN_MSGHEADERCS + buffer[IND_LEN]);

	// Read next message or else timeout
	if (mtReadMessageRaw(buffer, &msgLen) == X_XOK){
		// Message received
		if (buffer[IND_MID] == (mid+1)) {
			// Acknowledge received
			value = 0;
			if (param == 0xFF){
				swapEndian(&buffer[IND_DATA0],(unsigned char *)&value, buffer[IND_LEN]);
			}
			else{
				swapEndian(&buffer[IND_DATA0]+1,(unsigned char *)&value, buffer[IND_LEN]-1);
			}
			return (m_retVal = X_XOK);				
		}
		else if (buffer[IND_MID] == MID_ERROR){
			m_errorCode = buffer[IND_DATA0];
			return (m_retVal = X_RECVERRORMSG);	// Error message received
		}
		else{
			return (m_retVal = X_UNEXPECTEDMSG);// Unexpected message
		}
	}
	return m_retVal;
}

////////////////////////////////////////////////////////////////////
// mtReqSetting (byte array & no param variant)
//
// Send a message to the device and the data of acknowledge message
// will be returned
//
// Input
//	 mid		: Message ID of message to send
//   bid		: Bus ID of message to send (def 0xFF)
//	 
// Output
//   = X_XOK if an Ack message is received
//	 = X_RECVERRORMSG if an error message is received
//	 = X_TIMEOUT if timeout occurred
//
//	 data[] contains the data of the acknowledge message
//	 dataLen contains the number bytes returned
//
short CXbus::mtReqSetting(const unsigned char mid, 
						  unsigned char data[], short &dataLen, const unsigned char bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;

	buffer[IND_PREAMBLE] = PREAMBLE;
	buffer[IND_BID] = bid;
	buffer[IND_MID] = mid;
	buffer[IND_LEN] = 0;
	calcChecksum(buffer,LEN_MSGHEADER + buffer[IND_LEN]);

	// Send message
	mtSendRawString(buffer, LEN_MSGHEADERCS);

	dataLen = 0;
	// Read next message or else timeout
	if (mtReadMessageRaw(buffer, &msgLen) == X_XOK){
		// Message received
		if (buffer[IND_MID] == (mid+1)) {
			// Acknowledge received
			if (buffer[IND_LEN] != EXTLENCODE) {
				dataLen = buffer[IND_LEN];
				memcpy(data, &buffer[IND_DATA0], dataLen);
			}
			else{
				dataLen = buffer[IND_LENEXTH]*256 + buffer[IND_LENEXTL];
				memcpy(data, &buffer[IND_DATAEXT0], dataLen);
			}
			return (m_retVal = X_XOK);				
		}
		else if (buffer[IND_MID] == MID_ERROR){
			m_errorCode = buffer[IND_DATA0];
			return (m_retVal = X_RECVERRORMSG);	// Error message received
		}
		else{
			return (m_retVal = X_UNEXPECTEDMSG);// Unexpected message
		}
	}
	return m_retVal;
}

////////////////////////////////////////////////////////////////////
// mtReqSetting (byte array & param variant)
//
// Send a message to the device and the data of acknowledge message
// will be returned
//
// Input
//	 mid		: Message ID of message to send
//	 param		: For messages that need a parameter (optional)
//   bid		: Bus ID of message to send (def 0xFF)
//	 
// Output
//   = X_XOK if an Ack message is received
//	 = X_RECVERRORMSG if an error message is received
//	 = X_TIMEOUT if timeout occurred
//
//	 data[] contains the data of the acknowledge message (including param!!)
//	 dataLen contains the number bytes returned
//
short CXbus::mtReqSetting(const unsigned char mid, const unsigned char param, 
						  unsigned char data[], short &dataLen, const unsigned char bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;

	buffer[IND_PREAMBLE] = PREAMBLE;
	buffer[IND_BID] = bid;
	buffer[IND_MID] = mid;
	if (param != 0xFF) {
		buffer[IND_LEN] = 1;
		buffer[IND_DATA0] = param;
	}
	else{
		buffer[IND_LEN] = 0;
	}
	calcChecksum(buffer,LEN_MSGHEADER + buffer[IND_LEN]);

	// Send message
	mtSendRawString(buffer, LEN_MSGHEADERCS);

	dataLen = 0;
	// Read next message or else timeout
	if (mtReadMessageRaw(buffer, &msgLen) == X_XOK){
		// Message received
		if (buffer[IND_MID] == (mid+1)) {
			// Acknowledge received
			if (buffer[IND_LEN] != EXTLENCODE) {
				dataLen = buffer[IND_LEN];
				memcpy(data, &buffer[IND_DATA0], dataLen);
			}
			else{
				dataLen = buffer[IND_LENEXTH]*256 + buffer[IND_LENEXTL];
				memcpy(data, &buffer[IND_DATAEXT0], dataLen);
			}
			return (m_retVal = X_XOK);				
		}
		else if (buffer[IND_MID] == MID_ERROR){
			m_errorCode = buffer[IND_DATA0];
			return (m_retVal = X_RECVERRORMSG);	// Error message received
		}
		else{
			return (m_retVal = X_UNEXPECTEDMSG);// Unexpected message
		}
	}
	return m_retVal;
}

////////////////////////////////////////////////////////////////////
// mtSetSetting (integer & no param variant)
//
// Sets a integer setting of the device. This setting
// can be an unsigned 1,2 or 4 bytes setting
//
// Input
//	 mid		: Message ID of message to send
//   bid		: Bus ID of message to send (def 0xFF)
//	 value		: Contains the integer value to be used
//	 valuelen	: Length in bytes of the value
//
// Output
//   = X_XOK if an Ack message is received
//	 = X_RECVERRORMSG if an error message is received
//	 = X_TIMEOUT if timeout occurred
//
//
short CXbus::mtSetSetting(const unsigned char mid,
						  const unsigned long value, const unsigned short valuelen,
						  const unsigned char bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;

	msgLen = LEN_MSGHEADER;
	buffer[IND_PREAMBLE] = PREAMBLE;
	buffer[IND_BID] = bid;
	buffer[IND_MID] = mid;
	buffer[IND_LEN] = valuelen;
	swapEndian((unsigned char *)&value, &buffer[msgLen], valuelen);
	calcChecksum(buffer,LEN_MSGHEADER + buffer[IND_LEN]);

	// Send message
	mtSendRawString(buffer, LEN_MSGHEADERCS + buffer[IND_LEN]);

	// Read next received message
	if (mtReadMessageRaw(buffer, &msgLen) == X_XOK){
		// Message received
		if (buffer[IND_MID] == (mid+1)) {
			return (m_retVal = X_XOK);				// Acknowledge received
		}
		else if (buffer[IND_MID] == MID_ERROR){
			m_errorCode = buffer[IND_DATA0];
			return (m_retVal = X_RECVERRORMSG);	// Error message received
		}
	}
	return m_retVal;
}

////////////////////////////////////////////////////////////////////
// mtSetSetting (integer & param variant)
//
// Sets a integer setting of the device. This setting
// can be an unsigned 1,2 or 4 bytes setting
//
// Input
//	 mid		: Message ID of message to send
//	 param		: For messages that need a parameter (optional)
//   bid		: Bus ID of message to send (def 0xFF)
//	 value		: Contains the integer value to be used
//	 valuelen	: Length in bytes of the value
//
// Output
//   = X_XOK if an Ack message is received
//	 = X_RECVERRORMSG if an error message is received
//	 = X_TIMEOUT if timeout occurred
//
//
short CXbus::mtSetSetting(const unsigned char mid, const unsigned char param,
						  const unsigned long value, const unsigned short valuelen,
						  const unsigned char bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;

	msgLen = LEN_MSGHEADER;
	buffer[IND_PREAMBLE] = PREAMBLE;
	buffer[IND_BID] = bid;
	buffer[IND_MID] = mid;
	if (param != 0xFF) {
		msgLen++;
		buffer[IND_LEN] = valuelen+1;
		buffer[IND_DATA0] = param;
	}
	else{
		buffer[IND_LEN] = valuelen;
	}
	swapEndian((unsigned char *)&value, &buffer[msgLen], valuelen);
	calcChecksum(buffer,LEN_MSGHEADER + buffer[IND_LEN]);

	// Send message
	mtSendRawString(buffer, LEN_MSGHEADERCS + buffer[IND_LEN]);

	// Read next received message
	if (mtReadMessageRaw(buffer, &msgLen) == X_XOK){
		// Message received
		if (buffer[IND_MID] == (mid+1)) {
			return (m_retVal = X_XOK);				// Acknowledge received
		}
		else if (buffer[IND_MID] == MID_ERROR){
			m_errorCode = buffer[IND_DATA0];
			return (m_retVal = X_RECVERRORMSG);	// Error message received
		}
	}
	return m_retVal;
}

////////////////////////////////////////////////////////////////////
// mtSetSetting (float & no param variant)
//
// Sets a float setting of the device. 
//
// Input
//	 mid		: Message ID of message to send
//   bid		: Bus ID of message to send (def 0xFF)
//	 value		: Contains the float value to be used
//
// Output
//   = X_XOK if an Ack message is received
//	 = X_RECVERRORMSG if an error message is received
//	 = X_TIMEOUT if timeout occurred
//
short CXbus::mtSetSetting(const unsigned char mid, const float value, const unsigned char bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;

	msgLen = LEN_MSGHEADER;
	buffer[IND_PREAMBLE] = PREAMBLE;
	buffer[IND_BID] = bid;
	buffer[IND_MID] = mid;
	buffer[IND_LEN] = LEN_FLOAT;
	swapEndian((unsigned char *)&value, &buffer[msgLen], LEN_FLOAT);
	calcChecksum(buffer,LEN_MSGHEADER + LEN_FLOAT);

	// Send message
	mtSendRawString(buffer, LEN_MSGHEADERCS + LEN_FLOAT);

	// Read next received message
	if (mtReadMessageRaw(buffer, &msgLen) == X_XOK){
		// Message received
		if (buffer[IND_MID] == (mid+1)) {
			return (m_retVal = X_XOK);				// Acknowledge received
		}
		else if (buffer[IND_MID] == MID_ERROR){
			m_errorCode = buffer[IND_DATA0];
			return (m_retVal = X_RECVERRORMSG);	// Error message received
		}
	}
	return m_retVal;
}

////////////////////////////////////////////////////////////////////
// mtSetSetting (float & param variant)
//
// Sets a float setting of the device. 
//
// Input
//	 mid		: Message ID of message to send
//	 param		: For messages that need a parameter (optional)
//   bid		: Bus ID of message to send (def 0xFF)
//	 value		: Contains the float value to be used
//
// Output
//   = X_XOK if an Ack message is received
//	 = X_RECVERRORMSG if an error message is received
//	 = X_TIMEOUT if timeout occurred
//
//
short CXbus::mtSetSetting(const unsigned char mid, const unsigned char param,
						  const float value, const unsigned char bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;

	msgLen = LEN_MSGHEADER;
	buffer[IND_PREAMBLE] = PREAMBLE;
	buffer[IND_BID] = bid;
	buffer[IND_MID] = mid;
	if (param != 0xFF) {
		msgLen++;
		buffer[IND_LEN] = LEN_FLOAT+1;
		buffer[IND_DATA0] = param;
	}
	else{
		buffer[IND_LEN] = LEN_FLOAT;
	}
	swapEndian((unsigned char *)&value, &buffer[msgLen], LEN_FLOAT);
	calcChecksum(buffer,LEN_MSGHEADER + buffer[IND_LEN]);

	// Send message
	mtSendRawString(buffer, LEN_MSGHEADERCS + buffer[IND_LEN]);

	// Read next received message
	if (mtReadMessageRaw(buffer, &msgLen) == X_XOK){
		// Message received
		if (buffer[IND_MID] == (mid+1)) {
			return (m_retVal = X_XOK);				// Acknowledge received
		}
		else if (buffer[IND_MID] == MID_ERROR){
			m_errorCode = buffer[IND_DATA0];
			return (m_retVal = X_RECVERRORMSG);	// Error message received
		}
	}
	return m_retVal;
}

////////////////////////////////////////////////////////////////////
// mtSetSetting (float & param & store variant)
//
// Sets a float setting of the device and with the Store field
//
// Input
//	 mid		: Message ID of message to send
//	 param		: For messages that need a parameter (optional)
//	 value		: Contains the float value to be used
//	 store		; Store in non-volatile memory (1) or not (0)
//   bid		: Bus ID of message to send (def 0xFF)
//
// Output
//   = X_XOK if an Ack message is received
//	 = X_RECVERRORMSG if an error message is received
//	 = X_TIMEOUT if timeout occurred
//
//
short CXbus::mtSetSetting(const unsigned char mid, const unsigned char param,
						  const float value, const unsigned char store, const unsigned char bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;

	msgLen = LEN_MSGHEADER;
	buffer[IND_PREAMBLE] = PREAMBLE;
	buffer[IND_BID] = bid;
	buffer[IND_MID] = mid;
	if (param != 0xFF) {
		msgLen++;
		buffer[IND_LEN] = LEN_FLOAT+2;
		buffer[IND_DATA0] = param;
	}
	else{
		buffer[IND_LEN] = LEN_FLOAT+1;
	}
	swapEndian((unsigned char *)&value, &buffer[msgLen], LEN_FLOAT);
	buffer[msgLen+LEN_FLOAT] = store;
	calcChecksum(buffer,LEN_MSGHEADER + buffer[IND_LEN]);

	// Send message
	mtSendRawString(buffer, LEN_MSGHEADERCS + buffer[IND_LEN]);

	// Read next received message
	if (mtReadMessageRaw(buffer, &msgLen) == X_XOK){
		// Message received
		if (buffer[IND_MID] == (mid+1)) {
			return (m_retVal = X_XOK);			// Acknowledge received
		}
		else if (buffer[IND_MID] == MID_ERROR){
			m_errorCode = buffer[IND_DATA0];
			return (m_retVal = X_RECVERRORMSG);	// Error message received
		}
	}
	return m_retVal;
}

////////////////////////////////////////////////////////////////////
// mtReadDataMessage
//
// Read a MTData or XMData message from serial port (using TimeOut val)
//
// Input
//   data		: pointer to buffer in which the DATA field of MTData/XMData is stored
//   dataLen	: number of bytes in buffer (= number bytes in DATA field)
// Output
//   returns X_XOK if MTData / XMData message has been read else <>X_XOK
//
// Remarks
//   allocate enough memory for data buffer
//   use setTimeOut for different timeout value
short CXbus::mtReadDataMessage(unsigned char data[], short &dataLen)
{
	unsigned char buffer[MAXMSGLEN];
	short buflen;

	if (mtReadMessageRaw(buffer,&buflen) == X_XOK){
		if (buffer[IND_MID] == MID_MTDATA) {	// MID_XMDATA is same
			if (buffer[IND_LEN] != EXTLENCODE) {
				dataLen = buffer[IND_LEN];
				memcpy(data, &buffer[IND_DATA0], dataLen);
			}
			else{
				dataLen = buffer[IND_LENEXTH]*256 + buffer[IND_LENEXTL];
				memcpy(data, &buffer[IND_DATAEXT0], dataLen);
			}
			return (m_retVal = X_XOK);
		}
		else if (buffer[IND_MID] == MID_ERROR){
			m_errorCode = buffer[IND_DATA0];
			return (m_retVal = X_RECVERRORMSG);	// Error message received
		}
	}
	return m_retVal;
}

////////////////////////////////////////////////////////////////////
// mtWaitForMessage
//
// Read messages from serial port within the timeout period
//  until the received message is equal to a specific message identifier 
//
// Input/Output
//   mid			: message identifier of message that should be returned
//   data			: pointer to buffer in which the data of the requested msg will be stored
//   dataLen		: integer to number of data bytes
//	 bid			: optional, pointer which holds the bid of the returned message
// Output
//   returns X_XOK if the message has been read else != X_XOK
//
// Remarks
//   allocate enough memory for data message buffer
//   use setTimeOut for different timeout value
short CXbus::mtWaitForMessage(const unsigned char mid, unsigned char data[], short *dataLen, unsigned char *bid)
{
	unsigned char buffer[MAXMSGLEN];
	short buflen;

	clock_t clkStart;
	short	defTimeOut = m_timeOut;

	clkStart = clockms();		// Get current processor time

	do{
		if (mtReadMessageRaw(buffer, &buflen) == X_XOK){
			if (buffer[IND_MID] == mid){
				if (bid != NULL) {
					*bid = buffer[IND_BID];
				}
				if (data != NULL && dataLen != NULL) {
					if (buffer[IND_LEN] != EXTLENCODE) {
						*dataLen = buffer[IND_LEN];
						memcpy(data, &buffer[IND_DATA0], *dataLen);
					}
					else{
						*dataLen = buffer[IND_LENEXTH]*256 + buffer[IND_LENEXTL];
						memcpy(data, &buffer[IND_DATAEXT0], *dataLen);
					}
				}
				else if (dataLen != NULL)
				{
					dataLen = 0;
				}
				m_timeOut = defTimeOut;		// Restore timeout value
				return (m_retVal = X_XOK);
			}
		}

		m_timeOut = m_timeOut - (clockms() - clkStart);
	}while (m_timeOut > 0);

	m_timeOut = defTimeOut;					// Restore timeout value
	return (m_retVal = X_TIMEOUT);
}

////////////////////////////////////////////////////////////////////
// mtReadMessage
//
// Reads the next message from serial port. To be read within current
//	time out period
//
// Input
// Output
//	 mid		: MessageID of message received
//	 data	    : array pointer to data bytes (no header)
//	 dataLen    : number of data bytes read
//   bid		: BID or address of message read (optional)
//	
//   returns X_XOK if a message has been read else <>X_XOK
//
// Remarks
//   allocate enough memory for message buffer
//   use setTimeOut for different timeout value
short CXbus::mtReadMessage(unsigned char &mid, unsigned char data[], short &dataLen, unsigned char *bid)
{
	unsigned char buffer[MAXMSGLEN];
	short msgLen;

	if (mtReadMessageRaw(buffer, &msgLen) == X_XOK) {
		// Message read
		mid = buffer[IND_MID];
		if (bid != NULL) {
			*bid = buffer[IND_BID];
		}
		if (buffer[IND_LEN] != EXTLENCODE) {
			dataLen = buffer[IND_LEN];
			memcpy(data, &buffer[IND_DATA0], dataLen);
		}
		else{
			dataLen = buffer[IND_LENEXTH]*256 + buffer[IND_LENEXTL];
			memcpy(data, &buffer[IND_DATAEXT0], dataLen);
		}
	}
	return m_retVal;
}

////////////////////////////////////////////////////////////////////
// mtReadMessageRaw
//
// Read a message from serial port for a certain period
//
// Input
//   msgBuffer		: pointer to buffer in which next msg will be stored
//   msgBufferLength: integer to number of bytes written in buffer (header + data + chksum)
// Output
//   returns X_XOK if a message has been read else <>X_XOK
//
// Remarks
//   allocate enough memory for message buffer
//   use setTimeOut for different timeout value
short CXbus::mtReadMessageRaw(unsigned char *msgBuffer, short *msgBufferLength)
{
	clock_t	clkStart;
	int		state = 0;
	int		nBytesToRead = 1;
	int		nBytesRead = 0;
	int		nOffset = 0;
	int		nMsgDataLen = 0;
	int		nMsgLen;
	unsigned char	chCheckSum;
	bool	Synced = false;

	clkStart = clockms();		// Get current processor time

	do{
		do{
			// First check if we already have some bytes read
			if (nBytesRead > 0 && nBytesToRead > 0) {
				if (nBytesToRead > nBytesRead) {
					nOffset += nBytesRead;
					nBytesToRead -= nBytesRead;
					nBytesRead = 0;
				}
				else{
					nOffset += nBytesToRead;
					nBytesRead -= nBytesToRead;
					nBytesToRead = 0;
				}
			}
			
			// Check if serial port buffer must be read
			if (nBytesToRead > 0) {
				nBytesRead = readSerialPort(msgBuffer+nOffset, nBytesToRead);
				nOffset += nBytesRead;
				nBytesToRead -= nBytesRead;
				nBytesRead = 0;
			}
			
			if (nBytesToRead == 0){
				switch(state){
				case 0:					// Check preamble
					if (msgBuffer[IND_PREAMBLE] == PREAMBLE){
						state++;
						nBytesToRead = 3;
					}
					else{
						nOffset = 0;
						nBytesToRead = 1;
					}
					break;
				case 1:					// Check ADDR, MID, LEN
					if (msgBuffer[IND_LEN] != 0xFF) {
						state = 3;
						nBytesToRead = (nMsgDataLen = msgBuffer[IND_LEN]) + 1; // Read LEN + 1 (chksum)
					}
					else {
						state = 2;
						nBytesToRead = 2;	// Read extended length
					}	
					break;
				case 2:
					state = 3;
					nBytesToRead = (nMsgDataLen = msgBuffer[IND_LENEXTH] * 256 + msgBuffer[IND_LENEXTL]) + 1;	// Read LENEXT + CS
					break;
				case 3:					// Check msg
					chCheckSum = 0;
					nMsgLen = nMsgDataLen + 5 + (msgBuffer[IND_LEN] == 0xFF?2:0);

					for (int i = 1; i < nMsgLen; i++){
						chCheckSum += msgBuffer[i];
					}

					if (chCheckSum == 0){				// Checksum ok?
						*msgBufferLength = nMsgLen;
						Synced = true;
						return (m_retVal = X_XOK);
					}
					else{
						if (!Synced){
							// Not synced - maybe recheck for preamble in this incorrect message
							for (int i = 1; i < nMsgLen; i++) {
								if (msgBuffer[i] == PREAMBLE) {
									// Found a maybe preamble - 'fill buffer'
									nBytesRead = nMsgLen - i;
									memmove(msgBuffer, msgBuffer+i,nBytesRead);
									break;
								}
							}
						}
						Synced = false;
						nOffset = 0;
						state = 0;
						nBytesToRead = 1;			// Start looking for preamble
					}
					break;
				}
			}
		}while (((clockms()-clkStart) < m_timeOut) || nBytesRead != 0);

		// Check if pending message has a valid message
		if (state > 0){
			int i;
			// Search for preamble
			for (i = 1; i < nOffset; i++) {
				if (msgBuffer[i] == PREAMBLE) {
					// Found a maybe preamble - 'fill buffer'
					nBytesRead = nOffset-i-1; // no preamble
					memmove(msgBuffer+1, msgBuffer+i+1,nBytesRead);
					break;
				}
			}
			if (i < nOffset) {
				// Found preamble in message - recheck
				nOffset = 1;
				state = 1;
				nBytesToRead = 3;			// Start looking for preamble
				continue;
			}
		}
		break;
	}while (true);
	
	return (m_retVal = X_TIMEOUT);
}

//////////////////////////////////////////////////////////////////////
// mtReadRawString
//
// Reads characters from serial port
//
// Input
//   rawBuffer		: pointer to buffer in which the received bytes will be stored
//   nBytes			: integer to number of bytes to be read
// Output
//   returns X_XOK if the number of bytes have been read, else X_TIMEOUT
//   use setTimeOut for different timeout value
short CXbus::mtReadRawString(unsigned char *rawBuffer, const short nBytes)
{
	clock_t			clkStart;
	unsigned long	nBytesRead = 0;
	int				nBytesToRead = nBytes;
	int				index = 0;

	clkStart = clockms();		// Get current processor time

	do {
		nBytesRead = readSerialPort(rawBuffer+index, 1);
		index += nBytesRead;
		if (nBytesRead == 1) {
			nBytesToRead--;
			if (nBytesToRead == 0)
				return (m_retVal = X_XOK);
		}
	}while ((clockms()-clkStart) < m_timeOut);

	return (m_retVal = X_TIMEOUT);
}


//////////////////////////////////////////////////////////////////////
// mtSendRawString
//
// Sends characters to serial port without checksum calc
//
// Input
//   msgBuffer	: a pointer to a char buffer containing
//				  the characters to be written to serial port
//   nBytes		: number of buffer bytes to write to serial port
//
// Output
//   number of bytes actually written
short CXbus::mtSendRawString(const unsigned char *msgBuffer, const short nBytes)
{
	int bytesWritten;

	bytesWritten = writeSerialPort(msgBuffer, nBytes);

	return (short)bytesWritten;
}

//////////////////////////////////////////////////////////////////////
// getLastRetVal
//
// Returns the returned value of the last called function
//
// Output
//   Return value
short CXbus::getLastRetVal()
{
	return m_retVal;
}

//////////////////////////////////////////////////////////////////////
// getLastErrorCode
//
// Returns the last reported error of the latest received Error 
//	message
//
// Output
//   Error code
short CXbus::getLastErrorCode()
{
	return m_errorCode;
}

//////////////////////////////////////////////////////////////////////
// setTimeOut
//
// Sets the time out value in milliseconds used by the functions
//
// Output
//   X_XOK is set, X_INVALIDTIMEOUT if time value <= 0
short CXbus::setTimeOut(short timeOutMs)
{
	if (timeOutMs > 0){
		m_timeOut = timeOutMs;
		return X_XOK;
	}
	else
		return X_INVALIDTIMEOUT;
}

//////////////////////////////////////////////////////////////////////
// Useful functions

//////////////////////////////////////////////////////////////////////
// calcChecksum
//
// Calculate and append checksum to msgBuffer
//
void CXbus::calcChecksum(unsigned char *msgBuffer, const int msgBufferLength)
{
	unsigned char checkSum = 0;
	int i;

	for (i = 1; i < msgBufferLength; i++)		
		checkSum += msgBuffer[i];

	msgBuffer[msgBufferLength] = -checkSum;	// Store chksum
}

//////////////////////////////////////////////////////////////////////
// checkChecksum
//
// Checks if message checksum is valid
//
// Output
//   returns true checksum is OK
bool CXbus::checkChecksum(const unsigned char *msgBuffer, const int msgBufferLength)
{
	unsigned char checkSum = 0;
	int i;

	for (i = 1; i < msgBufferLength; i++)
		checkSum += msgBuffer[i];

	if (checkSum == 0) {
		return true;
	}
	else {
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
// swapEndian
//
// Convert 2 or 4 bytes data from little to big endian or back
//
// Input
//	 input	: Pointer to data to be converted
//   output	: Pointer where converted data is stored
//   length	: Length of setting (0,2 & 4)
//
// Remarks:
//	 Allocate enough bytes for output buffer
void CXbus::swapEndian(const unsigned char input[], unsigned char output[], const short length)
{
	switch(length) {
	case 0:
		break;
	case 1:
		output[0] = input[0];
		break;
	case 2:
		output[0] = input[1];
		output[1] = input[0];
		break;
	case 4:
		output[0] = input[3];
		output[1] = input[2];
		output[2] = input[1];
		output[3] = input[0];
		break;
	default:
		break;
	}
}

