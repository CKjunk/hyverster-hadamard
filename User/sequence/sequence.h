#ifndef ___SEQUENCE_H
#define ___SEQUENCE_H 
#include "stm32f4xx.h"
#define FOURORDER 4
#define SIXORDER 6
#define EIGHTORDER 8
#define TENORDER 10
#define TWELVEORDER 12
#define THIRTEENORDER 13 
typedef struct four_matrix{
  int m_sequence_length;
	int address[15];
}FOUR;

typedef struct six_matrix{
  int m_sequence_length;
	int address[63];
}SIX;

typedef struct eight_matrix{
  int m_sequence_length;
	int address[255];
}EIGHT;

typedef struct ten_matrix{
  int m_sequence_length;
	int address[1024];//确保关闭的时候离子门是关断的状态，所以多加一个元素
}TEN;

typedef struct twelve_matrix{
  int m_sequence_length;
	int address[4095];
}TWELVE;

typedef struct thirteen_matrix{
  int m_sequence_length;
	int address[8191];
}THIRTEEN;

FOUR* FourOrder_Sequence(int a[FOURORDER],FOUR* matrix);
SIX* SixOrder_Sequence(int a[SIXORDER],SIX* matrix);
EIGHT* EightOrder_Sequence(int a[EIGHTORDER],EIGHT* matrix);
TEN* TenOrder_Sequence(int a[TENORDER],TEN* matrix);
TWELVE* TwelveOrder_Sequence(int a[TWELVEORDER],TWELVE* matrix);
THIRTEEN* ThirteenOrder_Sequence(int a[THIRTEENORDER],THIRTEEN* matrix);


#endif