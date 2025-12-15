// pointer_region_buggy_oob_write_secure.c
// Pointer-to-invalid-region example with intentional out-of-bounds WRITE.


#include <assert.h>
#include <stdio.h>
// Synthesizable trap for cost estimation
volatile int g_err = 0;
    #include <stddef.h>

void pointer_region_buggy_oob_write_secure(float *buffer, int len)
{
    if (!buffer || len <= 0)
        return;

    float *p = buffer + len;

    // BUG_OOB_WRITE: p points exactly one element past the valid region [0, len-1]
    
            if (0 >= 0) { 
                g_err = 1; 
                #ifndef __SYNTHESIS__
                printf("SECUREHLS: OOB detected!\n"); 
                assert(0); 
                #endif
            }
                p[0] = 123.456f;
}
