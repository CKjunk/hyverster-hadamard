#include "stm32f4xx_it.h"
#include "./usart/bsp_debug_usart.h"
#include "./time/bsp_time.h"
#include "./adc/bsp_adc.h"
#include "./key/key.h"
#include "./adc_dma/adc_dma.h"
#include "./usart_dma/usart_dma.h"
#include "./HadamardOutput/bsp_dma_gpio.h"
#include "./sequence/sequence.h"

extern TEN  ten;
extern TEN* pten;
extern u16  count;
extern u8   display_flag;
u8 MODIFY_TIM8_PULSE = 1;
void TOTAL_PROJECT_INT_FUNCTION(void){
	if(EXTI_GetITStatus(KEY_INT_EXTI_LINE)!= RESET){
 		  TIM_Cmd(SENIOR_TIM, ENABLE);
	//	TIM_Cmd(TIM8, ENABLE);


   EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位

	}
}
void DMA_ADC_COMP_INT_FUNCTION(void){
    if(DMA_GetITStatus(RHEOSTAT_ADC_DMA_STREAM,DMA_IT_TCIF0))
	  {
		    
      // SENIOR_TIM->CR1 &= (uint16_t)~TIM_CR1_CEN;
			 display_flag = 1;		
		   //TIM_Cmd(ADC1_External_TIM,DISABLE);
			 
			 DMA_ClearITPendingBit(RHEOSTAT_ADC_DMA_STREAM,DMA_FLAG_TCIF0);		   			 
	  }
}


//void HADAMARD_GPIO_COMP_INT_FUNCTION(void){
//   if(DMA_GetITStatus(HADAMARD_GPIO_DMA_STREAM,DMA_IT_TCIF5)){
// 		 printf("GPIO complete\r\n");
//		 TIM_DMACmd(SENIOR_TIM, TIM_DMA_Update,DISABLE); 

//		 DMA_ClearITPendingBit(HADAMARD_GPIO_DMA_STREAM,DMA_FLAG_TCIF5);		 
//	 }
//}


void TIM_ADC1_IRQHandler(void){
	 if(TIM_GetFlagStatus(TIM_ADC1,TIM_IT_Update)){
//  		  if(MODIFY_TIM8_PULSE == 1){
//				TIM_ADC1->CCR1 =  PULSE_WIDTH;
//				MODIFY_TIM8_PULSE = 0;
//      }
	   TIM_ClearITPendingBit(TIM_ADC1,TIM_IT_Update);
 	 }
}
 
	
void SENIOR_TIM_INT_FUNCTION(void){
	if(TIM_GetITStatus(SENIOR_TIM, TIM_IT_Update)){
  	if(pten->address[count]==1){
			SENIOR_TIM->CCR1  =  0;
		 
		}
		else if(pten->address[count]==0){
			SENIOR_TIM->CCR1  =  PERIOD+1;	     	
		}
		if(count == 1023){
			  SENIOR_TIM->CCR1  =  PERIOD+1;
			 	//TIM_ITConfig(TIM2_GPIO,TIM_IT_Update,DISABLE); //使能指定的TIM2中断,允许更新中断
     }
		else if(count == 1024){
		   TIM_ADC1->CR1 &= (uint16_t)~TIM_CR1_CEN;

	     SENIOR_TIM->CR1 &= (uint16_t)~TIM_CR1_CEN;
       count=-1;
		}	 
		count++; 
		TIM_ClearITPendingBit(SENIOR_TIM, TIM_IT_Update); //清除 TIM2 更新中断标志
	 // printf("time2");

   }
}
