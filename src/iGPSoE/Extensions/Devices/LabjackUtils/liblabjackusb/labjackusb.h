/*

Modified

*/



/***************************************************************************************/



//---------------------------------------------------------------------------
//
//  labjackusb.h
// 
//	  Header file for the labjackusb library.
//
//  support@labjack.com
//  Dec 12, 2006
//----------------------------------------------------------------------
//
//  Linux Version History
//
//  0.90 - Initial release (LJUSB_AbortPipe not supported)
//
//  1.00 - Added LJUSB_SetBulkReadTimeout
//
//  1.10 - Changed the HANDLE to a void * (previously int)
//       - Added LJUSB_GetLibraryVersion
//       - Removed UE9_PIPE_EP2_OUT
//       - changed the values of the pipes (incremented by 1)
//       - removed function LJUSB_SetBulkReadTimeout
//       - changed LJUSB_LINUX_DRIVER_VERSION define name to
//         LJUSB_LINUX_LIBRARY_VERSION
//----------------------------------------------------------------------
//

#ifndef _LABJACKUSB_H_
#define _LABJACKUSB_H_

#include "OSCore.h"

#define LJUSB_LINUX_LIBRARY_VERSION 1.10

#ifdef USELESS
typedef void * HANDLE;
typedef unsigned long ULONG;
typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef int BOOL;
typedef unsigned int DWORD;
#endif // USELESS

//Product IDs
#define UE9_PRODUCT_ID    9
#define U3_PRODUCT_ID     3

//The max number of devices that the driver can support.
#define UE9_MAX_DEVICES   16
#define U3_MAX_DEVICES    16

//UE9 pipes to read/write through
#define UE9_PIPE_EP1_OUT  1
#define UE9_PIPE_EP1_IN   2
#define UE9_PIPE_EP2_IN   4

//U3 pipes to read/write through
#define U3_PIPE_EP1_OUT   1
#define U3_PIPE_EP1_IN    2
#define U3_PIPE_EP2_IN    4


#ifdef __cplusplus
extern "C"{
#endif


float LJUSB_GetLibraryVersion();
//Returns the labjackusb library version number


ULONG LJUSB_GetDevCount(ULONG ProductID);
//Returns the total number of LabJack USB devices connected.
//ProductID = The product ID of the devices you want to get the count of.


HANDLE LJUSB_OpenDevice(UINT DevNum, DWORD dwReserved, ULONG ProductID);
//Obtains a handle for a LabJack USB device.  Returns NULL if there is an
//error.
//DevNum = The device number of the LabJack USB device you want to open.  For
//         example, if there is one device connected, set DevNum = 1.  If you
//         have two devices connected, then set DevNum = 1, or DevNum = 2.
//dwReserved = Not used, set to 0.
//ProductID = The product ID of the LabJack USB device.  Currently the U3 and
//            UE9 are supported.


ULONG LJUSB_BulkRead(HANDLE hDevice, ULONG Pipe, BYTE *pBuff, ULONG Count);
//Reads from a bulk endpoint.  Returns the count of the number of bytes read,
//or 0 on error.  If there is no response within a certain amount of time, the
//read will timeout.
//hDevice = Handle of the LabJack USB device.
//Pipe = The pipe you want to read your data through (xxx_PIPE_EP1_IN or
//       xxx_PIPE_EP2_IN).
//*pBuff = Pointer a buffer that will be read from the device.
//Count = The size of the buffer to be read from the device.


ULONG LJUSB_BulkWrite(HANDLE hDevice, ULONG Pipe, BYTE *pBuff, ULONG Count);
//Writes to a bulk endpoint.  Returns the count of the number of bytes wrote,
//or 0 on error.
//hDevice = Handle of the LabJack USB device.
//Pipe = The pipe you want to write your data through (xxx_PIPE_EP1_OUT or
//       xxx_PIPE_EP2_OUT).
//*pBuff = Pointer to the buffer that will be written to the device.
//Count = The size of the buffer to be written to the device.


void LJUSB_CloseDevice(HANDLE hDevice);
//Closes the handle of a LabJack USB device.


BOOL LJUSB_AbortPipe(HANDLE hDevice, ULONG Pipe);
//Not supported under Linux and will return false (0).  The output pipes should
//not stall, and the read pipes will timeout depending on the READ_TIMEOUT set
//in the driver, which by default is 1 sec.


//Note:  For all function errors, use errno to retrieve system error numbers.

#ifdef __cplusplus
}
#endif

#endif
