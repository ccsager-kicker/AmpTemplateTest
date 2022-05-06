/* ************************************************************************** */
 /* Fernando Eid Pires 09/05/2019
 * 
  @Company
 Stillwater Designs

  @Description
 User DSP functions
 */
/* ************************************************************************** */

#include "Main.h"
#include <math.h>   

static void Main_DspLoadChannels(void);
static void DspRollbackTasks(void);
static void GetOutputClipping(void);
static void GetGainMatch(void);

void Main_DspInit(void) {
    
    //TODO: Set function pointers for DSP, ADC, DAC, etc.
//    IADCAudio_SetTransmitFunc(&ISPI3_Write);
//    IADCAudio_SetChipSelectPinFunc(&GPIO_SetAdcChipSelect);
//    IADCAudio_SetResetFunc(&GPIO_SetAdcReset);
//    IADCAudio_SetDelayMsFunc(&IDelay_ms);
    
//    IDSP_SetDelayMsFunc(&IDelay_ms);
//    IDSP_SetDelayUsFunc(&IDelay_us);
//    IDSP_SetTransmitFunc(&ISPI3_Write);
//    IDSP_SetReadFunc(&ISPI3_Read);
//    IDSP_SetResetPinFunc(&GPIO_SetDspReset);
//    IDSP_SetChipSelectPinFunc(&GPIO_SetDspChipSelect);
    
    //TODO: Initialize DSP, ADC, DAC, etc.
//    IADCAudio_Init();
    IDSP_Init(DSP_SPI);
    Main_DspLoadChannels();
}

/* For recurring tasks involving the DSP. */
void Main_DspTasks(uint16_t taskInterval_ms) {
    DspRollbackTasks();
    GetOutputClipping();
    GetGainMatch();
    /* Maybe translate IO into DSP adjustments. 
     * e.g. turn a pot, change the cutoff frequency. */
}

static void Main_DspLoadChannels(void) {
    
    uint8_t i;
    for(i = 0; i < mVars.product.channelCount; i++) {
        channel_t *mCh = &mVars.channels[i];
        IDSP_SetGainValue(mCh, mCh->gain, DSP_INPUT_GAIN);
        IDSP_SetGainValue(mCh, mCh->gain, DSP_OUTPUT_GAIN);
        IDSP_SetXoverValue(mCh, mCh->highPass, DSP_XOVER_ENABLED);
        IDSP_SetXoverValue(mCh, mCh->lowPass,  DSP_XOVER_ENABLED);
        
        if(mCh->channelType == SUBWOOFER) {
            IDSP_SetEqValue(mCh, mCh->kickerEq.boost, DSP_EQ_GAIN);
            IDSP_SetOutputPolarity(mCh, mCh->outputPolarity);
            
            DSP_SetSuperBassIntensity(mCh->superBass.intensity);
        }
        else {
            IDSP_SetSumming(mCh, mCh->channelMode, DSP_SUM_STEREO);
            IDSP_SetDelayValue(mCh, mCh->delay, DSP_DELAY_TIME);
        }
    }
}

static void DspRollbackTasks(void) {
    //   Thermal rollback.
    // +   Power rollback.
    // -------------------
    //    System rollback.
}

static void GetOutputClipping(void) {
    
}

static void GetGainMatch(void) {
    
}

