// Xbus.h: interface for the CXbus class.
//
// Version 1.0.0 - Public version
//
// ----------------------------------------------------------------------------
//  This file is an Xsens Code Example.
//
//  Copyright (C) Xsens Technologies B.V., 2005.  All rights reserved.
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

#if !defined(AFX_XBUS_H__F4580A3F_2CF2_4ED2_A747_B4B015A0328E__INCLUDED_)
#define AFX_XBUS_H__F4580A3F_2CF2_4ED2_A747_B4B015A0328E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
// rajouté par Alain Bertholom le 16/01/2007
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
// fin ajout
#endif

// Field message indexes
#define IND_PREAMBLE				0
#define IND_BID						1
#define IND_MID						2
#define IND_LEN						3
#define IND_DATA0					4
#define IND_LENEXTH					4
#define IND_LENEXTL					5
#define IND_DATAEXT0				6

// Maximum number of sensors supported
#define MAXSENSORS					15

#define PREAMBLE					(const unsigned char)0xFA
#define BID_MT						(const unsigned char)0xFF
#define EXTLENCODE					0xFF

#define LEN_MSGHEADER				(const unsigned short)4
#define LEN_MSGEXTHEADER			(const unsigned short)6
#define LEN_MSGHEADERCS				(const unsigned short)5
#define LEN_MSGEXTHEADERCS			(const unsigned short)7
#define LEN_CHECKSUM				(const unsigned short)1
#define LEN_FLOAT					(const unsigned short)4

// DID Type (high nibble)
#define DID_TYPEH_MASK				(const unsigned long)0x00F00000
#define DID_TYPEH_MT				(const unsigned long)0x00000000
#define DID_TYPEH_XM				(const unsigned long)0x00100000
#define DID_TYPEH_MTI_MTX			(const unsigned long)0x00300000

// All Message identifiers
// WakeUp state messages
#define MID_WAKEUP					(const unsigned char)0x3E
#define MID_WAKEUPACK				(const unsigned char)0x3F

// Config state messages
#define MID_REQDID					(const unsigned char)0x00
#define MID_DEVICEID				(const unsigned char)0x01
#define LEN_DEVICEID				(const unsigned short)4
#define MID_INITBUS					(const unsigned char)0x02
#define MID_INITBUSRESULTS			(const unsigned char)0x03
#define LEN_INITBUSRESULTS			(const unsigned short)4
#define MID_REQPERIOD				(const unsigned char)0x04
#define MID_REQPERIODACK			(const unsigned char)0x05
#define LEN_PERIOD					(const unsigned short)2
#define MID_SETPERIOD				(const unsigned char)0x04
#define MID_SETPERIODACK			(const unsigned char)0x05
// XbusMaster
#define MID_SETBID					(const unsigned char)0x06
#define MID_SETBIDACK				(const unsigned char)0x07
#define MID_AUTOSTART				(const unsigned char)0x06
#define MID_AUTOSTARTACK			(const unsigned char)0x07
#define MID_BUSPWROFF				(const unsigned char)0x08
#define MID_BUSPWROFFACK			(const unsigned char)0x09
// End XbusMaster
#define MID_REQDATALENGTH			(const unsigned char)0x0A
#define MID_DATALENGTH				(const unsigned char)0x0B
#define LEN_DATALENGTH				(const unsigned short)2
#define MID_REQCONFIGURATION		(const unsigned char)0x0C
#define MID_CONFIGURATION			(const unsigned char)0x0D
#define LEN_CONFIGURATION			(const unsigned short)118
#define MID_RESTOREFACTORYDEF		(const unsigned char)0x0E
#define MID_RESTOREFACTORYDEFACK	(const unsigned char)0x0F

#define MID_GOTOMEASUREMENT			(const unsigned char)0x10
#define MID_GOTOMEASUREMENTACK		(const unsigned char)0x11
#define MID_REQFWREV				(const unsigned char)0x12
#define MID_FIRMWAREREV				(const unsigned char)0x13
#define LEN_FIRMWAREREV				(const unsigned short)3
// XbusMaster
#define MID_REQBTDISABLE			(const unsigned char)0x14
#define MID_REQBTDISABLEACK			(const unsigned char)0x15
#define MID_DISABLEBT				(const unsigned char)0x14
#define MID_DISABLEBTACK			(const unsigned char)0x15
#define MID_REQOPMODE				(const unsigned char)0x16
#define MID_REQOPMODEACK			(const unsigned char)0x17
#define MID_SETOPMODE				(const unsigned char)0x16
#define MID_SETOPMODEACK			(const unsigned char)0x17
// End XbusMaster
#define MID_REQBAUDRATE				(const unsigned char)0x18
#define MID_REQBAUDRATEACK			(const unsigned char)0x19
#define LEN_BAUDRATE				(const unsigned short)1
#define MID_SETBAUDRATE				(const unsigned char)0x18
#define MID_SETBAUDRATEACK			(const unsigned char)0x19
// XbusMaster
#define MID_REQSYNCMODE				(const unsigned char)0x1A
#define MID_REQSYNCMODEACK			(const unsigned char)0x1B
#define MID_SETSYNCMODE				(const unsigned char)0x1A
#define MID_SETSYNCMODEACK			(const unsigned char)0x1B
// End XbusMaster

