/*
 * @Company
 *  Stillwater Designs & Audio, Inc.
 *
 * @Filename
 *  Main.h
 * 
 * @Author
 *  Luke Lau
 * 
 * @Summary
 *  Brief description of the file.
 * 
 * Creation Date: Aril 20, 2022
 */

#ifndef _MAIN_H
#define _MAIN_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "ChannelStructs.h"
#include "System.h"
#include "IDelay.h"
#include "ISystemLingo.h"
#include "KickerSdkTypes.h"
#include "PinFunctions.h"
#include "PinDefinitions.h"
#include "AmpDefs.h"
#include "ISPI.h"
#include "CCP.h"
#include "ADC.h"
#include "ADAU1701.h"
#include "IKickerProduct.h"
#include "system_definitions.h"
#include "system_config.h"
#include "system/common/sys_module.h"   // SYS function prototypes
#include "Scheduler.h"
#include "FirmwareVersion.h"

#define LOW_BYTE(x)     ((unsigned char)((x)&0xFF))
#define HIGH_BYTE(x)    ((unsigned char)(((x)>>8)&0xFF))

typedef enum ampStatus{
    OFF,
    INIT,
    NORMAL,
    PROTECTION,
    SHUTDOWN
} ampStatus_t;

struct mainVars{
    channel_t channels[CHANNEL_COUNT];
    bool isClipping[CHANNEL_COUNT];
        
    double vBatt;
    double temperatureC;
    
    kProduct_t product;
    
    sdkProtection_t protections;
    
    volatile ampStatus_t state;
    volatile ampStatus_t prevState;
} mVars;
static const struct mainVars initialmVars;  // ensures expected values at startup.

void Main_Init(void);
void Main_InitTasks(uint16_t taskInterval_ms);
void Main_ShutdownTasks(uint16_t taskInterval_ms);

void Main_SetAmpState(ampStatus_t thisState);

void Main_RemoteEnableInit(void);

void Main_ClassDInit(void);

void Main_ChannelInit(channel_t *channels, uint8_t channelCount);

void Main_SaveSettingsInit(void);
void Main_SaveSettings(void);
void Main_SaveSettingsReset(void);

void Main_LedInit(void);
void Main_LedTasks(uint16_t taskInterval_ms);

void Main_SdkInit(void);
void Main_SdkTasks(uint16_t taskInterval_ms);

void Main_DspInit(void);
void Main_DspTasks(uint16_t taskInterval_ms);

void Main_GpioInit(void);
void Main_GpioTasks(uint16_t taskInterval_ms);

void Main_PsuInit(void);
void Main_PsuStopMainPsu(void);
void Main_PsuStartMainPsu(void);

void Main_ProtectionInit(void);
void Main_ProtectionTasks(uint16_t taskInterval_ms);

void Main_AdcInit(void);
void Main_AdcTasks(uint16_t taskInterval_ms);

#endif
/*End MAIN_H */
