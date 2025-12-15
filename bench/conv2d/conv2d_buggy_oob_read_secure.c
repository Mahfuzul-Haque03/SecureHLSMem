// conv2d_buggy_oob_read_secure.c
// 2D convolution with intentional out-of-bounds READ at borders.


#include <assert.h>
#include <stdio.h>
// Synthesizable trap for cost estimation
volatile int g_err = 0;
    #include <stddef.h>

void conv2d_buggy_oob_read_secure(const float *input,
                           int height,
                           int width,
                           const float *kernel,
                           int kh,
                           int kw,
                           float *output)
{
    if (!input || !kernel || !output ||
        height <= 0 || width <= 0 || kh <= 0 || kw <= 0)
        return;

    // BUG_OOB_READ: loops go over full height/width rather than valid output range.
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            float acc = 0.0f;
            for (int r = 0; r < kh; ++r) {
                for (int c = 0; c < kw; ++c) {
                    int in_r = i + r;
                    int in_c = j + c;
                    // No bounds check: in_r/in_c can exceed [0, height/width)
                    int idx_in = in_r * width + in_c; // BUG_OOB_READ
                    int idx_k  = r * kw + c;
                    
            if (idx_in < 0 || idx_in >= 16) { 
                g_err = 1; 
                #ifndef __SYNTHESIS__
                printf("SECUREHLS: OOB detected!\n"); 
                assert(0); 
                #endif
            }
                acc += input[idx_in] * kernel[idx_k];
                }
            }
            int out_idx = i * width + j;
            output[out_idx] = acc;
        }
    }
}
