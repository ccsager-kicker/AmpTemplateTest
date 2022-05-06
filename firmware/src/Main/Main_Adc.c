/*
 * File:   ADC_readings.c
 * Author: fepire
 *
 * Created on March 31, 2020, 11:12 AM
 */

#include "Main.h"
#include "ADC.h"
#include <string.h>

#define AVG_BUFFER_SIZE         10
#define BATTERY_ADC_CHANNEL     12
#define THERM_ADC_CHANNEL       13

// Voltage divider resistors used to divide battery voltage down to a safe range.
#define VOLT_DIV_R_SERIES       12.0
#define VOLT_DIV_R_GROUND       2.2

#define DIODE_VF                0.40    //For accurate battery voltage reading

static void _AdcFillAveragingBuffers(void);
static uint16_t _AdcMaxValue(void);
static double _GetBatteryVolt(void);
static double _GetTemperature(uint8_t channel);
static double _GetAvgTemperature(void);
static double _GetAvgBatteryVoltage(void);

static double thermistorReadings[AVG_BUFFER_SIZE], voltageReadings[AVG_BUFFER_SIZE];
static double thermistorRollingSum, voltageRollingSum;

void Main_AdcInit(void) {
    ADC_Init();
    
    _AdcFillAveragingBuffers();
}

void Main_AdcTasks(uint16_t taskInterval_ms) {
    
    mVars.temperatureC = _GetAvgTemperature();
    mVars.vBatt = _GetAvgBatteryVoltage();
}

static void _AdcFillAveragingBuffers(void) {
    uint8_t i;
    for(i = 0; i < AVG_BUFFER_SIZE; i++) {
        thermistorReadings[i] = _GetTemperature(THERM_ADC_CHANNEL);
        thermistorRollingSum += thermistorReadings[i];
        
        voltageReadings[i] = _GetBatteryVolt();
        voltageRollingSum += voltageReadings[i];
    }
}

static double _GetAvgBatteryVoltage(void) {
    
    double thisVoltage = _GetBatteryVolt();
    static uint8_t index = 0;
    
    voltageRollingSum -= voltageReadings[index];    //Remove oldest reading
    voltageRollingSum += thisVoltage;   //Add new value to rolling sum
    voltageReadings[index] = thisVoltage;   //Add new value to buffer
    if(++index >= AVG_BUFFER_SIZE) 
        index = 0;
    
    return voltageRollingSum / AVG_BUFFER_SIZE;
}

static uint16_t _AdcMaxValue(void) {
    return ((1 << ADC_RESOLUTION_BITS) - 1);
}

static double _GetBatteryVolt(void) {

    uint16_t adcSample = IADC_Sample (BATTERY_ADC_CHANNEL);
    // resistor multiplication factor
    double vdivMultiplier = 1 / (VOLT_DIV_R_GROUND / (VOLT_DIV_R_SERIES + VOLT_DIV_R_GROUND));
    // voltage at the ADC pin
    double adcPinVoltage = (double)adcSample * ADC_VREF / _AdcMaxValue();
    // voltage measured is the ADC pin voltage * resistor network
    double vPlus = adcPinVoltage * vdivMultiplier;

    return vPlus + DIODE_VF;
}

static double _GetAvgTemperature(void) {
    
    double thisTemp = _GetTemperature(THERM_ADC_CHANNEL);
    static uint8_t index = 0;
    
    thermistorRollingSum -= thermistorReadings[index];
    thermistorRollingSum += thisTemp;
    thermistorReadings[index] = thisTemp;
    if(++index >= AVG_BUFFER_SIZE) 
        index = 0;
    
    return thermistorRollingSum / AVG_BUFFER_SIZE;
}

static double _GetTemperature(uint8_t channel)  {

    uint16_t adcSample = IADC_Sample (channel);
    // voltage at the ADC pin
    double adcPinVoltage = (double)adcSample * ADC_VREF / _AdcMaxValue();
    double temp = adcPinVoltage;

    temp = temp - 0.5;                // Subtract 500mV (normal output at 0 degrees C)
    temp = temp * 100;                // Multiply to achieve the 1 degree C per 10mV.
    return temp;
}


