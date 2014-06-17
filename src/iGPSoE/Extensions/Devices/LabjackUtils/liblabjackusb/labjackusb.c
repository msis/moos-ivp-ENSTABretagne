/*

Modified

*/



/***************************************************************************************/



//---------------------------------------------------------------------------
//
//  labjackusb.c
//
//    Library for accessing a U3 and UE9 over USB.
//
//  support@labjack.com
//  Dec 12, 2006
//----------------------------------------------------------------------
//

#include "labjackusb.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


const char *ue9DeviceFileName = "/dev/usb/labjackue9_";
const char *u3DeviceFileName = "/dev/usb/labjacku3_";
int ue9FdArray[UE9_MAX_DEVICES] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int u3FdArray[U3_MAX_DEVICES] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};


//handle to file descriptor
int handleToFD(void * hDevice)
{
  //Check if the handle is with in the bounds of the fdArray to prevent a segfault
  if ( ((int *)hDevice <= &ue9FdArray[UE9_MAX_DEVICES-1] && (int *)hDevice >= &ue9FdArray[0]) ||
      ((int *)hDevice <= &u3FdArray[U3_MAX_DEVICES-1] && (int *)hDevice >= &u3FdArray[0]) )
  {
    return *(int *)hDevice;
  }
  else
  {
    errno = ENODEV;
    return -1;
  }
}


float LJUSB_GetLibraryVersion() 
{
  return LJUSB_LINUX_LIBRARY_VERSION;
}


HANDLE LJUSB_OpenDevice(UINT DevNum, DWORD dwReserved, ULONG ProductID)
{
  UINT deviceCount = 0;
  ULONG maxDevices = 0;
  char deviceFileName[24];
  char fullDeviceFileName[30];
  int i = 0;
  int fd;
  int *fdArray;
 
  //Matching a product ID and setting the max devices and device file name 
  //prefix
  if (ProductID == UE9_PRODUCT_ID)
  {
    maxDevices = UE9_MAX_DEVICES;
    sprintf(deviceFileName, "%s", ue9DeviceFileName);
    fdArray = ue9FdArray;
  }
  else if (ProductID == U3_PRODUCT_ID)
  {
    maxDevices = U3_MAX_DEVICES;
    sprintf(deviceFileName, "%s", u3DeviceFileName);
    fdArray = u3FdArray;
  }
  else
    goto Invalid;

  if (DevNum > maxDevices || DevNum < 1)
    goto Invalid;	

  //Finding device specified by devNum
  for (i = 0; i < (int)maxDevices; i++) 
  {
    sprintf(fullDeviceFileName, "%s%d", deviceFileName, i);
    if ((fd = open(fullDeviceFileName, O_RDWR)) != -1)
    {
      if (fdArray[i] != -1 && fdArray[i] != fd)
        close(fdArray[i]);
      fdArray[i] = fd;

      deviceCount++;

      if (deviceCount == DevNum)
        return (void *)&fdArray[i];
    }
    else
      fdArray[i] = 0;
  }

  return NULL;

Invalid:
  errno = EINVAL;
  return NULL;
}


ULONG LJUSB_BulkRead(HANDLE hDevice, ULONG Pipe, BYTE *pBuff, ULONG Count)
{
  long bytesRead = -1;
  ULONG ret = 0;
  int fd = -1;

  if ((fd = handleToFD(hDevice)) != -1)
  {
    //Checking endpoint pipe and performing the appropriate read
    if (Pipe == UE9_PIPE_EP1_IN || Pipe == U3_PIPE_EP1_IN)
      bytesRead = read(fd, pBuff, Count);	
    else if (Pipe == UE9_PIPE_EP2_IN || Pipe == U3_PIPE_EP2_IN)
      bytesRead = read(fd, pBuff, Count + 32768);
    else
      errno = EINVAL;
  }

  if (bytesRead <= -1)
    ret = 0;
  else
    ret = bytesRead;

  return ret;
}


ULONG LJUSB_BulkWrite(HANDLE hDevice, ULONG Pipe, BYTE *pBuff, ULONG Count)
{
  long bytesWrote = -1;
  ULONG ret = 0;
  int fd = -1;

  if ((fd = handleToFD(hDevice)) != -1)
  {
    //Checking endpoint pipe and performing the appropriate write
    if (Pipe == UE9_PIPE_EP1_OUT || Pipe == U3_PIPE_EP1_OUT)
      bytesWrote = write(fd, pBuff, Count);
     else
      errno = EINVAL;
  }

  if (bytesWrote <= -1)
    ret = 0;
  else
    ret = bytesWrote;

  return ret;
}


void LJUSB_CloseDevice(HANDLE hDevice)
{
  int fd;

  if ((fd = handleToFD(hDevice)) != -1)
    close(fd);
}


ULONG LJUSB_GetDevCount(ULONG ProductID)
{
  ULONG maxDevices;
  char deviceFileName[24];
  char fullDeviceFileName[30];
  int count = 0;
  int i = 0;
  int fd;

  //Matching a product ID and setting the max devices and device file name 
  //prefix	
  if (ProductID == UE9_PRODUCT_ID)
  {
    maxDevices = UE9_MAX_DEVICES;
    sprintf(deviceFileName, "%s", ue9DeviceFileName);
  }
  else if (ProductID == U3_PRODUCT_ID)
  {
    maxDevices = U3_MAX_DEVICES;
    sprintf(deviceFileName, "%s", u3DeviceFileName);
  }
  else
  {
    errno = EINVAL;
    goto Count_return;
  }

  //Finding devices that can be opened and incrementing device count
  for (i = 0; i < (int)maxDevices; i++)
  {
    sprintf(fullDeviceFileName, "%s%d", deviceFileName, i);
    if ((fd = open(fullDeviceFileName, O_RDWR)) != -1)
    {
      count++;
      close(fd);
    }
  }
Count_return:
  return count;
}


//not supported
BOOL LJUSB_AbortPipe(HANDLE hDevice, ULONG Pipe)
{
  errno = ENOSYS;
  return 0;
}
