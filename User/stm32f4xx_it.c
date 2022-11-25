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
#include "stdlib.h"
#include "math.h"
#include "hadamard.h"
extern int numberOfZero;
extern void beginSample(int number);
extern  int adcSequenceCount;
extern  int frameCount;
extern int recSize;
extern  int flag;
extern volatile char Usart_Rx_Buf[5];
extern void TimingDelay_Decrement(void);
extern int sequenceLength ;
extern u8* sendSequence;
extern int sequenceCount ;
extern int sequencePeriod  ;   
extern int sequenceLowTime ;
extern int AdcPeriod;
extern int AdcLowTime;	
extern uint16_t* HT_IMS_ADC_ConvertedValue;
extern int errorSample;
extern int sampleSize;
u8 MODIFY_TIM8_PULSE = 1;
void TOTAL_PROJECT_INT_FUNCTION(void){
	if(EXTI_GetITStatus(KEY_INT_EXTI_LINE)!= RESET){
 		/*��ʼ�ɼ�*/
	  
   flag = 0  ;  
   EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ

	}
}
void DMA_ADC_COMP_INT_FUNCTION(void){
    if(DMA_GetITStatus(RHEOSTAT_ADC_DMA_STREAM,DMA_IT_TCIF0))
	  {			
 			DMA_ClearITPendingBit(RHEOSTAT_ADC_DMA_STREAM,DMA_FLAG_TCIF0);		   			 	
       
		  DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM,DISABLE);          //��ʹ��DMAͨ��
      flag = 2;
	  }
}

 
void SENIOR_TIM_INT_FUNCTION(void){
 	if(TIM_GetITStatus(SENIOR_TIM, TIM_IT_Update)){	
	  TIM_ClearITPendingBit(SENIOR_TIM, TIM_IT_Update); //��� TIM2 �����жϱ�־
 	  if(sendSequence[sequenceCount]==1){			 
			SENIOR_TIM->CCR1  =  0;		 
		}
		else{
			SENIOR_TIM->CCR1  =  SEQUENCE_PERIOD+1;	     	
		}

		if(sequenceCount == sequenceLength){
			SENIOR_TIM->CCR1  =  SEQUENCE_PERIOD + 1;
    }
		if(sequenceCount == sequenceLength+1){
              
      /*������������ϣ���Ϊ��һ��ʹ�ܵĳ��Ե�ƽ*/
  		 SENIOR_TIM->CCR1 = SEQUENCE_PERIOD+1;
			// TIM_ADC1->CR1 &= (uint16_t)~TIM_CR1_CEN;//���е�ʱ��disable
       //SENIOR_TIM->CR1 &= (uint16_t)~TIM_CR1_CEN;//���е�ʱ��disable
      
			 TIM_ITConfig(SENIOR_TIM,TIM_IT_Update,DISABLE); //ʹ��ָ����TIM8�ж�,��������ж�

		}	 
		sequenceCount++; 
		
     
   } 
}
	
		
		
 

void TIM_ADC1_IRQHandler(){
  if(TIM_GetITStatus(TIM_ADC1, TIM_IT_Update)){	
    TIM_ClearITPendingBit(TIM_ADC1, TIM_IT_Update); //��� TIM2 �����жϱ�־
 	  	TIM_ADC1->CCR1  = AdcLowTime;
         
		 
 
		 if(adcSequenceCount == sequenceLength+1){
        //TIM_ClearITPendingBit(TIM_ADC1, TIM_IT_Update);
       TIM_ADC1->CCR1  =  AdcPeriod + 1;
      // TIM_ADC1->CR1 &= (uint16_t)~TIM_CR1_CEN;//���е�ʱ��disable
			 TIM_ClearFlag(TIM_ADC1,TIM_FLAG_Update);
	     TIM_ClearITPendingBit(TIM_ADC1, TIM_IT_Update); //��� TIM2 �����жϱ�־
			 TIM_ITConfig(TIM_ADC1,TIM_IT_Update,DISABLE); //ʹ��ָ����TIM8�ж�,��������ж�

		 }
		 adcSequenceCount++;

	   

  }
}
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

//void DEBUG_USART_1_IRQHandler(void)
//{
//	#if USE_USART_DMA_RX
//		/* ʹ�ô���DMA */
//		if(USART_GetITStatus(DEBUG_USART,USART_IT_IDLE)== SET)
//		{		
//			
// 		}	
//	#else
//		
//		/* �����ж� */
//		uint8_t temp;  
//    if(USART_GetITStatus(DEBUG_USART_1, USART_IT_RXNE) != RESET)//���յ�һ���ֽڣ�����һ�ν����ж�  
//    {           

//        Usart_Rx_Buf[recSize++] = DEBUG_USART_1->DR; //�����յ����ݴ���rx_buff�� 
////			  if(Usart_Rx_Buf[0] == 's'){
////			         flag = 1;
////			}
//    }   
//  
//    if(USART_GetITStatus(DEBUG_USART_1, USART_IT_IDLE) != RESET)//���������ݺ��������ж�  
//    {     
//    	  temp = DEBUG_USART_1->SR; //�ȶ�SR,�ٶ�DR�������idle�жϵ����㣬�����һֱ�����жϡ�  
//        temp = DEBUG_USART_1->DR; 
//  
//        flag = 1; //��⵽����״̬����λ�������λ  
//    }  
//	#endif
//}