#define MID_REQOUTPUTMODE			(const unsigned char)0xD0
#define MID_REQOUTPUTMODEACK		(const unsigned char)0xD1
#define LEN_OUTPUTMODE		 		(const unsigned short)2
#define MID_SETOUTPUTMODE			(const unsigned char)0xD0
#define MID_SETOUTPUTMODEACK		(const unsigned char)0xD1

#define MID_REQOUTPUTSETTINGS		(const unsigned char)0xD2
#define MID_REQOUTPUTSETTINGSACK	(const unsigned char)0xD3
#define LEN_OUTPUTSETTINGS		 	(const unsigned short)4
#define MID_SETOUTPUTSETTINGS		(const unsigned char)0xD2
#define MID_SETOUTPUTSETTINGSACK	(const unsigned char)0xD3

#define MID_REQOUTPUTSKIPFACTOR		(const unsigned char)0xD4
#define MID_REQOUTPUTSKIPFACTORACK	(const unsigned char)0xD5
#define LEN_OUTPUTSKIPFACTOR		(const unsigned short)2
#define MID_SETOUTPUTSKIPFACTOR		(const unsigned char)0xD4
#define MID_SETOUTPUTSKIPFACTORACK	(const unsigned char)0xD5

#define MID_REQSYNCINSETTINGS		(const unsigned char)0xD6
#define MID_REQSYNCINSETTINGSACK	(const unsigned char)0xD7
#define LEN_SYNCINMODE				(const unsigned short)2
#define LEN_SYNCINSKIPFACTOR		(const unsigned short)2
#define LEN_SYNCINOFFSET			(const unsigned short)4
#define MID_SETSYNCINSETTINGS		(const unsigned char)0xD6
#define MID_SETSYNCINSETTINGSACK	(const unsigned char)0xD7

#define MID_REQSYNCOUTSETTINGS		(const unsigned char)0xD8
#define MID_REQSYNCOUTSETTINGSACK	(const unsigned char)0xD9
#define LEN_SYNCOUTMODE				(const unsigned short)2
#define LEN_SYNCOUTSKIPFACTOR		(const unsigned short)2
#define LEN_SYNCOUTOFFSET			(const unsigned short)4
#define LEN_SYNCOUTPULSEWIDTH		(const unsigned short)4
#define MID_SETSYNCOUTSETTINGS		(const unsigned char)0xD8
#define MID_SETSYNCOUTSETTINGSACK	(const unsigned char)0xD9

#define MID_REQERRORMODE			(const unsigned char)0xDA
#define MID_REQERRORMODEACK			(const unsigned char)0xDB
#define LEN_ERRORMODE				(const unsigned short)2
#define MID_SETERRORMODE			(const unsigned char)0xDA
#define MID_SETERRORMODEACK			(const unsigned char)0xDB

#define MID_REQHEADING				(const unsigned char)0x82
#define MID_REQHEADINGACK			(const unsigned char)0x83
#define LEN_HEADING		 			(const unsigned short)4
#define MID_SETHEADING				(const unsigned char)0x82
#define MID_SETHEADINGACK			(const unsigned char)0x83

#define MID_REQLOCATIONID			(const unsigned char)0x84
#define MID_REQLOCATIONIDACK		(const unsigned char)0x85
#define LEN_LOCATIONID				(const unsigned short)2
#define MID_SETLOCATIONID			(const unsigned char)0x84
#define MID_SETLOCATIONIDACK		(const unsigned char)0x85

#define MID_REQEXTOUTPUTMODE		(const unsigned char)0x86
#define MID_REQEXTOUTPUTMODEACK		(const unsigned char)0x87
#define LEN_EXTOUTPUTMODE			(const unsigned short)2
#define MID_SETEXTOUTPUTMODE		(const unsigned char)0x86
#define MID_SETEXTOUTPUTMODEACK		(const unsigned char)0x87

#define MID_REQINITTRACKMODE		(const unsigned char)0x88
#define MID_REQINITTRACKMODEACK		(const unsigned char)0x89
#define LEN_INITTRACKMODE			(const unsigned short)2
#define MID_SETINITTRACKMODE		(const unsigned char)0x88
#define MID_SETINITTRACKMODEACK		(const unsigned char)0x89

#define MID_STOREFILTERSTATE		(const unsigned char)0x8A
#define MID_STOREFILTERSTATEACK		(const unsigned char)0x8B

