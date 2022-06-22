#include "./time/bsp_time.h"
#include "./usart/bsp_debug_usart.h"
#include "./HadamardOutput/bsp_dma_gpio.h"
void TIM1_GPIO_Config(u16 period,u16 psc,u16 adc_pwm_lowtime){

	GPIO_InitTypeDef GPIO_InitStructure;
	/*������ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd (HADAMARD_GPIO_CLK,ENABLE); 
	RCC_AHB1PeriphClockCmd (HADAMARD_GPIO_CLK_ADDING,ENABLE); 

  /* ��ʱ��ͨ�����Ÿ��� */
	GPIO_PinAFConfig(HADAMARD_GPIO_PORT,HADAMARD_PWM_PINSOURCE,HADAMARD_GPIO_PIN_PWM_AF); 
	GPIO_PinAFConfig(HADAMARD_GPIO_PORT_ADDING,HADAMARD_PWM_PINSOURCE_ADDING,HADAMARD_GPIO_PIN_PWM_AF); 

	/* ��ʱ��ͨ���������� */															   
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
	// ����TIMx_CLK,x[2,3,4,5,12,13,14] 
  RCC_APB2PeriphClockCmd(SENIOR_TIM_CLK, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  TIM_TimeBaseStructure.TIM_Period = period;       
	
	// ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=90MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=100KHz
  TIM_TimeBaseStructure.TIM_Prescaler = psc;	
  // ����ʱ�ӷ�Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  // ������ʽ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	// ��ʼ����ʱ��TIMx, x[2,3,4,5,12,13,14] 
	TIM_TimeBaseInit(SENIOR_TIM, &TIM_TimeBaseStructure);
	
	/*PWMģʽ����*/
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;	    //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = adc_pwm_lowtime;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  //����ʱ������ֵС��CCR1_ValʱΪ�͵�ƽ
  TIM_OC1Init(SENIOR_TIM, &TIM_OCInitStructure);	 //ʹ��ͨ��1

	NVIC_InitStructure.NVIC_IRQChannel = SENIOR_TIM_INT_CHANNEL;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	
  TIM_ClearFlag(SENIOR_TIM, TIM_FLAG_Update);
	TIM_ITConfig(SENIOR_TIM,TIM_IT_Update,ENABLE); //ʹ��ָ����TIM2�ж�,��������ж�
	/*ʹ��ͨ��1����*/
	TIM_OC1PreloadConfig(SENIOR_TIM, TIM_OCPreload_Enable);
  
	TIM_CtrlPWMOutputs(SENIOR_TIM, ENABLE);///////�����ʹ��

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
    // ������ʽ
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    // �ظ�������������ûʹ��
    TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
    TIM_TimeBaseInit(TIM_ADC1, &TIM_TimeBaseStructure);
		
		//TIM_SelectOutputTrigger(TIM_ADC1,TIM_TRGOSource_Update);
    TIM_SelectOutputTrigger(TIM_ADC1,TIM_TRGOSource_OC1Ref);

     /* ================== ����ṹ���ʼ��=================== */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    // �����ʹ��
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
      // ���ñȽ�ֵ
    TIM_OCInitStructure.TIM_Pulse = adc_pwm_lowtime;
    // ������ߵ�ƽ��Ч
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    // ������ڱ���ֹʱΪ�͵�ƽ
    // ͨ����ʼ��
    TIM_OC1Init(TIM_ADC1, &TIM_OCInitStructure);
    // ʹ��ͨ����װ��
    TIM_OC1PreloadConfig(TIM_ADC1, TIM_OCPreload_Enable);
		
		NVIC_InitStructure.NVIC_IRQChannel = TIM_ADC1_IRQn;  //TIM8�ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
		NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
		
	  TIM_DMACmd(TIM_ADC1, TIM_DMA_Update, ENABLE);  //��ʱ�������¼�����DMA����

	  TIM_ClearFlag(TIM_ADC1, TIM_FLAG_Update);

	//	TIM_ITConfig(TIM_ADC1,TIM_IT_Update,ENABLE); //ʹ��ָ����TIM8�ж�,��������ж�
    TIM_CtrlPWMOutputs(TIM_ADC1, ENABLE);///////�����ʹ��
		
// ʹ�ܶ�ʱ��
	 TIM_SelectMasterSlaveMode(SENIOR_TIM, TIM_MasterSlaveMode_Enable);//����ģʽMSM
	 TIM_SelectOutputTrigger(SENIOR_TIM, TIM_TRGOSource_Enable);//����ѡ��,ITR0��TIM1

	 TIM_SelectSlaveMode(TIM_ADC1, TIM_SlaveMode_Trigger);
	 TIM_SelectInputTrigger(TIM_ADC1, TIM_TS_ITR0);
}
 