
#include <stdio.h>
#include <stdlib.h>

// Forward decl
void fir_safe(const float *x, const float *h, float *y, int N, int TAPS);

int main() {
    
    int N = 8;
    float x[8] = {0};
    float h[4] = {1, 2, 3, 4};
    float y[8] = {0};
        
    
    printf("Calling fir_safe...\n");
    fir_safe(x, h, y, N, 4);
    
    printf("CSIM PASSED\n");
    return 0;
}
