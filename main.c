// STM32F103
#include "stm32f10x.h"

// cstd
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>

//user
#include "main.h"
#include "init.h"
#include "GPIO.h"
#include "timer.h"
#include "UART.h"
#include "LCD_8bit.h"
#include "debug_led.h"
#include "DMA.h"
#include "LCD_char.h"
#include "button.h"
#include "sound.h"
#include "SPI.h"
#include "W25Q64JV_memory.h"

// Global buffer used in LCD_8bit.c and LCD_char
uint16_t dma_buf[DMA_BUF_SIZE];

char text_buf[50];


const rgb_t red         = {0xFF, 0x00, 0x00};
const rgb_t orange      = {0xFF, 0x7F, 0x00};
const rgb_t yellow      = {0xFF, 0xFF, 0x00};
const rgb_t green       = {0x00, 0xFF, 0x00};
const rgb_t sky_blue    = {0x00, 0xBF, 0xFF};
const rgb_t blue        = {0x00, 0x00, 0xFF};
const rgb_t violet      = {0x7F, 0x00, 0xFF};
const rgb_t white       = {0xFF, 0xFF, 0xFF};
const rgb_t black       = {0x00, 0x00, 0x00};
const rgb_t brown       = {0x96, 0x4B, 0x00};
const rgb_t grey        = {0x80, 0x80, 0x80};

rgb_t text_color, back_color, btn_color, btn_text_color;


void Wait(void);

bool up_down = true;
int number = 0;
int i;
void UpdateNumber(void);
void UpdateButtons(void);

char ByteToHexHi(uint8_t);
char ByteToHexLo(uint8_t);

int main()
{    
    // System init
    InitRcc();
    InitGpio();
    InitSysTick();
    InitUart();
    InitInterrupt();
    InitLcd();
    InitDma();
    InitTim2();
    InitTim1();
    InitSpi();
    
    // Debug
    //SetPin(USART_RX_PIN);
    //TurnLedOff();
    
    
    // Buttons
    btn_t BtnL0 = {BTN_L0, 0, 20, false, false, false};
    btn_t BtnL1 = {BTN_L1, 0, 20, false, false, false};
    //btn_t BtnR0 = {BTN_R0, 0, 20, false, false, false};
    //btn_t BtnR1 = {BTN_R1, 0, 20, false, false, false};
    btn_t BtnR2 = {BTN_R2, 0, 20, false, false, false};
    btn_t BtnR3 = {BTN_R3, 0, 20, false, false, false};
    
    // Timings
    uint32_t BtnCheckCounter;
    
    // System colors
    back_color      = white;
    text_color      = blue;
    btn_color       = blue;
    btn_text_color  = red;
    
    
    FillRectangle(0, 0, LCD_MAX_X, LCD_MAX_Y, back_color);
    
    SetColorLcd(text_color, back_color);
    SetCoordinatesLcd(150, 200);
    
    PrintStringLnLcd("Hello ^_^");
    

    // Audio file reading
    
    /*const int mem_start_addr = 0;
    uint8_t mem_read;
    uint8_t buf1[256];
    uint8_t buf2[256];
    
    PrintStringLnLcd("erasing...");
    
    EraseMemory();
    WaitForNotBusy();
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
    PrintStringLnLcd("start");

    number = AudioFromUartToMem(buf1, buf2, mem_start_addr);
    
    // Verification
    for (i = 0; i < 520; i++) {
        ReadMemory(&mem_read, i, 1);
        
        buf1[0] = ByteToHexHi(mem_read);
        buf1[1] = ByteToHexLo(mem_read);
        buf1[2] = ' ';
        
        TransmitUart(buf1, 3);
        while (! TransmittedUart())
            ;
        if ((i % 16) == 0)
            TransmitUart("\r\n", 2);
        while (! TransmittedUart())
            ;
    }*/
    
    // End of audio file reading
    
    
    
    UpdateNumber();
    
    FillRectangle(0, 20, 100, 55, btn_color);
    FillRectangle(0, 245, 100, 55, btn_color);
    
    UpdateButtons();
    
    // Start buttons check
    BtnCheckCounter = sys_tick;
    
  
    while (true) {
        
        // Buttons check
        if (sys_tick - BtnCheckCounter > 1) {
            BtnCheckCounter = sys_tick;
            
            CheckBtn(&BtnL0);
            CheckBtn(&BtnL1);
            //CheckBtn(&BtnR0);
            //CheckBtn(&BtnR1);
            CheckBtn(&BtnR2);
            CheckBtn(&BtnR3);
        }
        
        // +/-
        if (WasPressed(&BtnL1)) {
            up_down = ! up_down;
            UpdateButtons();
        }
        
        /*if (WasPressed(&BtnR0)) {
            if (up_down)
                number += 1;
            else
                number -= 1;
            UpdateNumber();
        }
        if (WasPressed(&BtnR1)) {
            if (up_down)
                number += 10;
            else
                number -= 10;
            UpdateNumber();
        }*/
        if (WasPressed(&BtnR2)) {
            if (up_down)
                number += 1;
            else
                number -= 1;
            UpdateNumber();
        }
        /*if (WasPressed(&BtnR2)) {
            i++;
            ReadMemory(&j, i, 1);
            number = j;
            number++;
            
            UpdateNumber();
        }*/
        // Sound
        if (WasPressed(&BtnR3))
            StartTrack(0);
        // reset
        if (WasPressed(&BtnL0)) {
            number = 0;
            UpdateNumber();
        }
        
        // Sound!
        UpdateSample();
    }

}

