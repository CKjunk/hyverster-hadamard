#ifndef BSP_DMA_GPIO_H
#define	BSP_DMA_GPIO_H

#include "stm32f4xx.h"
 // ADC GPIO 宏定义
  
#define HADAMARD_GPIO_PORT                     GPIOA
#define HADAMARD_GPIO_PIN                      GPIO_Pin_8
#define HADAMARD_PWM_PINSOURCE                 GPIO_PinSource8
#define HADAMARD_GPIO_PIN_PWM_AF				       GPIO_AF_TIM1

#define HADAMARD_GPIO_PORT_ADDING              GPIOC
#define HADAMARD_GPIO_PIN_ADDING               GPIO_Pin_9
#define HADAMARD_PWM_PINSOURCE_ADDING          GPIO_PinSource9
 #define HADAMARD_GPIO_CLK_ADDING               RCC_AHB1Periph_GPIOC

#define HADAMARD_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define HADAMARD_TIME_GPIO_TIM        TIM1
#define HADAMARD_GPIO_DMA_CHANNEL     DMA_Channel_6
#define HADAMARD_GPIO_DMA_STREAM      DMA2_Stream5//对应的是TIM1_UP
#define HADAMARD_GPIO_INT_CHANNEL     DMA2_Stream5_IRQn
#define HADAMARD_GPIO_COMP_INT_FUNCTION    DMA2_Stream5_IRQHandler
//#define HADAMARD_TIME_GPIO_TIM        TIM2
//#define HADAMARD_GPIO_DMA_CHANNEL     DMA_Channel_3
//#define HADAMARD_GPIO_DMA_STREAM      DMA1_Stream1//对应的是TIM1_UP
//#define HADAMARD_GPIO_INT_CHANNEL     DMA1_Stream1_IRQn
//#define HADAMARD_GPIO_COMP_INT_FUNCTION    DMA1_Stream1_IRQHandler
void HadamardDmaGPIO(void);

#endif 