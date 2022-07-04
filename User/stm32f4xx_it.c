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
extern volatile u8 coefInitialFlag;  //��λ�����д����־λ
extern volatile uint16_t* HT_IMS_ADC_ConvertedValue;
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
			printf("adc complete");
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
    

    if(sequenceProduct[count]==1){			 
			SENIOR_TIM->CCR1  =  0;		 
		}
		else{
			SENIOR_TIM->CCR1  =  SEQUENCE_PERIOD+1;	     	
		}
		TIM_ClearITPendingBit(SENIOR_TIM, TIM_IT_Update); //��� TIM2 �����жϱ�־

		if(count == sequenceLength){
			  SENIOR_TIM->CCR1  =  SEQUENCE_PERIOD+1;
			 	//TIM_ITConfig(TIM2_GPIO,TIM_IT_Update,DISABLE); //ʹ��ָ����TIM2�ж�,��������ж�
     }
		else if(count == sequenceLength+1){
	     TIM_ADC1->CR1 &= (uint16_t)~TIM_CR1_CEN; //adc��ʱ��disable
       SENIOR_TIM->CR1 &= (uint16_t)~TIM_CR1_CEN;//���е�ʱ��disable
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
		/* ʹ�ô���DMA */
		if(USART_GetITStatus(DEBUG_USART,USART_IT_IDLE)== SET)
		{		
			// ��������жϱ�־λ
				USART_ClearFlag(DEBUG_USART,USART_FLAG_IDLE);  
 			/* �������� */
		//	Receive_DataPack();
			DMA_Cmd(DEBUG_USART_DMA_STREAM, DISABLE);/* ��ʱ�ر�dma��������δ���� */  
 		  DMA_SetCurrDataCounter(DEBUG_USART_DMA_STREAM,USART_BUFF_SIZE);//�������ô������������
			DMA_Cmd(DEBUG_USART_DMA_STREAM, ENABLE);  
      USART_ReceiveData(DEBUG_USART);
			if(Usart_Rx_Buf[0]=='s'){//��������������ʹ��ADC��ȡ����֡������
			//	printf("%c\r\n",Usart_Rx_Buf[0]);
				beginSampleFlag = 1;
        Usart_Rx_Buf[0] = ' ';
				printf("==s\r\n");
				for(int i = 0;i<20;i++){
					  printf("%c",Usart_Rx_Buf[i]);
					}
				printf("\r\n");
			}
  	//��ȡһ�����ݣ���Ȼ��һֱ���ж�
			else if(Usart_Rx_Buf[0]!='s'){//��������������
				  coefInitialFlag = 1;

			    for(int i = 0;i<20;i++){
					  printf("%c",Usart_Rx_Buf[i]);
					}
					printf("\r\n");
 			} 
 			 
			              //������ڿ����жϱ�־λ

 		}	
		
	
	#else
		
		/* �����ж� */
		if(USART_GetITStatus(DEBUG_USART,USART_IT_RXNE)!=RESET)
		{		
		uint8_t ucTemp;
		USART_ClearITPendingBit(DEBUG_USART,USART_IT_RXNE); //����жϱ�־.
			ucTemp = USART_ReceiveData( DEBUG_USART );
			if((char)ucTemp == 's'){
	//	  TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//����ʱ�ӳ�ʼ��
	//    TIM8_ADC_Config(TIM_ADC1_PERIOD,SENIOR_PSC,TIM_ADC1_LOWTIME);//adcʱ�ӳ�ʼ��
				TIM_Cmd(SENIOR_TIM, ENABLE);//��������ʱ���е���ʱ��
			
			}
			USART_SendData(DEBUG_USART,ucTemp); 

			}
		}
	#endif
	}