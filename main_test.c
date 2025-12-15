// main_test.c
// Minimal harness to call safe and buggy kernels with small sizes.
// Not required for static analysis, but useful for manual sanity checks.

#include <stdio.h>
#include <stdlib.h>

// Declarations (so we don't have to share headers for this demo)
void fir_safe(const float *x, const float *h, float *y, int N, int TAPS);
void fir_buggy_oob_read(const float *x, const float *h, float *y, int N, int TAPS);

void gemm_safe(const float *A, const float *B, float *C, int M, int K, int N);
void gemm_buggy_oob_write(const float *A, const float *B, float *C, int M, int K, int N);

void conv2d_safe(const float *input, int height, int width,
                 const float *kernel, int kh, int kw, float *output);
void conv2d_buggy_oob_read(const float *input, int height, int width,
                           const float *kernel, int kh, int kw, float *output);

void maxpool2x2_safe(const float *input, int height, int width, float *output);
void maxpool2x2_buggy_oob_write(const float *input, int height, int width, float *output);

void pointer_region_safe(float *buffer, int len);
void pointer_region_buggy_oob_write(float *buffer, int len);
void null_deref_buggy(float *buffer, int len, int use_null);

int main(void)
{
    int N = 8;
    float x[8] = {0};
    float h[4] = {1, 2, 3, 4};
    float y[8] = {0};

    fir_safe(x, h, y, N, 4);
    fir_buggy_oob_read(x, h, y, N, 4);

    int M = 4, K = 4, W = 4;
    float A[16] = {0};
    float B[16] = {0};
    float C[16] = {0};

    gemm_safe(A, B, C, M, K, W);
    gemm_buggy_oob_write(A, B, C, M, K, W);

    int H = 4;
    float input[16] = {0};
    float kernel[9] = {0};
    float out_conv[4] = {0}; // Strictly sized for valid output (2x2)

    conv2d_safe(input, H, W, kernel, 3, 3, out_conv);
    // conv2d_buggy_oob_read will read OOB on input regardless of output size
    conv2d_buggy_oob_read(input, H, W, kernel, 3, 3, out_conv);

    float out_pool[4] = {0}; // Strictly sized for valid output (2x2)
    maxpool2x2_safe(input, H, W, out_pool);
    // maxpool2x2_buggy_oob_write will write to indices > 3, causing actual OOB write
    maxpool2x2_buggy_oob_write(input, H, W, out_pool);

    float buf[8] = {0};
    pointer_region_safe(buf, 8);
    pointer_region_buggy_oob_write(buf, 8);

    null_deref_buggy(buf, 8, 0); // safe (no null)
    null_deref_buggy(buf, 8, 1); // will dereference null

    printf("Test harness executed.\n");
    return 0;
}
