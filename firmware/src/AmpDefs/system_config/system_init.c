/*******************************************************************************
  System Initialization File

  File Name:
    system_init.c

  Summary:
    This file contains source code necessary to initialize the system.

  Description:
    This file contains source code necessary to initialize the system.  It
    implements the "SYS_Initialize" function, defines the configuration bits,
    and allocates any necessary global system resources, such as the
    sysObj structure that contains the object handles to all the MPLAB Harmony
    module objects in the system.
 *******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "system_config.h"
#include "system_definitions.h"

/*** TMR Driver Initialization Data ***/
const DRV_TMR_INIT drvTmr0InitData = {
    .moduleInit.sys.powerState = DRV_TMR_POWER_STATE_IDX0,
    .tmrId = DRV_TMR_PERIPHERAL_ID_IDX0,
    .clockSource = DRV_TMR_CLOCK_SOURCE_IDX0,
    .prescale = DRV_TMR_PRESCALE_IDX0,
    .mode = DRV_TMR_OPERATION_MODE_IDX0,
    .interruptSource = DRV_TMR_INTERRUPT_SOURCE_IDX0,
    .asyncWriteEnable = false,
};
/* Structure to hold the object handles for the modules in the system. */
SYSTEM_OBJECTS sysObj;

/*** TMR Service Initialization Data ***/
const SYS_TMR_INIT sysTmrInitData = {
    .moduleInit = {SYS_MODULE_POWER_RUN_FULL},
    .drvIndex = DRV_TMR_INDEX_0,
    .tmrFreq = 1000, 
};

/******************************************************
 * USB Driver Initialization
 ******************************************************/
struct {
    USB_DEVICE_DESCRIPTOR deviceDescriptor;
    USB_DEVICE_MASTER_DESCRIPTOR masterDescriptor;
    USB_DEVICE_INIT devInitData;
} usbVars;

/****************************************************
 * Endpoint Table needed by the Device Layer.
 ****************************************************/
uint8_t __attribute__((aligned(512))) endPointTable[DRV_USBFS_ENDPOINTS_NUMBER * 32];
const DRV_USBFS_INIT drvUSBInit = {
    /* Assign the endpoint table */
    .endpointTable= endPointTable,

    /* Interrupt Source for USB module */
    .interruptSource = INT_SOURCE_USB_1,
    
    /* System module initialization */
    .moduleInit = {SYS_MODULE_POWER_RUN_FULL},
    
    .operationMode = DRV_USBFS_OPMODE_DEVICE,
    
    .operationSpeed = USB_SPEED_FULL,
    
    /* Stop in idle */
    .stopInIdle = false,

    /* Suspend in sleep */
    .suspendInSleep = false,

    /* Identifies peripheral (PLIB-level) ID */
    .usbID = USB_ID_1
};


/****************************************************
 * Class specific descriptor - HID Report descriptor
 ****************************************************/
const uint8_t hid_rpt0[] = {
    0x06, 0x00, 0xFF,   // Usage Page = 0xFF00 (Vendor Defined Page 1)
    0x09, 0x01,             // Usage (Vendor Usage 1)
    0xA1, 0x01,             // Colsslection (Application)
    0x19, 0x01,             // Usage Minimum
    0x29, 0x40,             // Usage Maximum 	//64 input usages total (0x01 to 0x40)
    0x15, 0x01,             // Logical Minimum (data bytes in the report may have minimum value = 0x00)
    0x25, 0x40,      	    // Logical Maximum (data bytes in the report may have maximum value = 0x00FF = unsigned 255)
    0x75, 0x08,             // Report Size: 8-bit field size
    0x95, 0x40,             // Report Count: Make sixty-four 8-bit fields (the next time the parser hits an "Input", "Output", or "Feature" item)
    0x81, 0x00,             // Input (Data, Array, Abs): Instantiates input packet fields based on the above report size, count, logical min/max, and usage.
    0x19, 0x01,             // Usage Minimum
    0x29, 0x40,             // Usage Maximum 	//64 output usages total (0x01 to 0x40)
    0x91, 0x00,             // Output (Data, Array, Abs): Instantiates output packet fields.  Uses same report size and count as "Input" fields, since nothing new/different was specified to the parser since the "Input" item.
    0xC0                    // End Collection
};

/**************************************************
 * USB Device Function Driver Init Data
 **************************************************/
    const USB_DEVICE_HID_INIT hidInit0 = {
        .hidReportDescriptorSize = sizeof(hid_rpt0),
        .hidReportDescriptor = &hid_rpt0,
        .queueSizeReportReceive = 1,
        .queueSizeReportSend = 1
    };
/**************************************************
 * USB Device Layer Function Driver Registration 
 * Table
 **************************************************/
