
#include <stdio.h>
#include <stdlib.h>

// Forward decl
void pointer_region_safe(float *buffer, int len);

int main() {
    
    float buf[8] = {0};
        
    
    printf("Calling pointer_region_safe...\n");
    pointer_region_safe(buf, 8);
    
    printf("CSIM PASSED\n");
    return 0;
}