// Measurement state
#define MID_GOTOCONFIG				(const unsigned char)0x30
#define MID_GOTOCONFIGACK			(const unsigned char)0x31
#define MID_BUSDATA					(const unsigned char)0x32
#define MID_MTDATA					(const unsigned char)0x32

// Manual
#define MID_PREPAREDATA				(const unsigned char)0x32
#define MID_REQDATA					(const unsigned char)0x34
#define MID_REQDATAACK				(const unsigned char)0x35

// MTData defines 
// Length of data blocks in bytes
#define LEN_RAWDATA					(const unsigned short)20
#define LEN_CALIBDATA				(const unsigned short)36
#define LEN_ORIENT_QUATDATA			(const unsigned short)16
#define LEN_ORIENT_EULERDATA		(const unsigned short)12
#define LEN_ORIENT_MATRIXDATA		(const unsigned short)36
#define LEN_SAMPLECNT				(const unsigned short)2

// Length of data blocks in floats
#define LEN_CALIBDATA_FLT			(const unsigned short)9
#define LEN_ORIENT_QUATDATA_FLT		(const unsigned short)4
#define LEN_ORIENT_EULERDATA_FLT	(const unsigned short)3
#define LEN_ORIENT_MATRIXDATA_FLT	(const unsigned short)9

// Indices of fields in DATA field of MTData message in bytes
// use in combination with LEN_CALIB etc
// Un-calibrated raw data
#define IND_RAW_ACCX				0
#define IND_RAW_ACCY				2
#define IND_RAW_ACCZ				4
#define IND_RAW_GYRX				6
#define IND_RAW_GYRY				8
#define IND_RAW_GYRZ				10
#define IND_RAW_MAGX				12
#define IND_RAW_MAGY				14
#define IND_RAW_MAGZ				16
#define IND_RAW_TEMP				18
// Calibrated data
#define IND_CALIB_ACCX				0
#define IND_CALIB_ACCY				4
#define IND_CALIB_ACCZ				8
#define IND_CALIB_GYRX				12
#define IND_CALIB_GYRY				16
#define IND_CALIB_GYRZ				20
#define IND_CALIB_MAGX				24
#define IND_CALIB_MAGY				28
#define IND_CALIB_MAGZ				32
// Orientation data - quat
#define IND_ORIENT_Q0				0
#define IND_ORIENT_Q1				4
#define IND_ORIENT_Q2				8
#define IND_ORIENT_Q3				12
// Orientation data - euler
#define IND_ORIENT_ROLL				0
#define IND_ORIENT_PITCH			4
#define IND_ORIENT_YAW				8
// Orientation data - matrix
#define IND_ORIENT_A				0
#define IND_ORIENT_B				4
#define IND_ORIENT_C				8
#define IND_ORIENT_D				12
#define IND_ORIENT_E				16
#define IND_ORIENT_F				20
#define IND_ORIENT_G				24
#define IND_ORIENT_H				28
#define IND_ORIENT_I				32
// Orientation data - euler
#define IND_SAMPLECNTH				0
#define IND_SAMPLECNTL				1

