// fir_safe.c
// Safe 1D FIR filter for HLS-style evaluation.

#include <stddef.h>

void fir_safe(const float *x,
              const float *h,
              float *y,
              int N,
              int TAPS)
{
    if (!x || !h || !y || N <= 0 || TAPS <= 0)
        return;

    for (int n = 0; n < N; ++n) {
        float acc = 0.0f;
        for (int k = 0; k < TAPS; ++k) {
            int idx = n - k;
            if (idx >= 0 && idx < N) {
                acc += h[k] * x[idx];
            }
        }
        y[n] = acc;
    }
}
