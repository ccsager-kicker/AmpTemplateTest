/*
  @Company
    Stillwater Designs & Audio, Inc.
 
  @Filename
    Main_Remote.c
 
  @Author
   Ian Oswald

  @Summary
    Brief description of the file.

  Creation Date: August 8, 2020
 */

/* Section: Included Files                                                    */
#include <stdint.h>
#include <stdbool.h>

#include "Main.h"
#include "IEEE754.h"
#include "IUART.h"
#include "KickerSdkTypes.h"
#include "IKickerSdk.h"
#include "ISystemLingo.h"
#include "ISoundLingo.h"
#include "IRemoteLingo.h"
#include "ITimeDelayLingo.h"
#include "IEqualizerLingo.h"
#include "IDSP.h"
#include "ADAU1701.h"
#include "usb.h"
#include "IKickerProduct.h"

//<editor-fold defaultstate="collapsed" desc="Bootloader Vars">
uint32_t __attribute__((persistent,keep,section(".startBootLoad"),address(0xA0001000))) startBootload;
uint16_t __attribute__((persistent,keep,section(".productID"),address(0xA0001020))) productID;

// Version information
uint16_t __attribute__((persistent,keep,section(".versionMajor"),address(0xA0001030))) versionMajor;
uint16_t __attribute__((persistent,keep,section(".versionMinor"),address(0xA0001040))) versionMinor;
uint16_t __attribute__((persistent,keep,section(".versionRev"),address(0xA0001050))) versionRev;

// Build information
uint16_t __attribute__((persistent,keep,section(".buildYear"),address(0xA0001060))) buildYear;
uint16_t __attribute__((persistent,keep,section(".buildMonth"),address(0xA0001070))) buildMonth;
uint16_t __attribute__((persistent,keep,section(".buildDay"),address(0xA0001080))) buildDay;
uint16_t __attribute__((persistent,keep,section(".buildRev"),address(0xA0001090))) buildRev;
//</editor-fold>


static void Main_RemoteSdkReceiveComplete(sdkBuffer_t sdkBuffer);
static void Main_ProcessSdkRx(sdkBuffer_t *thisBuffer);
static void Main_ProcessSdkTx(void);
static bool SdkBufferHasData(void);
static bool HasUpdateToSend(void);
static bool ProcessSdkInputBuffer(void);
static uint8_t GetSdkChannel(void);
static TxFunction SdkTransmit;

static sdkBuffer_t processBuffer[SDK_RX_DEPTH];
static uint8_t sdkProcessBufferHead;
static uint8_t sdkProcessBufferTail;

typedef struct remoteFlag {
    unsigned txComplete         :1;
    unsigned rxComplete         :1;
}remoteFlag_t;
volatile remoteFlag_t remoteFlags;

uint8_t sdkIndex;


void Main_SdkInit() {
    //TODO: User set comm protocol
    uartIsr_t mIsr = { .txEnabled = false, .rxEnabled = true };
    IUart_Init(3, 115200, mIsr);
    IUart_SetRxInterruptHandler(3, &IKickerSdk_ReceiveByteHandler);
    
    USB_SetReceiveHandler(&IKickerSdk_ReceiveBufferHandler);
    SdkTransmit = IUart_GetTransmitFunc(3);
    
    SysLingo_SetTransmitFunc(SdkTransmit);
    
    SndLingo_SetTransmitFunc(SdkTransmit);
    SndLingo_SetGetChannelFunc(GetSdkChannel);
    
    EqLingo_SetTransmitFunc(SdkTransmit);
    EqLingo_SetGetChannelFunc(GetSdkChannel);
    
    DlyLingo_SetTransmitFunc(SdkTransmit);
    DlyLingo_SetGetChannelFunc(GetSdkChannel);
    
    RemLingo_SetTransmitFunc(SdkTransmit);
    
    IKickerSdk_SetRxCompleteCallback(Main_RemoteSdkReceiveComplete);
}

static bool SdkBufferHasData() {
    return sdkProcessBufferHead != sdkProcessBufferTail;
}

static bool ProcessSdkInputBuffer() {
    /* Start processing SDK data from the tail end. FIFO. */
    if (processBuffer[sdkProcessBufferTail].ready) {
        Main_ProcessSdkRx(&processBuffer[sdkProcessBufferTail]);
        processBuffer[sdkProcessBufferTail++].ready = false;
    }
    if(sdkProcessBufferTail >= SDK_RX_DEPTH) // Wrap tail index / bounds checking.
    { sdkProcessBufferTail = 0; }
}

