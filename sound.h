// cstd
#include <stdbool.h>
#include <stdint.h>

#ifndef __SOUND_H__
#define __SOUND_H__

// Sine table size
#define WAVE_TABLE_ARRAY_SIZE       1024

// Returns duty value for TIM1; calculates it from wave table
// Should be called by TIM1 update interrupt
uint16_t GetNewDutyTime(void);
// Turn sound on and off
void TurnSoundOn(void);
void TurnSoundOff(void);
// Start playing track from memory (address mem_addr)
void StartTrack(uint32_t mem_addr);
// Get new sample from memory
void UpdateSample(void);
// Read audio file from UART and write to memory through SPI at start_addr
// start_addr must be multiple of 256
// Buffers size = 256 bytes
// Audio data size placed in first 4 bytes: LSB first
// Returns data size (including 4 byte of size)
uint32_t AudioFromUartToMem(uint8_t * buf1, uint8_t * buf2, uint32_t start_addr);

#endif
