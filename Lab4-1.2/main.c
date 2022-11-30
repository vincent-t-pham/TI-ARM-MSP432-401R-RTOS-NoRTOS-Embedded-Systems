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

#define TRUE   1
#define FALSE  0

//bank 0 sec 0 0x0000_0000 0x0000_0FFF
//bank 0 sec 1 0x0000_1000 0x0000_1FFF
//bank 0 sec 2 0x0000_2000 0x0000_2FFF

char* string1  = "xyz123";
char string2[] = "xyz123";

//memorySpace is the flash memory bank and sectorMask indicates which sector(s) to unprotect.
bool get_flash_bank_sector(uint32_t mem_address, uint32_t* bank_number, uint32_t* sector_mask);
void main(void)
{

    uint32_t sector_mask;
    uint32_t bank_number;

//    FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK0, FLASH_SECTOR2);
    MAP_WDT_A_holdTimer();
    printf("\nstring1: %s\n", string1);
    printf("string2: %s\n", string2);

    string1[0] = '0';
    string2[0] = '0';
    string2[1] = '0';

    printf("address string1: %x\n", string1);

    get_flash_bank_sector(string1, &bank_number, &sector_mask);

    FlashCtl_unprotectSector(bank_number, sector_mask);
    ROM_FlashCtl_programMemory(string2, string1, 2);

    printf("After unprotect and programMemory:\n");
    printf("string1: %s\n", string1);
    printf("string2: %s\n", string2);
}


bool get_flash_bank_sector(uint32_t mem_address, uint32_t* bank_number, uint32_t* sector_mask) {
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
        return TRUE;
    else
        return FALSE;

}
