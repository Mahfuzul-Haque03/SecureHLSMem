// null_deref_buggy_secure.c
// Intentional NULL pointer dereference pattern.


#include <assert.h>
#include <stdio.h>
// Synthesizable trap for cost estimation
volatile int g_err = 0;
    #include <stddef.h>

void null_deref_buggy_secure(float *buffer, int len, int use_null)
{
    float *local = buffer;

    if (use_null) {
        local = NULL;
    }

    // BUG_NULL_DEREF: potential NULL pointer dereference when use_null != 0
    for (int i = 0; i < len; ++i) {
        
            if (local == NULL) { 
                g_err = 1; 
                #ifndef __SYNTHESIS__
                printf("SECUREHLS: OOB detected!\n"); 
                assert(0); 
                #endif
            }
                local[i] = (float)i;
    }
}
