#ifndef ADC_DMA_H
#define	ADC_DMA_H

#include "stm32f4xx.h"
 // ADC DR寄存器宏定义，ADC转换后的数字值则存放在这里
#define RHEOSTAT_ADC_DR_ADDR    ((u32)ADC1+0x4c)

// ADC DMA 通道宏定义，这里我们使用DMA传输
#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_0
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0
#define DMA_ADC_COMP_INT_CHANNEL     DMA2_Stream0_IRQn
#define DMA_ADC_COMP_INT_FUNCTION    DMA2_Stream0_IRQHandler
void  ADC_DMA_Config(void);
#endif /* __BSP_ADC_H */

 

 