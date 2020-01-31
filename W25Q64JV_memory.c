// cstd
#include <stdbool.h>
#include <stdint.h>

//user
#include "W25Q64JV_memory.h"
#include "SPI.h"

// Local constants
// Commands
static const uint8_t write_enable_command             = 0x06;     // before every program/erase command
static const uint8_t read_data_command                = 0x03;     // command -> 24-bit address
static const uint8_t page_program_command             = 0x02;     // command -> 24-bit address (256 bytes max)
static const uint8_t chip_erase_command               = 0x60;
static const uint8_t read_status_register1_command    = 0x05;

// Before every program/erase command
void EnableWrite(void);

// Erase the whole chip
void EraseMemory(void)
{
    uint8_t transmit_buf = chip_erase_command;
    
    EnableWrite();
    
    TransmitSpi(&transmit_buf, 1);  // transmit command
    while (! TransmittedSpi())
        ;
    
    DeselectChipSpi();
}
// Write 256 byte max; within 1 page
void WriteMemory(uint8_t * buf, uint32_t addr, uint16_t num)
{
    uint8_t command_buf = page_program_command;
    uint8_t addr_to_transmit[3];
    addr_to_transmit[0] = addr >> 16;       // MSB first
    addr_to_transmit[1] = addr >> 8;
    addr_to_transmit[2] = addr;
    
    if (num > 256)
        num = 256;
    
    EnableWrite();
    
    TransmitSpi(&command_buf, 1);       // transmit command
    while (! TransmittedSpi())
        ;
    TransmitSpi(addr_to_transmit, 3);   // transmit address
    while (! TransmittedSpi())
        ;
    TransmitSpi(buf, num);              // transmit data
    while (! TransmittedSpi())
        ;
    
    DeselectChipSpi();
}
void ReadMemory(uint8_t * buf, uint32_t addr, uint32_t num)
{
    uint8_t command_buf = read_data_command;
    uint8_t addr_to_transmit[3];
    addr_to_transmit[0] = addr >> 16;       // MSB first
    addr_to_transmit[1] = addr >> 8;
    addr_to_transmit[2] = addr;
    
    TransmitSpi(&command_buf, 1);       // transmit command
    while (! TransmittedSpi())
        ;
    TransmitSpi(addr_to_transmit, 3);   // transmit address
    while (! TransmittedSpi())
        ;
    
    TransmitSpi(buf, num);          // transmit dummy byte
    ReceiveSpi(buf, num);           // read register
    while (! ReceivedSpi())
        ;
    
    DeselectChipSpi();
}
// If previous erase/write operation is over
bool IsBusy(void)
{
    uint8_t transmit_buf = read_status_register1_command;
    uint8_t receive_buf;
    
    TransmitSpi(&transmit_buf, 1);  // transmit command
    while (! TransmittedSpi())
        ;
    
    TransmitSpi(&transmit_buf, 1);  // transmit dummy byte
    ReceiveSpi(&receive_buf, 1);    // read register
    while (! ReceivedSpi())
        ;
    DeselectChipSpi();
    
    return ((receive_buf & 0x01) != 0);
}
void WaitForNotBusy(void)
{
    uint8_t transmit_buf = read_status_register1_command;
    uint8_t receive_buf;
    
    TransmitSpi(&transmit_buf, 1);  // transmit command
    while (! TransmittedSpi())
        ;
    
    do {
        TransmitSpi(&transmit_buf, 1);  // transmit dummy byte
        ReceiveSpi(&receive_buf, 1);    // read register
        while (! ReceivedSpi())
            ;
    } while ((receive_buf & 0x01) != 0);
    
    DeselectChipSpi();
}
// Before every program/erase command
void EnableWrite(void)
{
    uint8_t transmit_buf = write_enable_command;
    
    TransmitSpi(&transmit_buf, 1);  // transmit command
    while (! TransmittedSpi())
        ;
    DeselectChipSpi();
}
