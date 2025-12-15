// gemm_buggy_oob_write.c
// GEMM with intentional out-of-bounds WRITE on C due to loop bound.

#include <stddef.h>

void gemm_buggy_oob_write(const float *A,
                          const float *B,
                          float *C,
                          int M,
                          int K,
                          int N)
{
    if (!A || !B || !C || M <= 0 || K <= 0 || N <= 0)
        return;

    for (int i = 0; i < M; ++i) {
        // BUG_OOB_WRITE: j <= N writes one element past the end of C's buffer
        for (int j = 0; j <= N; ++j) {
            float acc = 0.0f;
            for (int k = 0; k < K; ++k) {
                int idxA = i * K + k;
                int safe_j = (j < N) ? j : (N - 1);
                int idxB = k * N + safe_j; // keep B in-bounds
                acc += A[idxA] * B[idxB];
            }
            int idxC = i * N + j;   // j == N is out-of-bounds
            C[idxC] = acc;          // BUG_OOB_WRITE here
        }
    }
}
