#ifndef USART_DMA_H
#define	USART_DMA_H

#include "stm32f4xx.h"
#include <stdio.h>
//DMA
//DMA
#define DEBUG_USART                             USART1

#define DEBUG_USART_DR_BASE               (USART1_BASE+0x04)		
#define USART_SENDBUFF_SIZE                     100*6	+ 3			//发送的数据量
#define DEBUG_USART_DMA_CLK               RCC_AHB1Periph_DMA2	
#define DEBUG_USART_DMA_CHANNEL           DMA_Channel_4
#define DEBUG_USART_DMA_STREAM            DMA2_Stream7
#define DMA_USART_COMP_INT_CHANNEL				DMA2_Stream7_IRQn
#define DMA_USART_COMP_INT_FUNCTION				DMA2_Stream7_IRQHandler
void USART_DMA_Config(void);
#endif 