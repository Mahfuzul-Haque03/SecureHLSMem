// pointer_region_buggy_oob_write.c
// Pointer-to-invalid-region example with intentional out-of-bounds WRITE.

#include <stddef.h>

void pointer_region_buggy_oob_write(float *buffer, int len)
{
    if (!buffer || len <= 0)
        return;

    float *p = buffer + len;

    // BUG_OOB_WRITE: p points exactly one element past the valid region [0, len-1]
    p[0] = 123.456f;
}
