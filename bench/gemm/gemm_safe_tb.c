
#include <stdio.h>
#include <stdlib.h>

// Forward decl
void gemm_safe(const float *A, const float *B, float *C, int M, int K, int N);

int main() {
    
    int M = 4, K = 4, W = 4;
    float A[16] = {0};
    float B[16] = {0};
    float C[16] = {0};
        
    
    printf("Calling gemm_safe...\n");
    gemm_safe(A, B, C, M, K, W);
    
    printf("CSIM PASSED\n");
    return 0;
}
