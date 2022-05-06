/*******************************************************************************
 * Title: 
 * 
 * Author: Luke Lau
 * 
 * File: 
 * 
 * Description:
 * 
 * ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "PinMacros.h"
#include "PinFunctions.h"
#include "PinDefinitions.h"

typedef enum {
    PPS_NONE = 0,
    C1OUT,
    C2OUT,
    C3OUT,
    U2TX,
    U2RTS,
    U3TX,
    U3RTS,
    SDO2,
    SCK2OUT,
    SS2OUT,
    SCCP4,
    SCCP5,
    SCCP6,
    SCCP7,
    SCCP8,
    SCCP9,
    CLC1OUT,
    CLC2OUT,
    CLC3OUT,
    CLC4OUT
}ppsOut_t;

void GPIO_Init(void) {
    
    TRISACLR = 0xFFFFFFFF;
    TRISBCLR = 0xFFFFFFFF;
    TRISCCLR = 0xFFFFFFFF;
    TRISDCLR = 0xFFFFFFFF;
    
    LATACLR = 0xFFFFFFFF;
    LATBCLR = 0xFFFFFFFF;
    LATCCLR = 0xFFFFFFFF;
    LATDCLR = 0xFFFFFFFF;
    
    ANSELACLR = 0xFFFFFFFF;
    ANSELBCLR = 0xFFFFFFFF;
    ANSELCCLR = 0xFFFFFFFF;
    ANSELDCLR = 0xFFFFFFFF;
    
    SetDirection(USB_VBUS) = input;
    
    SetDirection(IO_SELF_REMOTE) = output;
    SetValue(IO_SELF_REMOTE) = 0;
    
    SetDirection(TRIG_AMP_MUTE) = output;
    GPIO_SetClassDDisable(true);
    
    SetDirection(LED_POWER) = output;
    
    SetDirection(LED_PROTECT) = output;
    
    SetDirection(IO_REM_INPUT) = input;
    
    SetDirection(PROTECT_PS) = input;
    
    SetDirection(PROTECT_CLASS_D) = input;
    
    SetDirection(SW_LED_BRIGHTNESS) = input;
    SetValue(SW_LED_BRIGHTNESS) = 0;
    CNPUBbits.CNPUB2 = 1;   //Weak Pullup
    SetDirection(SW_E2P_RESET) = input;
    SetValue(SW_E2P_RESET) = 0;
    CNPUCbits.CNPUC10 = 1;  //Weak Pullup
    SetDirection(SW_LED_COLOR) = input;
    SetValue(SW_LED_COLOR) = 0;
    CNPUCbits.CNPUC13 = 1;  //Weak Pullup
    
    SetDirection(LED_GAIN_MATCH_A1) = output;
    
    SetDirection(LED_GAIN_MATCH_A2) = output;
    
    SetDirection(LED_GAIN_MATCH_A3) = output;
    
    SetDirection(LED_GAIN_MATCH_SUB) = output;
    
    SetDirection(IO_ROLLBACK_PS_SUB) = input;
    
    SetDirection(IO_ROLLBACK_PS_FR) = input;
    
    SetDirection(IO_E2P_CHIP_SEL) = output;
    SetValue(IO_E2P_CHIP_SEL) = 1;  // not selected
    
    SetDirection(IO_DSP_RESET) = output;
    SetValue(IO_DSP_RESET) = 0; // in reset
    
    SetDirection(IO_DSP_CHIP_SEL) = output;
    SetValue(IO_DSP_CHIP_SEL) = 1;  // not selected
    
    SetDirection(IO_DAC_CHIP_SEL) = output;
    SetValue(IO_DAC_CHIP_SEL) = 1;  // not selected
    
    SetDirection(IO_DAC_RESET) = output;
    SetValue(IO_DAC_RESET) = 0; // in reset
    
    SetDirection(IO_ADC_CHIP_SEL) = output;
    SetValue(IO_ADC_CHIP_SEL) = 1;  // not selected
    
    SetDirection(IO_ADC_RESET) = output;
    SetValue(IO_ADC_RESET) = 0; // in reset
    
    SetDirection(IO_MULTI_CH_AMP_SEL) = input;
    
    SetDirection(ADC_TEMPERATURE) = input;
    SetIsAnalog(ADC_TEMPERATURE) = 1;
    
    SetDirection(ADC_BATTERY_VOLTAGE) = input;
    SetIsAnalog(ADC_BATTERY_VOLTAGE) = 1;
    
    SetDirection(UART_RX_PIN) = input;
    
    SetDirection(UART_TX_PIN) = output;
    
    SetDirection(SPI_DATA_IN) = input;
    
    SetDirection(REMOTE_RESET_PIN) = output;
    SetValue(REMOTE_RESET_PIN) = 1;
}

void GPIO_RemappablePinsConfig(void) {
    
    //UART
    RPINR8bits.U3RXR = 5;   // UART3 Receive on RP5
    RPOR5bits.RP24R = U3TX; // UART3 Tx(6) on RP24
}

bool GPIO_GetSwitchValue(swType_t switchNum) {
    switch(switchNum) {
        case LED_BRIGHTNESS:
            return GetValue(SW_LED_BRIGHTNESS);
        case LED_COLOR:
            return GetValue(SW_LED_COLOR);
        case EEPROM_RESET:
            return GetValue(SW_E2P_RESET);
        case DEFAULT:
            break;
    }
    
    return true;
}

bool GPIO_IsAmpMuted(void) {
    return GetValue(TRIG_AMP_MUTE) == mute;
}

void GPIO_SetClassDDisable(bool isDisabled) {
    SetValue(TRIG_AMP_MUTE) = (isDisabled ? mute : unMute);
}

bool GPIO_IsRemoteConnected(void) {
    return GetValue(IO_REM_INPUT) == remoteOn;
}

void GPIO_SetSelfRemote(bool isEnabled) {
    SetValue(IO_SELF_REMOTE) = (isEnabled ? selfRemoteOn : selfRemoteOff);
}

bool GPIO_IsPressedLedColor(void) {
    return GetValue(SW_LED_COLOR) == isPressed;
}

bool GPIO_IsPressedLedBrightness(void) {
    return GetValue(SW_LED_BRIGHTNESS) == isPressed;
}

bool GPIO_GetPowerLed(void) {
    return GetValue(LED_POWER);
}
void GPIO_SetPowerLed(bool isOn) {
    SetValue(LED_POWER) = isOn;
}

bool GPIO_GetProtectLed(void) {
    return GetValue(LED_PROTECT);
}
void GPIO_SetProtectLed(bool isOn) {
    SetValue(LED_PROTECT) = isOn;
}

void GPIO_SetGainMatchLed(uint8_t thisLed, bool isOn) {
    switch (thisLed) {
        case LED_GM_AMP_1:
            SetValue(LED_GAIN_MATCH_A1) = isOn;
            break;
        case LED_GM_AMP_2:
            SetValue(LED_GAIN_MATCH_A2) = isOn;
            break;
        case LED_GM_AMP_3:
            SetValue(LED_GAIN_MATCH_A3) = isOn;
            break;
        case LED_GM_SUB:
            SetValue(LED_GAIN_MATCH_SUB) = isOn;
            break;
    }
}

void GPIO_SetEepromChipSelect(bool isSelected) 
{ SetValue(IO_E2P_CHIP_SEL) = isSelected; }

void GPIO_SetAdcChipSelect(bool isSelected) 
{ SetValue(IO_ADC_CHIP_SEL) = (isSelected == activeLow); }

void GPIO_SetAdcReset(bool isReset)
{ SetValue(IO_ADC_RESET) = (isReset == activeLow); }

void GPIO_SetDacChipSelect(bool isSelected)
{ SetValue(IO_DAC_CHIP_SEL) = (isSelected == activeLow); }

void GPIO_SetDacReset(bool isReset)
{ SetValue(IO_DAC_RESET) = (isReset == activeLow); }

void GPIO_SetDspChipSelect(bool isSelected)
{ SetValue(IO_DSP_CHIP_SEL) = (isSelected == activeLow); }

void GPIO_SetDspReset(bool isReset)
{ SetValue(IO_DSP_RESET) = (isReset == activeLow); }

bool GPIO_GetPowerSupplyProtection(void)
{ return GetValue(PROTECT_PS); }

bool GPIO_IsRollingBackFullRange(void)
{ return GetValue(IO_ROLLBACK_PS_FR); }

bool GPIO_IsRollingBackSub(void) 
{ return GetValue(IO_ROLLBACK_PS_SUB); }

bool GPIO_GetMultiChannelAmpSelect()
{ return GetValue(IO_MULTI_CH_AMP_SEL); }

bool GPIO_IsProtectedClassD(void) {
    return !GetValue(PROTECT_CLASS_D);  // Active-Low
}