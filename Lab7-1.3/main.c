// Program to digitize the MCU's internal temperature sensor's output voltage
// DriverLib Includes
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
// Standard Includes
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
volatile bool adc_done;
float calcTempF(uint32_t adc_value);

int main(void)
{
    // Halt WDT
    WDT_A_holdTimer();
    printf("MCLK: %u\n", MAP_CS_getMCLK());
    // Set reference voltage to 1.2 V and enable temperature sensor
    REF_A_enableReferenceVoltage();
    REF_A_enableTempSensor();
    REF_A_setReferenceVoltage(REF_A_VREF1_2V);
    // Initializing ADC (MCLK/1/1) with temperature sensor routed
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, ADC_TEMPSENSEMAP);
    // Set resolution
    ADC14_setResolution(ADC_14BIT);
    // Configure ADC Memory for temperature sensor data
    ADC14_configureSingleSampleMode(ADC_MEM0, false);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS,
ADC_INPUT_A22, false);
    // Configure the sample/hold time
    ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_192, ADC_PULSE_WIDTH_192);
    // Enable sample timer in manual iteration mode and interrupts
    ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);
    // Enable conversion
    ADC14_enableConversion();
    // Enabling Interrupts
    ADC14_enableInterrupt(ADC_INT0);
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();
    // Trigger conversion with software
    adc_done = false;
    while(1)
    {
        ADC14_toggleConversionTrigger();
        while (!adc_done)
        {
        }
        uint32_t adc_value = ADC14_getResult(ADC_MEM0);
//        printf("%u %fmV\n", adc_value, digitizedToAnalog(adc_value, 14, 1.2)*100);
        float temp = calcTempF(adc_value);
        printf("%u %fF\n", adc_value, temp);
        _delay_cycles(500 * 3000);
    }

}

float calcTempF(uint32_t adc_value){
    uint16_t calibration30C = SysCtl_getTempCalibrationConstant(SYSCTL_1_2V_REF,
   SYSCTL_30_DEGREES_C);
    uint16_t calibration85C = SysCtl_getTempCalibrationConstant(SYSCTL_1_2V_REF,
   SYSCTL_85_DEGREES_C);
    float slope = (float)(calibration85C - calibration30C) / (85 - 30);
    float y_intercept = calibration30C - (slope * 30);
   // printf("ADC_30: %d\n", adc_30C);
   // printf("ADC_85: %d\n", adc_85C);
   // printf("SLOPE: %f\n", slope);
   // printf("Y_INT: %f\n", y_int);
    float temp = (adc_value - y_intercept) / slope;
    return ((temp*1.8)+32);
}

double digitizedToAnalog(uint32_t adc_value, int resolution, float voltage)
{
   return (double)(voltage * (adc_value / (pow(2, resolution) - 1)));
}

// This interrupt happens every time a conversion has completed
void ADC14_IRQHandler(void)
{
    uint64_t status;
    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);
    if(status & ADC_INT0)
    {
        adc_done = true;
    }
}