// Indices of fields in DATA field of MTData message
// Un-calibrated raw data
#define FLDNUM_RAW_ACCX				0
#define FLDNUM_RAW_ACCY				1
#define FLDNUM_RAW_ACCZ				2
#define FLDNUM_RAW_GYRX				3
#define FLDNUM_RAW_GYRY				4
#define FLDNUM_RAW_GYRZ				5
#define FLDNUM_RAW_MAGX				6
#define FLDNUM_RAW_MAGY				7
#define FLDNUM_RAW_MAGZ				8
#define FLDNUM_RAW_TEMP				9
// Calibrated data
#define FLDNUM_CALIB_ACCX			0
#define FLDNUM_CALIB_ACCY			1
#define FLDNUM_CALIB_ACCZ			2
#define FLDNUM_CALIB_GYRX			3
#define FLDNUM_CALIB_GYRY			4
#define FLDNUM_CALIB_GYRZ			5
#define FLDNUM_CALIB_MAGX			6
#define FLDNUM_CALIB_MAGY			7
#define FLDNUM_CALIB_MAGZ			8
// Orientation data - quat
#define FLDNUM_ORIENT_Q0			0
#define FLDNUM_ORIENT_Q1			1
#define FLDNUM_ORIENT_Q2			2
#define FLDNUM_ORIENT_Q3			3
// Orientation data - euler
#define FLDNUM_ORIENT_ROLL			0
#define FLDNUM_ORIENT_PITCH			1
#define FLDNUM_ORIENT_YAW			2
// Orientation data - matrix
#define FLDNUM_ORIENT_A				0
#define FLDNUM_ORIENT_B				1
#define FLDNUM_ORIENT_C				2
#define FLDNUM_ORIENT_D				3
#define FLDNUM_ORIENT_E				4
#define FLDNUM_ORIENT_F				5
#define FLDNUM_ORIENT_G				6
#define FLDNUM_ORIENT_H				7
#define FLDNUM_ORIENT_I				8
// Length
// Uncalibrated raw data
#define LEN_RAW_ACCX				2
#define LEN_RAW_ACCY				2
#define LEN_RAW_ACCZ				2
#define LEN_RAW_GYRX				2
#define LEN_RAW_GYRY				2
#define LEN_RAW_GYRZ				2
#define LEN_RAW_MAGX				2
#define LEN_RAW_MAGY				2
#define LEN_RAW_MAGZ				2
#define LEN_RAW_TEMP				2
// Calibrated data
#define LEN_CALIB_ACCX				4
#define LEN_CALIB_ACCY				4
#define LEN_CALIB_ACCZ				4
#define LEN_CALIB_GYRX				4
#define LEN_CALIB_GYRY				4
#define LEN_CALIB_GYRZ				4
#define LEN_CALIB_MAGX				4
#define LEN_CALIB_MAGY				4
#define LEN_CALIB_MAGZ				4
// Orientation data - quat
#define LEN_ORIENT_Q0				4
#define LEN_ORIENT_Q1				4
#define LEN_ORIENT_Q2				4
#define LEN_ORIENT_Q3				4
// Orientation data - euler
#define LEN_ORIENT_ROLL				4
#define LEN_ORIENT_PITCH			4
#define LEN_ORIENT_YAW				4
// Orientation data - matrix
#define LEN_ORIENT_A				4
#define LEN_ORIENT_B				4
#define LEN_ORIENT_C				4
#define LEN_ORIENT_D				4
#define LEN_ORIENT_E				4
#define LEN_ORIENT_F				4
#define LEN_ORIENT_G				4
#define LEN_ORIENT_H				4
#define LEN_ORIENT_I				4
// Sample counter

// Valid in all states
#define MID_RESET					(const unsigned char)0x40
#define MID_RESETACK				(const unsigned char)0x41
#define MID_ERROR					(const unsigned char)0x42
#define LEN_ERROR					(const unsigned short)1
// XbusMaster
#define MID_XMPWROFF				(const unsigned char)0x44
// End XbusMaster

#define MID_REQFILTERSETTINGS		(const unsigned char)0xA0
#define MID_REQFILTERSETTINGSACK	(const unsigned char)0xA1
#define LEN_FILTERSETTINGS			(const unsigned short)4
#define MID_SETFILTERSETTINGS		(const unsigned char)0xA0
#define MID_SETFILTERSETTINGSACK	(const unsigned char)0xA1
#define MID_REQAMD					(const unsigned char)0xA2
#define MID_REQAMDACK				(const unsigned char)0xA3
#define LEN_AMD						(const unsigned short)2
#define MID_SETAMD					(const unsigned char)0xA2
#define MID_SETAMDACK				(const unsigned char)0xA3
#define MID_RESETORIENTATION		(const unsigned char)0xA4
#define MID_RESETORIENTATIONACK		(const unsigned char)0xA5
#define LEN_RESETORIENTATION		(const unsigned short)2

