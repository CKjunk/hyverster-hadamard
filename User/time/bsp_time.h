#ifndef BSP_TIME_H
#define	BSP_TIME_H
//注意一下，周期比实际值小一，分频系数比实际减一，宽度就是实际值
#include "stm32f4xx.h"
#include <stdio.h>
#define SENIOR_PSC  180-1  
#define COMMON_PSC  90-1 //// 通用控制定时器时钟源TIMxCLK = HCLK/2=90MHz 

#define OVERSAMPLING 8 
#define TIM_ADC1_PERIOD 200-1  //实际周期为PWM_PERIOD
#define TIM_ADC1_LOWTIME 100


#define SEQUENCE_PERIOD   200-1
  
#define SENIOR_TIM                   TIM1
#define SENIOR_TIM_CLK               RCC_APB2Periph_TIM1
#define SENIOR_TIM_INT_FUNCTION      TIM1_UP_TIM10_IRQHandler
#define SENIOR_TIM_INT_CHANNEL			 TIM1_UP_TIM10_IRQn


/*通用定时器*/
/* 通用定时器 */
#define TIM2_GPIO                   TIM2
#define TIM2_GPIO_CLK               RCC_APB1Periph_TIM2
#define TIM2_GPIO_INT_FUNCTION      TIM2_IRQHandler
#define TIM2_GPIO_INT_CHANNEL				TIM2_IRQn


#define TIM_ADC1                TIM8
#define TIM_ADC1_CLK            RCC_APB2Periph_TIM8
#define TIM_ADC1_PWM_PIN        GPIO_Pin_6
#define TIM_ADC1_GPIO_PORT      GPIOC
#define TIM_ADC1_GPIO_CLK       RCC_AHB1Periph_GPIOC
#define TIM_ADC1_PWM_PINSOURCE  GPIO_PinSource6
#define TIM_ADC1_PWM_AF				  GPIO_AF_TIM8

#define TIM_ADC1_IRQn         TIM8_UP_TIM13_IRQn
#define TIM_ADC1_IRQHandler   TIM8_UP_TIM13_IRQHandler
//void TIM1_GPIO_Config(u16 Arr,u16 Psc);
void TIM1_GPIO_Config(u16 arr,u16 psc,u16 adc_pwm_lowtime);
  
void TIM8_ADC_Config(u16 arr,u16 psc,u16 adc_pwm_lowtime);
#endif 