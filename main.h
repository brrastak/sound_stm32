// STM32F103
#include "stm32f10x.h"

// cstd
#include <stdbool.h>

#ifndef __MAIN_H__
#define __MAIN_H__

// Buffer for DMA images
// must be multiple of 6
#define DMA_BUF_SIZE    34*32*6
extern uint16_t dma_buf[DMA_BUF_SIZE];
extern const int16_t sine_table1[1024];


// Ports and pin numbers
// LCD
#define PARALLEL_PORT           GPIOA
#define WR_BIT                  10
#define WR_PIN                  GPIOA, WR_BIT
#define RD_PIN                  GPIOB, 0
#define RS_PIN                  GPIOB, 1
#define RST_PIN                 GPIOB, 10
// Buttons
// Left side
#define BTN_L0                  GPIOB, 11
#define BTN_L1                  GPIOB, 12
// Right side
//#define BTN_R0                  GPIOA, 11
//#define BTN_R1                  GPIOA, 12
#define BTN_R2                  GPIOB, 8
#define BTN_R3                  GPIOB, 9
// Onboard LED
#define LED_PIN                 GPIOC, 13
// USART
#define USART_TX_PIN            GPIOB, 6
//#define USART_TX_PIN            GPIOA, 9
#define USART_RX_PIN            GPIOB, 7
// SPI
#define SPI_NSS_PIN             GPIOA, 15
#define SPI_SCK_PIN             GPIOB, 3
#define SPI_MISO_PIN            GPIOB, 4
#define SPI_MOSI_PIN            GPIOB, 5
// TIM1 outputs for full-bridge PWM
#define PWM_1P                  GPIOA, 8
#define PWM_1N                  GPIOB, 13
#define PWM_2P                  GPIOA, 9
#define PWM_2N                  GPIOB, 14

#endif