// All Messages
// WakeUp state messages
#define MSG_WAKEUPLEN				5
#define MSG_WAKEUPACK				(const unsigned char *)"\xFA\xFF\x3F\x00"
#define MSG_WAKEUPACKLEN			4
// Config state messages
#define MSG_REQDID					(const unsigned char *)"\xFA\xFF\x00\x00"
#define MSG_REQDIDLEN				4
#define MSG_DEVICEIDLEN				9
#define MSG_INITBUS					(const unsigned char *)"\xFA\xFF\x02\x00"
#define MSG_INITBUSLEN				4
#define MSG_INITBUSRESMAXLEN		(5 + 4 * MAXSENSORS)
#define MSG_REQPERIOD				(const unsigned char *)"\xFA\xFF\x04\x00"
#define MSG_REQPERIODLEN			4
#define MSG_REQPERIODACKLEN			7
#define MSG_SETPERIOD				(const unsigned char *)"\xFA\xFF\x04\x02"
#define MSG_SETPERIODLEN			6
#define MSG_SETPERIODACKLEN			5
#define MSG_SETBID					(const unsigned char *)"\xFA\xFF\x06\x05"
#define MSG_SETBIDLEN				9
#define MSG_SETBIDACKLEN			5
#define MSG_AUTOSTART				(const unsigned char *)"\xFA\xFF\x06\x00"
#define MSG_AUTOSTARTLEN			4
#define MSG_AUTOSTARTACKLEN			5
#define MSG_BUSPWROFF				(const unsigned char *)"\xFA\xFF\x08\x00"
#define MSG_BUSPWROFFLEN			4
#define MSG_BUSPWROFFACKLEN			5
#define MSG_RESTOREFACTORYDEF		(const unsigned char *)"\xFA\xFF\x0E\x00"
#define MSG_RESTOREFACTORYDEFLEN	4
#define MSG_RESTOREFACTORYDEFACKLEN	5
#define MSG_REQDATALENGTH			(const unsigned char *)"\xFA\xFF\x0A\x00"
#define MSG_REQDATALENGTHLEN		4
#define MSG_DATALENGTHLEN			7
#define MSG_REQCONFIGURATION		(const unsigned char *)"\xFA\xFF\x0C\x00"
#define MSG_REQCONFIGURATIONLEN		4
#define MSG_GOTOMEASUREMENT			(const unsigned char *)"\xFA\xFF\x10\x00"
#define MSG_GOTOMEASUREMENTLEN		4
#define MSG_GOTOMEASMAN				(const unsigned char *)"\xFA\x01\x10\x00"
#define MSG_GOTOMEASMANLEN			4
#define MSG_GOTOMEASACKLEN			5
#define MSG_REQFWREV				(const unsigned char *)"\xFA\xFF\x12\x00"
#define MSG_REQFWREVLEN				4
#define MSG_FIRMWAREREVLEN			8
#define MSG_REQBTDISABLED			(const unsigned char *)"\xFA\xFF\x14\x00"
#define MSG_REQBTDISABLEDLEN		4
#define MSG_REQBTDISABLEDACKLEN		6
#define MSG_DISABLEBT				(const unsigned char *)"\xFA\xFF\x14\x01"
#define MSG_DISABLEBTLEN			5
#define MSG_DISABLEBTACKLEN			5
#define MSG_REQOPMODE				(const unsigned char *)"\xFA\xFF\x16\x00"
#define MSG_REQOPMODELEN			4
#define MSG_REQOPMODEACKLEN			6
#define MSG_SETOPMODE				(const unsigned char *)"\xFA\xFF\x16\x01"
#define MSG_SETOPMODELEN			5
#define MSG_SETOPMODEACKLEN			5
#define MSG_REQBAUDRATE				(const unsigned char *)"\xFA\xFF\x18\x00"
#define MSG_REQBAUDRATELEN			4
#define MSG_REQBAUDRATEACKLEN		6	
#define MSG_SETBAUDRATE				(const unsigned char *)"\xFA\xFF\x18\x01"
#define MSG_SETBAUDRATELEN			5
#define MSG_SETBAUDRATEACKLEN		5
#define MSG_REQSYNCMODE				(const unsigned char *)"\xFA\xFF\x1A\x00"
#define MSG_REQSYNCMODELEN			4
#define MSG_REQSYNCMODEACKLEN		6
#define MSG_SETSYNCMODE				(const unsigned char *)"\xFA\xFF\x1A\x01"
#define MSG_SETSYNCMODELEN			5
#define MSG_SETSYNCMODEACKLEN		6
#define MSG_REQMTS					(const unsigned char *)"\xFA\xFF\x90\x01"
#define MSG_REQMTSLEN				5
#define MSG_MTSDATA					61
#define MSG_STORECUSMTS				(const unsigned char *)"\xFA\xFF\x92\x58"
#define MSG_STORECUSMTSLEN			92
#define MSG_STORECUSMTSACKLEN		5
#define MSG_REVTOORGMTS				(const unsigned char *)"\xFA\xFF\x94\x00"
#define MSG_REVTOORGMTSLEN			4
#define MSG_REVTOORGMTSACKLEN		5
#define MSG_STOREMTS				(const unsigned char *)"\xFA\xFF\x96\x41"
#define MSG_STOREMTSLEN				69
#define MSG_STOREMTSACKLEN			5
#define MSG_REQSYNCOUTMODE			(const unsigned char *)"\xFA\xFF\xD8\x01\x00"
#define MSG_REQSYNCOUTMODELEN		5
#define MSG_REQSYNCOUTSKIPFACTOR	(const unsigned char *)"\xFA\xFF\xD8\x01\x01"
#define MSG_REQSYNCOUTSKIPFACTORLEN	5
#define MSG_REQSYNCOUTOFFSET		(const unsigned char *)"\xFA\xFF\xD8\x01\x02"
#define MSG_REQSYNCOUTOFFSETLEN		5
#define MSG_REQSYNCOUTPULSEWIDTH	(const unsigned char *)"\xFA\xFF\xD8\x01\x03"
#define MSG_REQSYNCOUTPULSEWIDTHLEN	5
#define MSG_REQERRORMODE			(const unsigned char *)"\xFA\xFF\xDA\x00"
#define MSG_REQERRORMODELEN			4
#define MSG_REQERRORMODEACKLEN		7
// Measurement state - auto messages
#define MSG_GOTOCONFIG				(const unsigned char *)"\xFA\xFF\x30\x00"
#define MSG_GOTOCONFIGLEN			4
#define MSG_GOTOCONFIGACKLEN		5
// Measurement state - manual messages (Use DID = 0x01)
#define MSG_GOTOCONFIGM				(const unsigned char *)"\xFA\x01\x30\x00"
#define MSG_GOTOCONFIGMLEN			4
#define MSG_GOTOCONFIGMACKLEN		5
#define MSG_PREPAREDATA				(const unsigned char *)"\xFA\x01\x32\x00"
#define MSG_PREPAREDATALEN			4
#define MSG_REQDATA					(const unsigned char *)"\xFA\x01\x34\x00"
#define MSG_REQDATALEN				4
// Valid in all states
#define MSG_RESET					(const unsigned char *)"\xFA\xFF\x40\x00"
#define MSG_RESETLEN				4
#define MSG_RESETACKLEN				5
#define MSG_XMPWROFF				(const unsigned char *)"\xFA\xFF\x44\x00"
#define MSG_XMPWROFFLEN				4
#define MSG_XMPWROFFACKLEN			5

