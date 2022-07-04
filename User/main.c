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
extern volatile uint16_t* HT_IMS_ADC_ConvertedValue;// ADC转换的电压值通过DMA方式传到SRAM

//extern uint16_t HT_IMS_ADC_ConvertedValue[1026];// ADC转换的电压值通过DMA方式传到SRAM
volatile u8 display_flag = 0;  //adc dma数据传输完成标志位
volatile u8 coefInitialFlag = 0;  //上位机序列传输标志位
volatile u16 count = 0;  	 //发送的序列计数
volatile u16 frame_count = 1;  //帧数计数
int orderLength = 0;
int* porderLength = &orderLength; //系数序列长度
volatile int  sequenceLength = 0;
volatile int beginSampleFlag = 0;
volatile int initialFlay = 0;
volatile int* psequenceLength = &sequenceLength;//s序列长度
extern volatile  int buffsize ;   //记录adc开辟的数组大小
int* coef;    //上位机发过来的序列首地址
u8* sequenceProduct;
volatile u8 isFreeFlag = 0; 
int main(void)
{	
 	  SysTick_Init();
 	  EXTI_Key_Config(); //按键初始化
	  Debug_USART_Config();//串口初始化
 
     while (1){
			if(coefInitialFlag == 1){
				coefInitialFlag = 0;		

				if(isFreeFlag == 1){
				  free(coef);
          free(sequenceProduct);
          free(HT_IMS_ADC_ConvertedValue);			
				
				}
         coef = createInitialCoef(Usart_Rx_Buf,porderLength);	//构造系数
         sequenceProduct = createSequence(coef,orderLength);//产生s序列
			
				 sequenceLength = pow(2,orderLength)-1; //计算序列长度
				 buffsize = sequenceLength+3;  //实际开辟的数组大小需要比sequenceLength大3
			   printf("sequenceLength:%d\r\n",sequenceLength);
				 HT_IMS_ADC_ConvertedValue = (uint16_t*)malloc((buffsize)*sizeof(uint16_t));//根据上位机的命令开辟adc数据存储空间     
				 if(HT_IMS_ADC_ConvertedValue == NULL){
				  printf("adc malloc failure\r\n");
		     }
				 memset(HT_IMS_ADC_ConvertedValue, 0, sizeof(uint16_t));
         Rheostat_Init();//adc初始化
  	     TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//序列时钟初始化
	       TIM8_ADC_Config(TIM_ADC1_PERIOD,SENIOR_PSC,TIM_ADC1_LOWTIME);//adc时钟初始化
 			 	 isFreeFlag = 1;
			}
      if(beginSampleFlag == 1) {
				 beginSampleFlag = 0;

				 printf("begin sample\r\n");
				// Rheostat_Init();
//				 TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//序列时钟初始化
//         TIM8_ADC_Config(TIM_ADC1_PERIOD,SENIOR_PSC,TIM_ADC1_LOWTIME);//adc时钟初始化
			 	 TIM_Cmd(SENIOR_TIM, ENABLE);//开启主从时钟中的主时钟
         Delay_ms(2); 
     	}
	     
			if(display_flag == 1){ //表示一轮已经采集完
				  display_flag = 0;

 					for(int i = 0;i < buffsize;i++){ //数据上传到上位机
            printf("%d,%d\r\n",i,HT_IMS_ADC_ConvertedValue[i]);	
            HT_IMS_ADC_ConvertedValue[i] = 0;
				 }
 			   RHEOSTAT_ADC_DMA_STREAM->NDTR = buffsize; //重新装填DMA需要传输的数据个数
			   DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE); //先使能DMA通道
			   RHEOSTAT_ADC->SR &=~(0X01<<5);   //将由硬件置1的ADC状态寄存器的溢出标志位清除，不然容易出错
         TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//序列时钟初始化
         TIM8_ADC_Config(TIM_ADC1_PERIOD,SENIOR_PSC,TIM_ADC1_LOWTIME);//adc时钟初始化
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


