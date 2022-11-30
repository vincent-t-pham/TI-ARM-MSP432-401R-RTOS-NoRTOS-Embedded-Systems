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

#define DELAY_TIME_MS 2

float t0, frequency;


void main(void)
{
    int timer_reached = 1;
    // TIMER INITIALIZATION //
    Timer_A_ContinuousModeConfig timer_continuous_obj;
    timer_continuous_obj.clockSource = TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK;
    timer_continuous_obj.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer_continuous_obj.timerInterruptEnable_TAIE =
    TIMER_A_TAIE_INTERRUPT_DISABLE;
    timer_continuous_obj.timerClear = TIMER_A_DO_CLEAR;
    MAP_Timer_A_configureContinuousMode(TIMER_A2_BASE, &timer_continuous_obj);
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_CONTINUOUS_MODE);

    // TIMER INITIALIZATION //
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
    TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    MAP_WDT_A_holdTimer();

    printf("MCLK: %u\n", MAP_CS_getMCLK());

    CAPTIO0CTL = 0;         // Clear control register
    CAPTIO0CTL |= 0x0100;   // Enable CAPTIO
    CAPTIO0CTL |= 4 << 4;   // Select Port 4. Place port number in Bits 7-4
    CAPTIO0CTL |= 1 << 1;   // Select Pin 1. Place pin number in Bits 3-1
    CAPTIO0CTL |= 1 << 8;   // Enable touch

    // Calculation for Timer measurement = 10s //
    float clock_speed = (float)MAP_CS_getMCLK();
    uint32_t start = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t target = start - (10000 * clock_speed)/1000;
//    _delay_cycles(10000 * 3000);
    uint32_t t1 = MAP_Timer32_getValue(TIMER32_0_BASE);

    while(timer_reached){
        MAP_Timer_A_clearTimer(TIMER_A2_BASE);
        _delay_cycles(DELAY_TIME_MS * 3000);
        t0 = MAP_Timer_A_getCounterValue(TIMER_A2_BASE);
        frequency = (t0 * 1000)/DELAY_TIME_MS;

        uint32_t t1 = MAP_Timer32_getValue(TIMER32_0_BASE);

        if(t1>target){
            printf("%f\t", ((float)(start-t1) / MAP_CS_getMCLK()) * 1000);
            printf("%0.2f\n", frequency);
        }
        else{
            timer_reached = 0;
        }

    }
}

