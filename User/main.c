#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./adc/bsp_adc.h"
#include "./sequence/sequence.h"
#include "./time/bsp_time.h"
#include "./key/key.h"
 #include "./led/bsp_led.h"  
#include "./usart_dma/usart_dma.h"
#include "./HadamardOutput/bsp_dma_gpio.h"
#include "./systick/bsp_SysTick.h"
#include "stdlib.h"
#include "math.h"
int* createInitialCoef(char Usart_Rx_Buf[],int* orderLength);

extern char Usart_Rx_Buf[USART_BUFF_SIZE];
extern volatile uint16_t* HT_IMS_ADC_ConvertedValue;// ADCת���ĵ�ѹֵͨ��DMA��ʽ����SRAM

//extern uint16_t HT_IMS_ADC_ConvertedValue[1026];// ADCת���ĵ�ѹֵͨ��DMA��ʽ����SRAM
volatile u8 display_flag = 0;  //adc dma���ݴ�����ɱ�־λ
volatile u8 coefInitialFlag = 0;  //��λ�����д����־λ
volatile u16 count = 0;  	 //���͵����м���
volatile u16 frame_count = 1;  //֡������
int orderLength = 0;
int* porderLength = &orderLength; //ϵ�����г���
volatile int  sequenceLength = 0;
volatile int beginSampleFlag = 0;
volatile int initialFlay = 0;
volatile int* psequenceLength = &sequenceLength;//s���г���
extern volatile  int buffsize ;   //��¼adc���ٵ������С
int* coef;    //��λ���������������׵�ַ
u8* sequenceProduct;
volatile u8 isFreeFlag = 0; 
int main(void)
{	
 	  SysTick_Init();
 	  EXTI_Key_Config(); //������ʼ��
	  Debug_USART_Config();//���ڳ�ʼ��
 
     while (1){
			if(coefInitialFlag == 1){
				coefInitialFlag = 0;		

				if(isFreeFlag == 1){
				  free(coef);
          free(sequenceProduct);
          free(HT_IMS_ADC_ConvertedValue);			
				
				}
         coef = createInitialCoef(Usart_Rx_Buf,porderLength);	//����ϵ��
         sequenceProduct = createSequence(coef,orderLength);//����s����
			
				 sequenceLength = pow(2,orderLength)-1; //�������г���
				 buffsize = sequenceLength+3;  //ʵ�ʿ��ٵ������С��Ҫ��sequenceLength��3
			   printf("sequenceLength:%d\r\n",sequenceLength);
				 HT_IMS_ADC_ConvertedValue = (uint16_t*)malloc((buffsize)*sizeof(uint16_t));//������λ���������adc���ݴ洢�ռ�     
				 if(HT_IMS_ADC_ConvertedValue == NULL){
				  printf("adc malloc failure\r\n");
		     }
				 memset(HT_IMS_ADC_ConvertedValue, 0, sizeof(uint16_t));
         Rheostat_Init();//adc��ʼ��
  	     TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//����ʱ�ӳ�ʼ��
	       TIM8_ADC_Config(TIM_ADC1_PERIOD,SENIOR_PSC,TIM_ADC1_LOWTIME);//adcʱ�ӳ�ʼ��
 			 	 isFreeFlag = 1;
			}
      if(beginSampleFlag == 1) {
				 beginSampleFlag = 0;

				 printf("begin sample\r\n");
				// Rheostat_Init();
//				 TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//����ʱ�ӳ�ʼ��
//         TIM8_ADC_Config(TIM_ADC1_PERIOD,SENIOR_PSC,TIM_ADC1_LOWTIME);//adcʱ�ӳ�ʼ��
			 	 TIM_Cmd(SENIOR_TIM, ENABLE);//��������ʱ���е���ʱ��
         Delay_ms(2); 
     	}
	     
			if(display_flag == 1){ //��ʾһ���Ѿ��ɼ���
				  display_flag = 0;

 					for(int i = 0;i < buffsize;i++){ //�����ϴ�����λ��
            printf("%d,%d\r\n",i,HT_IMS_ADC_ConvertedValue[i]);	
            HT_IMS_ADC_ConvertedValue[i] = 0;
				 }
 			   RHEOSTAT_ADC_DMA_STREAM->NDTR = buffsize; //����װ��DMA��Ҫ��������ݸ���
			   DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE); //��ʹ��DMAͨ��
			   RHEOSTAT_ADC->SR &=~(0X01<<5);   //����Ӳ����1��ADC״̬�Ĵ����������־λ�������Ȼ���׳���
         TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//����ʱ�ӳ�ʼ��
         TIM8_ADC_Config(TIM_ADC1_PERIOD,SENIOR_PSC,TIM_ADC1_LOWTIME);//adcʱ�ӳ�ʼ��
	 				frame_count++;
			}
			 //0000001001a
			 //00000000101a
			 //000001010011a
			 //0000000011011a
		}
  }

int* createInitialCoef(char Usart_Rx_Buf[],int* orderLength){
	 int count = 0;
   while(Usart_Rx_Buf[count]!='a'){
	    count++;
	 }
    int* coef = (int*)malloc(count*sizeof(int));
	  count = 0;
    while(Usart_Rx_Buf[count]!='a'){
			coef[count] = (int)Usart_Rx_Buf[count] - 48;
	    count++;
	 }
		*orderLength = count;
   for(int i = 0;i < count;i++){	 
	     printf("%d ",coef[i]);
	 }
 
 	 return coef;
}


