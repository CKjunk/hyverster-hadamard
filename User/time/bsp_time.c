#include "./time/bsp_time.h"
#include "./usart/bsp_debug_usart.h"
#include "./HadamardOutput/bsp_dma_gpio.h"
void TIM1_GPIO_Config(u16 period,u16 psc,u16 adc_pwm_lowtime){

	GPIO_InitTypeDef GPIO_InitStructure;
	/*开启相关的GPIO外设时钟*/
	RCC_AHB1PeriphClockCmd (HADAMARD_GPIO_CLK,ENABLE); 
	RCC_AHB1PeriphClockCmd (HADAMARD_GPIO_CLK_ADDING,ENABLE); 

  /* 定时器通道引脚复用 */
	GPIO_PinAFConfig(HADAMARD_GPIO_PORT,HADAMARD_PWM_PINSOURCE,HADAMARD_GPIO_PIN_PWM_AF); 
	GPIO_PinAFConfig(HADAMARD_GPIO_PORT_ADDING,HADAMARD_PWM_PINSOURCE_ADDING,HADAMARD_GPIO_PIN_PWM_AF); 

	/* 定时器通道引脚配置 */															   
	GPIO_InitStructure.GPIO_Pin = HADAMARD_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
	GPIO_Init(HADAMARD_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = HADAMARD_GPIO_PIN_ADDING;	
	GPIO_Init(HADAMARD_GPIO_PORT_ADDING, &GPIO_InitStructure);

 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	// 开启TIMx_CLK,x[2,3,4,5,12,13,14] 
  RCC_APB2PeriphClockCmd(SENIOR_TIM_CLK, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  TIM_TimeBaseStructure.TIM_Period = period;       
	
	// 通用控制定时器时钟源TIMxCLK = HCLK/2=90MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100KHz
  TIM_TimeBaseStructure.TIM_Prescaler = psc;	
  // 采样时钟分频
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	// 初始化定时器TIMx, x[2,3,4,5,12,13,14] 
	TIM_TimeBaseInit(SENIOR_TIM, &TIM_TimeBaseStructure);
	
	/*PWM模式配置*/
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = adc_pwm_lowtime;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  //当定时器计数值小于CCR1_Val时为低电平
  TIM_OC1Init(SENIOR_TIM, &TIM_OCInitStructure);	 //使能通道1

	NVIC_InitStructure.NVIC_IRQChannel = SENIOR_TIM_INT_CHANNEL;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	
  TIM_ClearFlag(SENIOR_TIM, TIM_FLAG_Update);
	TIM_ITConfig(SENIOR_TIM,TIM_IT_Update,ENABLE); //使能指定的TIM2中断,允许更新中断
	/*使能通道1重载*/
	TIM_OC1PreloadConfig(SENIOR_TIM, TIM_OCPreload_Enable);
  
	TIM_CtrlPWMOutputs(SENIOR_TIM, ENABLE);///////主输出使能

}

void TIM8_ADC_Config(u16 arr,u16 psc,u16 adc_pwm_lowtime ){
	  GPIO_InitTypeDef GPIO_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(TIM_ADC1_GPIO_CLK,ENABLE);
	  GPIO_PinAFConfig(TIM_ADC1_GPIO_PORT,TIM_ADC1_PWM_PINSOURCE,TIM_ADC1_PWM_AF); 

		GPIO_InitStructure.GPIO_Pin = TIM_ADC1_PWM_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(TIM_ADC1_GPIO_PORT, &GPIO_InitStructure);
//	
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
	   
    RCC_APB2PeriphClockCmd(TIM_ADC1_CLK, ENABLE);
   
    TIM_TimeBaseStructure.TIM_Period = arr;
	
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
	
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    // 计数方式
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    // 重复计数器，这里没使用
    TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
    TIM_TimeBaseInit(TIM_ADC1, &TIM_TimeBaseStructure);
		
		//TIM_SelectOutputTrigger(TIM_ADC1,TIM_TRGOSource_Update);
    TIM_SelectOutputTrigger(TIM_ADC1,TIM_TRGOSource_OC1Ref);

     /* ================== 输出结构体初始化=================== */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    // 主输出使能
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
      // 配置比较值
    TIM_OCInitStructure.TIM_Pulse = adc_pwm_lowtime;
    // 主输出高电平有效
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    // 主输出在被禁止时为低电平
    // 通道初始化
    TIM_OC1Init(TIM_ADC1, &TIM_OCInitStructure);
    // 使能通道重装载
    TIM_OC1PreloadConfig(TIM_ADC1, TIM_OCPreload_Enable);
		
		NVIC_InitStructure.NVIC_IRQChannel = TIM_ADC1_IRQn;  //TIM8中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
		NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
		
	  TIM_DMACmd(TIM_ADC1, TIM_DMA_Update, ENABLE);  //定时器更新事件触发DMA传输

	  TIM_ClearFlag(TIM_ADC1, TIM_FLAG_Update);

	//	TIM_ITConfig(TIM_ADC1,TIM_IT_Update,ENABLE); //使能指定的TIM8中断,允许更新中断
    TIM_CtrlPWMOutputs(TIM_ADC1, ENABLE);///////主输出使能
		
// 使能定时器
	 TIM_SelectMasterSlaveMode(SENIOR_TIM, TIM_MasterSlaveMode_Enable);//主从模式MSM
	 TIM_SelectOutputTrigger(SENIOR_TIM, TIM_TRGOSource_Enable);//触发选择,ITR0即TIM1

	 TIM_SelectSlaveMode(TIM_ADC1, TIM_SlaveMode_Trigger);
	 TIM_SelectInputTrigger(TIM_ADC1, TIM_TS_ITR0);
}
 