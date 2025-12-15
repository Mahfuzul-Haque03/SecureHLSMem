// maxpool2x2_buggy_oob_write.c
// MaxPool2x2 with intentional out-of-bounds WRITE into output.

#include <stddef.h>

void maxpool2x2_buggy_oob_write(const float *input,
                                int height,
                                int width,
                                float *output)
{
    if (!input || !output || height <= 1 || width <= 1)
        return;

    int out_h = height / 2;
    int out_w = width  / 2;

    // BUG_OOB_WRITE: iterate over height and width instead of out_h/out_w,
    // and use i,j directly for output indexing.
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int r0 = (i / 2) * 2;
            int c0 = (j / 2) * 2;

            int idx0 = r0 * width + c0;
            int idx1 = r0 * width + (c0 + 1 < width ? c0 + 1 : c0);
            int idx2 = (r0 + 1 < height ? r0 + 1 : r0) * width + c0;
            int idx3 = (r0 + 1 < height ? r0 + 1 : r0) * width +
                       (c0 + 1 < width ? c0 + 1 : c0);

            float m0 = input[idx0];
            float m1 = input[idx1];
            float m2 = input[idx2];
            float m3 = input[idx3];

            float max01 = (m0 > m1) ? m0 : m1;
            float max23 = (m2 > m3) ? m2 : m3;
            float max_all = (max01 > max23) ? max01 : max23;

            int out_i = i;   // can exceed [0, out_h)
            int out_j = j;   // can exceed [0, out_w)
            int out_idx = out_i * out_w + out_j; // BUG_OOB_WRITE if out_i/out_j too large
            output[out_idx] = max_all;
        }
    }
}