const USB_DEVICE_FUNCTION_REGISTRATION_TABLE funcRegistrationTable[1] = {
    /* Function 1 */
    { 
        .configurationValue = 1,    /* Configuration value */ 
        .interfaceNumber = 0,       /* First interfaceNumber of this function */ 
        .speed = USB_SPEED_FULL,    /* Function Speed */ 
        .numberOfInterfaces = 1,    /* Number of interfaces */
        .funcDriverIndex = 0,  /* Index of HID Function Driver */
        .driver = (void*)USB_DEVICE_HID_FUNCTION_DRIVER,    /* USB HID function data exposed to device layer */
        .funcDriverInit = (void*)&hidInit0,    /* Function driver init data*/
    },
};

/*******************************************
 *  USB Device Descriptor 
 *******************************************/
void initDeviceDescriptor(uint16_t productId) {
    USB_DEVICE_DESCRIPTOR devDescriptor = {
        0x12,                           // Size of this descriptor in bytes
        USB_DESCRIPTOR_DEVICE,          // DEVICE descriptor type
        0x0200,                         // USB Spec Release Number in BCD format
        0x00,                           // Class Code
        0x00,                           // Subclass code
        0x00,                           // Protocol code
        USB_DEVICE_EP0_BUFFER_SIZE,     // Max packet size for EP0, see system_config.h
        USB_VENDOR_ID,                         // Vendor ID
        productId,                         // Product ID
        0x0100,                         // Device release number in BCD format
        0x01,                           // Manufacturer string index
        0x02,                           // Product string index
        0x00,                           // Device serial number string index
        0x01                            // Number of possible configurations
    };
    usbVars.deviceDescriptor = devDescriptor;
}

/*******************************************
 *  USB Full Speed Configuration Descriptor
 *******************************************/
const uint8_t fullSpeedConfigurationDescriptor[] = {
    /* Configuration Descriptor */
    0x09,                                               // Size of this descriptor in bytes
    USB_DESCRIPTOR_CONFIGURATION,                       // Descriptor Type
    41,0,                //(41 Bytes)Size of the Config descriptor.e
    1,                                               // Number of interfaces in this cfg
    0x01,                                               // Index value of this configuration
    0x00,                                               // Configuration string index
    USB_ATTRIBUTE_DEFAULT | USB_ATTRIBUTE_SELF_POWERED, // Attributes
    50,                                                 // Max power consumption (2X mA)
    /* Descriptor for Function 1 - HID     */ 
    
	/* Interface Descriptor */
    0x09,                               // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,           // INTERFACE descriptor type
    0,                                  // Interface Number
    0,                                  // Alternate Setting Number
    2,                                  // Number of endpoints in this interface
    USB_HID_CLASS_CODE,                 // Class code
    USB_HID_SUBCLASS_CODE_NO_SUBCLASS , // Subclass code
    USB_HID_PROTOCOL_CODE_NONE,         // No Protocol
    0,                                  // Interface string index

    /* HID Class-Specific Descriptor */
    0x09,                           // Size of this descriptor in bytes
    USB_HID_DESCRIPTOR_TYPES_HID,   // HID descriptor type
    0x11,0x01,                      // HID Spec Release Number in BCD format (1.11)
    0x00,                           // Country Code (0x00 for Not supported)
    1,                              // Number of class descriptors, see usbcfg.h
    USB_HID_DESCRIPTOR_TYPES_REPORT,// Report descriptor type
    USB_DEVICE_16bitTo8bitArrange(sizeof(hid_rpt0)),   // Size of the report descriptor

    /* Endpoint Descriptor */
    0x07,                           // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,        // Endpoint Descriptor
    1 | USB_EP_DIRECTION_IN,    // EndpointAddress ( EP1 IN )
    USB_TRANSFER_TYPE_INTERRUPT,    // Attributes
    0x40,0x00,                      // Size
    0x01,                           // Interval

    /* Endpoint Descriptor */
    0x07,                           // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,        // Endpoint Descriptor
    1 | USB_EP_DIRECTION_OUT,   // EndpointAddress ( EP1 OUT )
    USB_TRANSFER_TYPE_INTERRUPT,    // Attributes
    0x40,0x00,                      // size
    0x01,                           // Interval
};

/*******************************************
 * Array of Full speed config descriptors
 *******************************************/
USB_DEVICE_CONFIGURATION_DESCRIPTORS_TABLE fullSpeedConfigDescSet[1] = {
    fullSpeedConfigurationDescriptor
};

 /*******************************************
 *  Language code string descriptor
 *******************************************/
    const struct {
        uint8_t bLength;
        uint8_t bDscType;
        uint16_t string[1];
    }
    sd000 = {
        sizeof(sd000),          // Size of this descriptor in bytes
        USB_DESCRIPTOR_STRING,  // STRING descriptor type
        {0x0409}                // Language ID
    };
/*******************************************
 *  Manufacturer string descriptor
 *******************************************/
    const struct {
        uint8_t bLength;        // Size of this descriptor in bytes
        uint8_t bDscType;       // STRING descriptor type
        uint16_t string[32];    // String
    }
    sd001 = {
        sizeof(sd001),
        USB_DESCRIPTOR_STRING,
        {'S','t','i','l','l','w','a','t','e','r',' ','D','e','s','i','g','n','s',' ','&',' ','A','u','d','i','o',',',' ','I','n','c','.'}
    };

