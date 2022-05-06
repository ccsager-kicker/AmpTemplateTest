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
#include "PinFunctions.h"
#include "AmpDefs.h"

static void PollSwitches(void);
struct {
    switch_t sw;
} gpioVars;

enum {
    POLLING,
    DEBOUNCING,
    WAIT_RELEASE,
    HOLDING,
    PROCESSING
} gpioState;


void Main_GpioInit(void) {
    GPIO_Init();
    GPIO_RemappablePinsConfig();
    
    // TODO: Set Initial pin values here. e.g. may not want reset pins low on startup.
}

void Main_GpioTasks(uint16_t taskInterval_ms) {
    
    PollSwitches();
}

static void PollSwitches(void) {
    uint8_t x;
    
    switch(gpioState) {
        case POLLING:
            for(x = 0; x < DEFAULT; x++) {
                bool mValue = GPIO_GetSwitchValue(x);
                if(!mValue) {
                    gpioVars.sw.isDebouncing = true;
                    gpioVars.sw.counter = 0;
                    gpioVars.sw.type = x;
                    gpioState = DEBOUNCING;
                    break;
                }
            }
            break;
            
        case DEBOUNCING:
            gpioVars.sw.counter++;
            if(gpioVars.sw.counter > 5) {
                gpioVars.sw.isDebouncing = false;
                gpioState = !GPIO_GetSwitchValue(gpioVars.sw.type) ? WAIT_RELEASE : POLLING;
            }
            break;
            
        case WAIT_RELEASE:
            
            if(!GPIO_GetSwitchValue(gpioVars.sw.type)) {
                gpioVars.sw.counter++;
                if(gpioVars.sw.counter > 75) {
                    gpioState = HOLDING;
                }
            }
            else {
                gpioState = PROCESSING;
            }
            break;
            
        case HOLDING:
            
            if(GPIO_GetSwitchValue(gpioVars.sw.type)) {
                gpioState = POLLING;
            }
            else {
                gpioVars.sw.counter++;
                
                switch (gpioVars.sw.type) {
                    case EEPROM_RESET:
                        // if(gpioVars.sw.counter > DATA_EE_ERASE_BTN_HOLD) {
                        //     Main_EEPromReset();
                        //     gpioState = POLLING;
                        // }
                        break;
                    default:
                        break;
                }
            }
            
            break;
            
        case PROCESSING:
            
            switch(gpioVars.sw.type) {
                case LED_BRIGHTNESS:
                    // Main_RgbLedIncBrightness();
                    break;
                    
                case LED_COLOR:
                    // Main_RgbLedChangeColor();
                    break;
                    
                case EEPROM_RESET:
                    
                    break;
                    
            }
            gpioState = POLLING;
            break;
    }
}