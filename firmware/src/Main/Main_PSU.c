/* 
  @Company
    Stillwater Designs & Audio, Inc.
  
  @Filename
    CCP_PIC32MM.c
  
  @Author
    LUKE LAU
 
  @Summary
    Brief description of the file.
 
  Creation Date: August 31, 2021
 */

#include <xc.h>
#include <stdbool.h>
#include "Main.h"
#include "IPSU.h"

#define MAIN_PSU_CCP            1
#define AUX_PSU_CCP             3

#define MAIN_PSU_FREQ           66500
#define AUX_PSU_FREQ            133000

// DEAD-TIME in Seconds = (1 / PSU_FREQ) * (0.5 - <DESIRED DUTY CYCLE>) // <DESIRED DUTY CYCLE> Should be no greater than .5
#define MAIN_PSU_DEAD_TIME_NS   113
#define AUX_PSU_DEAD_TIME_NS    264

void Main_PsuInit(void) {
    IPSU_ConfigPushPull(MAIN_PSU_CCP, MAIN_PSU_FREQ, MAIN_PSU_DEAD_TIME_NS);
    IPSU_ConfigPushPull(AUX_PSU_CCP, AUX_PSU_FREQ, AUX_PSU_DEAD_TIME_NS);

    IPSU_SelectCcpModule(MAIN_PSU_CCP, CCP_OUT_ED);
    IPSU_SelectCcpModule(AUX_PSU_CCP, CCP_OUT_ED);

    IPSU_SoftStartPushPull(MAIN_PSU_CCP);
    //TODO: May need to include a delay here.
    IPSU_SoftStartPushPull(AUX_PSU_CCP);
}

void Main_PsuStopMainPsu(void) {
    IPSU_StopPushPull(MAIN_PSU_CCP);
}

void Main_PsuStartMainPsu(void) {
    IPSU_SoftStartPushPull(MAIN_PSU_CCP);
}