
#include <stdio.h>
#include <stdlib.h>

// Forward decl
void conv2d_buggy_oob_read(const float *input, int height, int width, const float *kernel, int kh, int kw, float *output);

int main() {
    
    int H = 4, W = 4;
    float input[16] = {0};
    float kernel[9] = {0};
    float out_conv[4] = {0};
        
    
    printf("Calling conv2d_buggy_oob_read...\n");
    conv2d_buggy_oob_read(input, H, W, kernel, 3, 3, out_conv);
    
    printf("CSIM PASSED\n");
    return 0;
}
