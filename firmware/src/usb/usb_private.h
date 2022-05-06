/*
  @Company
    Stillwater Designs & Audio, Inc.
 
  @Filename
    usb_private.h
 
  @Author
    Ian Oswald

  @Summary
    Brief description of the file.

  Creation Date: May 13, 2021
 */

#ifndef _USB_PRIVATE_H
#define _USB_PRIVATE_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define USB_BUFFER_SIZE 64

typedef enum {
    USB_STATE_INIT,
    USB_STATE_WAIT_FOR_CONFIGURATION,
    USB_STATE_MAIN_TASK,
    USB_STATE_ERROR
} usbState_t;

typedef struct {
    /* The application's current state */
    usbState_t state;

      /* Device layer handle returned by device layer open function */
    USB_DEVICE_HANDLE usbDevHandle;

    /* Receive data buffer */
    uint8_t* receiveDataBuffer;

    /* Transmit data buffer */
    uint8_t* transmitDataBuffer;

    /* Device configured */
    bool deviceConfigured;

    bool txDataReady;
    /* Send report transfer handle*/
    USB_DEVICE_HID_TRANSFER_HANDLE txTransferHandle;

    /* Receive report transfer handle */
    USB_DEVICE_HID_TRANSFER_HANDLE rxTransferHandle;

    /* Configuration value selected by the host*/
    uint8_t configurationValue;

    /* HID data received flag*/
    bool hidDataReceived;

    /* HID data transmitted flag */
    bool hidDataTransmitted;

     /* USB HID current Idle */
    uint8_t idleRate;
} usbData_t;


#endif
/*End USB_PRIVATE_H */
