/* 
  @Company
    Stillwater Designs & Audio, Inc.
  
  @Filename
    Main_LEDs.c
  
  @Author
    iaoswa
 
  @Summary
    Brief description of the file.
 
  Creation Date: October 14, 2020
 */

/* Section: Included Files */
#include "Main.h"
#include "PinDefinitions.h"
#include "AmpDefs.h"
#include "KickerSdkTypes.h"
#include "PinFunctions.h"
#include "CCP.h"


static void PowerLedTasks(void);
static void GainMatchLedTasks(void);

void Main_LedInit(void) {
    GPIO_SetProtectLed(true);
    GPIO_SetPowerLed(false);
}

void Main_LedTasks(uint16_t taskInterval_ms) {
    PowerLedTasks();
    GainMatchLedTasks();
}

static void GainMatchLedTasks(void) {
    uint8_t i;
    for(i = 0; i < mVars.product.channelCount; i++) {
        GPIO_SetGainMatchLed(i, mVars.isClipping[i]);
    }
}

static void PowerLedTasks(void) {
    
    switch (mVars.state) {
        case INIT:
        case PROTECTION:
            GPIO_SetProtectLed(true);
            GPIO_SetPowerLed(false);
            break;
        case SHUTDOWN:
            GPIO_SetProtectLed(true);
            GPIO_SetPowerLed(true);
            break;
        case NORMAL:
            GPIO_SetProtectLed(false);
            GPIO_SetPowerLed(true);
            break;
    }
    
    if(mVars.protections.overVoltageWarning || mVars.protections.underVoltageWarning || mVars.protections.thermalWarning) {
        GPIO_SetProtectLed(true);
        GPIO_SetPowerLed(true);
    }
}


