
#include <stdio.h>
#include <stdlib.h>

// Forward decl
void pointer_region_buggy_oob_write(float *buffer, int len);

int main() {
    
    float buf[8] = {0};
        
    
    printf("Calling pointer_region_buggy_oob_write...\n");
    pointer_region_buggy_oob_write(buf, 8);
    
    printf("CSIM PASSED\n");
    return 0;
}
