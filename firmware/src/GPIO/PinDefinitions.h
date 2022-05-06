/* 
  @Company
    Stillwater Designs & Audio, Inc.
  
  @Filename
    PinDefinitions.h
  
  @Author
    iaoswa
 
  @Summary
    Brief description of the file.
 
  Creation Date: October 14, 2020
 */

#ifndef _PINDEFEINITIONS_H
#define	_PINDEFINITIONS_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <xc.h>

// TODO: End-user specific
#define IO_SELF_REMOTE(f)           f(A, 6)
#define TRIG_AMP_MUTE(f)            f(B, 3)
#define LED_POWER(f)                f(B, 8)
#define LED_PROTECT(f)              f(B, 7)
#define IO_REM_INPUT(f)             f(B, 9)
#define PROTECT_PS(f)               f(D, 4)
#define PROTECT_CLASS_D(f)          f(D, 2)

#define SW_E2P_RESET(f)             f(C,10)
#define SW_LED_COLOR(f)             f(C,13)
#define SW_LED_BRIGHTNESS(f)        f(B, 2)

#define LED_GAIN_MATCH_A1(f)        f(C, 6)
#define LED_GAIN_MATCH_A2(f)        f(C, 7)
#define LED_GAIN_MATCH_A3(f)        f(C, 8)
#define LED_GAIN_MATCH_SUB(f)       f(D, 1)

#define IO_ROLLBACK_PS_FR(f)        f(A,12)
#define IO_ROLLBACK_PS_SUB(f)       f(A,11)

#define IO_E2P_CHIP_SEL(f)          f(C, 3)

#define IO_DSP_RESET(f)             f(A,14)
#define IO_DSP_CHIP_SEL(f)          f(A,15)

#define IO_DAC_RESET(f)             f(D, 3)
#define IO_DAC_CHIP_SEL(f)          f(C, 4)

#define IO_ADC_RESET(f)             f(D, 0)
#define IO_ADC_CHIP_SEL(f)          f(C, 5)

#define IO_MULTI_CH_AMP_SEL(f)      f(A, 5)

//#define ADC_SUB_SENSE(f)            f(A, 3)

#define ADC_TEMPERATURE(f)          f(C, 1)
#define ADC_BATTERY_VOLTAGE(f)      f(C, 0)

#define UART_RX_PIN(f)              f(A, 4)
#define UART_TX_PIN(f)              f(A, 9)

#define SPI_DATA_IN(f)              f(A, 7)

#define REMOTE_RESET_PIN(f)         f(A,13)

#define USB_VBUS(f)                 f(B, 6)

#endif
