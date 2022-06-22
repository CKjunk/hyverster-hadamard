#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./adc/bsp_adc.h"
#include "./sequence/sequence.h"
#include "./time/bsp_time.h"
#include "./key/key.h"
#include "./adc_dma/adc_dma.h"
#include "./led/bsp_led.h"  
#include "./usart_dma/usart_dma.h"
#include "./HadamardOutput/bsp_dma_gpio.h"
// ADC转换的电压值通过DMA方式传到SRAM
extern uint16_t HT_IMS_ADC_ConvertedValue[HADAMARD_BUFFSIZE];
u8 display_flag = 0;
extern u8 array[HADAMARD_BUFFSIZE];
u16 count = 0;  	 

 
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
  //pfour = FourOrder_Sequence(array_four,pfour);  
	  EXTI_Key_Config();
	  Debug_USART_Config();
    Rheostat_Init();
	  TIM1_GPIO_Config(PERIOD,SENIOR_PSC,PERIOD+1);
    TIM8_ADC_Config(PERIOD,SENIOR_PSC,PULSE_WIDTH); 
    while (1){
	 
			if(display_flag == 1){
//				for(int i = 0;i < HADAMARD_BUFFSIZE;i++){ 
//					  printf("%f\r\n",(float)HT_IMS_ADC_ConvertedValue[i]/4096*(float)3.3);				 
//				}
					for(int i = 0;i < HADAMARD_BUFFSIZE;i++){ 
					  printf("%d\r\n",HT_IMS_ADC_ConvertedValue[i]);				 
				 }
				for(int i = 0;i < HADAMARD_BUFFSIZE;i++){ 
              HT_IMS_ADC_ConvertedValue[i] = 0;
				}
          display_flag = 0;
			}
			  RHEOSTAT_ADC_DMA_STREAM->NDTR = HADAMARD_BUFFSIZE;                 //重新装填DMA需要传输的数据个数
        DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);          //先使能DMA通道
        RHEOSTAT_ADC->SR &=~(0X01<<5);                         //将由硬件置1的ADC状态寄存器的溢出标志位清除，不然容易出错
        ADC_DMACmd(RHEOSTAT_ADC,ENABLE);                 //再使能ADC的DMA传输    
   }
}



