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
#include "arm_math.h"
#include "hadamard.h"
#define HADAMARDORDER 128
int numberOfZero = 0;

int averageNumber = 40;

int AdcPeriod = 200-1;  //过采样系数为1的时候的adc定时器周期
int AdcLowTime = 100;    //定时器一个周期内低电平持续时间
int sequencePeriod = 200 - 1;  //过采样系数为1的时候的adc定时器周期
int sequenceLowTime = 200;      //定时器一个周期内低电平持续时间
void showSendSequence(u8* sendSuenquence,int length);
void beginSample(int number);
void showHadamard(MatrixElementType *pHadamard,int hadamardOrder);
void DEBUG_USART_2_Send_Array(uint16_t send_array[],int number);
void sendOverlap(MatrixElementType*pOverlapSignal,int hadamardOrder,int row);
void getOverlopSignal(uint16_t*HT_IMS_ADC_ConvertedValue,MatrixElementType *pOverlapSignal,int hadamardOrder,int framecount);
void showOriginalValue(uint16_t*HT_IMS_ADC_ConvertedValue, int hadamardOrder);
void getAverageSignal(MatrixElementType*pRecoverySignal,MatrixElementType* pAverageResult ,int hadamardOrder,int averageNumber);
void showRecoverySignal(MatrixElementType* array, int hadamardOrder);
void arraycircShift(MatrixElementType* array,int length,int shiftNumber);
void showResultSignal(MatrixElementType* array, int hadamardOrder);
void productSendSequence(MatrixElementType* pPrbs,u8* sendSuenquence,int frameCount,int sequenceLength);


int sequenceLength = HADAMARDORDER-1;

extern uint16_t* HT_IMS_ADC_ConvertedValue;

u8* sendSequence = NULL;

