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
 		/*开始采集*/
	  
   flag = 0  ;  
   EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位

	}
}
void DMA_ADC_COMP_INT_FUNCTION(void){
    if(DMA_GetITStatus(RHEOSTAT_ADC_DMA_STREAM,DMA_IT_TCIF0))
	  {			
 			DMA_ClearITPendingBit(RHEOSTAT_ADC_DMA_STREAM,DMA_FLAG_TCIF0);		   			 	
       
		  DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM,DISABLE);          //先使能DMA通道
      flag = 2;
	  }
}

 
void SENIOR_TIM_INT_FUNCTION(void){
 	if(TIM_GetITStatus(SENIOR_TIM, TIM_IT_Update)){	
	  TIM_ClearITPendingBit(SENIOR_TIM, TIM_IT_Update); //清除 TIM2 更新中断标志
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
              
      /*下面这句必须加上，作为下一次使能的初试电平*/
  		 SENIOR_TIM->CCR1 = SEQUENCE_PERIOD+1;
			// TIM_ADC1->CR1 &= (uint16_t)~TIM_CR1_CEN;//序列的时钟disable
       //SENIOR_TIM->CR1 &= (uint16_t)~TIM_CR1_CEN;//序列的时钟disable
      
			 TIM_ITConfig(SENIOR_TIM,TIM_IT_Update,DISABLE); //使能指定的TIM8中断,允许更新中断

		}	 
		sequenceCount++; 
		
     
   } 
}
	
		
		
 

void TIM_ADC1_IRQHandler(){
  if(TIM_GetITStatus(TIM_ADC1, TIM_IT_Update)){	
    TIM_ClearITPendingBit(TIM_ADC1, TIM_IT_Update); //清除 TIM2 更新中断标志
 	  	TIM_ADC1->CCR1  = AdcLowTime;
         
		 
 
		 if(adcSequenceCount == sequenceLength+1){
        //TIM_ClearITPendingBit(TIM_ADC1, TIM_IT_Update);
       TIM_ADC1->CCR1  =  AdcPeriod + 1;
      // TIM_ADC1->CR1 &= (uint16_t)~TIM_CR1_CEN;//序列的时钟disable
			 TIM_ClearFlag(TIM_ADC1,TIM_FLAG_Update);
	     TIM_ClearITPendingBit(TIM_ADC1, TIM_IT_Update); //清除 TIM2 更新中断标志
			 TIM_ITConfig(TIM_ADC1,TIM_IT_Update,DISABLE); //使能指定的TIM8中断,允许更新中断

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
//		/* 使用串口DMA */
//		if(USART_GetITStatus(DEBUG_USART,USART_IT_IDLE)== SET)
//		{		
//			
// 		}	
//	#else
//		
//		/* 接收中断 */
//		uint8_t temp;  
//    if(USART_GetITStatus(DEBUG_USART_1, USART_IT_RXNE) != RESET)//接收到一个字节，进入一次接收中断  
//    {           

//        Usart_Rx_Buf[recSize++] = DEBUG_USART_1->DR; //将接收的数据存入rx_buff中 
////			  if(Usart_Rx_Buf[0] == 's'){
////			         flag = 1;
////			}
//    }   
//  
//    if(USART_GetITStatus(DEBUG_USART_1, USART_IT_IDLE) != RESET)//接收完数据后进入空闲中断  
//    {     
//    	  temp = DEBUG_USART_1->SR; //先读SR,再读DR才能完成idle中断的清零，否则会一直进入中断。  
//        temp = DEBUG_USART_1->DR; 
//  
//        flag = 1; //检测到空闲状态，置位接收完成位  
//    }  
//	#endif
//}