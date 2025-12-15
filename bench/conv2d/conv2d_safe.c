// conv2d_safe.c
// Safe 2D valid convolution (no padding).

#include <stddef.h>

void conv2d_safe(const float *input,
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

    int out_h = height - kh + 1;
    int out_w = width  - kw + 1;

    if (out_h <= 0 || out_w <= 0)
        return;

    for (int i = 0; i < out_h; ++i) {
        for (int j = 0; j < out_w; ++j) {
            float acc = 0.0f;
            for (int r = 0; r < kh; ++r) {
                for (int c = 0; c < kw; ++c) {
                    int in_r = i + r;
                    int in_c = j + c;
                    int idx_in = in_r * width + in_c;
                    int idx_k  = r * kw + c;
                    acc += input[idx_in] * kernel[idx_k];
                }
            }
            output[i * out_w + j] = acc;
        }
    }
}