// Message length
#define MAXMSGLEN					(const unsigned short)2048+7
#define MAXSHORTMSGLEN				(const unsigned short)254+5

// Additional baudrate defines (initSerialPort)
#define CBR_230400					230400
#define CBR_460800					460800
#define CBR_921600					921600

// Baudrate defines for SetBaudrate message
#define BAUDRATE_9K6				0x09
#define BAUDRATE_14K4				0x08
#define BAUDRATE_19K2				0x07
#define BAUDRATE_28K8				0x06
#define BAUDRATE_38K4				0x05
#define BAUDRATE_57K6				0x04
#define BAUDRATE_76K8				0x03
#define BAUDRATE_115K2				0x02
#define BAUDRATE_230K4				0x01
#define BAUDRATE_460K8				0x00
#define BAUDRATE_921K6				0x80

// Xbus protocol error codes (Error)
#define ERROR_PERIODINVALID			0x03
#define ERROR_MESSAGEINVALID		0x04
#define ERROR_TIMEROVERFLOW			0x1E
#define ERROR_BAUDRATEINVALID		0x20
#define ERROR_PARAMETERINVALID		0x21

// Error modes (SetErrorMode)
#define ERRORMODE_IGNORE					0x0000
#define ERRORMODE_INCSAMPLECNT				0x0001
#define ERRORMODE_INCSAMPLECNT_SENDERROR	0x0002
#define ERRORMODE_SENDERROR_GOTOCONFIG		0x0003

// Configuration message defines
#define CONF_MASTERDID				0
#define CONF_PERIOD					4
#define CONF_OUTPUTSKIPFACTOR		6
#define CONF_SYNCIN_MODE			8
#define CONF_SYNCIN_SKIPFACTOR		10
#define CONF_SYNCIN_OFFSET			12
#define CONF_DATE					16
#define CONF_TIME					24
#define CONF_NUMSENSORS				96
// Configuration sensor block properties
#define CONF_DID					98
#define CONF_DATALENGTH				102
#define CONF_OUTPUTMODE				104
#define CONF_OUTPUTSETTINGS			106
#define	CONF_BLOCKLEN				20
// To calculate the offset in data field for output mode of sensor #2 use
//		CONF_OUTPUTMODE + 1*CONF_BLOCKLEN
#define CONF_MASTERDIDLEN			4
#define CONF_PERIODLEN				2
#define CONF_OUTPUTSKIPFACTORLEN	2
#define CONF_SYNCIN_MODELEN			2
#define CONF_SYNCIN_SKIPFACTORLEN	2
#define CONF_SYNCIN_OFFSETLEN		4
#define CONF_DATELEN				8
#define CONF_TIMELEN				8
#define CONF_NUMSENSORSLEN			2
// Configuration sensor block properties
#define CONF_DIDLEN					4
#define CONF_DATALENGTHLEN			2
#define CONF_OUTPUTMODELEN			2
#define CONF_OUTPUTSETTINGSLEN		4

// SyncIn params
#define PARAM_SYNCIN_MODE			(const unsigned char)0x00
#define PARAM_SYNCIN_SKIPFACTOR		(const unsigned char)0x01
#define PARAM_SYNCIN_OFFSET			(const unsigned char)0x02

// SyncIn mode
#define SYNCIN_DISABLED				0x0000
#define SYNCIN_EDGE_RISING			0x0001
#define SYNCIN_EDGE_FALLING			0x0002
#define SYNCIN_EDGE_BOTH			0x0003
#define SYNCIN_TYPE_SENDLASTDATA	0x0004
#define SYNCIN_TYPE_DOSAMPLING		0x0000
#define SYNCIN_EDGE_MASK			0x0003
#define SYNCIN_TYPE_MASK			0x000C

