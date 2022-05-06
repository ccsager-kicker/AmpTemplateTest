/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    usb.c

 *******************************************************************************/
#include "usb.h"
#include "usb_private.h"
#include <stdbool.h>
#include <stdio.h>
#include "system/common/sys_common.h"

static void SetReadRequest();

uint8_t receiveDataBuffer[USB_BUFFER_SIZE];
uint8_t transmitDataBuffer[USB_BUFFER_SIZE];
usbData_t usbData;

void USB_Init(void) {
    usbData.state = USB_STATE_INIT;
    usbData.usbDevHandle = USB_DEVICE_HANDLE_INVALID;
    usbData.deviceConfigured = false;
    usbData.txTransferHandle = USB_DEVICE_HID_TRANSFER_HANDLE_INVALID;
    usbData.rxTransferHandle = USB_DEVICE_HID_TRANSFER_HANDLE_INVALID;
    usbData.hidDataReceived = false;
    usbData.hidDataTransmitted = true;
    usbData.receiveDataBuffer = &receiveDataBuffer[0];
    usbData.transmitDataBuffer = &transmitDataBuffer[0];
}

void (*UsbRxHandler)(uint8_t* data, uint16_t length);
void USB_SetReceiveHandler(void (*Function)(uint8_t *rxData, uint16_t length))
{ UsbRxHandler = Function; }

bool USB_TransmitData(uint8_t *txData, uint16_t length) {
    if(length > 0 && usbData.hidDataTransmitted) {
        memcpy(&transmitDataBuffer[0], txData, length);
        usbData.txDataReady = false;
        return true;
    }
    return false;
}

bool USB_TxBusy()
{ return !usbData.txDataReady; }

    //<editor-fold defaultstate="collapsed" desc="Section: USB Callback Functions">
static USB_DEVICE_HID_EVENT_RESPONSE USB_USBDeviceHIDEventHandler(USB_DEVICE_HID_INDEX iHID,
                                                            USB_DEVICE_HID_EVENT event,
                                                            void * eventData,
                                                            uintptr_t userData) {
    USB_DEVICE_HID_EVENT_DATA_REPORT_SENT * reportSent;
    USB_DEVICE_HID_EVENT_DATA_REPORT_RECEIVED * reportReceived;

    /* Check type of event */
    switch (event) {
        case USB_DEVICE_HID_EVENT_REPORT_SENT:
            /* The eventData parameter will be USB_DEVICE_HID_EVENT_REPORT_SENT
             * pointer type containing details about the report that was
             * sent. */
            reportSent = (USB_DEVICE_HID_EVENT_DATA_REPORT_SENT *) eventData;
            if(reportSent->handle == usbData.txTransferHandle ) {
                // Transfer progressed.
                usbData.hidDataTransmitted = true;
            }
            break;
        case USB_DEVICE_HID_EVENT_REPORT_RECEIVED:
            /* The eventData parameter will be USB_DEVICE_HID_EVENT_REPORT_RECEIVED
             * pointer type containing details about the report that was
             * received. */
            reportReceived = (USB_DEVICE_HID_EVENT_DATA_REPORT_RECEIVED *) eventData;
            if(reportReceived->handle == usbData.rxTransferHandle ) {
                // Transfer progressed.
                usbData.hidDataReceived = true;
            }
            break;
        case USB_DEVICE_HID_EVENT_SET_IDLE:
            /* For now we just accept this request as is. We acknowledge
             * this request using the USB_DEVICE_HID_ControlStatus()
             * function with a USB_DEVICE_CONTROL_STATUS_OK flag */
            USB_DEVICE_ControlStatus(usbData.usbDevHandle, USB_DEVICE_CONTROL_STATUS_OK);

            /* Save Idle rate received from Host */
            usbData.idleRate = ((USB_DEVICE_HID_EVENT_DATA_SET_IDLE*)eventData)->duration;
            break;
        case USB_DEVICE_HID_EVENT_GET_IDLE:
            /* Host is requesting for Idle rate. Now send the Idle rate */
            USB_DEVICE_ControlSend(usbData.usbDevHandle, & (usbData.idleRate),1);

            /* On successfully receiving Idle rate, the Host would acknowledge back with a
               Zero Length packet. The HID function driver returns an event
               USB_DEVICE_HID_EVENT_CONTROL_TRANSFER_DATA_SENT to the application upon
               receiving this Zero Length packet from Host.
               USB_DEVICE_HID_EVENT_CONTROL_TRANSFER_DATA_SENT event indicates this control transfer
               event is complete */
            break;
        default:
            // Nothing to do.
            break;
    }
    return USB_DEVICE_HID_EVENT_RESPONSE_NONE;
}

