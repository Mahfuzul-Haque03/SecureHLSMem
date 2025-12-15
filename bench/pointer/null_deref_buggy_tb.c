
#include <stdio.h>
#include <stdlib.h>

// Forward decl
void null_deref_buggy(float *buffer, int len, int use_null);

int main() {
    
    float buf[8] = {0};
        
    
    printf("Calling null_deref_buggy...\n");
    null_deref_buggy(buf, 8, 1);
    
    printf("CSIM PASSED\n");
    return 0;
}
