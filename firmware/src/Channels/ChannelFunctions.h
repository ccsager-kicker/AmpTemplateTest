/* 
  @Company
    Stillwater Designs & Audio, Inc.
  
  @Filename
    ChannelFunctions.h
  
  @Author
    iaoswa
 
  @Summary
    Brief description of the file.
 
  Creation Date: October 9, 2020
 */

#ifndef _CHANNELFUNCTIONS_H
#define	_CHANNELFUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "ChannelStructs.h"

uint16_t Channel_Save(uint16_t address, channel_t *channel);
uint16_t Channel_Load(uint16_t address, channel_t *channel);

#endif
