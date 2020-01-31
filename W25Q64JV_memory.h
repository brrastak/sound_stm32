/*
    Library for W25Q64JV memory IC
    SPI up to 133 MHz; 0 or 3 mode; MSB first; 8-bit format
    8 MByte; 256-byte pages
*/

// cstd
#include <stdbool.h>
#include <stdint.h>

#ifndef __W25Q64JV_MEMORY_H__
#define __W25Q64JV_MEMORY_H__

// Erase the whole chip
void EraseMemory(void);
// Write 256 byte max; within 1 page
void WriteMemory(uint8_t * buf, uint32_t addr, uint16_t num);
void ReadMemory(uint8_t * buf, uint32_t addr, uint32_t num);
// If previous erase/write operation is over
bool IsBusy(void);
void WaitForNotBusy(void);


#endif
