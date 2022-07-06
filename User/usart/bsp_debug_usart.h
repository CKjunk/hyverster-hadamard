#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f4xx.h"
#include <stdio.h>
#define USE_USART_DMA_RX 0

//引脚定义
/*******************************************************/
#define DEBUG_USART_1                             USART1
#define DEBUG_USART_1_CLK                         RCC_APB2Periph_USART1
#define DEBUG_USART_1_BAUDRATE                    115200  //串口波特率

#define DEBUG_USART_1_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_1_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_1_RX_PIN                      GPIO_Pin_10
#define DEBUG_USART_1_RX_AF                       GPIO_AF_USART1
#define DEBUG_USART_1_RX_SOURCE                   GPIO_PinSource10

#define DEBUG_USART_1_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_1_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_1_TX_PIN                      GPIO_Pin_9
#define DEBUG_USART_1_TX_AF                       GPIO_AF_USART1
#define DEBUG_USART_1_TX_SOURCE                   GPIO_PinSource9

#define DEBUG_USART_1_IRQHandler                  USART1_IRQHandler
#define DEBUG_USART_1_IRQ                 				USART1_IRQn
/************************************************************/
//引脚定义
/*******************************************************/
#define DEBUG_USART_2                             USART2
#define DEBUG_USART_2_CLK                         RCC_APB1Periph_USART2
#define DEBUG_USART_2_BAUDRATE                    115200  //串口波特率

#define DEBUG_USART_2_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_2_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_2_RX_PIN                      GPIO_Pin_3
#define DEBUG_USART_2_RX_AF                       GPIO_AF_USART2
#define DEBUG_USART_2_RX_SOURCE                   GPIO_PinSource3

#define DEBUG_USART_2_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_2_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_2_TX_PIN                      GPIO_Pin_2
#define DEBUG_USART_2_TX_AF                       GPIO_AF_USART2
#define DEBUG_USART_2_TX_SOURCE                   GPIO_PinSource2

#define DEBUG_USART_2_IRQHandler                  USART2_IRQHandler
#define DEBUG_USART_2_IRQ                 				USART2_IRQn
/************************************************************/
void Debug_USART_Config();
void DEBUG_USART_1_Config(void);
void DEBUG_USART_2_Config(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);

void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);

#endif /* __USART1_H */
