/*
 * @Company
 * Stillwater Designs & Audio, Inc.
 *
 * @Filename
 *  main.c
 *
 * @Author
 *  Luke Lau
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>     // Defines EXIT_FAILURE
#include "Main.h"



void Main_SetAmpState(ampStatus_t thisState) {
    // prevent overwriting of the prev state.
    if (thisState == mVars.state) { return; }
    mVars.prevState = mVars.state;
    mVars.state = thisState;
}

void Main_Init(void) {
    mVars = initialmVars;   // for predictable variable values on startup.
    
    // TODO: User set these vars.
    // mVars.product.productId = USB_PRODUCT_ID_4CH;
    // mVars.product.channelCount = 2;
    
    mVars.state = INIT;
    mVars.prevState = OFF;
    
    mVars.product.version = (version_t) {
        .major = VERSION_MAJOR,
        .minor = VERSION_MINOR,
        .rev = VERSION_REV
    };
    
    mVars.product.build = (build_t) {
        .year = BUILD_YEAR,
        .month = BUILD_MONTH,
        .day = BUILD_DAY,
        .rev = BUILD_REV
    };
    
    // Config PLL for USB and SYS_CLK, then init SYS clock switch to PLL as source
    //TODO: Configure the main system clock.
    SYS_MainOscillatorInit();
    SYS_ConfigInterrupts(true); // configure as multi-vector interrupts
    SYS_SchedulerTimerInit();
    SPI3_Init();
}

void Main_InitTasks(uint16_t taskInterval_ms) {
    if (mVars.state != INIT) { return; }
    
    static uint16_t startupTimer = 0;
    startupTimer += taskInterval_ms;
    
    if (startupTimer > AMP_START_DELAY_MS) {
        Main_SetAmpState(NORMAL);
        //TODO: un-mute, etc.
    }
}

void Main_ShutdownTasks(uint16_t taskInterval_ms) {
    if (mVars.state != SHUTDOWN) { return; }
    
    static uint16_t tmrShutdown = 0;
    tmrShutdown += taskInterval_ms;
    
    if (tmrShutdown > SHUTDOWN_DELAY_MS) {
        tmrShutdown = 0;
        
        if(!GPIO_IsRemoteConnected()) {
            GPIO_SetSelfRemote(true);
            //TODO: mute and kill self-remote, save settings.
        }
        else {
            //TODO: restore previous state
        }
    }
}

int main ( void )
{
    //<editor-fold desc="Initializations" defaultstate="collapsed">
    SYS_DisableGlobalInterrupts();
    Main_GpioInit();
    Main_Init();
    Main_RemoteEnableInit();
    Main_ClassDInit();
    Main_SdkInit();
    Main_LedInit();
    Main_AdcInit();
    // TODO: init channels here if you don't intend to save them using SaveSettings
    //Main_ChannelInit(mVars.channels, mVars.product.channelCount);
    Main_SaveSettingsInit();
    Main_DspInit();
    // Necessary for USB init.
    SYS_Initialize(&mVars.product);
    USB_Init();
    Main_ProtectionInit();
    Main_PsuInit();
    SYS_EnableGlobalInterrupts();
    //</editor-fold>

    while(true) {
        static schedulerStatus_t schedulerStatus;
        schedulerStatus = SCH_TickerStatus();
        
        SYS_Tasks();    // Handle USB
        
        //<editor-fold defaultstate="collapsed" desc="1ms">
        if (schedulerStatus.mSecTick) {
            
        }
        //</editor-fold>
        //<editor-fold defaultstate="collapsed" desc="10ms">
        if (schedulerStatus.mSecTenTick) {
            
        }
        //</editor-fold>
        //<editor-fold defaultstate="collapsed" desc="100ms">
        if (schedulerStatus.mSecHundredTick) {
            
        }
        //</editor-fold>
        //<editor-fold defaultstate="collapsed" desc="1 sec">
        if (schedulerStatus.SecTick) {

        }
        //</editor-fold>
        //<editor-fold defaultstate="collapsed" desc="1 min">
        if (schedulerStatus.MinTick) {
            
        }
        //</editor-fold>
        
        //<editor-fold defaultstate="collapsed" desc="Clear Ticker and Wait">
        if (schedulerStatus.TicksBitMap) {
           SCH_TickerClear();
        }   
        while(!IFS0bits.T1IF);
        IFS0bits.T1IF = 0;
        //</editor-fold>

        SCH_SchedulerTicker();
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

