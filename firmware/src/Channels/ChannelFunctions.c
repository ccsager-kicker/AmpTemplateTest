/* 
  @Company
    Stillwater Designs & Audio, Inc.
  
  @Filename
    ChannelFunctions.c
  
  @Author
    iaoswa
 
  @Summary
    Saving and loading amplifier-channel data using ChannelStructs.h to 
    categorize and convert the data, saving it to assigned spots in memory.
 
  Creation Date: October 9, 2020
 */

/* Section: Included Files                                                    */
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "ChannelFunctions.h"
#include "ISaveSetting.h"
#include "IEEE754.h"

static uint16_t SaveFullRangeChannel(uint16_t address, channel_t *channel);
static uint16_t SaveSubwooferChannel(uint16_t address, channel_t *channel);
static uint16_t LoadFullRangeChannel(uint16_t address, channel_t *channel);
static uint16_t LoadSubwooferChannel(uint16_t address, channel_t *channel);


uint16_t UnPackUINT16(uint8_t *pData) {
    return pData[0] << 8 | pData[1];
}

float UnPackDouble(uint8_t *pData){
    uint32_t int32 = ((pData[0] << 24) | (pData[1] << 16) | (pData[2] << 8) | (pData[3]));
    return FromIEEE754(int32); 
}

void SetFloatData(uint8_t *buffer, uint16_t *cntr, float value) {
    uint32_t data = ToIEEE754(value);
    buffer[(*cntr)++] = (uint8_t)(data>>24);
    buffer[(*cntr)++] = (uint8_t)(data>>16);
    buffer[(*cntr)++] = (uint8_t)(data>>8);
    buffer[(*cntr)++] = (uint8_t)(data);
}

/**
 * 
 * @param address
 *  The starting address in memory, to which the channel will be written.
 * @param channel
 *  The channel to be saved.
 * @return 
 *  Returns the number of bytes written to memory.
 */
uint16_t Channel_Save(uint16_t address, channel_t *channel) {
    if(channel->channelType == FULL_RANGE) {
        return SaveFullRangeChannel(address, channel);
    }
    return SaveSubwooferChannel(address, channel);
}

/**
 * 
 * @param address
 *  The starting address in memory, to which the channel will be written.
 * @param channel
 *  The channel to be loaded from memory.
 * @return 
 *  Returns the number of bytes read from memory.
 */
uint16_t Channel_Load(uint16_t address, channel_t *channel) {
    if(channel->channelType == FULL_RANGE) {
        return LoadFullRangeChannel(address, channel);
    }
    return LoadSubwooferChannel(address, channel);
}

static uint16_t SaveFullRangeChannel(uint16_t address, channel_t *channel) {
    uint16_t i = 0;
    uint8_t data[23];
    data[i++] = channel->channelMode;
    data[i++] = channel->lowPass.isEnabled;
    data[i++] = channel->lowPass.slope;
    data[i++] = channel->lowPass.alignment;
    data[i++] = (uint8_t)(channel->lowPass.frequency>>8);
    data[i++] = (uint8_t)channel->lowPass.frequency;
    data[i++] = channel->highPass.isEnabled;
    data[i++] = channel->highPass.slope;
    data[i++] = channel->highPass.alignment;
    data[i++] = (uint8_t)(channel->highPass.frequency>>8);
    data[i++] = (uint8_t)channel->highPass.frequency;
    
    SetFloatData(data, &i, channel->delay.timeMs);
    SetFloatData(data, &i, channel->gain.input);
    SetFloatData(data, &i, channel->gain.output);
    
    uint16_t length = sizeof(data) / sizeof(data[0]);
    ISaveSetting_WriteData(address, data, length);
    return length;
}

