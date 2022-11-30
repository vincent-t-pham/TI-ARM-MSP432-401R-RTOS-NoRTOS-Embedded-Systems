/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*******************************************************************************
 * MSP432 GPIO - Input Interrupt
 *
 * Description: This example demonstrates a very simple use case of the
 * DriverLib GPIO APIs. P1.1 (which has a switch connected to it) is configured
 * as an input with interrupts enabled and P1.0 (which has an LED connected)
 * is configured as an output. When the switch is pressed, the LED output
 * is toggled.
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P1.0  |---> P1.0 LED
 *            |                  |
 *            |            P1.1  |<--Toggle Switch
 *            |                  |
 *            |                  |
 *
 ******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#define QUEUE_SIZE 10
struct record {
    uint32_t timestamp;
    uint8_t state;
};
struct record queue[QUEUE_SIZE];
volatile int read_index;
volatile int write_index;

volatile uint32_t button_press = 1;

int main(void)
{
    float diff;

    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();

    /* Configuring P1.0 as output and P1.1 (switch) as input */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Configuring P1.1 as an input and enabling interrupts */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    MAP_Interrupt_enableInterrupt(INT_PORT1);

    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
    /* Enabling SRAM Bank Retention */
    MAP_SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK1);
    
    /* Enabling MASTER interrupts */
    MAP_Interrupt_enableMaster();   

    read_index=0;
    write_index=0;

    struct record before, after;

    /* Going to LPM3 */
    while (1)
    {
//        MAP_PCM_gotoLPM3();
//        PORT1_IRQHandler();
        if (write_index != read_index){
            before = after;
            after = queue[read_index];

            if (read_index < QUEUE_SIZE){
                read_index++;
            }
            else
                read_index=0;
        }

        if (after.state == 0 && before.state == 1){
            diff = (float)(before.timestamp - after.timestamp);
            uint32_t duration = (diff / MAP_CS_getMCLK()) * 1000;
            printf("Duration: %u ms\n", duration);
            before = after;
        }
    }
}

/* GPIO ISR */
void PORT1_IRQHandler(void)
{
    uint32_t status;
    struct record rec;
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

//    printf("Status: %x\n", status);
//    printf("Pin: %x\n\n", GPIO_PIN1);
    /* Toggling the output on the LED */
    if(button_press == 0)
    {
        button_press = 1;
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_LOW_TO_HIGH_TRANSITION);

        // Set timestamp and state
        rec.timestamp = MAP_Timer32_getValue(TIMER32_0_BASE);
        rec.state = button_press;

        // Add to queue and increment
        queue[write_index] = rec;
        if (write_index < QUEUE_SIZE)
            write_index++;
        else write_index=0;
    }
    else if(button_press == 1)
    {
        button_press = 0;
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);

        // Set timestamp and state
        rec.timestamp = MAP_Timer32_getValue(TIMER32_0_BASE);
        rec.state = button_press;

        // Add to queue and increment
        queue[write_index] = rec;
        if (write_index < QUEUE_SIZE){
            write_index++;
        }
        else write_index=0;
    }


}
