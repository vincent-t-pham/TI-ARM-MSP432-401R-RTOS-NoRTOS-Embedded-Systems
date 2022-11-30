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
#define TASKSTACKSIZE   2048
Void task1(UArg arg0, UArg arg1);
Void task2(UArg arg0, UArg arg1);
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
}
Void task2(UArg arg0, UArg arg1)
{
    printf("Task2\n");
}