static void USB_USBDeviceEventHandler(USB_DEVICE_EVENT event, void * eventData, uintptr_t context) {
    switch(event) {
        case USB_DEVICE_EVENT_RESET:
        case USB_DEVICE_EVENT_DECONFIGURED:
            /* Host has de configured the device or a bus reset has happened.
             * Device layer is going to de-initialize all function drivers.
             * Hence close handles to all function drivers (Only if they are
             * opened previously. */
            usbData.deviceConfigured = false;
            usbData.state = USB_STATE_WAIT_FOR_CONFIGURATION;
            break;
        case USB_DEVICE_EVENT_CONFIGURED:
            /* Set the flag indicating device is configured. */
            usbData.deviceConfigured = true;

            /* Save the other details for later use. */
            usbData.configurationValue = ((USB_DEVICE_EVENT_DATA_CONFIGURED*)eventData)->configurationValue;

            /* Register application HID event handler */
            USB_DEVICE_HID_EventHandlerSet(USB_DEVICE_HID_INDEX_0, USB_USBDeviceHIDEventHandler, (uintptr_t)&usbData);
            break;
        case USB_DEVICE_EVENT_SUSPENDED:
            break;
        case USB_DEVICE_EVENT_POWER_DETECTED:
            /* VBUS was detected. We can attach the device */
            USB_DEVICE_Attach (usbData.usbDevHandle);
            usbData.state = USB_STATE_WAIT_FOR_CONFIGURATION;
            break;
        case USB_DEVICE_EVENT_POWER_REMOVED:
            /* VBUS is not available */
            USB_DEVICE_Detach(usbData.usbDevHandle);
            usbData.state = USB_STATE_MAIN_TASK;
            break;
        /* These events are not used in this demo */
        case USB_DEVICE_EVENT_RESUMED:
        case USB_DEVICE_EVENT_ERROR:
        default:
            break;
    }
}
//</editor-fold>

void USB_Tasks (void) {
    /* Check if device is configured.  See if it is configured with correct
     * configuration value  */
    switch(usbData.state) {
        case USB_STATE_INIT:
                /* Open the device layer */
                usbData.usbDevHandle = USB_DEVICE_Open(USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE);

                if(usbData.usbDevHandle != USB_DEVICE_HANDLE_INVALID) {
                    /* Register a callback with device layer to get event notification (for end point 0) */
                    USB_DEVICE_EventHandlerSet(usbData.usbDevHandle, USB_USBDeviceEventHandler, 0);

                    // TODO: Check if USB has power, if so wait, otherwise main task.
                    if (USB_DEVICE_StateGet(usbData.usbDevHandle) == USB_DEVICE_STATE_DETACHED) {
                        usbData.state = USB_STATE_MAIN_TASK;
                    }
                    else {
                        usbData.state = USB_STATE_WAIT_FOR_CONFIGURATION;
                    }
                }
                else {
                    /* The Device Layer is not ready to be opened. We should try
                     * again later. */
                }
            break;
        case USB_STATE_WAIT_FOR_CONFIGURATION:
            if(usbData.deviceConfigured == true) {
                /* Device is ready to run the main task */
                usbData.hidDataReceived = false;
                usbData.hidDataTransmitted = true;
                usbData.state = USB_STATE_MAIN_TASK;

                /* Place a new read request. */
                USB_DEVICE_HID_ReportReceive(USB_DEVICE_HID_INDEX_0,
                        &usbData.rxTransferHandle, usbData.receiveDataBuffer, 64);
            }
            break;
        case USB_STATE_MAIN_TASK:
            if(!usbData.deviceConfigured && USB_DEVICE_StateGet(usbData.usbDevHandle) == USB_DEVICE_STATE_ATTACHED) {
                /* Device is not configured */
                usbData.state = USB_STATE_WAIT_FOR_CONFIGURATION;
            }
            else if(usbData.hidDataReceived) {      
                /* Look at the data the host sent, to see what
                 * kind of application specific command it sent. */
                if(UsbRxHandler)
                { UsbRxHandler(usbData.receiveDataBuffer, USB_BUFFER_SIZE); }
                if(usbData.hidDataTransmitted)
                { SetReadRequest(); }
            }
            if(!usbData.hidDataTransmitted) {
                /* Prepare the USB module to send the data packet to the host */
                USB_DEVICE_HID_ReportSend (USB_DEVICE_HID_INDEX_0,
                        &usbData.txTransferHandle, usbData.transmitDataBuffer, 64 );

                SetReadRequest();
            }
        case USB_STATE_ERROR:
            break;
        default:
            break;
    }
}

static void SetReadRequest() {
    usbData.hidDataReceived = false;

    /* Place a new read request. */
    USB_DEVICE_HID_ReportReceive (USB_DEVICE_HID_INDEX_0,
            &usbData.rxTransferHandle, usbData.receiveDataBuffer, 64 );
}

void __ISR(_TIMER_2_VECTOR, ipl1AUTO) IntHandlerDrvTmrInstance0(void)
{
    DRV_TMR_Tasks(sysObj.drvTmr0);
}
 void __ISR(_USB_VECTOR, ipl4AUTO) _IntHandlerUSBInstance0(void)
{
    DRV_USBFS_Tasks_ISR(sysObj.drvUSBObject);
}