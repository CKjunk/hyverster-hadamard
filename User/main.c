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

// ADCת���ĵ�ѹֵͨ��DMA��ʽ����SRAM
extern uint16_t HT_IMS_ADC_ConvertedValue[HADAMARD_BUFFSIZE];
u8 display_flag = 0;
extern u8 array[HADAMARD_BUFFSIZE];
u16 count = 0;  	 
u16 frame_count = 1;
 
FOUR four = {0,NULL};
FOUR* pfour = &four;
SIX six = {0,NULL};
SIX* psix = &six;
EIGHT eight = {0,NULL};
EIGHT* peight = &eight;
TEN ten = {0,NULL};
TEN* pten = &ten;
TWELVE twelve = {0,NULL};
TWELVE* ptwelve = &twelve;
THIRTEEN thirteen = {0,NULL};
THIRTEEN* pthirteen = &thirteen;
  

int array_four[4] = {1,0,0,1};
int array_ten[10] = {1, 0 ,0, 0 ,0 ,0, 0 ,0 ,0 ,1};
int array_twelve[12] = {1, 0, 0 ,0, 0, 0, 0 ,0, 0 ,0 ,0, 1};
int array_thirteen[13] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
int main(void)
{	
	  pten = TenOrder_Sequence(array_ten,pten);
	  SysTick_Init();
  //pfour = FourOrder_Sequence(array_four,pfour);  
	  EXTI_Key_Config(); //������ʼ��
	  Debug_USART_Config();//���ڳ�ʼ��
    Rheostat_Init();//adc��ʼ��
	  TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//����ʱ�ӳ�ʼ��
    TIM8_ADC_Config(TIM_ADC1_PERIOD,SENIOR_PSC,TIM_ADC1_LOWTIME);//adcʱ�ӳ�ʼ��
    while (1){
	 
			if(display_flag == 1){ //��ʾһ���Ѿ��ɼ���
 
 					for(int i = 0;i < HADAMARD_BUFFSIZE;i++){ //�����ϴ�����λ��
					  printf("%d\r\n",HT_IMS_ADC_ConvertedValue[i]);	
						HT_IMS_ADC_ConvertedValue[i] = 0; //����֮�󽫶�Ӧλ����
				 }
 			   RHEOSTAT_ADC_DMA_STREAM->NDTR = HADAMARD_BUFFSIZE; //����װ��DMA��Ҫ��������ݸ���
			   DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);          //��ʹ��DMAͨ��
			   RHEOSTAT_ADC->SR &=~(0X01<<5);   //����Ӳ����1��ADC״̬�Ĵ����������־λ�������Ȼ���׳���
         TIM1_GPIO_Config(SEQUENCE_PERIOD,SENIOR_PSC,SEQUENCE_PERIOD+1);//����ʱ�ӳ�ʼ��
         TIM8_ADC_Config(TIM_ADC1_PERIOD,SENIOR_PSC,TIM_ADC1_LOWTIME);//adcʱ�ӳ�ʼ��
         display_flag = 0;
				 frame_count++;
			}
	 

		}
}



