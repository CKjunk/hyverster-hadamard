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
//#define SENIOR_PSC  180-1  
//#define COMMON_PSC  90-1 //// 通用控制定时器时钟源TIMxCLK = HCLK/2=90MHz 
//#define OVERSAMPLING 8 
//#define TIM_ADC1_PERIOD 200-1  //实际周期为PWM_PERIOD
//#define TIM_ADC1_LOWTIME 100
//#define SEQUENCE_PERIOD  200-1
int overSampling = 1;
int timAdcPeriod = 200;
int adcLowTime = 0;
void show_Usart_Rx_Buf();
void initialSequence();
void beginSample();
void clear_Usart_Rx_Buf();
void show_Sequence();
void show_coef();
void DEBUG_USART_2_Send_Array(uint16_t send_array[],int number);
u8* createInitialCoef(char Usart_Rx_Buf[],u8* orderLength);
char Usart_Rx_Buf[20];  //用作接收上位机传输的命令字符串
int firstSample = 1;
extern  uint16_t* HT_IMS_ADC_ConvertedValue;// ADC转换的电压值通过DMA方式传到SRAM
int recSize = 0;
int flag = 0;
int sequenceCount = 0;  	 //发送的序列计数
volatile int frame_count = 0;  //帧数计数
u8 orderLength = 0;
u8* porderLength = &orderLength; //系数序列长度
volatile int  sequenceLength = 0;
volatile int* psequenceLength = &sequenceLength;//s序列长度
volatile int buffsize = 0;   //记录adc开辟的数组大小
u8* coef;    //上位机发过来的序列首地址
u8* sequenceProduct;
volatile u8 initialNumber = 0;
int main(void)
{	
 	  SysTick_Init();
 	  EXTI_Key_Config(); //按键初始化
	  Debug_USART_Config();//串口初始化
    while (1){
			
			 switch(flag){
				 case 1: //数据接收标志位
 				   flag = 10;
				   show_Usart_Rx_Buf();
					 if(Usart_Rx_Buf[0] == 's'){
						 show_Sequence();

						 beginSample(); //开始采集
 					 printf("sequenceProduct address:%p\r\n",sequenceProduct);

					 clear_Usart_Rx_Buf();
					 recSize = 0;
 					 }
					 else{
						 	initialNumber++;  //初始化次数加一

              initialSequence();//初始化
						  frame_count = 0;
					 	  show_Sequence();
						 printf("sequenceProduct address:%p,buffsize:%d\r\n",sequenceProduct,buffsize);
						  clear_Usart_Rx_Buf();
						  recSize = 0;
					 }
				  break;
			   case 2: //adc完成标志位
 					  flag = 10;
			      printf("after DMA_GetCurrDataCounter:%d sequenceCount :%d sequenceLength :%d\r\n",DMA_GetCurrDataCounter(RHEOSTAT_ADC_DMA_STREAM),sequenceCount,sequenceLength);

					  if(frame_count > 1&&initialNumber == 1){
								DEBUG_USART_2_Send_Array(HT_IMS_ADC_ConvertedValue,buffsize -1);
								printf("sequenceCount:%d\r\n",sequenceCount);					  }
						if(initialNumber>1){
								DEBUG_USART_2_Send_Array(HT_IMS_ADC_ConvertedValue,buffsize );
                printf("sequenceCount:%d\r\n",sequenceCount);
						}
				  //  DEBUG_USART_2_Send_Array(HT_IMS_ADC_ConvertedValue);
 				 break;

			 }
       //0000011a
			 //00000001001a
			 //000000000101a
			 //0000001010011a
			 //00000000011011a
 		}
  }

u8* createInitialCoef(char Usart_Rx_Buf[],u8* orderLength){
	 int count = 0;
	 overSampling = Usart_Rx_Buf[0] - 48; //拿到第一位的倍频系数
	 count++;
   while(Usart_Rx_Buf[count]!='a'){
	    count++;
	 }
    u8* coefinside = (u8*)malloc((count-1)*sizeof(u8));
	 	memset(coefinside, 0, sizeof(u8));

	  count = 1;
    while(Usart_Rx_Buf[count]!='a'){
			coefinside[count-1] = (int)Usart_Rx_Buf[count] - 48;
	    count++;
	 }
		*orderLength = count-1;
   for(int i = 0;i < count - 1 ;i++){	 
	     printf("%d ",coefinside[i]);
	 }
 	     printf("\r\n");

 	 return coefinside;
}

