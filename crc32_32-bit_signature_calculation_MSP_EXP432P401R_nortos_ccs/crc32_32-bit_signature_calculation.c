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
 * MSP432 CRC32 - CRC32 Calculation
 *
 * Description: In this example, the CRC32 module is used to calculate a CRC32
 * checksum in CRC32 mode. This value is compared versus a software calculated
 * checksum. The idea here is to have the user use the debugger to step through
 * the code and observe the CRC calculation in the debugger. Note that this
 * code example was made to use the standard CRC32 polynomial value of
 * 0xCBF43926.
 *
 *              MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 ******************************************************************************/
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define CRC32_POLY              0xEDB88320
#define CRC32_INIT              0xFFFFFFFF

static uint8_t myData[10240] =
{ 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39 };
static uint32_t calculateCRC32(uint8_t* data, uint32_t length);
uint32_t compute_simple_checksum(uint8_t* data, uint32_t length);

volatile uint32_t hwCalculatedCRC, swCalculatedCRC, pCalculatedCRC;

//![Simple CRC32 Example] 
int main(void)
{
    uint32_t i, ii;

    for (i=0; i<10240; i++){
        myData[i] = i+1;
    }


    // Sets up counter
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
    TIMER32_FREE_RUN_MODE);
    // Starts timer
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
//    printf("%u\n", MAP_CS_getMCLK());
//    uint32_t end = MAP_Timer32_getValue(TIMER32_0_BASE);
    /* Stop WDT */

//    MAP_WDT_A_holdTimer();


    uint32_t start  = 0;
    uint32_t end    = 0;
    uint32_t    duration;
    uint32_t    hwduration;
    uint32_t    swduration;
    uint32_t    speedup;

    double    diff=0;
    double    frequency = (double)MAP_CS_getMCLK();
//    uint32_t    duration;


    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);

    for (ii = 0; ii < 9; ii++)
        MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

    // SIMPLE CHECKSUM //
    start = MAP_Timer32_getValue(TIMER32_0_BASE);
    pCalculatedCRC = compute_simple_checksum(myData, 10240);
    /* Getting the result from the software module */
    end = MAP_Timer32_getValue(TIMER32_0_BASE);
    diff = (double)(start-end);
    duration = (diff/ MAP_CS_getMCLK() * 1000000);
    printf("Program\nChecksum: %x\nTime: %x\n\n", pCalculatedCRC, duration);


    // HARDWARE SIM //
    start = MAP_Timer32_getValue(TIMER32_0_BASE);
    /* Getting the result from the hardware module */
    hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
    end = MAP_Timer32_getValue(TIMER32_0_BASE);
    diff = (double)(start-end);
    hwduration = (diff/ MAP_CS_getMCLK() * 1000000);
    printf("Hardware\nChecksum: %x\nTime: %x\n\n", hwCalculatedCRC, hwduration);


    // SOFTWARE SIM //
    start = MAP_Timer32_getValue(TIMER32_0_BASE);
    /* Calculating the CRC32 checksum through software */
    swCalculatedCRC = calculateCRC32((uint8_t*) myData, 9);    end = MAP_Timer32_getValue(TIMER32_0_BASE);
    diff = (double)(start-end);
    swduration = (diff/ MAP_CS_getMCLK() * 1000000);
    printf("Software\nChecksum: %x\nTime: %x\n\n", swCalculatedCRC, swduration);

    speedup = swduration/hwduration;
    printf("Speedup: 1:%x\n", speedup);


    /* Pause for the debugger */
    __no_operation();
}

uint32_t compute_simple_checksum(uint8_t* data, uint32_t length){
     uint32_t i = 0;
     uint32_t sum = 0;

    for (i=0; i<length; i++){
        uint32_t byte = data[i];
        sum+=byte;
    }
    return ~(sum);
}

//![Simple CRC32 Example] 

/* Standard software calculation of CRC32 */
static uint32_t calculateCRC32(uint8_t* data, uint32_t length)
{
    uint32_t ii, jj, byte, crc, mask;;

    crc = 0xFFFFFFFF;

    for(ii=0;ii<length;ii++)
    {
        byte = data[ii];
        crc = crc ^ byte;

        for (jj = 0; jj < 8; jj++)
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (CRC32_POLY & mask);
        }

    }

    return ~crc;
}
