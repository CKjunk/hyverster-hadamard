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
#define HADAMARDORDER 256

int numberOfZero = 0;
 

int overSample = 8;
int AdcPeriod = 100-1;  //������ϵ��Ϊ1��ʱ���adc��ʱ������
int AdcLowTime = 80;    //��ʱ��һ�������ڵ͵�ƽ����ʱ��
int sequencePeriod = 100 - 1;  //������ϵ��Ϊ1��ʱ���adc��ʱ������
int errFrameFlag = 0;
void showSendSequence(u8* sendSuenquence,int length);
void beginSample(int number);
void showHadamard(MatrixElementType *pHadamard,int hadamardOrder);
void DEBUG_USART_2_Send_Array(uint16_t send_array[],int number);
void sendOverlap(MatrixElementType*pOverlapSignal,int hadamardOrder,int row);
void getOverlopSignal(uint16_t*HT_IMS_ADC_ConvertedValue,MatrixElementType *pOverlapSignal,int hadamardOrder,int framecount);
void showOriginalValue(uint16_t*HT_IMS_ADC_ConvertedValue, int hadamardOrder);
void getAverageSignal(MatrixElementType*pRecoverySignal,MatrixElementType* pAverageResult ,int hadamardOrder,int averageNumber,int totalFrame);
void showRecoverySignal(MatrixElementType* array, int hadamardOrder);
void arraycircShift(MatrixElementType* array,int length,int shiftNumber);
void showResultSignal(MatrixElementType* array, int hadamardOrder);
void productSendSequence(MatrixElementType* pPrbs,u8* sendSuenquence,int frameCount,int sequenceLength);


int sequenceLength = HADAMARDORDER-1;

extern uint16_t* HT_IMS_ADC_ConvertedValue;

u8* sendSequence = NULL;

int overSampling = 1;  //������ϵ��
 
