#include "./usart_dma/usart_dma.h"
#include "./usart/bsp_debug_usart.h"
#include "stdlib.h"
volatile char Usart_Rx_Buf[USART_BUFF_SIZE];  //����������λ������������ַ���

 void USART_DMA_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  /*����DMAʱ��*/
  RCC_AHB1PeriphClockCmd(DEBUG_USART_DMA_CLK, ENABLE);
  
  /* ��λ��ʼ��DMA������ */
  DMA_DeInit(DEBUG_USART_DMA_STREAM);

  /* ȷ��DMA��������λ��� */
  while (DMA_GetCmdStatus(DEBUG_USART_DMA_STREAM) != DISABLE)  {
  }

  /*usart1 rx��Ӧdma2��ͨ��4��������2*/	
  DMA_InitStructure.DMA_Channel = DEBUG_USART_DMA_CHANNEL;  
  /*����DMAԴ���������ݼĴ�����ַ*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = DEBUG_USART_DR_BASE;	 
  /*�ڴ��ַ(Ҫ����ı�����ָ��)*/
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)Usart_Rx_Buf;
  /*���򣺴��ڴ浽����*/		
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
  /*�����СDMA_BufferSize=RECEIVEBUFF_SIZE*/	
  DMA_InitStructure.DMA_BufferSize = USART_BUFF_SIZE;
  /*�����ַ����*/	    
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
  /*�ڴ��ַ����*/
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
  /*�������ݵ�λ*/	
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /*�ڴ����ݵ�λ 8bit*/
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	
  /*DMAģʽ������ѭ��*/
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 
  /*���ȼ�����*/	
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;      
  /*����FIFO*/
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
  /*�洢��ͻ������ 16������*/
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
  /*����ͻ������ 1������*/
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    
  /*����DMA2��������2*/		   
  DMA_Init(DEBUG_USART_DMA_STREAM, &DMA_InitStructure);
  
  /*ʹ��DMA*/
  DMA_Cmd(DEBUG_USART_DMA_STREAM, ENABLE);
  USART_DMACmd(DEBUG_USART, USART_DMAReq_Rx, ENABLE);

  /* �ȴ�DMA��������Ч*/
  while(DMA_GetCmdStatus(DEBUG_USART_DMA_STREAM) != ENABLE)
  {
  }   
	 
  
  
}
 