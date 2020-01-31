// cstd
#include <stdbool.h>
#include <stdint.h>

//user
#include "sound.h"
#include "main.h"
#include "UART.h"
#include "W25Q64JV_memory.h"
#include "debug_led.h"

// Local variables
static uint32_t sample_counter = 0;     // sample playing
static uint32_t sample_num;             // number of samples
static uint32_t cur_addr;               // memory address of current sample
static bool on_off = false;             // sound on/off

uint8_t cur_sample;                      // current sample, returning by GetNewDutyTime
bool sample_updated = false;            // need to update sample

// Returns duty value for TIM1; calculates it from wave table
// should be called by TIM1 update interrupt
uint16_t GetNewDutyTime(void)
{    
    if (on_off == false)
        return 128;
    
    sample_counter++;
    cur_addr += 1;                  // next sample address: sample size = 1 byte
    if (sample_counter >= sample_num)      // track end
        on_off = false;
    
    // Need new sample
    sample_updated = false;
    
    return cur_sample;
    
    
    /*int16_t res;
    static int counter = 0;

    res = sine_table1[counter];
    counter += 102;
    if (counter >= WAVE_TABLE_ARRAY_SIZE) {
        counter = 0;
    }
    
    res >>= 5;      // 12-bit value -> 7-bit value
    
    return res;*/
}
// Get new sample from memory
void UpdateSample(void)
{       
    uint8_t res;
    
    if (on_off == false)
        return;
    
    // Read new sample
    ReadMemory((uint8_t *)&res, cur_addr, 1);
    /*if (res >= 0)
        res -= 127;
    else
        res += 128;*/
    cur_sample = res;
    
    sample_updated = true;
}
// Turn sound on and off
void TurnSoundOn(void)
{
    on_off = true;
}
void TurnSoundOff(void)
{
    on_off = false;
}
// Start playing track from memory (address mem_addr)
void StartTrack(uint32_t mem_addr)
{
    uint32_t audio_size;
    
    // Get track size in bytes
    ReadMemory((uint8_t *)&audio_size, mem_addr, 4);
    sample_num = audio_size;    // sample size = 1 byte
    
    sample_counter = 0;         // start from 0 sample
    
    cur_addr = mem_addr + 4;    // first 4 bytes - track size
    
    TurnSoundOn();
}
// Read audio file from UART and write to memory through SPI at start_addr
// start_addr must be multiple of 256
// Buffers size = 256 bytes
// Audio data size placed in first 4 bytes: LSB first
// Returns data size (including 4 byte of size)
uint32_t AudioFromUartToMem(uint8_t * buf1, uint8_t * buf2, uint32_t start_addr)
{
    const int page_size = 256;              // memory page size, bytes
    
    uint32_t audio_size;                // total data size
    uint32_t page_left;                 // full page to be written
    uint8_t last_page;                  // size of the last (not full) page
    uint32_t cur_addr = start_addr;     // current memory writing address
    bool buf1_full;                     // or buf2
    uint8_t * receive_buf;
    uint8_t * transmit_buf;
    
    // Receive very first page
    ReceiveUart(buf1, page_size);
    while (!ReceivedUart())     // wait for UART reading
            ;
    // Get data size (first 4 bytes)
    audio_size = buf1[0] + (buf1[1] << 8) + (buf1[2] << 16) + (buf1[3] << 24);
    // Including 4 byte of size
    audio_size += 4;
    
    // Calculate full page quality needed (page size = 256 bytes)
    page_left = audio_size / page_size;
    // Consider the last (not full) page
    last_page = audio_size % page_size;
    
    // temp
    //return audio_size;

    audio_size = 0;     // written bytes quality
    buf1_full = true;   // last received into buf1

    // Full pages writing process (writing speed must exceed UART speed)
    while (page_left > 0) {
        
        if (buf1_full) {
            receive_buf = buf2;
            transmit_buf = buf1;
        }
        else {
            receive_buf = buf1;
            transmit_buf = buf2;
        }
        
        // Full or not full page receiving
        if (page_left > 1)
            ReceiveUart(receive_buf, page_size);
        else
            ReceiveUart(receive_buf, last_page);
        // Page transmitting
        WriteMemory(transmit_buf, cur_addr, page_size);
        cur_addr += page_size;
        audio_size += page_size;

        WaitForNotBusy();           // wait for memory writing (the last page reading is too fast)
        while (!ReceivedUart())     // wait for UART reading
            ;
        
        // Buffers swap
        buf1_full = ! buf1_full;
        // Left pages counter
        page_left--;
        
        BlinkLed();
    }
    
    // The last page transmit buffer
    if (buf1_full)
            transmit_buf = buf1;
        else
            transmit_buf = buf2;
    
    // The last page writing process
    if (last_page != 0) {
        WriteMemory(transmit_buf, cur_addr, page_size);
        audio_size += last_page;
        WaitForNotBusy();
    }
    
    return audio_size;
}
