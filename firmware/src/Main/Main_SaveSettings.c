/* 
  @Company
    Stillwater Designs & Audio, Inc.
  
  @Filename
    Main_EEProm.c
  
  @Author
    iaoswa
 
  @Summary
    Brief description of the file.
 
  Creation Date: October 14, 2020
 */

/* Section: Included Files */
#include "Main.h"
#include "ISaveSetting.h"
#include "ChannelFunctions.h"
#include "PinDefinitions.h"
#include "PinFunctions.h"

#define CHANNEL_DATA_SAVE_CHECK_ADDR    0x1F00
#define CHANNEL_DATA_SAVED              0x55
#define CHANNEL_DATA_VOID               0xAA

static void _Upload(void);
static void _Download(void);
static void _EraseChannelData(void);
static bool _IsChannelDataSaved(void);

void Main_SaveSettingsInit(void) {
    
    // TODO: Set function pointers
//    ISaveSetting_SetChipSelectPinFunc(&GPIO_SetEepromChipSelect);
//    ISaveSetting_SetTransmitFunc(&ISPI3_Write);
//    ISaveSetting_SetReceiveFunc(&ISPI3_Read);
//    ISaveSetting_SetDelayMsFunc(&IDelay_ms);
    
    if(!GPIO_GetSwitchValue(EEPROM_RESET))
    { _EraseChannelData(); }
    
    // Initialize channels and load any saved data.
    Main_ChannelInit(mVars.channels, mVars.product.channelCount);
    if (_IsChannelDataSaved())
        _Download();
}

void Main_SaveSettings(void) {
    /*TODO: sometimes, such as with EEPROMs, the supply voltage will need to be checked
     * to verify the voltage is sufficient to perform a write. */
    _Upload();
}

void Main_SaveSettingsReset(void) {
    _EraseChannelData();
    
    //Reload DSP data
//    Main_DspLoadChannels();
}

/**
 * Saves a channel to NVM and writes a byte to notify the program of saved data
 * upon the next initialization sequence.
 */
static void _Upload(void) {
    
    uint8_t chNum;
    uint16_t channelAddress = 0;
    for (chNum = 0; chNum < mVars.product.channelCount; chNum++) {
        channelAddress += Channel_Save(channelAddress, &mVars.channels[chNum]);
    }
    ISaveSetting_WriteData(CHANNEL_DATA_SAVE_CHECK_ADDR, (uint8_t[]) {CHANNEL_DATA_SAVED}, 1);
}

/**
 * Downloads channel data from NVM.
 */
static void _Download(void) {
    
    uint8_t chNum;
    uint16_t channelAddress = 0;
    for (chNum = 0; chNum < mVars.product.channelCount; chNum++) {
        channelAddress += Channel_Load(channelAddress, &mVars.channels[chNum]);
    }
}

/**
 * Erases saved channel data from NVM by overwriting the known value
 * that's checked for in _IsChannelDataSaved(), and reinitializing
 * the channel_t data structure (usually mVars.channels) to its default
 * values.
 */
static void _EraseChannelData(void) {
    // TODO: Add indication the memory is being erased.
    ISaveSetting_WriteData(CHANNEL_DATA_SAVE_CHECK_ADDR, (uint8_t[]) {CHANNEL_DATA_VOID}, 1);
    Main_ChannelInit(mVars.channels, mVars.product.channelCount);
}

/**
 * Read an address in memory and compare it against a known value that
 * would be stored there if channel data has been saved previously.
 * @return 
 *  True if channel data is present, false otherwise.
 */
static bool _IsChannelDataSaved() {
    uint8_t saveCheck;
    ISaveSetting_ReadData(CHANNEL_DATA_SAVE_CHECK_ADDR, &saveCheck, 1);
    
    return saveCheck == CHANNEL_DATA_SAVED;
}