int overSampling = 1;  //过采样系数
 

  
int frameCount = 0;
int flag = 10;  //总的flag
int sequenceCount = 0;  	 //发送的序列计数
int initialNumber = 0;
int adcSequenceCount = 0;
int errorSample = 3;
int sampleSize = 0;
int main(void)
{		
		SysTick_Init();
 	  EXTI_Key_Config(); //按键初始化
	  Debug_USART_Config();//串口初
 	  float32_t scale = 1.0/HADAMARDORDER;
    sampleSize = sequenceLength+1;
    volatile int frame_count = 0;  //帧数计数
	 	
	  MatrixType  matrixPrbs;
	
		MatrixType  matrixPrbsTemp;

    MatrixType  matrixPrbsInverse; 
	
	  MatrixType  matrixRecoverySignal;  
	
	  MatrixType  matrixValidateResult;  
	
	  MatrixType  matrixOverlapSignal;  
	
    MatrixType  matrixResult;

    HT_IMS_ADC_ConvertedValue = (uint16_t*)malloc((sequenceLength + 2)* sizeof(uint16_t)); 
	 	
	  MatrixElementType* pHadamard = NULL;
	 
  	MatrixElementType* pPrbs = NULL; 
	   /*主要是用来*/
    MatrixElementType* pPrbsTemp = NULL;  	
	  /*显示PRBS逆矩阵序列*/
	  MatrixElementType* pPrbsInverse = (MatrixElementType*)malloc(sequenceLength*sequenceLength * sizeof(MatrixElementType));
    /*验证逆矩阵序列是否正确*/
 		MatrixElementType* pValidateResult = (MatrixElementType*)malloc(sequenceLength*sequenceLength* sizeof(MatrixElementType));//验证信号序列

   	MatrixElementType* pOverlapSignal = (MatrixElementType*)malloc(sequenceLength*sequenceLength * sizeof(MatrixElementType)); //叠加信号序列
    
		MatrixElementType* pRecoverySignal = (MatrixElementType*)malloc(sequenceLength*sequenceLength * sizeof(MatrixElementType));//恢复信号序列
    
  	MatrixElementType* pAverageResult  = (MatrixElementType*)malloc(sequenceLength * sizeof(MatrixElementType));//验证信号序列
	  
		MatrixElementType* pResult = (MatrixElementType*)malloc(sequenceLength*sequenceLength*sizeof(MatrixElementType));//验证信号序列

 	  sendSequence = (u8*)malloc(sequenceLength * sizeof(u8));//哈达玛序列插零之后的序列
	 
    memset(pAverageResult,0,sizeof(MatrixElementType)*sequenceLength);
		
		memset(HT_IMS_ADC_ConvertedValue,0,sizeof(uint16_t)*(sequenceLength + 2));

   	pHadamard = hadamard(HADAMARDORDER);
 		pPrbsTemp = prbs(pHadamard,HADAMARDORDER);
		pPrbs = prbs(pHadamard,HADAMARDORDER);

    /*显示HADAMARD序列*/
	  //printf("hadamard\r\n");
	 // showHadamard(pHadamard,HADAMARDORDER);//显示新序列
		/*显示PRBS新序列*/
		 		
		// printf("pPrbs\r\n");
    // showHadamard(pPrbs,sequenceLength);
		
		MatrixInit(&matrixPrbs,sequenceLength,sequenceLength,pPrbs);
		
    MatrixInit(&matrixPrbsTemp,sequenceLength,sequenceLength,pPrbsTemp);
		
		MatrixInit(&matrixPrbsInverse,sequenceLength,sequenceLength,pPrbsInverse);

		MatrixInit(&matrixResult,sequenceLength,sequenceLength,pResult);
		
 		MatrixInit(&matrixValidateResult,sequenceLength,sequenceLength,pValidateResult);
		
		MatrixInit(&matrixRecoverySignal,sequenceLength,sequenceLength,pRecoverySignal);
		
   	MatrixInit(&matrixOverlapSignal,sequenceLength,sequenceLength,pOverlapSignal);

 		 
		MatrixInverse(&matrixPrbsTemp,&matrixPrbsInverse);
    /*显示一下矩阵的逆*/
    //showHadamard(pPrbsInverse,sequenceLength);
	//	printf("pPrbs\r\n");
   // showHadamard(pPrbs,sequenceLength);
	  productSendSequence(pPrbs,sendSequence,1,sequenceLength);
	 // showSendSequence(sendSequence,sequenceLength );
    /*开始采一次样并把采样值抛掉*/
 	  beginSample(sampleSize);
		 
    while (1){
			
		 switch(flag){
			 case 0:
        flag = 10;	
			 // showHadamard(pPrbsTemp,sequenceLength);
				productSendSequence(pPrbs,sendSequence,1,sequenceLength);
			 // showHadamard(pPrbs,sequenceLength);
			// printf("frameCount:%d\r\n",frameCount);
			 //showSendSequence(sendSequence,sequenceLength );
				//printf("sampleSize:%d\r\n",sampleSize);
				beginSample(sampleSize);
			  // Delay_ms(20);
			 	// showOriginalValue(HT_IMS_ADC_ConvertedValue,(sequenceLength + 2));
			 break;
			 /*数据接收标志位*/
			 case 1: 
				 
			 break;
			 /*adc完成标志位*/
			 
			 case 2:  
				  // printf("adc complete\r\n");
				    flag = 10;
			 		 /*framecount为2时才代表第一次采样*/						 
            frameCount++;
			    // printf("frameCount:%d\r\n",frameCount);
					 if(frameCount == 1){
						 // printf("frameCount:%d\r\n",frameCount);
					    //showOriginalValue(HT_IMS_ADC_ConvertedValue,sampleSize);

					    
					 }
			     if(frameCount > 1&&frameCount <= sequenceLength){
					//	printf("HT_IMS_ADC_ConvertedValue\r\n");
						/*打印采的AD值*/						 
					 // printf("frameCount:%d\r\n",frameCount-1);
					  //showOriginalValue(HT_IMS_ADC_ConvertedValue,sampleSize);
						/*去掉零时刻的值*/	
						 getOverlopSignal(HT_IMS_ADC_ConvertedValue,pOverlapSignal,sequenceLength,frameCount);
 						 Delay_ms(5);
  					 
						 productSendSequence(pPrbs,sendSequence,frameCount,sequenceLength);
             //printf("frameCount:%d\r\n",frameCount);
			     //  showSendSequence(sendSequence,sequenceLength );
						 beginSample(sampleSize);
					 }
 				   if(frameCount == sequenceLength+1){
						 /*打印采的AD值*/	
					   //printf("frameCount:%d\r\n",frameCount-1);
					   //showOriginalValue(HT_IMS_ADC_ConvertedValue,sampleSize);
					   /*去掉零时刻的值*/	
  			     getOverlopSignal(HT_IMS_ADC_ConvertedValue,pOverlapSignal,sequenceLength,frameCount); 
						 /*打印所有去掉零时刻的值，为n*n矩阵*/
             //printf("pOverlapSignal\r\n");							 
					   //showRecoverySignal(pOverlapSignal,sequenceLength);
						 /*采样结束了*/	
						  MatrixMultiply(&matrixPrbsInverse,&matrixOverlapSignal,&matrixRecoverySignal);  
						 /*打印采样值n*n矩阵*/
						 //printf("RecoverySignal\r\n");	
						 //showRecoverySignal(pRecoverySignal,sequenceLength);
						 /*对恢复的信号矩阵乘以scale*/
						 //MatrixScale(&matrixRecoverySignal,scale, &matrixResult); 
						 /*打印缩放后的n*n矩阵*/
						 //printf("scalepResultSignal\r\n");	
						 //showRecoverySignal(pResult,sequenceLength);	
						 /*发送缩放后的n*n矩阵的第二行，发送的行可以在函数里设置*/						 
						 //sendOverlap(pResult,sequenceLength);	
						 /*循环移位后取平均*/	
            						 
						 getAverageSignal(pRecoverySignal,pAverageResult,sequenceLength,averageNumber); 
 						 /*打印平均数*/
					   showResultSignal(pAverageResult,sequenceLength);
						 frameCount = 1;
						 flag = 0;
           }
					 
					 break;
				}
 	
   }
}

 

 
void  beginSample(int number){
	   //printf("1.sequenceCount:%d,adcSequenceCount:%d\r\n",sequenceCount,adcSequenceCount);

     sequenceCount = 0;
	   adcSequenceCount = 0;
		if(frameCount == 0){	
			 ADCX_Init(number);

			 TIM1_GPIO_Config(sequencePeriod,SENIOR_PSC,sequencePeriod+1);//序列时钟初始化
			
  		 TIM8_ADC_Config(AdcPeriod,SENIOR_PSC,AdcLowTime);//adc时钟初始化
		}
		else{
			
			 TIM_ClearFlag(TIM_ADC1,TIM_FLAG_Update);
	     TIM_ClearITPendingBit(TIM_ADC1, TIM_IT_Update);    //清除 TIM2 更新中断标志
			 TIM_ClearFlag(SENIOR_TIM,TIM_FLAG_Update);
       TIM_ClearITPendingBit(SENIOR_TIM, TIM_IT_Update);  //清除 TIM2 更新中断标志
       
			 RHEOSTAT_ADC_DMA_STREAM->NDTR = number;            //重新装填DMA需要传输的数据个数
       DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);          //先使能DMA通道
       RHEOSTAT_ADC->SR &=~(0X01<<5);                     //将由硬件置1的ADC状态寄存器的溢出标志位清除，不然容易出错
       ADC_DMACmd(RHEOSTAT_ADC, ENABLE);  
			
			 TIM_ITConfig(TIM_ADC1,TIM_IT_Update,ENABLE);    
			 TIM_ITConfig(SENIOR_TIM,TIM_IT_Update,ENABLE); 

		
		}
	  // printf("2.sequenceCount:%d,adcSequenceCount:%d\r\n",sequenceCount,adcSequenceCount);
    
 		 TIM_Cmd(SENIOR_TIM, ENABLE);//开始采集
}
	 
 
void showHadamard(MatrixElementType *pHadamard,int hadamardOrder){
   for(int i = 0;i < hadamardOrder*hadamardOrder;i++){
	   printf("%.0f ",pHadamard[i]);
		 if(i%hadamardOrder == hadamardOrder-1){
			     printf("\r\n");
			  }
	 }
 }
 

 
