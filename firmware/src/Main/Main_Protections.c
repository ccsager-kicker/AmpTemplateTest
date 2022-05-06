/* 
  @Company
    Stillwater Designs & Audio, Inc.
  
  @Filename
    Main_Protections.c
  
  @Author
    iaoswa
 
  @Summary
    Brief description of the file.
 
  Creation Date: October 9, 2020
 */

/* Section: Included Files                                                    */

#include <stdint.h>
#include <stdbool.h>

#include "Main.h"
#include "IPSU.h"
#include "System.h"

/* Elapsed time since protection was activated. */
uint16_t protectUpTime_ms;

static void _ClassDProtectionInit(void);
static void _ClassDProtectEvent(void);
static void _PowerSupplyProtectInit(void);
static void _PowerSupplyProtectEvent(void);

static void _UpdateProtectionUpTime(uint16_t interval_ms);

static void _CheckOverVoltage(void);
static void _CheckUnderVoltage(void);
static void _CheckThermal(void);
static void _ProcessProtections(void);

static void _ActivateProtection(void);
static void _ClearProtection(void);

static double CalculateSystemRollback(bool isFullRange);

void Main_ProtectionInit(void) {
    _ClassDProtectionInit();
    _PowerSupplyProtectInit();
}

void Main_ProtectionTasks(uint16_t taskInterval_ms) {
    if (mVars.state == SHUTDOWN) { return; }
    
    _UpdateProtectionUpTime(taskInterval_ms);
    
    _CheckOverVoltage();
    _CheckUnderVoltage();
    _CheckThermal();
    
    _ProcessProtections();
}

static void _ClassDProtectionInit(void) {
    SYS_InitInterrupt0(6, true);
    SYS_SetInt0CallbackFunc(&_ClassDProtectEvent);
    SYS_EnableInterrupt0(true);
}

static void _ClassDProtectEvent(void) {
    
    if(!GPIO_IsProtectedClassD()) {
        mVars.protections.overCurrentD = true;
        _ActivateProtection();
    }
}

static void _PowerSupplyProtectInit(void) {
    SYS_InitInterrupt1(6, true);
    SYS_SetInt1CallbackFunc(&_PowerSupplyProtectEvent);
    SYS_EnableInterrupt1(true);
}

static void _PowerSupplyProtectEvent(void) {
    
    mVars.protections.overCurrentPS = true;
    _ActivateProtection();
    GPIO_SetSelfRemote(false);    // Shutdown amp.
}

static void _ActivateProtection(void) {
    if(mVars.protections.underVoltage || mVars.protections.overVoltage || mVars.protections.overCurrentPS) {
        Main_PsuStopMainPsu();
        GPIO_SetClassDDisable(true);
    }
    if(mVars.protections.thermalShutdown || mVars.protections.overCurrentD) {
        GPIO_SetClassDDisable(true);
    }
    
    Main_SetAmpState(PROTECTION);
}

static void _ClearProtection(void) {
    
    if (mVars.state == PROTECTION) {
        Main_SetAmpState(mVars.prevState);
        
        Main_PsuStartMainPsu();
        GPIO_SetClassDDisable(false);
    }
}

/**
 * Keep track of how long the amp has been in protection, by incrementing
 * the global variable protectUpTime_ms in intervals provided by interval_ms.
 * If the amp is not in protection, protectUpTime_ms is set to 0.
 * @param interval_ms
 *  The elapsed time, in milliseconds, since the last call.
 */
static void _UpdateProtectionUpTime(uint16_t interval_ms) {
    protectUpTime_ms = (mVars.state == PROTECTION) ? protectUpTime_ms + interval_ms : 0;
}

static void _CheckOverVoltage(void) {
    if (mVars.vBatt >= OVERVOLT_PROTECT_VOLT) {
        mVars.protections.overVoltage = true;
        mVars.protections.overVoltageWarning = false;
    }
    else if (mVars.vBatt < OVERVOLT_PROTECT_VOLT && mVars.vBatt > OVERVOLT_RECOVER_VOLT) {
        mVars.protections.overVoltageWarning = !mVars.protections.overVoltage;
    }
    else if (mVars.vBatt < OVERVOLT_RECOVER_VOLT && mVars.vBatt >= OVERVOLT_WARNING_VOLT) {
        mVars.protections.overVoltage = false;
        mVars.protections.overVoltageWarning = true;
    }
    else {
        mVars.protections.overVoltage = false;
        mVars.protections.overVoltageWarning = false;
    }
}

static void _CheckUnderVoltage(void) {
    if(mVars.vBatt <= UNDERVOLT_PROTECT_VOLT) {
        mVars.protections.underVoltage = true;
        mVars.protections.underVoltageWarning = false;
    }
    else if((mVars.vBatt > UNDERVOLT_PROTECT_VOLT) && (mVars.vBatt < UNDERVOLT_RECOVER_VOLT)) {
        mVars.protections.underVoltageWarning = !mVars.protections.underVoltage;
    }
    else if(mVars.vBatt >= UNDERVOLT_RECOVER_VOLT && mVars.vBatt <= UNDERVOLT_WARNING_VOLT) {
        mVars.protections.underVoltage = false;
        mVars.protections.underVoltageWarning = true;
    }
    else {
        mVars.protections.underVoltage = false;
        mVars.protections.underVoltageWarning = false;
    }
}

static void _CheckThermal(void) {
    if(mVars.temperatureC >= THERMAL_PROTECT_ACTIVATE_C) {
        mVars.protections.thermalShutdown = true;
    }
    else if(mVars.temperatureC < THERMAL_PROTECT_ACTIVATE_C && mVars.temperatureC > THERMAL_PROTECT_RECOVER_C) {
        mVars.protections.thermalWarning = !mVars.protections.thermalShutdown;
    }
    else if(mVars.temperatureC <= THERMAL_PROTECT_RECOVER_C && mVars.temperatureC >= ROLLBACK_ACTIVATE_C) {
        mVars.protections.thermalWarning = true;
        mVars.protections.thermalShutdown = false;
    }
    else if(mVars.temperatureC < ROLLBACK_ACTIVATE_C) {
        mVars.protections.thermalWarning = false;
        mVars.protections.thermalShutdown = false;
    }
}

static void _ProcessProtections(void) {
    
    // if a protection in 'protections' is active, activate protection.
    if (mVars.protections.protectionBitMap & SDK_PROTECTION_MASK) {
        
        if (mVars.state != PROTECTION) {
            _ActivateProtection();
        }
        else {
            /* 3 second wait for class D protection recovery. */
            if (protectUpTime_ms > PROTECT_RECOVER_RATE_MS) {
                protectUpTime_ms = 0;
                
                if (mVars.protections.overCurrentD) {
                    if (!GPIO_IsProtectedClassD()) {
                        mVars.protections.overCurrentD = false;
                    }
                }
            }
        }
    }
    // if a warning is active
    else if (mVars.protections.protectionBitMap & SDK_WARNING_MASK) {
        _ClearProtection();
    }
    else if (mVars.state != INIT){
        if (mVars.state != NORMAL) {
            _ClearProtection();
        }
    }
}