/*******************************************
 *  Product string descriptor
 *******************************************/
    const struct {
        uint8_t bLength;        // Size of this descriptor in bytes
        uint8_t bDscType;       // STRING descriptor type
        uint16_t string[10];    // String. Should be number of chars + 1 to include null term char '\0'
    }
    sd002 = {
        sizeof(sd002),
        USB_DESCRIPTOR_STRING,
        //TODO: Update string to match product.
		{'4','8','L','X','M','U','L','T','I'}
    }; 

/***************************************
 * Array of string descriptors
 ***************************************/
USB_DEVICE_STRING_DESCRIPTORS_TABLE stringDescriptors[3] = {
    (const uint8_t *const)&sd000,
    (const uint8_t *const)&sd001,
    (const uint8_t *const)&sd002
};

/*******************************************
 * USB Device Layer Master Descriptor Table 
 *******************************************/
void initUsbMasterDescriptor() {
    USB_DEVICE_MASTER_DESCRIPTOR masterDescriptor = {
        &usbVars.deviceDescriptor,          /* Full speed descriptor */
        1,                          /* Total number of full speed configurations available */
        fullSpeedConfigDescSet,     /* Pointer to array of full speed configurations descriptors*/
        NULL, 
        0, 
        NULL, 
        3,                          // Total number of string descriptors available.
        stringDescriptors,          // Pointer to array of string descriptors.
        NULL, 
        NULL
    };
    usbVars.masterDescriptor = masterDescriptor;
}

/****************************************************
 * USB Device Layer Initialization Data
 ****************************************************/
void initUsbDevInitData() {
    USB_DEVICE_INIT devInitData  = {
        /* System module initialization */
        .moduleInit = {SYS_MODULE_POWER_RUN_FULL},

        /* Number of function drivers registered to this instance of the
           USB device layer */
        .registeredFuncCount = 1,

        /* Function driver table registered to this instance of the USB device layer*/
        .registeredFunctions = (USB_DEVICE_FUNCTION_REGISTRATION_TABLE*)funcRegistrationTable,

        /* Pointer to USB Descriptor structure */
        .usbMasterDescriptor = &usbVars.masterDescriptor,

        /* USB Device Speed */
        .deviceSpeed = USB_SPEED_FULL,

        /* Index of the USB Driver to be used by this Device Layer Instance */
        .driverIndex = DRV_USBFS_INDEX_0,

        /* Pointer to the USB Driver Functions. */
        .usbDriverInterface = DRV_USBFS_DEVICE_INTERFACE,
    };
    usbVars.devInitData = devInitData;
}

/*******************************************************************************
  Function:
    void SYS_Initialize ( void *data )

  Summary:
    Initializes the board, services, drivers, application and other modules.

  Remarks:
    See prototype in system/common/sys_module.h.
 */
void SYS_Initialize(void* data) {
    
    /* Core Processor Initialization */
//    SYS_CLK_Initialize( NULL );
    SYS_DEVCON_Initialize(SYS_DEVCON_INDEX_0, (SYS_MODULE_INIT*)NULL);
    SYS_DEVCON_PerformanceConfig(SYS_CLK_SystemFrequencyGet());
    SYS_PORTS_Initialize();

    /* Initialize Drivers */

//    sysObj.drvTmr0 = DRV_TMR_Initialize(DRV_TMR_INDEX_0, (SYS_MODULE_INIT *)&drvTmr0InitData);
//
//    SYS_INT_VectorPrioritySet(INT_VECTOR_T2, INT_PRIORITY_LEVEL1);
//    SYS_INT_VectorSubprioritySet(INT_VECTOR_T2, INT_SUBPRIORITY_LEVEL0);
 
 
     /* Initialize USB Driver */ 
    sysObj.drvUSBObject = DRV_USBFS_Initialize(DRV_USBFS_INDEX_0, (SYS_MODULE_INIT *) &drvUSBInit);

    /* Initialize System Services */

    /*** Interrupt Service Initialization Code ***/
    SYS_INT_Initialize();

    /*** TMR Service Initialization Code ***/
    sysObj.sysTmr  = SYS_TMR_Initialize(SYS_TMR_INDEX_0, (const SYS_MODULE_INIT  * const)&sysTmrInitData);

    /* Set priority of USB interrupt source */
    SYS_INT_VectorPrioritySet(INT_VECTOR_USB1, INT_PRIORITY_LEVEL4);

    /* Set Sub-priority of USB interrupt source */
    SYS_INT_VectorSubprioritySet(INT_VECTOR_USB1, INT_SUBPRIORITY_LEVEL0);
    
    kProduct_t* mProduct = (kProduct_t*)data;
    initDeviceDescriptor(mProduct->productId);
    initUsbMasterDescriptor();
    initUsbDevInitData();
    /* Initialize the USB device layer */
    sysObj.usbDevObject0 = USB_DEVICE_Initialize(USB_DEVICE_INDEX_0 , (SYS_MODULE_INIT*) &usbVars.devInitData);

    /* Enable Global Interrupts */
//    SYS_INT_Enable();
}

/*******************************************************************************
 End of File
*/