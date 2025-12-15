// null_deref_buggy.c
// Intentional NULL pointer dereference pattern.

#include <stddef.h>

void null_deref_buggy(float *buffer, int len, int use_null)
{
    float *local = buffer;

    if (use_null) {
        local = NULL;
    }

    // BUG_NULL_DEREF: potential NULL pointer dereference when use_null != 0
    for (int i = 0; i < len; ++i) {
        local[i] = (float)i;
    }
}