static uint16_t SaveSubwooferChannel(uint16_t address, channel_t *channel) {
    uint16_t i = 0;
    uint8_t data[33];
    
    data[i++] = channel->lowPass.isEnabled;
    data[i++] = channel->lowPass.slope;
    data[i++] = channel->lowPass.alignment;
    data[i++] = (uint8_t)(channel->lowPass.frequency>>8);
    data[i++] = (uint8_t)channel->lowPass.frequency;
    data[i++] = channel->highPass.isEnabled;
    data[i++] = channel->highPass.slope;
    data[i++] = channel->highPass.alignment;
    data[i++] = (uint8_t)(channel->highPass.frequency>>8);
    data[i++] = (uint8_t)channel->highPass.frequency;
    data[i++] = channel->outputPolarity;
    SetFloatData(data, &i, channel->gain.input);
    SetFloatData(data, &i, channel->gain.output);
    data[i++] = (uint8_t)(channel->kickerEq.boost.frequency >> 8);
    data[i++] = (uint8_t)(channel->kickerEq.boost.frequency);
    SetFloatData(data, &i, channel->kickerEq.boost.bandwidth);
    SetFloatData(data, &i, channel->kickerEq.boost.gain);
    SetFloatData(data, &i, channel->superBass.intensity);
    
    uint16_t length = sizeof(data) / sizeof(data[0]);
    ISaveSetting_WriteData(address, data, length);
    return length;
}


static uint16_t LoadFullRangeChannel(uint16_t address, channel_t *channel) {
    uint16_t i = address;
    uint8_t data[4];
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->channelMode = data[0];
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->lowPass.isEnabled = data[0];
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->lowPass.slope = data[0];
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->lowPass.alignment = data[0];
    
    ISaveSetting_ReadData(i, data, 2); i+=2;
    channel->lowPass.frequency = UnPackUINT16(data);
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->highPass.isEnabled = data[0];
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->highPass.slope = data[0];
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->highPass.alignment = data[0];
    
    ISaveSetting_ReadData(i, data, 2); i+=2;
    channel->highPass.frequency = UnPackUINT16(data);
    
    ISaveSetting_ReadData(i, data, 4); i+=4;
    channel->delay.timeMs = UnPackDouble(data);
    
    ISaveSetting_ReadData(i, data, 4); i+=4;
    channel->gain.input = UnPackDouble(data);
    
    ISaveSetting_ReadData(i, data, 4); i+=4;
    channel->gain.output = UnPackDouble(data);
    return i - address;
}

static uint16_t LoadSubwooferChannel(uint16_t address, channel_t *channel) {
    uint16_t i = address;
    uint8_t data[4];
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->lowPass.isEnabled = data[0];
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->lowPass.slope = data[0];
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->lowPass.alignment = data[0];
    
    ISaveSetting_ReadData(i, data, 2); i+=2;
    channel->lowPass.frequency = UnPackUINT16(data);
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->highPass.isEnabled = data[0];
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->highPass.slope = data[0];
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->highPass.alignment = data[0];
    
    ISaveSetting_ReadData(i, data, 2); i+=2;
    channel->highPass.frequency = UnPackUINT16(data);
    
    ISaveSetting_ReadData(i++, data, 1);
    channel->outputPolarity = data[0];
    
    ISaveSetting_ReadData(i, data, 4); i+=4;
    channel->gain.input = UnPackDouble(data);
    
    ISaveSetting_ReadData(i, data, 4); i+=4;
    channel->gain.output = UnPackDouble(data);
    
    ISaveSetting_ReadData(i, data, 2); i+=2;
    channel->kickerEq.boost.frequency = UnPackUINT16(data);
    
    ISaveSetting_ReadData(i, data, 4); i+=4;
    channel->kickerEq.boost.bandwidth = UnPackDouble(data);
    
    ISaveSetting_ReadData(i, data, 4); i+=4;
    channel->kickerEq.boost.gain = UnPackDouble(data);
    
    ISaveSetting_ReadData(i, data, 4); i+=4;
    channel->superBass.intensity = UnPackDouble(data);
    return i - address;
}