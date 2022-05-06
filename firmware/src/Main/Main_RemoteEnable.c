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
#include "Main.h"

static void RemoteRemovedEvent(void);

void Main_RemoteEnableInit(void) {
    SYS_InitInterrupt2(5, false);
    SYS_SetInt2CallbackFunc(&RemoteRemovedEvent);
    SYS_EnableInterrupt2(true);
}

static void RemoteRemovedEvent(void) {
    //TODO: Mute output ASAP.
    //    Main_DspSetMute(true);
    Main_SetAmpState(SHUTDOWN);
}
