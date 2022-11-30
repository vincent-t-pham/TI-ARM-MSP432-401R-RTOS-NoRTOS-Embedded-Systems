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

#define TRUE  1;
#define FALSE 0;

const uint32_t counter[1] = {0xFFFFFFFF};
uint32_t countdown = 0x00000000;

void delay_ms(uint32_t count);
bool get_flash_bank_sector(uint32_t mem_address, uint32_t* bank_number, uint32_t* sector_mask);

void main(void)
{
    uint32_t counter_mask;
    uint32_t counter_bank;

    get_flash_bank_sector((uint32_t)counter, &counter_bank, &counter_mask);
//
    printf("Counter: %x\n", counter);
    printf("Counter: %x\n", counter[0]);
    FlashCtl_unprotectSector(counter_bank, counter_mask);
    if (counter[0] == 0xFFFFFFFF)
    {
        FlashCtl_programMemory(&countdown, &counter, 1);

    }
    else if (counter[0] == 0xFFFFFF00)
    {
        FlashCtl_programMemory(&countdown, &counter, 2);
    }
    else if (counter[0] == 0xFFFF0000)
    {
        FlashCtl_programMemory(&countdown, &counter, 3);
    }
    // LED INITIALIZATION //
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    uint8_t *alias_addr_led = (uint8_t *)(((P2_OUT - PERIPHERAL_ADDR) * 32 + (1 * 4) + ALIAS_REGION_ADDR));

    printf("LED Alias Address: %x\n", alias_addr_led);

    // INITIALIZATION //
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
    TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    MAP_WDT_A_holdTimer();
    printf("MCLK: %u\n", MAP_CS_getMCLK());

    // CONTROL COUNTER //


    // PERFORM DELAY //

    while (1)
    {
        // LED Off and measure //
        printf("%x\n", counter[0]);
        if(counter[0] >= 0xFFFF0000)
        {
            *alias_addr_led = 0;
            delay_ms(200);

            *alias_addr_led = 1;
            delay_ms(200);
        }
        else if(counter[0] < 0xFFFF0000)
        {
            *alias_addr_led = 0;
            alias_addr_led = (uint8_t *)(((P2_OUT - PERIPHERAL_ADDR) * 32 + (0 * 4) + ALIAS_REGION_ADDR));
            *alias_addr_led = 1;
        }

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

bool get_flash_bank_sector(uint32_t mem_address, uint32_t* bank_number, uint32_t* sector_mask)
{
    // Find Memory Bank Number
    uint8_t sector;

    if      ( mem_address >= 0 && mem_address < 0x000020000)
    {
        *bank_number = FLASH_MAIN_MEMORY_SPACE_BANK0;
    }
    else if ( mem_address >= 0x20000 && mem_address <= 0x40000)
    {
        *bank_number = FLASH_MAIN_MEMORY_SPACE_BANK1;
    }

    if (*bank_number == FLASH_MAIN_MEMORY_SPACE_BANK0)
    {
        sector = mem_address / 0x1000;
    }
    else if (*bank_number == FLASH_MAIN_MEMORY_SPACE_BANK1)
    {
        sector = (mem_address - 0x20000) / 0x1000;
    }
//    printf("%x", sector);

    switch(sector){
    case 0: *sector_mask = FLASH_SECTOR0; break;
    case 1: *sector_mask = FLASH_SECTOR1; break;
    case 2: *sector_mask = FLASH_SECTOR2; break;
    case 3: *sector_mask = FLASH_SECTOR3; break;
    case 4: *sector_mask = FLASH_SECTOR4; break;
    case 5: *sector_mask = FLASH_SECTOR5; break;
    case 6: *sector_mask = FLASH_SECTOR6; break;
    case 7: *sector_mask = FLASH_SECTOR7; break;
    case 8: *sector_mask = FLASH_SECTOR8; break;
    case 9: *sector_mask = FLASH_SECTOR9; break;
    case 10: *sector_mask = FLASH_SECTOR10; break;
    case 11: *sector_mask = FLASH_SECTOR11; break;
    case 12: *sector_mask = FLASH_SECTOR12; break;
    case 13: *sector_mask = FLASH_SECTOR13; break;
    case 14: *sector_mask = FLASH_SECTOR14; break;
    case 15: *sector_mask = FLASH_SECTOR15; break;
    case 16: *sector_mask = FLASH_SECTOR16; break;
    case 17: *sector_mask = FLASH_SECTOR17; break;
    case 18: *sector_mask = FLASH_SECTOR18; break;
    case 19: *sector_mask = FLASH_SECTOR19; break;
    case 20: *sector_mask = FLASH_SECTOR20; break;
    case 21: *sector_mask = FLASH_SECTOR21; break;
    case 22: *sector_mask = FLASH_SECTOR22; break;
    case 23: *sector_mask = FLASH_SECTOR23; break;
    case 24: *sector_mask = FLASH_SECTOR24; break;
    case 25: *sector_mask = FLASH_SECTOR25; break;
    case 26: *sector_mask = FLASH_SECTOR26; break;
    case 27: *sector_mask = FLASH_SECTOR27; break;
    case 28: *sector_mask = FLASH_SECTOR28; break;
    case 29: *sector_mask = FLASH_SECTOR29; break;
    case 30: *sector_mask = FLASH_SECTOR30; break;
    case 31: *sector_mask = FLASH_SECTOR31; break;
    }

    if ((sector>=0 && sector <=31) && (*bank_number == FLASH_MAIN_MEMORY_SPACE_BANK0 || *bank_number == FLASH_MAIN_MEMORY_SPACE_BANK1))
    {
        return TRUE;
    }
    else{
        return FALSE;
    }

}
