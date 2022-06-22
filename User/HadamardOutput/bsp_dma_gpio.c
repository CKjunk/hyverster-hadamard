#include "./HadamardOutput/bsp_dma_gpio.h"
#include "./sequence/sequence.h"
extern TEN ten ;
extern TEN* pten ;
//u8 array[15]={1 ,1 ,0 ,0 ,0 ,1, 0 ,0, 1 ,1 ,0 ,1 ,0, 1 ,1 };
 void HadamardDmaGPIO(void){
		 NVIC_InitTypeDef NVIC_InitStructure;

	   GPIO_InitTypeDef GPIO_InitStructure;
	   
     DMA_InitTypeDef DMA_InitStructure;
     RCC_AHB1PeriphClockCmd(HADAMARD_GPIO_CLK,ENABLE);
	  /*ѡ��Ҫ���Ƶ�GPIO����*/															   
		 GPIO_InitStructure.GPIO_Pin = HADAMARD_GPIO_PIN;	
		/*��������ģʽΪ���ģʽ*/
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    
    /*�������ŵ��������Ϊ�������*/
     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    
    /*��������Ϊ����ģʽ*/
     GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

		/*������������Ϊ2MHz */   
		 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 

		/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
		 GPIO_Init(HADAMARD_GPIO_PORT, &GPIO_InitStructure);	
	   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

 		 DMA_DeInit(HADAMARD_GPIO_DMA_STREAM);
		 
		 while (DMA_GetCmdStatus(HADAMARD_GPIO_DMA_STREAM) != DISABLE){}                                     // �ȴ�DMA������

		 DMA_InitStructure.DMA_Channel = HADAMARD_GPIO_DMA_CHANNEL;                                              // ͨ��ѡ��
		 DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(HADAMARD_GPIO_PORT->ODR)); // DMA�����ַ(SOURCE ADDR)
		 DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(pten->address);                      // DMA �洢��0��ַ
		 DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                             // �ڴ浽����
		 DMA_InitStructure.DMA_BufferSize = HADAMARD_BUFFSIZE;                                                                   // ���ݴ�����
		 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                   // ��������ģʽ
		 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                          // �洢��������ģʽ
		 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;  // �������ݳ���:16λ
		 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;        // �洢�����ݳ���:16λ
		 DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
		 DMA_InitStructure.DMA_Priority = DMA_Priority_High;                                          // ���ȼ�
		 DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;

 		 DMA_Init(HADAMARD_GPIO_DMA_STREAM, &DMA_InitStructure);
			 
			 
		NVIC_InitStructure.NVIC_IRQChannel = HADAMARD_GPIO_INT_CHANNEL;  //TIM2�ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
		NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	
 
    DMA_ITConfig(HADAMARD_GPIO_DMA_STREAM,DMA_IT_TC, ENABLE);		//ʹ�ܴ�������ж�
    DMA_ClearFlag(HADAMARD_GPIO_DMA_STREAM,DMA_FLAG_TCIF1); /* ������״̬��ʶ */ 
 	   // ʹ��DMA��
    DMA_Cmd(HADAMARD_GPIO_DMA_STREAM, ENABLE);
}