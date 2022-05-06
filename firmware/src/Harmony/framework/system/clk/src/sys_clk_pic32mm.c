/*******************************************************************************
  SYS CLK Static Functions for Clock System Service

  Company:
    Microchip Technology Inc.

  File Name:
    sys_clk_static.c

  Summary:
    SYS CLK static function implementations for the Clock System Service.

  Description:
    The Clock System Service provides a simple interface to manage the oscillators
    on Microchip microcontrollers. This file defines the static implementation for the 
    Clock System Service.
    
  Remarks:
    Static functions incorporate all system clock configuration settings as
    determined by the user via the Microchip Harmony Configurator GUI.  It provides 
    static version of the routines, eliminating the need for an object ID or 
    object handle.
    
    Static single-open interfaces also eliminate the need for the open handle.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "system_config.h"
#include "system_definitions.h"
#include "peripheral/osc/plib_osc.h"
#include "system/devcon/sys_devcon.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

#if defined(__PIC32MM__)
// *****************************************************************************
/* Function:
    void SYS_CLK_Static_Initialize ( const SYS_CLK_INIT const * clkInit )

  Summary:
    Initializes hardware and internal data structure of the System Clock.

  Description:
    This function initializes the hardware and internal data structure of System
    Clock Service.

  Remarks:
    This is configuration values for the static version of the Clock System Service 
    module is determined by the user via the Microchip Harmony Configurator GUI.
    This template will build a sys_clk_static.h and sys_clk_static.c file with 
    the configuration per the user's choice.

    The objective is to eliminate the user's need to be knowledgeable in the function of
    the 'configuration bits' to configure the system oscillators. 
*/

void SYS_CLK_Initialize( const SYS_CLK_INIT const * clkInit )
{
    SYS_DEVCON_SystemUnlock();
    
    // ORPOL disabled; SIDL disabled; SRC SOSC; TUN Center frequency; POL disabled; ON disabled; 
    OSCTUN = 0x00;
    // PLLODIV 1:4; PLLMULT 8x; PLLICLK POSC; 
    PLIB_OSC_SysPLLOutputDivisorSet(OSC_ID_0, OSC_SYSPLL_OUT_DIV_4);
    PLIB_OSC_SysPLLMultiplierSelect(OSC_ID_0, OSC_SYSPLL_OUT_MULT_8);
    PLIB_OSC_SysPLLInputClockSourceSet(OSC_ID_0, OSC_SYSPLL_IN_CLK_SOURCE_POSC);

    // SBOREN disabled; VREGS disabled; RETEN disabled; 
    PWRCON = PWRCON | 0x00;
    
    //Clear NOSC,CLKLOCK and OSWEN bits
    OSCCONCLR = _OSCCON_NOSC_MASK | _OSCCON_CLKLOCK_MASK | _OSCCON_OSWEN_MASK;

    // CF No Clock Failure;
    // FRCDIV FRC/1; 
    // SLPEN Device will enter Idle mode when a WAIT instruction is issued; 
    // NOSC SPLL; 
    // SOSCEN disabled; 
    // CLKLOCK Clock and PLL selections are not locked and may be modified; 
    // OSWEN Switch is Complete; 
    PLIB_OSC_ClockSwitchingAbort(OSC_ID_0);
    PLIB_OSC_SysClockSelect(OSC_ID_0, OSC_SYS_PLL);
    PLIB_OSC_FRCDivisorSelect( OSC_ID_0, OSC_FRC_DIV_1);

    /* Disable REFCLKO1*/
    PLIB_OSC_ReferenceOscDisable ( OSC_ID_0, OSC_REFERENCE_1 );
    /* Disable REFCLK1_OE*/
    PLIB_OSC_ReferenceOutputDisable ( OSC_ID_0, OSC_REFERENCE_1 );

    SYS_DEVCON_SystemLock();
    
    // Wait for Clock switch to occur 
    while(!PLIB_OSC_ClockSwitchingIsComplete(OSC_ID_0)); 
    // ON disabled; DIVSWEN disabled; RSLP disabled; ROSEL SYSCLK; OE disabled; SIDL disabled; RODIV 0; 
    REFO1CON = REFO1CON | 0x00;
    while(!REFO1CONbits.ACTIVE & REFO1CONbits.ON);
    // ROTRIM 0; 
    REFO1TRIM = REFO1TRIM | 0x00;
}

//******************************************************************************
/* Function:
    inline uint32_t SYS_CLK_SystemFrequencyGet ( void )

  Summary:
    Gets the system clock frequency in Hertz.

  Description:
    This function gets the System clock frequency in Hertz.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    System clock frequency in Hertz.

  Example:
    <code>
    uint32_t sysClockHz;

    sysClockHz = SYS_CLK_SystemFrequencyGet ( );
    </code>

  Remarks:
 */

