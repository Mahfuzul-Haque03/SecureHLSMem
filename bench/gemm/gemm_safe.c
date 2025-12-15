// gemm_safe.c
// Safe GEMM: C[M x N] = A[M x K] * B[K x N].

#include <stddef.h>

void gemm_safe(const float *A,
               const float *B,
               float *C,
               int M,
               int K,
               int N)
{
    if (!A || !B || !C || M <= 0 || K <= 0 || N <= 0)
        return;

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            float acc = 0.0f;
            for (int k = 0; k < K; ++k) {
                int idxA = i * K + k;
                int idxB = k * N + j;
                acc += A[idxA] * B[idxB];
            }
            C[i * N + j] = acc;
        }
    }
}
