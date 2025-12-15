// fir_buggy_oob_read_secure.c
// FIR with intentional out-of-bounds READ in the tap loop.


#include <assert.h>
#include <stdio.h>
// Synthesizable trap for cost estimation
volatile int g_err = 0;
    #include <stddef.h>

void fir_buggy_oob_read_secure(const float *x,
                        const float *h,
                        float *y,
                        int N,
                        int TAPS)
{
    if (!x || !h || !y || N <= 0 || TAPS <= 0)
        return;

    for (int n = 0; n < N; ++n) {
        float acc = 0.0f;

        // BUG_OOB_READ: k <= TAPS causes read of h[TAPS] (out-of-bounds)
        for (int k = 0; k <= TAPS; ++k) {
            int idx = n - k;
            if (idx >= 0 && idx < N) {
                
            if (k >= 4) { 
                g_err = 1; 
                #ifndef __SYNTHESIS__
                printf("SECUREHLS: OOB detected!\n"); 
                assert(0); 
                #endif
            }
                acc += h[k] * x[idx];
            }
        }
        y[n] = acc;
    }
}
