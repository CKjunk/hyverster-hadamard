#ifndef __HADAMARD_H
#define	__HADAMARD_H

#include "stm32f4xx.h"
#include <stdio.h>
#include "arm_math.h"

typedef   float32_t MatrixElementType;
typedef   arm_matrix_instance_f32 MatrixType;
#define   MatrixMultiply  arm_mat_mult_f32  
#define   MatrixInit      arm_mat_init_f32  
#define   MatrixTrans     arm_mat_trans_f32  
#define   MatrixScale     arm_mat_scale_f32 
#define   MatrixInverse   arm_mat_inverse_f32
MatrixElementType * hadamard(int n);
void SWAP(MatrixElementType* a, MatrixElementType* b);
void transposingMatrix(MatrixElementType* a, int m, int n);
MatrixElementType * prbs(MatrixElementType *pHadamard,int n);
MatrixElementType* calculate(int a_row, int a_col, MatrixElementType *a, int b_row,int b_col, MatrixElementType *b);
void createInverseMatrix(MatrixElementType *prbs,int length);
#endif