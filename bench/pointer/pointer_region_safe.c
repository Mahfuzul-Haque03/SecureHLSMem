// pointer_region_safe.c
// Safe pointer-region example.

#include <stddef.h>

void pointer_region_safe(float *buffer, int len)
{
    if (!buffer || len <= 0)
        return;

    for (int i = 0; i < len; ++i) {
        buffer[i] = (float)i;
    }
}
