#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/drivers/Board.h>
#define __MSP432P4XX__
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

#define TASKSTACKSIZE   2048

Void task1(UArg arg0, UArg arg1);
Void task2(UArg arg0, UArg arg1);
Void task3(UArg arg0, UArg arg1);
void send_message(char* msg);
float printAvg(float *values);
float calcTempF(uint32_t adc_value);

bool uartReady = false;
UART_Handle uart;
UART_Params uartParams;
Task_Struct task1Struct, task2Struct, task3Struct;
Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE], task3Stack[TASKSTACKSIZE];
bool tempPrinted = false;

int main()
{
    /* Construct BIOS objects */
    Task_Params taskParams;
    /* Call driver init functions */
    Board_init();
    /* Construct task threads */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)task1, &taskParams, NULL);
    taskParams.stack = &task2Stack;
    Task_construct(&task2Struct, (Task_FuncPtr)task2, &taskParams, NULL);
    taskParams.stack = &task3Stack;
    Task_construct(&task3Struct, (Task_FuncPtr)task3, &taskParams, NULL);
    BIOS_start();    /* Does not return */

    return(0);
}
Void task1(UArg arg0, UArg arg1)
{
    printf("Task1\n");
    UART_init();    // Driver init

    // Set up communication parameters and open the device
    UART_Params_init(&uartParams);
    uartParams.readEcho = UART_ECHO_OFF;
    uart = UART_open(CONFIG_UART_0, &uartParams);
    if (uart == NULL) {
        printf("Failed to open UART.\n");
        while (1);
    }
    uartReady = true;

    char buffer[10];
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    int pressed = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);
    int state = pressed;

    while(1){
        int pressed = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);

        if (state != pressed){
//            printf("Pressed\n");
            sprintf(buffer, "S1%d ", pressed);
            state = pressed;
            UART_write(uart, buffer, strlen(buffer));
//            send_message("S10 ");
        }

        else{
//            printf("Not Pressed\n");
        }

        Task_sleep(1000);
    }
}
Void task2(UArg arg0, UArg arg1)
{
    printf("Task2\n");
    while(!uartReady){
        Task_sleep(50);
    }

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    int pressed = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4);
    int state = pressed;

    char buffer[10];
    while(1){
        int pressed = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4);

        if (state != pressed){
//            printf("Pressed\n");
            sprintf(buffer, "S2%d ", pressed);
            state = pressed;
            UART_write(uart, buffer, strlen(buffer));
            tempPrinted = false;
//            send_message("S10 ");
        }

        else{
//            printf("Not Pressed\n");
        }

        Task_sleep(1000);
    }
}

Void task3(UArg arg0, UArg arg1){
    printf("Task3\n");
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
    // Trigger conversion with software

    while(!uartReady){
        Task_sleep(50);
    }

    char buffer[10];
    float values[10];
    int index = 0;
    int size = 0;
    float prevAvg = 0;
    while(1)
    {
        ADC14_toggleConversionTrigger();
        while (ADC14_isBusy())
        {
            Task_sleep(10);
        }
        uint32_t adc_value = ADC14_getResult(ADC_MEM0);
    //        printf("%u %fmV\n", adc_value, digitizedToAnalog(adc_value, 14, 1.2)*100);
        float temp = calcTempF(adc_value);
        if(index<10){
            values[index] = temp;
        }
        else if(index==10){
            index = 0;
            values[index]=temp;
        }
        if (size<10)
        {
            size++;
        }
        else
        {
            size=0;
            if(prevAvg!=printAvg(values))
            {
                prevAvg = printAvg(values);
                sprintf(buffer, "T%0.1f ", printAvg(values));
                UART_write(uart, buffer, strlen(buffer));
                tempPrinted = true;
            }
        }
        index++;
        Task_sleep(500);
    }
}

float printAvg(float *values)
{
    int i;
    float sum = 0;
    for(i=0; i<10; i++)
        sum+=values[i];
    return sum/10;
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