void initialSequence(){
//			if(initialNumber>0){
//				free(coef);
//				coef = NULL;
//				free(sequenceProduct);
//				sequenceProduct = NULL;
//				free(HT_IMS_ADC_ConvertedValue);
//				HT_IMS_ADC_ConvertedValue = NULL;
//			
//			}	
      coef = createInitialCoef(Usart_Rx_Buf,porderLength);//构造系数
		 	printf("coefLength:%d overSampling：%d\r\n",orderLength,overSampling);
      show_coef();
			sequenceProduct = createSequence(coef,orderLength);//产生s序列
			sequenceLength = pow(2,orderLength)-1; //计算序列长度
	 
//	 	    buffsize = (initialNumber == 1)?sequenceLength*overSampling + 3:sequenceLength*overSampling + 2;//实际开辟的数组大小需要比sequenceLength大3
      buffsize = (initialNumber == 1)?sequenceLength + 3:sequenceLength + 2;//实际开辟的数组大小需要比sequenceLength大3

		  printf("sequenceLength:%d\r\n",sequenceLength);
	    printf("buffsize:%d\r\n",buffsize);
			HT_IMS_ADC_ConvertedValue = (uint16_t*)malloc((buffsize+5)*sizeof(uint16_t));//根据上位机的命令开辟adc数据存储空间     
			if(HT_IMS_ADC_ConvertedValue == NULL){
			printf("adc malloc failure\r\n");
			}
			memset(HT_IMS_ADC_ConvertedValue, 0, sizeof(uint16_t));
}
void  beginSample(){
	if(firstSample == 1){
	     ADCX_Init(buffsize);
			 printf("before firstSample DMA_GetCurrDataCounter:%d sequenceCount :%d sequenceLength :%d\r\n",DMA_GetCurrDataCounter(RHEOSTAT_ADC_DMA_STREAM),sequenceCount,sequenceLength);
		   TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//序列时钟初始化
  	   TIM8_ADC_Config(TIM_ADC1_PERIOD,SENIOR_PSC,TIM_ADC1_LOWTIME);//adc时钟初始化
 		   TIM_Cmd(SENIOR_TIM, ENABLE);//开始采集

  }
	else{
    if(initialNumber == 1){
	   //RHEOSTAT_ADC_DMA_STREAM->NDTR = buffsize-1 ; //重新装填DMA需要传输的数据个数
				     ADCX_Init(buffsize-1);

	  }
		else{
        ADCX_Init(buffsize);		
		}
		//DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);          //先使能DMA通道
	   TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//序列时钟初始化
  	 TIM8_ADC_Config(TIM_ADC1_PERIOD,SENIOR_PSC,TIM_ADC1_LOWTIME);//adc时钟初始化
		 sequenceCount = 0;
 		 TIM_Cmd(SENIOR_TIM, ENABLE);//开始采集
		 printf("before Sample DMA_GetCurrDataCounter:%d sequenceCount :%d sequenceLength :%d\r\n",DMA_GetCurrDataCounter(RHEOSTAT_ADC_DMA_STREAM),sequenceCount,sequenceLength);

	}
	 
	   firstSample = 0;
}
void DEBUG_USART_2_Send_Array(uint16_t send_array[],int number) //两个参数 一是数组(的数据) 二是数组长度1-255 
{
        //串口发送
        int i=0;  //定义一个局部变量  用来 发送字符串 ++运算
        while(i<number)
        {                
				//	 printf("%d\r\n",send_array[i]); 
					 printf("%d %d\r\n",i,send_array[i]); 
					 i++;					
        }
        
}

void clear_Usart_Rx_Buf(){
	int i = 0;
   while(i < recSize){
	   Usart_Rx_Buf[i] = '0';
		 i++;
	 }

}
void show_Usart_Rx_Buf(){
	int i = 0;
   while(i < recSize){
	   printf("%c",Usart_Rx_Buf[i]);
		 i++;
	 }
	 printf("\r\n");
}
void show_Sequence(){
   for(int i = 0;i < sequenceLength;i++){
	   printf("%d ",sequenceProduct[i]);
	 
	 }
	 printf("\r\n");
}
void show_coef(){
	 printf("show_coef\r\n");
   for(int i = 0;i < orderLength;i++){
	   printf("%d ",coef[i]);
	 
	 }
	 printf("\r\n");
}