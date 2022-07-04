#include "./sequence/sequence.h"
#include "math.h"
#include "./usart/bsp_debug_usart.h"
#include "stdlib.h"
 
//function [seq]=SequenceProduct(coef)
//%***************************************************
//% �˺�����������m����
//% coefΪ����ϵ������
//% Author: FastestSnail
//% Date: 2017-10-03
//%***************************************************
//m=length(coef);
//len=2^m-1; % �õ��������ɵ�m���еĳ���     
//backQ=0; % ��Ӧ�Ĵ���������ֵ�����ڵ�һ���Ĵ���
//seq=zeros(1,len); % �����ɵ�m����Ԥ����
//registers = [1 zeros(1, m-2) 1]; % ���Ĵ��������ʼ���
//for i=1:len
//    seq(i)=registers(m);
//    backQ = mod(sum(coef.*registers) , 2); %�ض��Ĵ�����ֵ����������㣬����Ӻ�ģ2
//    registers(2:length(registers)) = registers(1:length(registers)-1); % ��λ
//    registers(1)=backQ; % ������ֵ���ڵ�һ���Ĵ�����λ��
//end
//end
/*

   �˺�������������S����
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
	  registers[0] = 1;registers[initNumber-1] = 1; //��ʼ���Ĵ�������
 		for(int i = 0;i < initNumber;i++){
			 sum+=initial[i];  //���ϵ��������1�ĸ���
 		}
 		u8* onePosition = (u8*)malloc(sum*sizeof(u8));
		memset(onePosition, 0, sizeof(u8));
		int countPosition = 0;
		for(int i = 0;i < initNumber;i++){ //�洢ϵ��������1��λ��
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