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
void send_message(char* msg);

Task_Struct task1Struct, task2Struct;
Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE];

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
    BIOS_start();    /* Does not return */

    return(0);
}
Void task1(UArg arg0, UArg arg1)
{
    printf("Task1\n");
    UART_Handle uart;
    UART_Params uartParams;

    UART_init();    // Driver init

    // Set up communication parameters and open the device
    UART_Params_init(&uartParams);
    uartParams.readEcho = UART_ECHO_OFF;
    uart = UART_open(CONFIG_UART_0, &uartParams);
    if (uart == NULL) {
        printf("Failed to open UART.\n");
        while (1);
    }

    char buffer[10];
//    while (1) {
//        sprintf(buffer, "%d ", count++);
//        UART_write(uart, buffer, strlen(buffer));
//        Task_sleep(1000);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    int pressed = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);
    int state = pressed;

    while(1){
        int pressed = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);

        if (state != pressed){
//            printf("Pressed\n");
            sprintf(buffer, "S1%d", pressed);
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
}

