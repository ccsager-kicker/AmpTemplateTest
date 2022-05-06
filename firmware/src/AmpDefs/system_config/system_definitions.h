/*******************************************************************************
  System Definitions

  File Name:
    system_definitions.h

  Summary:
    MPLAB Harmony project system definitions.

  Description:
    This file contains the system-wide prototypes and definitions for an MPLAB
    Harmony project.
 *******************************************************************************/

#ifndef _SYS_DEFINITIONS_H
#define _SYS_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "system/common/sys_common.h"
#include "system/common/sys_module.h"
#include "system/devcon/sys_devcon.h"
      #include "system/reset/sys_reset.h"
      #include "system/wdt/sys_wdt.h"
#include "system/clk/sys_clk.h"
#include "system/int/sys_int.h"
#include "system/tmr/sys_tmr.h"
#include "driver/tmr/drv_tmr.h"
#include "system/ports/sys_ports.h"
#include "driver/usb/usbfs/drv_usbfs.h"
#include "usb/usb_device.h"
#include "usb/usb_device_hid.h"
#include "IKickerProduct.h"
#include "usb.h"


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* System Objects

  Summary:
    Structure holding the system's object handles

  Description:
    This structure contains the object handles for all objects in the
    MPLAB Harmony project's system configuration.

  Remarks:
    These handles are returned from the "Initialize" functions for each module
    and must be passed into the "Tasks" function for each module.
*/

typedef struct
{
    SYS_MODULE_OBJ  sysTmr;
    SYS_MODULE_OBJ  drvTmr0;
    SYS_MODULE_OBJ  drvUSBObject;
    
    SYS_MODULE_OBJ  usbDevObject0;



} SYSTEM_OBJECTS;

// *****************************************************************************
// *****************************************************************************
// Section: extern declarations
// *****************************************************************************
// *****************************************************************************

extern SYSTEM_OBJECTS sysObj;


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _SYS_DEFINITIONS_H */
/*******************************************************************************
 End of File
*/

