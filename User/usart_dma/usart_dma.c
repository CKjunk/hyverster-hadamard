#include "./usart_dma/usart_dma.h"
#include "./usart/bsp_debug_usart.h"
#include "stdlib.h"
volatile char Usart_Rx_Buf[USART_BUFF_SIZE];  //用作接收上位机传输的命令字符串

 void USART_DMA_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  /*开启DMA时钟*/
  RCC_AHB1PeriphClockCmd(DEBUG_USART_DMA_CLK, ENABLE);
  
  /* 复位初始化DMA数据流 */
  DMA_DeInit(DEBUG_USART_DMA_STREAM);

  /* 确保DMA数据流复位完成 */
  while (DMA_GetCmdStatus(DEBUG_USART_DMA_STREAM) != DISABLE)  {
  }

  /*usart1 rx对应dma2，通道4，数据流2*/	
  DMA_InitStructure.DMA_Channel = DEBUG_USART_DMA_CHANNEL;  
  /*设置DMA源：串口数据寄存器地址*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = DEBUG_USART_DR_BASE;	 
  /*内存地址(要传输的变量的指针)*/
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)Usart_Rx_Buf;
  /*方向：从内存到外设*/		
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
  /*传输大小DMA_BufferSize=RECEIVEBUFF_SIZE*/	
  DMA_InitStructure.DMA_BufferSize = USART_BUFF_SIZE;
  /*外设地址不增*/	    
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
  /*内存地址自增*/
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
  /*外设数据单位*/	
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /*内存数据单位 8bit*/
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	
  /*DMA模式：不断循环*/
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 
  /*优先级：中*/	
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;      
  /*禁用FIFO*/
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
  /*存储器突发传输 16个节拍*/
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
  /*外设突发传输 1个节拍*/
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    
  /*配置DMA2的数据流2*/		   
  DMA_Init(DEBUG_USART_DMA_STREAM, &DMA_InitStructure);
  
  /*使能DMA*/
  DMA_Cmd(DEBUG_USART_DMA_STREAM, ENABLE);
  USART_DMACmd(DEBUG_USART, USART_DMAReq_Rx, ENABLE);

  /* 等待DMA数据流有效*/
  while(DMA_GetCmdStatus(DEBUG_USART_DMA_STREAM) != ENABLE)
  {
  }   
	 
  
  
}
 