// SyncOut params
#define PARAM_SYNCOUT_MODE			(const unsigned char)0x00
#define PARAM_SYNCOUT_SKIPFACTOR	(const unsigned char)0x01
#define PARAM_SYNCOUT_OFFSET		(const unsigned char)0x02
#define PARAM_SYNCOUT_PULSEWIDTH	(const unsigned char)0x03

// SyncOut mode
#define SYNCOUT_DISABLED		0x0000
#define SYNCOUT_TYPE_TOGGLE		0x0001
#define SYNCOUT_TYPE_PULSE		0x0002
#define SYNCOUT_POL_NEG			0x0000
#define SYNCOUT_POL_POS			0x0010
#define SYNCOUT_TYPE_MASK		0x000F
#define SYNCOUT_POL_MASK		0x0010

// Sample frequencies (SetPeriod)
#define PERIOD_10HZ				0x2D00
#define PERIOD_12HZ				0x2580
#define PERIOD_15HZ				0x1E00
#define PERIOD_16HZ				0x1C20
#define PERIOD_18HZ				0x1900
#define PERIOD_20HZ				0x1680
#define PERIOD_24HZ				0x12C0
#define PERIOD_25HZ				0x1200
#define PERIOD_30HZ				0x0F00
#define PERIOD_32HZ				0x0E10
#define PERIOD_36HZ				0x0C80
#define PERIOD_40HZ				0x0B40
#define PERIOD_45HZ				0x0A00
#define PERIOD_48HZ				0x0960
#define PERIOD_50HZ				0x0900
#define PERIOD_60HZ				0x0780
#define PERIOD_64HZ				0x0708
#define PERIOD_72HZ				0x0640
#define PERIOD_75HZ				0x0600
#define PERIOD_80HZ				0x05A0
#define PERIOD_90HZ				0x0500
#define PERIOD_96HZ				0x04B0
#define PERIOD_100HZ			0x0480
#define PERIOD_120HZ			0x03C0
#define PERIOD_128HZ			0x0384
#define PERIOD_144HZ			0x0320
#define PERIOD_150HZ			0x0300
#define PERIOD_160HZ			0x02D0
#define PERIOD_180HZ			0x0280
#define PERIOD_192HZ			0x0258
#define PERIOD_200HZ			0x0240
#define PERIOD_225HZ			0x0200
#define PERIOD_240HZ			0x01E0
#define PERIOD_256HZ			0x01C2
#define PERIOD_288HZ			0x0190
#define PERIOD_300HZ			0x0180
#define PERIOD_320HZ			0x0168
#define PERIOD_360HZ			0x0140
#define PERIOD_384HZ			0x012C
#define PERIOD_400HZ			0x0120
#define PERIOD_450HZ			0x0100
#define PERIOD_480HZ			0x00F0
#define PERIOD_512HZ			0x00E1

// OutputModes
#define OUTPUTMODE_RAW				0x4000
#define OUTPUTMODE_CALIB			0x0002
#define OUTPUTMODE_ORIENT			0x0004

// OutputSettings
#define OUTPUTSETTINGS_NOTIMESTAMP		0x00000000
#define OUTPUTSETTINGS_SAMPLECNT		0x00000001
#define OUTPUTSETTINGS_QUATERNION		0x00000000
#define OUTPUTSETTINGS_EULER			0x00000004
#define OUTPUTSETTINGS_MATRIX			0x00000008
#define OUTPUTSETTINGS_TIMESTAMP_MASK	0x00000003
#define OUTPUTSETTINGS_ORIENTMODE_MASK	0x0000000C

// Initial tracking mode (SetInitTrackMode)
#define INITTRACKMODE_DISABLED		0x0000
#define INITTRACKMODE_ENABLED		0x0001

// Filter settings params
#define PARAM_FILTER_GAIN			(const unsigned char)0x00
#define PARAM_FILTER_RHO			(const unsigned char)0x01
#define DONOTSTORE					0x00
#define STORE						0x01

// AMDSetting (SetAMD)
#define AMDSETTING_DISABLED			0x0000
#define AMDSETTING_ENABLED			0x0001

// Reset orientation message type
#define RESETORIENTATION_STORE		0
#define RESETORIENTATION_HEADING	1
#define RESETORIENTATION_GLOBAL		2
#define RESETORIENTATION_OBJECT		3
#define RESETORIENTATION_ALIGN		4

// Send raw string mode
#define SENDRAWSTRING_INIT			0
#define SENDRAWSTRING_DEFAULT		1
#define SENDRAWSTRING_SEND			2

// Timeouts	
#define TO_DEFAULT					100
#define TO_INIT						250