void getOverlopSignal(uint16_t*HT_IMS_ADC_ConvertedValue,MatrixElementType *pOverlapSignal,int hadamardOrder,int framecount){
 	 int start = hadamardOrder*(framecount-2);
	 for(int i = 0;i < hadamardOrder;i++){
	    pOverlapSignal[i+start] = (MatrixElementType)HT_IMS_ADC_ConvertedValue[i+1];
	 }
}
void showRecoverySignal(MatrixElementType* array, int hadamardOrder){
     int length = hadamardOrder*hadamardOrder; 
     for(int i = 0;i < length;i++){
       printf("%.2f  ", array[i]); 
			 if(i%hadamardOrder == hadamardOrder-1){
			     printf("\r\n");
			 }
		}	 
}
void showResultSignal(MatrixElementType* array, int hadamardOrder){
     int length = hadamardOrder; 
     for(int i = 0;i < length;i++){
    // printf("%d,%.0f\r\n",i, array[i]); 
			 printf("%.2f\r\n",array[i]); 
		}	 
}
 void showOriginalValue(uint16_t*HT_IMS_ADC_ConvertedValue, int hadamardOrder){
	 int length = hadamardOrder*(numberOfZero+1);
	 for(int i = 1;i < length;i++){
       printf("%d,%d\r\n",i,HT_IMS_ADC_ConvertedValue[i]); 		
 		 }	
}	 

