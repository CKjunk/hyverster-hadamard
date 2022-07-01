#include "stm32f4xx_it.h"
#include "./usart/bsp_debug_usart.h"
#include "./time/bsp_time.h"
#include "./adc/bsp_adc.h"
#include "./key/key.h"
#include "./adc_dma/adc_dma.h"
#include "./usart_dma/usart_dma.h"
#include "./HadamardOutput/bsp_dma_gpio.h"
#include "./sequence/sequence.h"
#include "./systick/bsp_SysTick.h"

extern void TimingDelay_Decrement(void);
extern TEN  ten;
extern TEN* pten;
extern u16  count;
extern u8   display_flag;
extern u16  frame_count;

u8 MODIFY_TIM8_PULSE = 1;
void TOTAL_PROJECT_INT_FUNCTION(void){
	if(EXTI_GetITStatus(KEY_INT_EXTI_LINE)!= RESET){
 		  TIM_Cmd(SENIOR_TIM, ENABLE);//��������ʱ���е���ʱ��
      EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ

	}
}
void DMA_ADC_COMP_INT_FUNCTION(void){
    if(DMA_GetITStatus(RHEOSTAT_ADC_DMA_STREAM,DMA_IT_TCIF0))
	  {
 			DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, DISABLE);          //��ʹ��DMAͨ��

			display_flag = 1;		//dma������ɺ󽫷��ͱ�־λ��1
      DMA_ClearITPendingBit(RHEOSTAT_ADC_DMA_STREAM,DMA_FLAG_TCIF0);		   			 
	  }
}

void TIM_ADC1_IRQHandler(void){
	if(TIM_GetITStatus(SENIOR_TIM, TIM_IT_Update)){
	
	
	
	}


}
void SENIOR_TIM_INT_FUNCTION(void){
	if(TIM_GetITStatus(SENIOR_TIM, TIM_IT_Update)){
		
  	if(pten->address[count]==1){
 			 
			SENIOR_TIM->CCR1  =  0;		 
		}
		else if(pten->address[count]==0){
			SENIOR_TIM->CCR1  =  SEQUENCE_PERIOD+1;	     	
		}
		if(count == 1023){
			  SENIOR_TIM->CCR1  =  SEQUENCE_PERIOD+1;
			 	//TIM_ITConfig(TIM2_GPIO,TIM_IT_Update,DISABLE); //ʹ��ָ����TIM2�ж�,��������ж�
     }
		else if(count == 1024){
	     TIM_ADC1->CR1 &= (uint16_t)~TIM_CR1_CEN; //adc��ʱ��disable
       SENIOR_TIM->CR1 &= (uint16_t)~TIM_CR1_CEN;//���е�ʱ��disable
       count=-1;
		}	 
		count++; 
		TIM_ClearITPendingBit(SENIOR_TIM, TIM_IT_Update); //��� TIM2 �����жϱ�־
 

   }
}

void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

void DEBUG_USART_IRQHandler(void)
{

	if(USART_GetITStatus(DEBUG_USART,USART_IT_RXNE)!=RESET)
	{	
		 USART_ClearITPendingBit(USART1,USART_IT_RXNE); //����жϱ�־.

	   TIM_Cmd(SENIOR_TIM, ENABLE);//��������ʱ���е���ʱ��
 		}
 }	 

