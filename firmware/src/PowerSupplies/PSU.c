/* 
  @Company
    Stillwater Designs & Audio, Inc.
  
  @Filename
    IPSU.c
  
  @Author
    LUKE LAU
 
  @Summary
    Brief description of the file.
 
    Creation Date: September 1, 2021
 */

#include "IPSU.h"
#include "CCP.h"    // Include MCU peripheral for generating PWM.


void IPSU_ConfigPushPull(uint8_t ccpNum, float frequency, uint16_t deadTimeNs) {
    CCP_ConfigMCCP(ccpNum, PUSH_PULL, frequency, deadTimeNs);
}

void IPSU_SelectCcpModule(uint8_t ccpNum, uint8_t ccpPins) {
    CCP_OutputSelect(ccpNum, ccpPins);
}

void IPSU_SoftStartPushPull(uint8_t ccpNum) {
    CCP_SoftStart(ccpNum);
}

void IPSU_StartPushPull(uint8_t ccpNum) {
    CCP_Start(ccpNum);
}

void IPSU_StopPushPull(uint8_t ccpNum) {
    CCP_Stop(ccpNum);
}