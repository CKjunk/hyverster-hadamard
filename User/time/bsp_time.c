#include "./time/bsp_time.h"
static void SENIOR_TIMx_GPIO_Config(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启定时器相关的GPIO外设时钟*/
    RCC_AHB1PeriphClockCmd (ADVANCE_OCPWM1_GPIO_CLK  |
                            ADVANCE_OCNPWM1_GPIO_CLK ,
														ENABLE);
    /* 指定引脚复用功能 */
    GPIO_PinAFConfig(ADVANCE_OCPWM1_GPIO_PORT,
                    ADVANCE_OCPWM1_PINSOURCE,
                    ADVANCE_OCPWM1_AF);
    GPIO_PinAFConfig(ADVANCE_OCNPWM1_GPIO_PORT,
                    ADVANCE_OCNPWM1_PINSOURCE,
                    ADVANCE_OCNPWM1_AF);
    GPIO_PinAFConfig(ADVANCE_OCPWM8_GPIO_PORT,
                    ADVANCE_OCPWM8_PINSOURCE,
                    ADVANCE_OCPWM8_AF);
    GPIO_PinAFConfig(ADVANCE_OCNPWM8_GPIO_PORT,
                    ADVANCE_OCNPWM8_PINSOURCE,
                    ADVANCE_OCNPWM8_AF);

    /* 定时器功能引脚初始化 */
    GPIO_InitStructure.GPIO_Pin = ADVANCE_OCPWM1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(ADVANCE_OCPWM1_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ADVANCE_OCNPWM1_PIN;
    GPIO_Init(ADVANCE_OCNPWM1_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ADVANCE_OCPWM8_PIN;
    GPIO_Init(ADVANCE_OCPWM8_GPIO_PORT, &GPIO_InitStructure);
		
    GPIO_InitStructure.GPIO_Pin = ADVANCE_OCNPWM8_PIN;
    GPIO_Init(ADVANCE_OCNPWM8_GPIO_PORT, &GPIO_InitStructure);
}

static void SENIOR_TIMx_Mode_Config(u16 Arr,u16 Psc,u16 Pwm1_lowTime,u16 Pwm2_lowTime)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
	  //死区初始化结构体
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
    /* ================== 时基结构体初始化=================== */
    // 开启TIMx_CLK,x[1,8]
    RCC_APB2PeriphClockCmd(ADVANCE_TIM1_CLK|ADVANCE_TIM8_CLK, ENABLE);

    // 累计 TIM_Period+1个计数后产生一个更新或者中断
    // 当定时器从0计数到1023，即为1024次，为一个定时周期
    TIM_TimeBaseStructure.TIM_Period = Arr;
    // 高级控制定时器时钟源TIMxCLK = HCLK=168MHz
    // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100KHz
    TIM_TimeBaseStructure.TIM_Prescaler = Psc;
    // 时钟分频，在计算死区时间的时候会用到
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    // 计数方式
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    // 重复计数器，这里没使用
    TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
    // 初始化定时器TIMx, x[1,8]
    TIM_TimeBaseInit(ADVANCE_TIM1, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(ADVANCE_TIM8, &TIM_TimeBaseStructure);
    /* ================== 输出结构体初始化=================== */
    // 配置为PWM模式2，先输出低电平，达到比较值的时候再改变电平
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    // 主输出使能
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    // 互补输出使能
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    // 配置比较值
    TIM_OCInitStructure.TIM_Pulse = Pwm1_lowTime;
    // 主输出高电平有效
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    // 互补输出高电平有效
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    // 主输出在被禁止时为低电平
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCNIdleState_Reset;
    // 互补输出在被禁止时为高电平
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;
    // 通道初始化
    TIM_OC1Init(ADVANCE_TIM1, &TIM_OCInitStructure);

    // 使能通道重装载
    TIM_OC1PreloadConfig(ADVANCE_TIM1, TIM_OCPreload_Enable);
		
    TIM_OCInitStructure.TIM_Pulse = Pwm2_lowTime;
		
		TIM_OC1PreloadConfig(ADVANCE_TIM8, TIM_OCPreload_Enable);
		
    /* ================ 断路和死区结构体初始化================ 
    // 自动输出使能，断路、死区时间和锁定配置
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
    // 配置死区时间
    TIM_BDTRInitStructure.TIM_DeadTime = 11;
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
    // 配置刹车引脚电平，当引脚为配置的电平时，主和互补输出都被禁止
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(ADVANCE_TIM, &TIM_BDTRInitStructure);
*/
    // 使能定时器
    TIM_Cmd(ADVANCE_TIM1, ENABLE);

    // 主动输出使能，只有TIM1和TIM8才有这个配置
    TIM_CtrlPWMOutputs(ADVANCE_TIM1, ENABLE);
		TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);//主从模式MSM
		TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Enable);//Tim1使能输出作为触发信号        
		TIM_SelectMasterSlaveMode(TIM8, TIM_MasterSlaveMode_Enable);//主从模式MSM
		TIM_SelectInputTrigger(TIM8, TIM_TS_ITR0);//触发选择,ITR0即TIM1
		TIM_SelectSlaveMode(TIM8, TIM_SlaveMode_Trigger);//触发模式只启动;门模式起停都控制
	 
		
    TIM_CtrlPWMOutputs(TIM8, ENABLE);///////主输出使能
}

static void COMMON_TIMx_Config(u16 arr,u16 psc){

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	
	
  TIM_SelectMasterSlaveMode(TIM8, TIM_MasterSlaveMode_Enable);//主从模式MSM
	TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Enable);//Tim8使能输出作为触发信号        
	TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);//主从模式MSM
	TIM_SelectInputTrigger(TIM2, TIM_TS_ITR1);//触发选择,ITR0即TIM1
	TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Trigger);//触发模式只启动;门模式起停都控制
 
		
	TIM_CtrlPWMOutputs(TIM2, ENABLE);///////主输出使能


}