inline uint32_t SYS_CLK_SystemFrequencyGet ( void )
{
    return SYS_CLK_FREQ;
}

//******************************************************************************
/* Function:
    inline uint32_t SYS_CLK_PeripheralFrequencyGet ( CLK_BUSES_PERIPHERAL peripheralBus )

  Summary:
    Gets the selected clock peripheral bus frequency in Hertz.

  Description:
    This function gets the selected peripheral bus clock frequency in Hertz.

  Precondition:
    None.

  Parameters:
	peripheralBus - Reference clock bus selection. One of the possible value from
				CLK_BUSES_PERIPHERAL enum. For devices that do not have multiple
				clock channels for Reference clock, CLK_BUS_PERIPHERAL_1 should be
				the selection.

  Returns:
    Clock frequency in Hertz.

  Example:
    <code>
    unsigned long peripheralClockHz;

    peripheralClockHz = SYS_CLK_PeripheralFrequencyGet ( CLK_BUS_PERIPHERAL_5 );
    </code>

  Remarks:
	Most of the devices doesn't have multiple Peripheral clock buses. In that case, 
	pass CLK_USB_PERIPHERAL_1 as the bus number.
 */

inline uint32_t SYS_CLK_PeripheralFrequencyGet ( CLK_BUSES_PERIPHERAL peripheralBus )
{
    return SYS_CLK_BUS_PERIPHERAL_1;
}


//******************************************************************************
/* Function:
    inline uint32_t SYS_CLK_ReferenceFrequencyGet ( CLK_BUSES_REFERENCE referenceBus )

  Summary:
    Gets the selected Reference clock bus frequency in Hertz.

  Description:
    This function gets frequency of the selected Reference clock bus in Hertz.

  Precondition:
    None.

  Parameters:
	peripheralBus - Reference clock bus selection. One of the possible value from
				CLK_BUSES_REFERENCE enum. For devices that do not have multiple
				clock channels for Reference clock, CLK_BUS_REFERENCE_1 should be
				the selection.

  Returns:
    Clock frequency in Hz.

  Example:
    <code>
    unsigned long sysClockOutputHz;

    sysClockOutputHz = SYS_CLK_ReferenceFrequencyGet ( CLK_BUS_REFERENCE_3 );
    </code>

  Remarks:
    None.
 */

inline uint32_t SYS_CLK_ReferenceFrequencyGet ( CLK_BUSES_REFERENCE referenceBus )
{
	return 0;

}

/******************************************************************************
  Function:
    void SYS_CLK_SecondaryOscillatorEnable ( void )

  Summary:
    Enables the secondary oscillator.

  Description:
    This function enables the secondary oscillator.

  Remarks:
    For more details refer sys_clk.h.
*/

void SYS_CLK_SecondaryOscillatorEnable ( void )
{
    /* Check for secondary oscillator status */
    if (!PLIB_OSC_SecondaryIsEnabled(OSC_ID_0))
    {    
        /* Unlock and enable secondary oscillator */
        SYS_DEVCON_SystemUnlock();
        
        PLIB_OSC_SecondaryEnable(OSC_ID_0);
        
        SYS_DEVCON_SystemLock();
    }
}

/******************************************************************************
  Function:
    void SYS_CLK_SecondaryOscillatorDisable ( void )

  Summary:
    Disables the secondary oscillator.

  Description:
    This function disables the secondary oscillator.

  Remarks:
    For more details refer sys_clk.h.
*/

void SYS_CLK_SecondaryOscillatorDisable ( void )
{
    /* Check for secondary oscillator status */
    if (PLIB_OSC_SecondaryIsEnabled(OSC_ID_0))
    {    
        /* Unlock and disable secondary oscillator*/
        SYS_DEVCON_SystemUnlock();
        
        PLIB_OSC_SecondaryDisable(OSC_ID_0);
        
        SYS_DEVCON_SystemLock();
    }
}

/******************************************************************************
  Function:
    bool SYS_CLK_SecondaryOscillatorIsEnabled ( void )

  Summary:
    Identifies whether secondary oscillator is enabled or disabled.

  Description:
    This function identifies whether the secondary oscillator is enabled or 
    disabled.
    
  Remarks:
    For more details refer sys_clk.h.
*/

bool SYS_CLK_SecondaryOscillatorIsEnabled ( void )
{
    return (PLIB_OSC_SecondaryIsEnabled(OSC_ID_0));
}
#endif