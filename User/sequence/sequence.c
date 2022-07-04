#include "./sequence/sequence.h"
#include "math.h"
#include "./usart/bsp_debug_usart.h"
#include "stdlib.h"
 
//function [seq]=SequenceProduct(coef)
//%***************************************************
//% 此函数用来生成m序列
//% coef为反馈系数向量
//% Author: FastestSnail
//% Date: 2017-10-03
//%***************************************************
//m=length(coef);
//len=2^m-1; % 得到最终生成的m序列的长度     
//backQ=0; % 对应寄存器运算后的值，放在第一个寄存器
//seq=zeros(1,len); % 给生成的m序列预分配
//registers = [1 zeros(1, m-2) 1]; % 给寄存器分配初始结果
//for i=1:len
//    seq(i)=registers(m);
//    backQ = mod(sum(coef.*registers) , 2); %特定寄存器的值进行异或运算，即相加后模2
//    registers(2:length(registers)) = registers(1:length(registers)-1); % 移位
//    registers(1)=backQ; % 把异或的值放在第一个寄存器的位置
//end
//end
/*

   此函数可用来产生S序列
*/
u8* createSequence(int initial[],int initNumber){

    int len = pow(2,initNumber)-1; 
    u8* seq = (u8*)malloc(len*sizeof(u8));
	  if(seq == NULL){
				  printf("sequence malloc failure\r\n");
		}
	  u8* registers = (u8*)malloc(initNumber*sizeof(u8));
 	  int sum = 0;
	  int xorResult = 0; 
	  memset(registers, 0, sizeof(u8));
	  memset(seq, 0, sizeof(u8));
	  registers[0] = 1;registers[initNumber-1] = 1; //初始化寄存器序列
 		for(int i = 0;i < initNumber;i++){
			 sum+=initial[i];  //算出系数向量中1的个数
 		}
 		u8* onePosition = (u8*)malloc(sum*sizeof(u8));
		memset(onePosition, 0, sizeof(u8));
		int countPosition = 0;
		for(int i = 0;i < initNumber;i++){ //存储系数向量中1的位置
		   if(initial[i] == 1){
			   onePosition[countPosition] = i;
 				 countPosition++;
			 }
		}
		int first_element = 0;
    for(int i = 0;i < len;i++){
	    seq[i] = registers[initNumber-1];
       first_element = registers[0];

      for(int j = 0;j < sum;j++){
			  xorResult = xorResult ^ registers[onePosition[j]];
			}	
      registers[0] = xorResult;
      xorResult = 0; 			
 		  for(int j = initNumber - 1;j > 1;j--){
			    registers[j] = registers[j-1];
			}
      registers[1] = first_element;	  
	 } 
   return seq;

}