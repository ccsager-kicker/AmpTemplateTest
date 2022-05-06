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
#include "System.h"

#define CLASS_D_CLK_FREQ        133000

static void _ClassDClockInit(void);

void Main_ClassDInit(void) {
    _ClassDClockInit();
}

static void _ClassDClockInit(void) {
    SYS_RefClockInit(CLASS_D_CLK_FREQ, SYS_CLOCK, REFO_SYSCLK);
    SYS_RefClockOutputEnable(true);
}
