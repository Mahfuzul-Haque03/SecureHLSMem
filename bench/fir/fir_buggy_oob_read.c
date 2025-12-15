// fir_buggy_oob_read.c
// FIR with intentional out-of-bounds READ in the tap loop.

#include <stddef.h>

void fir_buggy_oob_read(const float *x,
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
                acc += h[k] * x[idx];
            }
        }
        y[n] = acc;
    }
}
