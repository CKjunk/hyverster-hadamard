#include "./sequence/sequence.h"
#include "math.h"
#include "./usart/bsp_debug_usart.h"
FOUR* FourOrder_Sequence(int a[FOURORDER],FOUR* matrix){
	 int len = pow(2,FOURORDER)-1;  
	 int i = 0;
	 int j ; 
	 int first_element;
	 for(i = 0;i < len;i++){
	    matrix->address[i] = a[3];
		 
      first_element = a[0];	 
		  a[0] = (a[3]^a[2]);
		  for(j = FOURORDER-1;j > 1;j--){
			    a[j] = a[j-1];
			}
      a[1] = first_element;	 
	 }
	 matrix->m_sequence_length = len;
	 return matrix;
}

SIX* SixOrder_Sequence(int a[SIXORDER],SIX* matrix){
	
	 int len = pow(2,SIXORDER)-1;  
	 int i = 0;
	 int j; 
	 int first_element;
	 for(i = 0;i < len;i++){
	    matrix->address[i] = a[SIXORDER-1];
      first_element = a[0];	 
		  a[0] = a[4]^a[5];//000011
		  for(j = SIXORDER - 1;j > 1;j--){
			    a[j] = a[j-1];
			}
      a[1] = first_element;	  
	 }
	 matrix->m_sequence_length = len;
	 return matrix;
}


EIGHT* EightOrder_Sequence(int a[EIGHTORDER],EIGHT* matrix){
	
	 int len = pow(2,EIGHTORDER)-1;  
	 int i = 0;
	 int j; 
	 int first_element;
	 for(i = 0;i < len;i++){
	    matrix->address[i] = a[EIGHTORDER-1];
      first_element = a[0];	 
		  a[0] = (a[3]^a[4]^a[5]^a[7]);//000 111 01
		  for(j = EIGHTORDER - 1;j > 1;j--){
			    a[j] = a[j-1];
			}
      a[1] = first_element;	  
	 }
	 matrix->m_sequence_length = len;
	 return matrix;
}

TEN* TenOrder_Sequence(int a[TENORDER],TEN* matrix){
	 int len = pow(2,TENORDER)-1;  
	 int i = 0;
	 int j; 
	 int first_element;
	 for(i = 0;i < len;i++){
	    matrix->address[i] = a[TENORDER-1];
      first_element = a[0];	 
		  a[0] = (a[6]^a[9]);//0 0 0 0 0 0 1 0 0 1
		  for(j = TENORDER - 1;j > 1;j--){
			    a[j] = a[j-1];
			}
      a[1] = first_element;	  
	 }
	 matrix->m_sequence_length = len+1;
	 a[len] = 0;
	 return matrix;
}
TWELVE* TwelveOrder_Sequence(int a[TWELVEORDER],TWELVE* matrix){
	 int len = pow(2,TWELVEORDER)-1;  
	 int i = 0;
	 int j; 
	 int first_element;
	 for(i = 0;i < len;i++){
	    matrix->address[i] = a[TWELVEORDER-1];
      first_element = a[0];	 
		  a[0] = (a[5]^a[7]^a[10]^a[11]);//0 0 0 0 0 1 0 1 0 0 1 1
		  for(j = TWELVEORDER - 1;j > 1;j--){
			    a[j] = a[j-1];
			}
      a[1] = first_element;	  
	 }
	 matrix->m_sequence_length = len;
	 return matrix;
}

THIRTEEN* ThirteenOrder_Sequence(int a[THIRTEENORDER],THIRTEEN* matrix){
	 int len = pow(2,THIRTEENORDER)-1;  
	 int i = 0;
	 int j; 
	 int first_element;
	 for(i = 0;i < len;i++){
	    matrix->address[i] = a[THIRTEENORDER-1];
      first_element = a[0];	 
		  a[0] = (a[8]^a[9]^a[11]^a[12]);//0 0 0 0 0 0 0 0 1 1 0 1 1
		  for(j = THIRTEENORDER - 1;j > 1;j--){
			    a[j] = a[j-1];
			}
      a[1] = first_element;	  
	 }
	 matrix->m_sequence_length = len;
	 return matrix;



}