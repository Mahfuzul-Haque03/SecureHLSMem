// maxpool2x2_safe.c
// Safe 2x2 max pooling with stride 2.

#include <stddef.h>

void maxpool2x2_safe(const float *input,
                     int height,
                     int width,
                     float *output)
{
    if (!input || !output || height <= 1 || width <= 1)
        return;

    int out_h = height / 2;
    int out_w = width  / 2;

    for (int i = 0; i < out_h; ++i) {
        for (int j = 0; j < out_w; ++j) {
            int r0 = 2 * i;
            int c0 = 2 * j;

            int idx0 = r0 * width + c0;
            int idx1 = r0 * width + (c0 + 1);
            int idx2 = (r0 + 1) * width + c0;
            int idx3 = (r0 + 1) * width + (c0 + 1);

            float m0 = input[idx0];
            float m1 = input[idx1];
            float m2 = input[idx2];
            float m3 = input[idx3];

            float max01 = (m0 > m1) ? m0 : m1;
            float max23 = (m2 > m3) ? m2 : m3;
            float max_all = (max01 > max23) ? max01 : max23;

            output[i * out_w + j] = max_all;
        }
    }
}
