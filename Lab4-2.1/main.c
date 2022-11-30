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




void delay_ms(uint32_t count);

void main(void)
{
    int i;

    // INITIALIZATION //
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
    TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    MAP_WDT_A_holdTimer();
    printf("MCLK: %u\n", MAP_CS_getMCLK());

    // ARRAY CONTAINING DELAY VALUES //
    int delay_times[] = {5000, 2000, 1000, 50, 20, 10, 5, 2, 1, 0};

    // PERFORM DELAY //
    for(i=0; i<10; i++)
    {
        printf("Delay Time: %d\n", delay_times[i]);
        uint32_t t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        delay_ms(delay_times[i]);
        uint32_t t1 = MAP_Timer32_getValue(TIMER32_0_BASE);

        // OUTPUT CLOCK VALUE //
        printf("T0: %u\n", t0);
        printf("T1: %u\n", t1);

        // CALCULATE DELAY, ERROR //
        float clock_speed = (float)MAP_CS_getMCLK();
        float time = (float)((t0-t1) / clock_speed) * 1000;
        float error = time - delay_times[i];
        printf("Time: %f\n", time);
        printf("Error: %f\n\n", error);
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
