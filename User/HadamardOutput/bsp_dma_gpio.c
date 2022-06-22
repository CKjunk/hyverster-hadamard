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
	  /*选择要控制的GPIO引脚*/															   
		 GPIO_InitStructure.GPIO_Pin = HADAMARD_GPIO_PIN;	
		/*设置引脚模式为输出模式*/
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    
    /*设置引脚的输出类型为推挽输出*/
     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    
    /*设置引脚为上拉模式*/
     GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

		/*设置引脚速率为2MHz */   
		 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 

		/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
		 GPIO_Init(HADAMARD_GPIO_PORT, &GPIO_InitStructure);	
	   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

 		 DMA_DeInit(HADAMARD_GPIO_DMA_STREAM);
		 
		 while (DMA_GetCmdStatus(HADAMARD_GPIO_DMA_STREAM) != DISABLE){}                                     // 等待DMA可配置

		 DMA_InitStructure.DMA_Channel = HADAMARD_GPIO_DMA_CHANNEL;                                              // 通道选择
		 DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(HADAMARD_GPIO_PORT->ODR)); // DMA外设地址(SOURCE ADDR)
		 DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(pten->address);                      // DMA 存储器0地址
		 DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                             // 内存到外设
		 DMA_InitStructure.DMA_BufferSize = HADAMARD_BUFFSIZE;                                                                   // 数据传输量
		 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                   // 外设增量模式
		 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                          // 存储器非增量模式
		 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;  // 外设数据长度:16位
		 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;        // 存储器数据长度:16位
		 DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
		 DMA_InitStructure.DMA_Priority = DMA_Priority_High;                                          // 优先级
		 DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;

 		 DMA_Init(HADAMARD_GPIO_DMA_STREAM, &DMA_InitStructure);
			 
			 
		NVIC_InitStructure.NVIC_IRQChannel = HADAMARD_GPIO_INT_CHANNEL;  //TIM2中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
		NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	
 
    DMA_ITConfig(HADAMARD_GPIO_DMA_STREAM,DMA_IT_TC, ENABLE);		//使能传输完成中断
    DMA_ClearFlag(HADAMARD_GPIO_DMA_STREAM,DMA_FLAG_TCIF1); /* 清除相关状态标识 */ 
 	   // 使能DMA流
    DMA_Cmd(HADAMARD_GPIO_DMA_STREAM, ENABLE);
}