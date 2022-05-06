/* 
 * File:   PinFunctions.h
 * Author: lulau
 *
 * Created on January 22, 2021, 2:20 PM
 */

#ifndef PINFUNCTIONS_H
#define	PINFUNCTIONS_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define isPressed       0
#define notPressed      1

#define activeLow       0
#define activeHigh      1

#define mute            0
#define unMute          1

#define remoteOn        0
#define remoteOff       1

#define selfRemoteOn    1
#define selfRemoteOff   0

// TODO: Give switches names
typedef enum {
    LED_BRIGHTNESS,
    LED_COLOR,
    EEPROM_RESET,
    DEFAULT
}swType_t;

typedef struct {
    swType_t type;
    uint16_t counter;
    bool isDebouncing;
}switch_t;

enum {
    LED_GM_AMP_1,
    LED_GM_AMP_2,
    LED_GM_AMP_3,
    LED_GM_SUB
};

void GPIO_Init(void);
void GPIO_RemappablePinsConfig(void);

bool GPIO_GetSwitchValue(swType_t switchNum);

bool GPIO_IsAmpMuted(void);
void GPIO_SetClassDDisable(bool isMuted);

bool GPIO_IsRemoteConnected(void);

void GPIO_SetSelfRemote(bool);

bool GPIO_IsPressedLedColor(void);
bool GPIO_IsPressedLedBrightness(void);

bool GPIO_GetPowerLed(void);
void GPIO_SetPowerLed(bool);

bool GPIO_GetProtectLed(void);
void GPIO_SetProtectLed(bool);

void GPIO_SetGainMatchLed(uint8_t thisLed, bool isOn);

void GPIO_SetEepromChipSelect(bool isSelected);

void GPIO_SetAdcChipSelect(bool isSelected);
void GPIO_SetAdcReset(bool isReset);

void GPIO_SetDacChipSelect(bool isSelected);
void GPIO_SetDacReset(bool isReset);

void GPIO_SetDspChipSelect(bool isSelected);
void GPIO_SetDspReset(bool isReset);

bool GPIO_GetPowerSupplyProtection(void);

bool GPIO_IsRollingBackFullRange(void);
bool GPIO_IsRollingBackSub(void);

bool GPIO_GetMultiChannelAmpSelect();

bool GPIO_IsProtectedClassD(void);

#endif	/* PINFUNCTIONS_H */

