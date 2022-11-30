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
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author:
*******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define PERIPHERAL_ADDR     0x40000000
#define P1_IN               0x40004C00
#define P2_OUT              0x40004c03
#define ALIAS_REGION_ADDR   0x42000000

void delay_ms(uint32_t count);

void main(void)
{
    float count=0, sum=0;

    // LED INITIALIZATION //
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    uint8_t *alias_addr_led = (uint8_t *)(((P2_OUT - PERIPHERAL_ADDR) * 32 + (0 * 4) + ALIAS_REGION_ADDR));

    printf("LED Alias Address: %x\n", alias_addr_led);

    // INITIALIZATION //
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
    TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    MAP_WDT_A_holdTimer();
    printf("MCLK: %u\n", MAP_CS_getMCLK());

    // 0.4Hz -> 2.5 seconds
    float delay_time = (1/0.4) * 1000;

    // PERFORM DELAY //

    while (1)
    {
        // LED Off and measure //
        uint32_t t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        delay_ms(delay_time);
        // LED Off //
        *alias_addr_led = 0;
        uint32_t t1 = MAP_Timer32_getValue(TIMER32_0_BASE);

        // Report //
        float clock_speed = (float)MAP_CS_getMCLK();
        float time = (float)((t0-t1) / clock_speed) * 1000;
        sum += time;
        count++;
        float calculated_frequency = (float)1/(time / 1000);
        float error = time - delay_time;
        printf("LED was Off for \n");
        printf("Time: %f\n", time);
        printf("Average: %f\n", sum/count);
        printf("Error: %f\n", error);
        printf("Calculated Frequency: %f\n\n", calculated_frequency);


        // LED On and measure //
        t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        delay_ms(delay_time);
        // LED On //
        *alias_addr_led = 1;
        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);

        // Report //
        time = (float)((t0-t1) / clock_speed) * 1000;
        sum += time;
        count++;
        error = time - delay_time;
        calculated_frequency = (float)1/(time / 1000);
        printf("LED was On for\n");
        printf("Time: %f\n", time);
        printf("Average: %f\n", sum/count);
        printf("Error: %f\n", error);
        printf("Calculated Frequency: %f\n\n", calculated_frequency);

    }

}

void delay_ms(uint32_t count)
{
    // STORE CLOCK VALUE //
    uint32_t t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    float clock_speed = (float)MAP_CS_getMCLK();

    // CALCULATE INITIAL TIME DIFFERENCE //
    float time = (float)((t0-t1) / clock_speed) * 1000;

    // LOOP UNTIL COUNT VALUE IS REACHED //
    while (time<=count)
    {
        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
        time = (float)((t0-t1) / clock_speed) * 1000;
    }

}