void Wait(void)
{
    while (! TransmittedDma())
        ;
}

void UpdateNumber()
{
    FillRectangle(200, 100, 140, 50, back_color);
    SetColorLcd(text_color, back_color);
    
    sprintf(text_buf, "%i", number);
    
    SetCoordinatesLcd(200, 100);
    PrintStringLcd(text_buf);
}
void UpdateButtons()
{
    SetColorLcd(btn_text_color, btn_color);
    
    FillRectangle(LCD_MAX_X-100, 20, 100, 55, btn_color);
    SetCoordinatesLcd(LCD_MAX_X-90, 30);
    PrintStringLcd("meow");
    
    FillRectangle(LCD_MAX_X-100, 95, 100, 55, btn_color);
    SetCoordinatesLcd(LCD_MAX_X-95, 105);
    if (up_down)
        PrintStringLcd("+1");
    else
        PrintStringLcd("-1");
    
    FillRectangle(LCD_MAX_X-100, 170, 100, 55, btn_color);
    FillRectangle(LCD_MAX_X-100, 245, 100, 55, btn_color);
    
    /*SetCoordinatesLcd(LCD_MAX_X-95, 180);
    if (up_down)
        PrintStringLcd("+10");
    else
        PrintStringLcd("-10");
    SetCoordinatesLcd(LCD_MAX_X-95, 255);
    if (up_down)
        PrintStringLcd("+1");
    else
        PrintStringLcd("-1");*/
    
    SetCoordinatesLcd(10, 30);
    PrintStringLcd("+/-");
    SetCoordinatesLcd(10, 255);
    PrintStringLcd("reset");
}

char ByteToHexHi(uint8_t b)
{
    b >>= 4;
    
    if (b < 10)
        return ('0' + b);
    
    switch (b)
    {
    case 10:
        return 'A';
    case 11:
        return 'B';
    case 12:
        return 'C';
    case 13:
        return 'D';
    case 14:
        return 'E';
    case 15:
        return 'F';
    }
      
    return '\n';
}
char ByteToHexLo(uint8_t b)
{
    b &= 0x0F;
    
    if (b < 10)
        return ('0' + b);
    
    switch (b)
    {
    case 10:
        return 'A';
    case 11:
        return 'B';
    case 12:
        return 'C';
    case 13:
        return 'D';
    case 14:
        return 'E';
    case 15:
        return 'F';
    }
     
    return '\n';
}