int discardFrameNumber[100] = {0,1,2,3,4,5,6,11,12,13,14,16,24,28,29,30,40,42,58,59,60,61,62,63};
//ȡǰ70��ͼ
int totalFrame = 70;
int averageNumber = 46;
int errFrameNumber = 24;
  
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
	 /* MatrixElementType* pPrbsInverse = (MatrixElementType*)malloc(sequenceLength*sequenceLength * sizeof(MatrixElementType));
 
		if(pPrbsInverse == NULL){
			printf("pPrbsInverse allocate failure\r\n");
		}
		*/
 	/*	MatrixElementType* pValidateResult = (MatrixElementType*)malloc(sequenceLength*sequenceLength* sizeof(MatrixElementType));//��֤�ź�����
    if(pValidateResult == NULL){
			printf("pValidateResult allocate failure\r\n");
		}
		*/
   	MatrixElementType* pOverlapSignal = (MatrixElementType*)malloc(sequenceLength*sequenceLength * sizeof(MatrixElementType)); //�����ź�����
    if(pOverlapSignal == NULL){
			printf("pOverlapSignal allocate failure\r\n");
		}
		MatrixElementType* pRecoverySignal = (MatrixElementType*)malloc(sequenceLength*sequenceLength * sizeof(MatrixElementType));//�ָ��ź�����
    if(pRecoverySignal == NULL){
			printf("pRecoverySignal allocate failure\r\n");
		}
  	MatrixElementType* pAverageResult  = (MatrixElementType*)malloc(sequenceLength * sizeof(MatrixElementType));//��֤�ź�����
	  if(pAverageResult == NULL){
			printf("pAverageResult allocate failure\r\n");
		}
	 /*	MatrixElementType* pResult = (MatrixElementType*)malloc(sequenceLength*sequenceLength*sizeof(MatrixElementType));//��֤�ź�����
    if(pResult == NULL){
			printf("pResult allocate failure\r\n");
		}
		*/
 	  sendSequence = (u8*)malloc(sequenceLength * sizeof(u8));//���������в���֮�������
	  
    memset(pAverageResult,0,sizeof(MatrixElementType)*sequenceLength);
		
		memset(HT_IMS_ADC_ConvertedValue,0,sizeof(uint16_t)*(sequenceLength + 2));

   	pHadamard = hadamard(HADAMARDORDER);
 		pPrbsTemp = prbs(pHadamard,HADAMARDORDER);
		pPrbs = prbs(pHadamard,HADAMARDORDER);

    /*��ʾHADAMARD����
	  printf("hadamard\r\n");
	  showHadamard(pHadamard,HADAMARDORDER);//��ʾ������
		*/
		/*��ʾPRBS������
		 		
		printf("pPrbsTemp\r\n");
    showHadamard(pPrbsTemp,sequenceLength);
		printf("===========\r\n");
		*/
		MatrixInit(&matrixPrbs,sequenceLength,sequenceLength,pPrbs);
		
    MatrixInit(&matrixPrbsTemp,sequenceLength,sequenceLength,pPrbsTemp);
		/*
		MatrixInit(&matrixPrbsInverse,sequenceLength,sequenceLength,pPrbsInverse);

		MatrixInit(&matrixResult,sequenceLength,sequenceLength,pResult);
		
 		MatrixInit(&matrixValidateResult,sequenceLength,sequenceLength,pValidateResult);
		*/
		MatrixInit(&matrixRecoverySignal,sequenceLength,sequenceLength,pRecoverySignal);
		
   	MatrixInit(&matrixOverlapSignal,sequenceLength,sequenceLength,pOverlapSignal);
 		createInverseMatrix(pPrbsTemp,sequenceLength);

		//MatrixInverse(&matrixPrbsTemp,&matrixPrbsInverse);
    /*��ʾһ�¾������
		  printf("pPrbsinverse\r\n");
      showHadamard(pPrbsTemp,sequenceLength);*/
		//MatrixMultiply(&matrixPrbsInverse,&matrixPrbs,&matrixValidateResult);  
    //showRecoverySignal(pValidateResult,sequenceLength);
		
		/*
	  printf("pPrbs\r\n");
    showHadamard(pPrbs,sequenceLength);
	  productSendSequence(pPrbs,sendSequence,1,sequenceLength);
	  showSendSequence(sendSequence,sequenceLength );
		*/
    /*��ʼ��һ�������Ѳ���ֵ�׵�*/
   // printf("can begin\r\n");
 	  beginSample(sampleSize);
		 
    while (1){
			
		 switch(flag){
			 case 0:
        flag = 10;	
			 // showHadamard(pPrbsTemp,sequenceLength);
				productSendSequence(pPrbs,sendSequence,1,sequenceLength);
			 // showHadamard(pPrbs,sequenceLength);
			 //printf("frameCount:%d\r\n",frameCount);
			  //showSendSequence(sendSequence,sequenceLength );
			  //printf("\r\n");
				//printf("sampleSize:%d\r\n",sampleSize);
				 beginSample(sampleSize);
			   
			 	// showOriginalValue(HT_IMS_ADC_ConvertedValue,(sequenceLength + 2));
			 break;
			 /*���ݽ��ձ�־λ*/
			 case 1: 
				 
			 break;
			 /*adc��ɱ�־λ*/
			 
			 case 2:  
				    //printf("adc complete\r\n");
				    flag = 10;
			 		 /*framecountΪ2ʱ�Ŵ����һ�β���*/						 
            frameCount++;
			    // printf("frameCount:%d\r\n",frameCount);
					 if(frameCount == 1){
						 // printf("frameCount:%d\r\n",frameCount);
					    //showOriginalValue(HT_IMS_ADC_ConvertedValue,sampleSize);
              flag = 0  ; 
						 Delay_ms(20);
					    
					 }
			     if(frameCount > 1&&frameCount <= sequenceLength){
					 //	printf("HT_IMS_ADC_ConvertedValue\r\n");
						/*��ӡ�ɵ�ADֵ*/						 
					 // printf("frameCount:%d\r\n",frameCount-1);
					 // showOriginalValue(HT_IMS_ADC_ConvertedValue,sampleSize);
						/*ȥ����ʱ�̵�ֵ*/	
						 getOverlopSignal(HT_IMS_ADC_ConvertedValue,pOverlapSignal,sequenceLength,frameCount);
 						 Delay_ms(15);
  					 
						 productSendSequence(pPrbs,sendSequence,frameCount,sequenceLength);
             //printf("frameCount:%d\r\n",frameCount);
			       //showSendSequence(sendSequence,sequenceLength );
						 //printf("\r\n");
						 beginSample(sampleSize);
					 }
 				   if(frameCount == sequenceLength+1){
						 /*��ӡ�ɵ�ADֵ*/	
					   //printf("frameCount:%d\r\n",frameCount-1);
					   //showOriginalValue(HT_IMS_ADC_ConvertedValue,sampleSize);
					   /*ȥ����ʱ�̵�ֵ*/	
  			     getOverlopSignal(HT_IMS_ADC_ConvertedValue,pOverlapSignal,sequenceLength,frameCount); 
						 showRecoverySignal(pOverlapSignal,sequenceLength);
						 Delay_ms(15000);
						 
						 /*��ӡ����ȥ����ʱ�̵�ֵ��Ϊn*n����*/
             //printf("pOverlapSignal\r\n");							 
					   //showRecoverySignal(pOverlapSignal,sequenceLength);
						 /*����������*/	
						 //printf("pPrbsInverse\r\n");
             //showHadamard(matrixPrbsInverse.pData,sequenceLength);	
						 transposingMatrix(pOverlapSignal,sequenceLength,sequenceLength);
						// MatrixMultiply(&matrixOverlapSignal,&matrixPrbsInverse,&matrixRecoverySignal);  
					   pRecoverySignal = calculate(sequenceLength,sequenceLength,pOverlapSignal,sequenceLength,sequenceLength, pPrbsTemp) ;
              
						 transposingMatrix(pRecoverySignal,sequenceLength,sequenceLength);
						 /*��ӡ����ֵn*n����*/
						// printf("RecoverySignal\r\n");	
				     showRecoverySignal(pRecoverySignal,sequenceLength);
						
				
						 Delay_ms(15000);
								 
						 getAverageSignal(pRecoverySignal,pAverageResult,sequenceLength,averageNumber,totalFrame); 
 						 /*��ӡƽ����*/
					   showResultSignal(pAverageResult,sequenceLength);
						 frameCount = 1;
						// flag = 0;
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
	   printf("%.7f ",pHadamard[i]);
		 if(i%hadamardOrder == hadamardOrder-1){
			     printf("\r\n");
			  }
	 }
 }
 

 
void getOverlopSignal(uint16_t* HT_IMS_ADC_ConvertedValue,MatrixElementType* pOverlapSignal,int hadamardOrder,int framecount){
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
      MatrixElementType t = array[0];
      for (j = 0; j < length; j++)
      array[j] = array[j+1];        //ǰһ��Ǻ�һ��
      array[length - 1] = 0;/*����ѭ��֮��ĩβԪ������*/
 
    }
}
void getAverageSignal(MatrixElementType*pRecoverySignal,MatrixElementType* pAverageResult ,int hadamardOrder,int averageNumber,int totalFrame){
     int start = 30;
	   for(int i = 0;i < totalFrame;i++){
		    arraycircShift(pRecoverySignal+hadamardOrder*i,hadamardOrder,i);
     }
/**/
		// printf("��λ��Signal\r\n");	
		// showRecoverySignal(pRecoverySignal,hadamardOrder);	

		 
		 
		 for(int i = 0;i < totalFrame;i++){  /*i����֡��*/
			    errFrameFlag = 0;
					for(int k = 0;k < errFrameNumber;k++){
								if(i == discardFrameNumber[k]){
										 errFrameFlag = 1;
									    break;
								}
					}
					
					if(errFrameFlag){
					    continue;
					}
			    //printf("ȡ��%d֡ ",i);
		      for(int j = 0;j < hadamardOrder;j++){ /*ÿһ֡����*/
        	   pAverageResult[j] = pAverageResult[j]+pRecoverySignal[i*hadamardOrder+j];
			   }
   }
	   for(int i = 0;i < hadamardOrder;i++){
			 pAverageResult[i] = pAverageResult[i]/(averageNumber);//��������Ҫ��ϸ����
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