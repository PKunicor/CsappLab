/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
  int t_1, t_2, t_3;
  int t_4, t_5;
  int i, j, ii, jj;
  int k;  //用于计算内存数组的行数
  k = N * M / 32 + (N * M % 32 == 0 ? 0 : 1);
  for(i = 0; i < k; i += 8){
    for(j = 0; j < 32; j += 8){
      //小于32的数组
      if(M <= 32){
        for(ii = i; ii < ((i + 8) < k ? i + 8 : k); ii++){
        //标记没有访问冲突
        t_4 = -1;
        for(jj = j; jj < ((j + 8) < 32 ? j + 8 : 32); jj++){
          //先计算内存中的位置   然后再转换为原数组 x y
          t_1 = ii * 32 + jj; 
          //x
          t_2 = t_1 / M;
          //y
          t_3 = t_1 % M;
          if((t_1 % 256 ) / 8 == ((t_3 * N + t_2) % 256) / 8){
              if(t_4 == -1){
                t_4 = t_2;
                t_5 = t_3;
              }
              continue;
          }
          //printf("**  2-- %d  3--  %d\n", t_2, t_3);
          B[t_3][t_2] = A[t_2][t_3];
        }
        if(t_4 != -1){
          B[t_5][t_4] = A[t_4][t_5];
        }
          }
      }
      else{
        //先访问前四列
        for(ii = i; ii < ((i + 8) < k ? i + 8 : k); ii++){
        //标记没有访问冲突
        t_4 = -1;
        for(jj = j; jj < ((j + 4) < 32 ? j + 4 : 32); jj++){
          //先计算内存中的位置   然后再转换为原数组 x y
          t_1 = ii * 32 + jj; 
          //x
          t_2 = t_1 / M;
          //y
          t_3 = t_1 % M;
          if((t_1 % 256 ) / 8 == ((t_3 * N + t_2) % 256) / 8){
              if(t_4 == -1){
                t_4 = t_2;
                t_5 = t_3;
              }
              continue;
          }
          //printf("**  2-- %d  3--  %d\n", t_2, t_3);
          B[t_3][t_2] = A[t_2][t_3];
        }
        if(t_4 != -1){
          B[t_5][t_4] = A[t_4][t_5];
        }
        }
        //后四列
        t_4 = -1;
        //先访问前四列
        for(ii = i; ii < ((i + 8) < k ? i + 8 : k); ii++){
        for(jj = j + 4; jj < ((j + 8) < 32 ? j + 8 : 32); jj++){
          //先计算内存中的位置   然后再转换为原数组 x y
          t_1 = ii * 32 + jj; 
          //x
          t_2 = t_1 / M;
          //y
          t_3 = t_1 % M;
          if((t_1 % 256 ) / 8 == ((t_3 * N + t_2) % 256) / 8){
              if(t_4 == -1){
                t_4 = t_2;
                t_5 = t_3;
              }
              continue;
          }
          //printf("**  2-- %d  3--  %d\n", t_2, t_3);
          B[t_3][t_2] = A[t_2][t_3];
        }
        if(t_4 != -1){
          B[t_5][t_4] = A[t_4][t_5];
        }

        }
      } 
    }
  }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 
char transpose_v1_desc[] = "Transpose version 1: 287  4611";
void transpose_v1(int M, int N, int A[N][M], int B[M][N])
{
  int t_1, t_2, t_3;
  int t_4, t_5;
  int t_6, t_7;
  int t_8;
  int i, j, ii, jj;
  int k;  //用于计算内存数组的行数
  k = N * M / 32 + (N * M % 32 == 0 ? 0 : 1);
  for(i = 0; i < k; i += 8){
    for(j = 0; j < 32; j += 8){
      for(ii = i; ii < ((i + 8) < k ? i + 8 : k); ii++){
        //标记没有访问冲突
        t_4 = -1;
        t_6 = -1;
        for(jj = j; jj < ((j + 8) < 32 ? j + 8 : 32); jj++){
          //先计算内存中的位置   然后再转换为原数组 x y
          t_1 = ii * 32 + jj; 
          //x
          t_2 = t_1 / M;
          //y
          t_3 = t_1 % M;
          //t_4 = t_2 * N + t_3 * M;
          if((t_1 % 256 ) / 8 == ((t_3 * N + t_2) % 256) / 8){
            if(t_4 == -1){
              t_4 = t_2;
              t_5 = t_3;
            }
            else{
              t_6 = t_2;
              t_7 = t_3;
            }
            continue;
          }
          //printf("**  2-- %d  3--  %d\n", t_2, t_3);
          B[t_3][t_2] = A[t_2][t_3];
        }
        if(t_4 != -1){
          if(t_6 != -1){
            //保存一个
            t_8 =  A[t_6][t_7];
            //替换另一个
            //printf("***   4-- %d  5--  %d\n", t_4, t_5);
            B[t_5][t_4] = A[t_4][t_5];
            //减少一个替换cache
            //printf("****   6-- %d  7--  %d\n", t_6, t_7);
            B[t_7][t_6] = t_8;
          }
          else{
            //printf("***   4-- %d  5--  %d\n", t_4, t_5);
            B[t_5][t_4] = A[t_4][t_5];
          }
        }

      } 
    }
  }
}
char transpose_v2_desc[] = "Transpose version 2 最简单的 32X32 287miss";
void transpose_v2(int M, int N, int A[N][M], int B[M][N])
{
  int t_1, t_2, t_3;
  int t_4, t_5;
  int i, j, ii, jj;
  int k;  //用于计算内存数组的行数
  k = N * M / 32 + (N * M % 32 == 0 ? 0 : 1);
  for(i = 0; i < k; i += 8){
    for(j = 0; j < 32; j += 8){
      for(ii = i; ii < ((i + 8) < k ? i + 8 : k); ii++){
        //标记没有访问冲突
        t_4 = -1;
        for(jj = j; jj < ((j + 8) < 32 ? j + 8 : 32); jj++){
          //先计算内存中的位置   然后再转换为原数组 x y
          t_1 = ii * 32 + jj; 
          //x
          t_2 = t_1 / M;
          //y
          t_3 = t_1 % M;
          if((t_1 % 256 ) / 8 == ((t_3 * N + t_2) % 256) / 8){
              if(t_4 == -1){
                t_4 = t_2;
                t_5 = t_3;
              }
              continue;
          }
          //printf("**  2-- %d  3--  %d\n", t_2, t_3);
          B[t_3][t_2] = A[t_2][t_3];
        }
        if(t_4 != -1){
          B[t_5][t_4] = A[t_4][t_5];
        }
      } 
    }
  }
}
/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 
    //registerTransFunction(transpose_v1, transpose_v1_desc); 
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