void Main_SdkTasks(uint16_t taskInterval_ms) {
    
    while(SdkBufferHasData()) {
        ProcessSdkInputBuffer();
    }
}

static void Main_RemoteSdkReceiveComplete(sdkBuffer_t sdkBuffer) {
    processBuffer[sdkProcessBufferHead++] = sdkBuffer;
    if(sdkProcessBufferHead >= SDK_RX_DEPTH)
    { sdkProcessBufferHead = 0; }
}

static uint32_t ArrayToUInt32(uint8_t *value) {
    return (value[0]<<24) | (value[1]<<16) | (value[2]<<8) | (value[3]);
}

static void SetSdkChannel(uint8_t index) {
    sdkIndex = index;
}

static uint8_t GetSdkChannel() {
    return sdkIndex;
}

static void Main_ProcessSdkRx(sdkBuffer_t *thisBuffer) {
    uint8_t byteValue;
    uint32_t value = 0;
    float floatValue;
    bool boolValue;
    bool success = true;
    channel_t *mChannel;
    if(thisBuffer->data[0] < mVars.product.channelCount) {
        mChannel = &mVars.channels[thisBuffer->data[0]];
        SetSdkChannel(mChannel->channelNumber);
    }
    
    switch (thisBuffer->lingo) {
        case SYSTEM:    //<editor-fold defaultstate="collapsed" desc="SYSTEM">
            switch(thisBuffer->command) {
                case SysAck:
                    break;
                case RetProductID:
                    break;
                case GetProductID:
                    SysLingo_RetPID(mVars.product.productId);
                    break;
                case EnterUpdateMode:
                    startBootload = 2;
                    productID = mVars.product.productId;
                    versionMajor = mVars.product.version.major;
                    versionMinor = mVars.product.version.minor;
                    versionRev = mVars.product.version.rev;

                    buildYear = mVars.product.build.year;
                    buildMonth = mVars.product.build.month;
                    buildDay = mVars.product.build.day;
                    buildRev = mVars.product.build.rev;
                    __pic32_software_reset();
                    break;
                case SetSystemColor:
                    break;
                case GetSystemColor:
                    break;
                case GetBatteryVoltage:
                    SysLingo_RetBatteryVoltage(mVars.vBatt);
                    break;
                case GetAmpTemperature:
                    if(thisBuffer->data[0]) {
                        SysLingo_RetTemperature(mVars.temperatureC, true);
                    }
                    break;
                case GetProtectionStatus:
                    SysLingo_RetProtectionStatus(mVars.protections);
                    break;
                default:
                    success = false;
                    SysLingo_Ack(thisBuffer->command, COMMAND_NOT_SUPPORTED);
                    break;
            }
            break;
            //</editor-fold>
        case SOUND:     //<editor-fold defaultstate="collapsed" desc="SOUND">
            switch(thisBuffer->command) {
                case SndAck:
                    break;
                case SetXoverFreq:
                    value = (thisBuffer->data[2]<<8) | (thisBuffer->data[3]);
                    if(thisBuffer->data[1] == HIGH_PASS) {
                        mChannel->highPass.frequency = (uint16_t)value;
                        IDSP_SetXoverValue(mChannel, mChannel->highPass, DSP_XOVER_FREQ);
                    }
                    else {
                        mChannel->lowPass.frequency = (uint16_t)value;
                        IDSP_SetXoverValue(mChannel, mChannel->lowPass, DSP_XOVER_FREQ);
                    }
                    
                    SndLingo_Ack(thisBuffer->command, SUCCESS);
                    break;
                case GetXoverFreq:
                    if(thisBuffer->data[1] == HIGH_PASS) {
                        SndLingo_RetHpXoverFreq(mChannel->highPass.frequency);
                    }
                    else {
                        SndLingo_RetLpXoverFreq(mChannel->lowPass.frequency);
                    }
                    break;
                case SetXoverAlignment:
                    if(thisBuffer->data[1] == HIGH_PASS) {
                        mChannel->highPass.alignment = thisBuffer->data[2];
                        IDSP_SetXoverValue(mChannel, mChannel->highPass, DSP_XOVER_ALIGNMENT);
                    }
                    else {
                        mChannel->lowPass.alignment = thisBuffer->data[2];
                        IDSP_SetXoverValue(mChannel, mChannel->lowPass, DSP_XOVER_ALIGNMENT);
                    }
                    SndLingo_Ack(thisBuffer->command, SUCCESS);
                    break;
                case GetXoverAlignment:
                    if(thisBuffer->data[1] == HIGH_PASS) {
                        SndLingo_RetHpXoverAlignment(mChannel->highPass.alignment);
                    }
                    else {
                        SndLingo_RetLpXoverAlignment(mChannel->lowPass.alignment);
                    }
                    break;
                case SetXoverSlope:
                    byteValue = thisBuffer->data[2];
                    if(thisBuffer->data[1] == HIGH_PASS) {
                        mChannel->highPass.slope = byteValue;
                        IDSP_SetXoverValue(mChannel, mChannel->highPass, DSP_XOVER_SLOPE);
                    }
                    else {
                        mChannel->lowPass.slope = byteValue;
                        IDSP_SetXoverValue(mChannel, mChannel->lowPass, DSP_XOVER_SLOPE);
                    }
                    SndLingo_Ack(thisBuffer->command, SUCCESS);
                    break;
                case GetXoverSlope:
                    if(thisBuffer->data[1] == HIGH_PASS) {
                        SndLingo_RetHpXoverSlope(mChannel->highPass.slope);
                    }
                    else {
                        SndLingo_RetLpXoverSlope(mChannel->lowPass.slope);
                    }
                    break;
                case SetXoverEnable:
                    if(thisBuffer->data[1] == HIGH_PASS) {
                        mChannel->highPass.isEnabled = thisBuffer->data[2];
                        IDSP_SetXoverValue(mChannel, mChannel->highPass, DSP_XOVER_ENABLED);
                    }
                    else {
                        mChannel->lowPass.isEnabled = thisBuffer->data[2];
                        IDSP_SetXoverValue(mChannel, mChannel->lowPass, DSP_XOVER_ENABLED);
                    }
                    SndLingo_Ack(thisBuffer->command, SUCCESS);
                    break;
                case GetXoverEnable:
                    if(thisBuffer->data[1] == HIGH_PASS) {
                        SndLingo_RetHpXoverEnable(mChannel->highPass.isEnabled);
                    }
                    else {
                        SndLingo_RetLpXoverEnable(mChannel->lowPass.isEnabled);
                    }
                    break;
                case SetChannelGain:
                    if(thisBuffer->data[1]) {   //output
                        mChannel->gain.output = FromIEEE754(ArrayToUInt32(&thisBuffer->data[2]));
                        IDSP_SetGainValue(mChannel, mChannel->gain, DSP_OUTPUT_GAIN);
                    }
                    else {  //input
                        mChannel->gain.input = FromIEEE754(ArrayToUInt32(&thisBuffer->data[2]));
                        IDSP_SetGainValue(mChannel, mChannel->gain, DSP_INPUT_GAIN);
                    }
                    SndLingo_Ack(thisBuffer->command, SUCCESS);
                    break;
                case GetChannelGain:
                    if(thisBuffer->data[1]) {   //output
                        SndLingo_RetGain(true, mChannel->gain.output);
                    }
                    else {  //input
                        SndLingo_RetGain(false, mChannel->gain.input);
                    }
                    break;
                case GetGainMatch:
                    if(mChannel->channelNumber < mVars.product.channelCount) {
                        SndLingo_RetGainMatch(mVars.isClipping[mChannel->channelNumber]);
                    }
                    else { success = false; }
                    break;
                case SetPolarity:
                    mChannel->outputPolarity = thisBuffer->data[1];
                    IDSP_SetOutputPolarity(mChannel, mChannel->outputPolarity);
                    SndLingo_Ack(thisBuffer->command, SUCCESS);
                    break;
                case GetPolarity:
                    SndLingo_RetPolarity(mChannel->outputPolarity);
                    break;
                case SetSummingStatus:
                    mChannel->channelMode = thisBuffer->data[1];
                    IDSP_SetSumming(mChannel, mChannel->channelMode, thisBuffer->data[1]);
                    SndLingo_Ack(thisBuffer->checksum, SUCCESS);
                    break;
                case GetSummingStatus:
                    SndLingo_RetSummingStatus(mChannel->channelMode);
                    break;
                default:
                    success = false;
                    SndLingo_Ack(thisBuffer->command, COMMAND_NOT_SUPPORTED);
                    break;
            }
//            if(success)
//            { SndLingo_Ack(processBuffer.command, SUCCESS); }
            break;
            //</editor-fold>
        case REMOTE:    //<editor-fold defaultstate="collapsed" desc="REMOTE">
            
            mChannel = &mVars.channels[mVars.product.channelCount - 1];
            SetSdkChannel(mChannel->channelNumber);
            switch(thisBuffer->command) {
                case RemAck:
                    break;
                case SetRemoteGainLevel:
                    mChannel->gain.output = FromIEEE754(ArrayToUInt32(&thisBuffer->data[0]));
                    IDSP_SetGainValue(mChannel, mChannel->gain, DSP_OUTPUT_GAIN);
                    RemLingo_Ack(thisBuffer->command, SUCCESS);
                    break;
                case GetRemoteGainLevel:
                    RemLingo_RetGainLevel(mChannel->gain.output);
                    break;
                case SetRemoteBassEnhanceLevel:   // KickEq Enhancement
                    mChannel->superBass.intensity = FromIEEE754(ArrayToUInt32(&thisBuffer->data[0]));
                    DSP_SetSuperBassIntensity(mChannel->superBass.intensity);
                    RemLingo_Ack(thisBuffer->command, SUCCESS);
                    break;
                case GetRemoteBassEnhanceLevel:   // KickEq Enhancement
                    RemLingo_RetBassEnhanceLevel(mChannel->superBass.intensity);
                    break;
                case GetClipStatus:
                    break;
                default:
                    success = false;
                    RemLingo_Ack(thisBuffer->command, COMMAND_NOT_SUPPORTED);
                    break;
            }
            break;
            //</editor-fold>
        case EQUALIZER: //<editor-fold defaultstate="collapsed" desc="EQ">
            switch(thisBuffer->command) {
                case EqAck:
                    break;
                case SetCenterFreq:
                    value = (thisBuffer->data[2]<<8) | (thisBuffer->data[3]);
                    mChannel->kickerEq.boost.frequency = value;
//                    Main_DspSetKickEq(mChannel);
                    IDSP_SetEqValue(mChannel, mChannel->kickerEq.boost, DSP_EQ_FREQ);
                    EqLingo_Ack(thisBuffer->command, SUCCESS);
                    break;
                case GetCenterFreq:
                    EqLingo_RetCenterFreq(0, (uint16_t)mChannel->kickerEq.boost.frequency);
                    break;
                case SetParametricQ:
                    mChannel->kickerEq.boost.bandwidth = FromIEEE754(ArrayToUInt32(&thisBuffer->data[2]));
//                    Main_DspSetKickEq(mChannel);
                    IDSP_SetEqValue(mChannel, mChannel->kickerEq.boost, DSP_EQ_BANDWIDTH);
                    EqLingo_Ack(thisBuffer->command, SUCCESS);
                    break;
                case GetParametricQ:
                    EqLingo_RetQValue(0, mChannel->kickerEq.boost.bandwidth);
                    break;
                case SetEqGain:
                    mChannel->kickerEq.boost.gain = FromIEEE754(ArrayToUInt32(&thisBuffer->data[2]));
//                    Main_DspSetKickEq(mChannel);
                    IDSP_SetEqValue(mChannel, mChannel->kickerEq.boost, DSP_EQ_GAIN);
                    EqLingo_Ack(thisBuffer->command, SUCCESS);
                    break;
                case GetEqGain:
                    EqLingo_RetGain(0, mChannel->kickerEq.boost.gain);
                    break;
                default:
                    success = false;
                    EqLingo_Ack(thisBuffer->command, COMMAND_NOT_SUPPORTED);
                    break;
            }
//            if(success)
//            { EqLingo_Ack(processBuffer->command, SUCCESS); }
            break;
            //</editor-fold>
        case TIME_DELAY://<editor-fold defaultstate="collapsed" desc="TIME-DELAY">
            switch(thisBuffer->command) {
                case DlyAck:
                    break;
                case SetTime:
                    mChannel->delay.timeMs = FromIEEE754(ArrayToUInt32(&thisBuffer->data[2]));
//                    Main_DspSetDelay(mChannel);
                    IDSP_SetDelayValue(mChannel, mChannel->delay, DSP_DELAY_TIME);
                    DlyLingo_Ack(thisBuffer->command, SUCCESS);
                    break;
                case GetTime:
                    DlyLingo_RetTime(0, mChannel->delay.timeMs);
                    break;
                default:
                    success = false;
                    DlyLingo_Ack(thisBuffer->command, COMMAND_NOT_SUPPORTED);
                    break;
            }
//            if(success)
//            { DlyLingo_Ack(processBuffer.command, SUCCESS); }
            break;
            //</editor-fold>
        default:
            success = false;
            SysLingo_Ack(thisBuffer->lingo, LINGO_NOT_SUPPORTED);
            break;
    }
    thisBuffer->ready = false;
}

/* *****************************************************************************
End of File
 */
