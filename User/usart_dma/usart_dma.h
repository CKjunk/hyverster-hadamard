#ifndef USART_DMA_H
#define	USART_DMA_H

#include "stm32f4xx.h"
#include <stdio.h>
//DMA
//DMA
#define DEBUG_USART                             USART1

#define DEBUG_USART_DR_BASE               (USART1_BASE+0x04)		
#define USART_BUFF_SIZE                   20		//接收的数据量
#define DEBUG_USART_DMA_CLK               RCC_AHB1Periph_DMA2	
#define DEBUG_USART_DMA_CHANNEL           DMA_Channel_4
#define DEBUG_USART_DMA_STREAM            DMA2_Stream2
#define DMA_USART_COMP_INT_CHANNEL				DMA2_Stream2_IRQn
#define DMA_USART_COMP_INT_FUNCTION				DMA2_Stream2_IRQHandler
void USART_DMA_Config(void);
 
#endif 