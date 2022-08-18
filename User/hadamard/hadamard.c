#include "hadamard.h"
#include "math.h"
#include "stdlib.h"
 
MatrixElementType * hadamard(int n){
    int iteration;
    for (int i = 0;i < 10;i++){
	   if(pow(2,i) == n){
		   iteration = i;   //需要的迭代次数
           break;
		 }
	   
	 }
   //int(*pointer)[n] = (int(*)[n])malloc(n*n*sizeof(int)); //开辟一个二维数组  数组指针无法返回，所以无法使用
   	MatrixElementType **pointer=(MatrixElementType**)malloc(n * sizeof(MatrixElementType*));//竖的格子申请好了
    for (int i = 0; i < n; i++)
    {
        pointer[i] = (MatrixElementType*)malloc(sizeof(MatrixElementType)*n);

    }
    pointer[0][0] = 1;
    pointer[0][1] = 1;
    pointer[1][0] = 1;
    pointer[1][1] = -1;

    for (int i = 0; i < iteration-1; i++)
    {
        int bound = pow(2,i+2);
        for(int j = 0;j < bound/2;j++){
            for (int k = bound/2; k < bound; k++)
            {
                 pointer[j][k] = pointer[j][k - bound/2];
            }
        }
        for(int j = bound/2;j < bound;j++){
            for (int k = 0; k < bound/2; k++)
            {
                 pointer[j][k] = pointer[j - bound/2][k];
            }
        }
        for(int j = bound/2;j < bound;j++){
            for (int k = bound/2; k < bound; k++)
            {
                 pointer[j][k] = -pointer[j - bound/2][k - bound/2];
            }
        }



    }
     MatrixElementType *p=(MatrixElementType*)malloc(n*n * sizeof(MatrixElementType)); 
     for(int i = 0;i < n*n ; i++){
		    p[i] = pointer[i/n][i%n];
		 
		 }
   
    return p;
   
 }
   
MatrixElementType * prbs(MatrixElementType *pHadamard,int n){
	  MatrixElementType* pPrbs = (MatrixElementType*)malloc((n - 1)*(n - 1) * sizeof(MatrixElementType)); ; 

		for(int i = 1;i < n;i++){
		   for(int j = 1;j < n;j++){
			      if(pHadamard[i*n+j]>0.5){
						    pPrbs[(i-1)*(n-1)+(j-1)] = 0;
						
						}
						else if(pHadamard[i*n+j]<-0.5){
						    pPrbs[(i-1)*(n-1)+(j-1)] = 1;
						}	
 			 }
 
		}
   return pPrbs;
}