void arraycircShift(MatrixElementType* array,int length,int shiftNumber){
	
	 int i, j, t;
    
   for (i = 0; i < shiftNumber; i++)
    {
      t = array[0];
      for (j = 0; j < length; j++)
      array[j] = array[j+1];        //前一项覆盖后一项
      array[length - 1] = t;
 
    }
}
void getAverageSignal(MatrixElementType*pResult,MatrixElementType* pAverageResult ,int hadamardOrder,int averageNumber){
     for(int i = 0;i < averageNumber;i++){
		    arraycircShift(pResult+hadamardOrder*i,hadamardOrder,i);
     }
		// printf("移位后Signal\r\n");	
		 //showRecoverySignal(pResult,hadamardOrder);	

		 for(int i = 0;i < hadamardOrder;i++){   
		  for(int j = 0;j < averageNumber;j++){
				pAverageResult[i] = pAverageResult[i]+pResult[i+hadamardOrder*j];
			  
			}
   }
	   for(int i = 0;i < hadamardOrder;i++){
			 pAverageResult[i] = pAverageResult[i]/averageNumber;
	 }

}

 void sendOverlap(MatrixElementType*pResult,int hadamardOrder,int row){
    for(int i = 0;i<hadamardOrder;i++){
       printf("%.2f\r\n",pResult[(row-1)*hadamardOrder+i]);
			// printf("%d,%.2f\r\n",i,pResult[hadamardOrder+i]);
		}
 }
 void productSendSequence(MatrixElementType* pPrbs,u8* sendSuenquence,int frameCount,int sequenceLength)
 {
	  int start = (frameCount-1)*sequenceLength;
//	   	 printf("pPrbs\r\n");
//     showHadamard(pPrbs,sequenceLength);
 		for(int i = 0;i < sequenceLength;i++){
			if(pPrbs[i+start]>0.5){
		     sendSuenquence[i] = 1;
			}
			else{
			   sendSuenquence[i] = 0;
			}
 
		}
 
 
 }
 
 void showSendSequence(u8* sendSuenquence,int length){
   for(int i = 0;i < length;i++){
	 		 	printf("%d ",sendSuenquence[i]);
   }  
 
 
 }