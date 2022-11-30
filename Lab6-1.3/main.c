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

#define BAUD_RATE 1200

void sendLow(float delay);
void sendHigh(float delay);
void delay_us(uint32_t count);
void send_message(char* msg);

void main(void)
    {

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN3);

    // INITIALIZATION //
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
    TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    MAP_WDT_A_holdTimer();
    printf("MCLK: %u\n", MAP_CS_getMCLK());

    send_message("The quick brown fox jumps over the lazy dog.\r\n");
}

void delay_us(uint32_t count)
{
    uint32_t start = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t elapsed = 0;
    uint32_t target = ((float)count/1000000) * MAP_CS_getMCLK();

    while((start-MAP_Timer32_getValue(TIMER32_0_BASE))<target)
    {
        //asdasd
    }
}

// Decimal to binary
int * getCharacterBinaryArray(int in, int *parity)
{
    static int arr[8] = {0};
    int i = 0;
    *parity = 0;

    if(in==32 || in == 92){
        arr[6]=0;
    }
    for (i = 0; in>0; i++)
    {
        arr[i] = in % 2;
        if (arr[i] == 1) {*parity=*parity +1;}
        in = in /2;
    }
    //    Debug printing
    for(i=7;i>=0;i--)
    {
        printf("%d",arr[i]);
    }
    printf("\n");

    return arr;

}

void send_message(char* msg)
{
//  00110100
    int in, i, j, parity=0;
    int *arr;
    int *parityPtr=&parity;

    float delay = 1000000/BAUD_RATE;
    printf("%f\n", delay);

    for(i=0; i<strlen(msg); i++)
    {
        in = msg[i];
        arr = getCharacterBinaryArray(in, parityPtr);
        if(*parityPtr%2==1){continue;}

//      Serialize Data             //
        sendLow(delay);                             // Header
        for(j=0; j<8; j++)                          // Data
        {
            if (*(arr+j))   {sendHigh(delay);}
            else            {sendLow(delay);}
//            if (*(arr+j))
//            {
//                GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN3);
//                delay_us(delay);
//            }
//            else{
//                GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN3);
//                delay_us(delay);
//            }

        }
        sendHigh(delay);                            // Parity
        sendHigh(delay);                            // Trailer
    }
//    printf("%d\n", *parityPtr);
}

void sendHigh(float delay)
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN3);
    delay_us(delay);
}

void sendLow(float delay)
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN3);
    delay_us(delay);
}
