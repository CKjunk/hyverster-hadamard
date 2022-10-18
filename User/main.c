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

int AdcPeriod = 200-1;  //������ϵ��Ϊ1��ʱ���adc��ʱ������
int AdcLowTime = 100;    //��ʱ��һ�������ڵ͵�ƽ����ʱ��
int sequencePeriod = 200 - 1;  //������ϵ��Ϊ1��ʱ���adc��ʱ������
int sequenceLowTime = 200;      //��ʱ��һ�������ڵ͵�ƽ����ʱ��
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

int overSampling = 1;  //������ϵ��
 

  
int frameCount = 0;
int flag = 10;  //�ܵ�flag
int sequenceCount = 0;  	 //���͵����м���
int initialNumber = 0;
int adcSequenceCount = 0;
int errorSample = 3;
int sampleSize = 0;
int main(void)
{		
		SysTick_Init();
 	  EXTI_Key_Config(); //������ʼ��
	  Debug_USART_Config();//���ڳ�
 	  float32_t scale = 1.0/HADAMARDORDER;
    sampleSize = sequenceLength+1;
    volatile int frame_count = 0;  //֡������
	 	
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
	   /*��Ҫ������*/
    MatrixElementType* pPrbsTemp = NULL;  	
	  /*��ʾPRBS���������*/
	  MatrixElementType* pPrbsInverse = (MatrixElementType*)malloc(sequenceLength*sequenceLength * sizeof(MatrixElementType));
    /*��֤����������Ƿ���ȷ*/
 		MatrixElementType* pValidateResult = (MatrixElementType*)malloc(sequenceLength*sequenceLength* sizeof(MatrixElementType));//��֤�ź�����

   	MatrixElementType* pOverlapSignal = (MatrixElementType*)malloc(sequenceLength*sequenceLength * sizeof(MatrixElementType)); //�����ź�����
    
		MatrixElementType* pRecoverySignal = (MatrixElementType*)malloc(sequenceLength*sequenceLength * sizeof(MatrixElementType));//�ָ��ź�����
    
  	MatrixElementType* pAverageResult  = (MatrixElementType*)malloc(sequenceLength * sizeof(MatrixElementType));//��֤�ź�����
	  
		MatrixElementType* pResult = (MatrixElementType*)malloc(sequenceLength*sequenceLength*sizeof(MatrixElementType));//��֤�ź�����

 	  sendSequence = (u8*)malloc(sequenceLength * sizeof(u8));//���������в���֮�������
	 
    memset(pAverageResult,0,sizeof(MatrixElementType)*sequenceLength);
		
		memset(HT_IMS_ADC_ConvertedValue,0,sizeof(uint16_t)*(sequenceLength + 2));

   	pHadamard = hadamard(HADAMARDORDER);
 		pPrbsTemp = prbs(pHadamard,HADAMARDORDER);
		pPrbs = prbs(pHadamard,HADAMARDORDER);

    /*��ʾHADAMARD����*/
	  //printf("hadamard\r\n");
	 // showHadamard(pHadamard,HADAMARDORDER);//��ʾ������
		/*��ʾPRBS������*/
		 		
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
    /*��ʾһ�¾������*/
    //showHadamard(pPrbsInverse,sequenceLength);
	//	printf("pPrbs\r\n");
   // showHadamard(pPrbs,sequenceLength);
	  productSendSequence(pPrbs,sendSequence,1,sequenceLength);
	 // showSendSequence(sendSequence,sequenceLength );
    /*��ʼ��һ�������Ѳ���ֵ�׵�*/
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
			 /*���ݽ��ձ�־λ*/
			 case 1: 
				 
			 break;
			 /*adc��ɱ�־λ*/
			 
			 case 2:  
				  // printf("adc complete\r\n");
				    flag = 10;
			 		 /*framecountΪ2ʱ�Ŵ����һ�β���*/						 
            frameCount++;
			    // printf("frameCount:%d\r\n",frameCount);
					 if(frameCount == 1){
						 // printf("frameCount:%d\r\n",frameCount);
					    //showOriginalValue(HT_IMS_ADC_ConvertedValue,sampleSize);

					    
					 }
			     if(frameCount > 1&&frameCount <= sequenceLength){
					//	printf("HT_IMS_ADC_ConvertedValue\r\n");
						/*��ӡ�ɵ�ADֵ*/						 
					 // printf("frameCount:%d\r\n",frameCount-1);
					  //showOriginalValue(HT_IMS_ADC_ConvertedValue,sampleSize);
						/*ȥ����ʱ�̵�ֵ*/	
						 getOverlopSignal(HT_IMS_ADC_ConvertedValue,pOverlapSignal,sequenceLength,frameCount);
 						 Delay_ms(5);
  					 
						 productSendSequence(pPrbs,sendSequence,frameCount,sequenceLength);
             //printf("frameCount:%d\r\n",frameCount);
			     //  showSendSequence(sendSequence,sequenceLength );
						 beginSample(sampleSize);
					 }
 				   if(frameCount == sequenceLength+1){
						 /*��ӡ�ɵ�ADֵ*/	
					   //printf("frameCount:%d\r\n",frameCount-1);
					   //showOriginalValue(HT_IMS_ADC_ConvertedValue,sampleSize);
					   /*ȥ����ʱ�̵�ֵ*/	
  			     getOverlopSignal(HT_IMS_ADC_ConvertedValue,pOverlapSignal,sequenceLength,frameCount); 
						 /*��ӡ����ȥ����ʱ�̵�ֵ��Ϊn*n����*/
             //printf("pOverlapSignal\r\n");							 
					   //showRecoverySignal(pOverlapSignal,sequenceLength);
						 /*����������*/	
						  MatrixMultiply(&matrixPrbsInverse,&matrixOverlapSignal,&matrixRecoverySignal);  
						 /*��ӡ����ֵn*n����*/
						 //printf("RecoverySignal\r\n");	
						 //showRecoverySignal(pRecoverySignal,sequenceLength);
						 /*�Իָ����źž������scale*/
						 //MatrixScale(&matrixRecoverySignal,scale, &matrixResult); 
						 /*��ӡ���ź��n*n����*/
						 //printf("scalepResultSignal\r\n");	
						 //showRecoverySignal(pResult,sequenceLength);	
						 /*�������ź��n*n����ĵڶ��У����͵��п����ں���������*/						 
						 //sendOverlap(pResult,sequenceLength);	
						 /*ѭ����λ��ȡƽ��*/	
            						 
						 getAverageSignal(pRecoverySignal,pAverageResult,sequenceLength,averageNumber); 
 						 /*��ӡƽ����*/
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

			 TIM1_GPIO_Config(sequencePeriod,SENIOR_PSC,sequencePeriod+1);//����ʱ�ӳ�ʼ��
			
  		 TIM8_ADC_Config(AdcPeriod,SENIOR_PSC,AdcLowTime);//adcʱ�ӳ�ʼ��
		}
		else{
			
			 TIM_ClearFlag(TIM_ADC1,TIM_FLAG_Update);
	     TIM_ClearITPendingBit(TIM_ADC1, TIM_IT_Update);    //��� TIM2 �����жϱ�־
			 TIM_ClearFlag(SENIOR_TIM,TIM_FLAG_Update);
       TIM_ClearITPendingBit(SENIOR_TIM, TIM_IT_Update);  //��� TIM2 �����жϱ�־
       
			 RHEOSTAT_ADC_DMA_STREAM->NDTR = number;            //����װ��DMA��Ҫ��������ݸ���
       DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);          //��ʹ��DMAͨ��
       RHEOSTAT_ADC->SR &=~(0X01<<5);                     //����Ӳ����1��ADC״̬�Ĵ����������־λ�������Ȼ���׳���
       ADC_DMACmd(RHEOSTAT_ADC, ENABLE);  
			
			 TIM_ITConfig(TIM_ADC1,TIM_IT_Update,ENABLE);    
			 TIM_ITConfig(SENIOR_TIM,TIM_IT_Update,ENABLE); 

		
		}
	  // printf("2.sequenceCount:%d,adcSequenceCount:%d\r\n",sequenceCount,adcSequenceCount);
    
 		 TIM_Cmd(SENIOR_TIM, ENABLE);//��ʼ�ɼ�
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
      array[j] = array[j+1];        //ǰһ��Ǻ�һ��
      array[length - 1] = t;
 
    }
}
void getAverageSignal(MatrixElementType*pResult,MatrixElementType* pAverageResult ,int hadamardOrder,int averageNumber){
     for(int i = 0;i < averageNumber;i++){
		    arraycircShift(pResult+hadamardOrder*i,hadamardOrder,i);
     }
		// printf("��λ��Signal\r\n");	
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