// Return values
// modifié par Alain Bertholom le 16/01/2007
//#define X_OK						0
#define X_XOK					0
// fin modif
#define X_TIMEOUT					1
#define X_COMPORTNOTOPEN			2
#define X_CHECKSUMFAULT				3
#define X_NODATA					4
#define X_RECVERRORMSG				5
#define X_OUTOFMEMORY				6
#define X_UNKNOWDATA				7
#define X_INVALIDTIMEOUT			8
#define X_UNEXPECTEDMSG				9

class CXbus  
{
public:
	CXbus();
	virtual ~CXbus();

	// Low level general functions
	clock_t clockms();

	// Low level COM port functions
// modifié par Alain Bertholom le 16/01/2007
	//short	initSerialPort(const int portNumber, const unsigned long baudrate = CBR_115200, const unsigned long inqueueSize = 4096, const unsigned long outqueueSize = 1024);
	short	initSerialPort(const int portNumber, const unsigned long baudrate, const unsigned long inqueueSize = 4096, const unsigned long outqueueSize = 1024);

	bool	isPortOpen();

	int		writeSerialPort(const unsigned char *msgBuffer, const int nBytesToWrite);

	int		readSerialPort(unsigned char *msgBuffer, const int nBytesToRead);	
	
	void	flushSerialPort();

	void	closeSerialPort();
	
	void	setSerialPortQueueSize(const unsigned long inqueueSize = 4096, const unsigned long outqueueSize = 1024);

	// High-level Motion Tracker / Xbus Master functions
	short	mtReqSetting(const unsigned char mid, unsigned long &value, const unsigned char bid = 0xFF);

	short	mtReqSetting(const unsigned char mid, const unsigned char param, unsigned long &value,  
							  const unsigned char bid = 0xFF);

	short	mtReqSetting(const unsigned char mid, float &value, const unsigned char bid = 0xFF);

	short	mtReqSetting(const unsigned char mid, const unsigned char param, float &value,
							  const unsigned char bid = 0xFF);

	short	mtReqSetting(const unsigned char mid, 
							  unsigned char data[], short &dataLen, const unsigned char bid = 0xFF);

	short	mtReqSetting(const unsigned char mid, const unsigned char param, 
							  unsigned char data[], short &dataLen, const unsigned char bid = 0xFF);

	short	mtSetSetting(const unsigned char mid,
							  const unsigned long value, const unsigned short valuelen,
							  const unsigned char bid = 0xFF);

	short	mtSetSetting(const unsigned char mid, const unsigned char param,
							  const unsigned long value, const unsigned short valuelen,
							  const unsigned char bid = 0xFF);

	short	mtSetSetting(const unsigned char mid, const float value, const unsigned char bid = 0xFF);

	short	mtSetSetting(const unsigned char mid, const unsigned char param,
							  const float value, const unsigned char bid = 0xFF);

	short	mtSetSetting(const unsigned char mid, const unsigned char param,
							  const float value, const unsigned char store, const unsigned char bid = 0xFF);

	short	mtReadDataMessage(unsigned char data[], short &dataLen);

	short	mtWaitForMessage(const unsigned char mid, unsigned char data[] = NULL, short *dataLen = NULL, unsigned char *bid = NULL);

	short   mtReadMessage(unsigned char &mid, unsigned char data[], short &dataLen, unsigned char *bid = NULL);

	short	mtSendMessage(const unsigned char mid, const unsigned long dataValue = 0, 
						  const unsigned char dataValueLen = 0, const unsigned char bid = 0xFF);
	
	short   mtSendMessage(const unsigned char mid, const unsigned char data[], 
							   const unsigned short &dataLen, const unsigned char bid = 0xFF);

	// low-level Motion Tracker / Xbus Master functions
	short	mtReadMessageRaw(unsigned char *msgBuffer, short *msgBufferLength);

	short	mtReadRawString(unsigned char *msgBuffer, const short nBytes);

	short	mtSendRawString(const unsigned char *msgBuffer, const short nBytes);

	// Generic Xbus functions
	short	getLastErrorCode();

	short	getLastRetVal();

	short	setTimeOut(short timeOutMs);

	void	calcChecksum(unsigned char *msgBuffer, const int msgBufferLength);

	bool	checkChecksum(const unsigned char *msgBuffer, const int msgBufferLength);

	void	swapEndian(const unsigned char input[], unsigned char output[], const short length);

protected:
	// COM port handle
#ifdef _WIN32
	HANDLE	m_serialPort;
#else
	int		m_serialPort;
#endif
	bool	m_portOpen;
	short	m_errorCode;
	short	m_retVal;
	short	m_timeOut;
private:
};

#endif // !defined(AFX_XBUS_H__F4580A3F_2CF2_4ED2_A747_B4B015A0328E__INCLUDED_)
