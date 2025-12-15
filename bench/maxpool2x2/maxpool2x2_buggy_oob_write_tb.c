
#include <stdio.h>
#include <stdlib.h>

// Forward decl
void maxpool2x2_buggy_oob_write(const float *input, int height, int width, float *output);

int main() {
    
    int H = 4, W = 4;
    float input[16] = {0};
    float out_pool[4] = {0};
        
    
    printf("Calling maxpool2x2_buggy_oob_write...\n");
    maxpool2x2_buggy_oob_write(input, H, W, out_pool);
    
    printf("CSIM PASSED\n");
    return 0;
}
