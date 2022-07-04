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

volatile int* createInitialCoef(char Usart_Rx_Buf[],int* orderLength);
extern volatile char Usart_Rx_Buf[USART_BUFF_SIZE];
extern void TimingDelay_Decrement(void);
extern volatile u8* sequenceProduct;
extern int* coef;
extern int* porderLength;
extern int* psequenceLength;
extern int  sequenceLength;
extern volatile u8 isFreeFlag; 
extern volatile u16  count;
extern int buffsize;
extern volatile u8   display_flag;
extern volatile int beginSampleFlag ;
extern volatile int initialFlay  ;
extern volatile u16  frame_count;
extern  int  orderLength;
extern volatile u8 coefInitialFlag;  //上位机序列传输标志位
extern volatile uint16_t* HT_IMS_ADC_ConvertedValue;
u8 MODIFY_TIM8_PULSE = 1;
void TOTAL_PROJECT_INT_FUNCTION(void){
	if(EXTI_GetITStatus(KEY_INT_EXTI_LINE)!= RESET){
 		  TIM_Cmd(SENIOR_TIM, ENABLE);//开启主从时钟中的主时钟
      EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位

	}
}
void DMA_ADC_COMP_INT_FUNCTION(void){
    if(DMA_GetITStatus(RHEOSTAT_ADC_DMA_STREAM,DMA_IT_TCIF0))
	  {
			printf("adc complete");
 			DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, DISABLE);          //先使能DMA通道

			display_flag = 1;		//dma传输完成后将发送标志位置1
      DMA_ClearITPendingBit(RHEOSTAT_ADC_DMA_STREAM,DMA_FLAG_TCIF0);		   			 
	  }
}

void TIM_ADC1_IRQHandler(void){
	if(TIM_GetITStatus(SENIOR_TIM, TIM_IT_Update)){
		
	}
}
void SENIOR_TIM_INT_FUNCTION(void){
 	if(TIM_GetITStatus(SENIOR_TIM, TIM_IT_Update)){	
    

    if(sequenceProduct[count]==1){			 
			SENIOR_TIM->CCR1  =  0;		 
		}
		else{
			SENIOR_TIM->CCR1  =  SEQUENCE_PERIOD+1;	     	
		}
		TIM_ClearITPendingBit(SENIOR_TIM, TIM_IT_Update); //清除 TIM2 更新中断标志

		if(count == sequenceLength){
			  SENIOR_TIM->CCR1  =  SEQUENCE_PERIOD+1;
			 	//TIM_ITConfig(TIM2_GPIO,TIM_IT_Update,DISABLE); //使能指定的TIM2中断,允许更新中断
     }
		else if(count == sequenceLength+1){
	     TIM_ADC1->CR1 &= (uint16_t)~TIM_CR1_CEN; //adc的时钟disable
       SENIOR_TIM->CR1 &= (uint16_t)~TIM_CR1_CEN;//序列的时钟disable
       count=-1;
		}	 
		count++; 

   }
}

void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

void DEBUG_USART_IRQHandler(void)
{
	#if USE_USART_DMA_RX
		/* 使用串口DMA */
		if(USART_GetITStatus(DEBUG_USART,USART_IT_IDLE)== SET)
		{		
			// 清除空闲中断标志位
				USART_ClearFlag(DEBUG_USART,USART_FLAG_IDLE);  
 			/* 接收数据 */
		//	Receive_DataPack();
			DMA_Cmd(DEBUG_USART_DMA_STREAM, DISABLE);/* 暂时关闭dma，数据尚未处理 */  
 		  DMA_SetCurrDataCounter(DEBUG_USART_DMA_STREAM,USART_BUFF_SIZE);//重新设置传输的数据数量
			DMA_Cmd(DEBUG_USART_DMA_STREAM, ENABLE);  
      USART_ReceiveData(DEBUG_USART);
			if(Usart_Rx_Buf[0]=='s'){//发过来的是重新使能ADC获取数据帧的命令
			//	printf("%c\r\n",Usart_Rx_Buf[0]);
				beginSampleFlag = 1;
        Usart_Rx_Buf[0] = ' ';
				printf("==s\r\n");
				for(int i = 0;i<20;i++){
					  printf("%c",Usart_Rx_Buf[i]);
					}
				printf("\r\n");
			}
  	//读取一次数据，不然会一直进中断
			else if(Usart_Rx_Buf[0]!='s'){//发过来的是序列
				  coefInitialFlag = 1;

			    for(int i = 0;i<20;i++){
					  printf("%c",Usart_Rx_Buf[i]);
					}
					printf("\r\n");
 			} 
 			 
			              //清除串口空闲中断标志位

 		}	
		
	
	#else
		
		/* 接收中断 */
		if(USART_GetITStatus(DEBUG_USART,USART_IT_RXNE)!=RESET)
		{		
		uint8_t ucTemp;
		USART_ClearITPendingBit(DEBUG_USART,USART_IT_RXNE); //清除中断标志.
			ucTemp = USART_ReceiveData( DEBUG_USART );
			if((char)ucTemp == 's'){
	//	  TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//序列时钟初始化
	//    TIM8_ADC_Config(TIM_ADC1_PERIOD,SENIOR_PSC,TIM_ADC1_LOWTIME);//adc时钟初始化
				TIM_Cmd(SENIOR_TIM, ENABLE);//开启主从时钟中的主时钟
			
			}
			USART_SendData(DEBUG_USART,ucTemp); 

			}
		}
	#endif
	}