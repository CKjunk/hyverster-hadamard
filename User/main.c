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
int overSampling = 1;  //过采样系数
int timAdcPeriod = 200;  //过采样系数为1的时候的adc定时器周期
int adcLowTime = 0;      //定时器一个周期内低电平持续时间
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
int recSize = 0;  //接收的命令大小
int flag = 0;  //总的flag
int sequenceCount = 0;  	 //发送的序列计数
volatile int frame_count = 0;  //帧数计数
u8 orderLength = 0;  //
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
			 //show_Usart_Rx_Buf();
				 if(Usart_Rx_Buf[0] == 's'){
				 //show_Sequence();
					 beginSample(); //开始采集
				 //printf("sequenceProduct address:%p\r\n",sequenceProduct);
					 clear_Usart_Rx_Buf(); 
					 recSize = 0;
				 }
				 else{
						initialNumber++;  //初始化次数加一
					//printf("initial number:%d\r\n",initialNumber);
						initialSequence();//初始化
						clear_Usart_Rx_Buf();
						frame_count = 0;
					//show_Sequence();
					//printf("sequenceProduct address:%p,buffsize:%d\r\n",sequenceProduct,buffsize);
						recSize = 0;
				 }
				break;
			 case 2: //adc完成标志位
				 //printf("initial number:%d\r\n",initialNumber);
					flag = 10;
				 //printf("after DMA_GetCurrDataCounter:%d sequenceCount :%d sequenceLength :%d\r\n",DMA_GetCurrDataCounter(RHEOSTAT_ADC_DMA_STREAM),sequenceCount,sequenceLength);
					if(frame_count > 1&&initialNumber == 1){
						//printf("display\r\n");
							DEBUG_USART_2_Send_Array(HT_IMS_ADC_ConvertedValue,buffsize - overSampling);
						//printf("sequenceCount:%d\r\n",sequenceCount);					
						}
					if(initialNumber>1){
						 // printf("display\r\n");
							DEBUG_USART_2_Send_Array(HT_IMS_ADC_ConvertedValue,buffsize );
							//printf("sequenceCount:%d\r\n",sequenceCount);
					}
				//  DEBUG_USART_2_Send_Array(HT_IMS_ADC_ConvertedValue);
			 break;
			}
       //2000011a
			 //20000001001a
			 //200000000101a
			 //2000001010011a
			 //20000000011011a
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
//   for(int i = 0;i < count - 1 ;i++){	 
//	     printf("%d ",coefinside[i]);
//	 }
// 	     printf("\r\n");

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
	    timAdcPeriod = 200/overSampling - 1;//根据过采样系数来计算ADC的采样周期
	    adcLowTime = 200/overSampling/2;
	//	 	printf("coefLength:%d overSampling：%d\r\n",orderLength,overSampling);
      //show_coef();
			sequenceProduct = createSequence(coef,orderLength);//产生s序列
			sequenceLength = pow(2,orderLength)-1; //计算序列长度
	 
	 	  buffsize = (initialNumber == 1)?(sequenceLength + 3)*overSampling:(sequenceLength + 2)*overSampling;//实际开辟的数组大小需要比sequenceLength大3
 //   buffsize = (initialNumber == 1)?sequenceLength + 3:sequenceLength + 2;//实际开辟的数组大小需要比sequenceLength大3

//		  printf("sequenceLength:%d\r\n",sequenceLength);
	//    printf("buffsize:%d\r\n",buffsize);
			HT_IMS_ADC_ConvertedValue = (uint16_t*)malloc((buffsize+5)*sizeof(uint16_t));//根据上位机的命令开辟adc数据存储空间     
			if(HT_IMS_ADC_ConvertedValue == NULL){
	   		printf("adc malloc failure\r\n");
			}
			memset(HT_IMS_ADC_ConvertedValue, 0, sizeof(uint16_t));
}
void  beginSample(){
	if(frame_count == 0){
	     ADCX_Init(buffsize);
	//		 printf("before firstSample DMA_GetCurrDataCounter:%d sequenceCount :%d sequenceLength :%d\r\n",DMA_GetCurrDataCounter(RHEOSTAT_ADC_DMA_STREAM),sequenceCount,sequenceLength);
		   TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//序列时钟初始化
  	   TIM8_ADC_Config(timAdcPeriod,SENIOR_PSC,adcLowTime);//adc时钟初始化
			 sequenceCount = 0;
       TIM_Cmd(SENIOR_TIM, ENABLE);//开始采集

  }
	else{
    if(initialNumber == 1){
 				ADCX_Init(buffsize-overSampling);

	  }
		else{
        ADCX_Init(buffsize);		
		}
     TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//序列时钟初始化
  	 TIM8_ADC_Config(timAdcPeriod,SENIOR_PSC,adcLowTime);//adc时钟初始化
		 sequenceCount = 0;
 		 TIM_Cmd(SENIOR_TIM, ENABLE);//开始采集
//		 printf("before Sample DMA_GetCurrDataCounter:%d sequenceCount :%d sequenceLength :%d\r\n",DMA_GetCurrDataCounter(RHEOSTAT_ADC_DMA_STREAM),sequenceCount,sequenceLength);

	}
	 
}
void DEBUG_USART_2_Send_Array(uint16_t send_array[],int number) //两个参数 一是数组(的数据) 二是数组长度1-255 
{
        //串口发送
        int i=0;  //定义一个局部变量  用来 发送字符串 ++运算
        while(i<number)
        {                
					 printf("%d\r\n",send_array[i]); 
					// printf("%d %d\r\n",i,send_array[i]); 
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