import os
import re

BENCH_DIR = "bench"
TCL_TEMPLATE = """
open_project {proj_name}
set_top {top_func}
add_files {src_file}
add_files -tb {tb_file}
open_solution "solution1"
set_part {{xczu3eg-sbva484-1-e}}
create_clock -period 3.33 -name default
csynth_design
csim_design
close_project
exit
"""

# Map kernel logic from main_test.c to individual TBs
# We'll use a generic template where we can inject the specific calling logic
TB_TEMPLATE = """
#include <stdio.h>
#include <stdlib.h>

// Forward decl
void {top_func}({sig_args});

int main() {{
    {setup_logic}
    
    printf("Calling {top_func}...\\n");
    {call_logic};
    
    printf("CSIM PASSED\\n");
    return 0;
}}
"""

KERNELS = {
    "fir": {
        "args": "const float *x, const float *h, float *y, int N, int TAPS",
        "setup": """
    int N = 8;
    float x[8] = {0};
    float h[4] = {1, 2, 3, 4};
    float y[8] = {0};
        """,
        "call": "{func}(x, h, y, N, 4)"
    },
    "gemm": {
        "args": "const float *A, const float *B, float *C, int M, int K, int N",
        "setup": """
    int M = 4, K = 4, W = 4;
    float A[16] = {0};
    float B[16] = {0};
    float C[16] = {0};
        """,
        "call": "{func}(A, B, C, M, K, W)"
    },
    "conv2d": {
        "args": "const float *input, int height, int width, const float *kernel, int kh, int kw, float *output",
        "setup": """
    int H = 4, W = 4;
    float input[16] = {0};
    float kernel[9] = {0};
    float out_conv[4] = {0};
        """,
        "call": "{func}(input, H, W, kernel, 3, 3, out_conv)"
    },
    "maxpool2x2": {
        "args": "const float *input, int height, int width, float *output",
        "setup": """
    int H = 4, W = 4;
    float input[16] = {0};
    float out_pool[4] = {0};
        """,
        "call": "{func}(input, H, W, out_pool)"
    },
    "pointer": { # covers pointer_region
        "args": "float *buffer, int len",
        "setup": """
    float buf[8] = {0};
        """,
        "call": "{func}(buf, 8)"
    },
    "null_deref": { # special case in pointer dir
        "args": "float *buffer, int len, int use_null",
        "setup": """
    float buf[8] = {0};
        """,
        "call": "{func}(buf, 8, 1)" # 1 triggers the bug path
    }
}

def main():
    # Walk bench dir
    for root, dirs, files in os.walk(BENCH_DIR):
        for f in files:
            if not f.endswith(".c") or "tb.c" in f:
                continue
            
            # Identify kernel type
            kernel_type = None
            for k in KERNELS:
                if k in root or k in f:
                    kernel_type = k
                    break
            
            if not kernel_type: 
                print(f"Skipping {f}, unknown type")
                continue
                
            # Special handling for null_deref in pointer dir
            if "null_deref" in f:
                kernel_type = "null_deref"
            elif "pointer" in f:
                kernel_type = "pointer"

            func_name = f.replace(".c", "")
            src_path = os.path.join(root, f)
            tb_path = os.path.join(root, func_name + "_tb.c")
            
            # Generate TB
            info = KERNELS[kernel_type]
            tb_content = TB_TEMPLATE.format(
                top_func=func_name,
                sig_args=info["args"],
                setup_logic=info["setup"],
                call_logic=info["call"].format(func=func_name)
            )
            
            with open(tb_path, "w") as tf:
                tf.write(tb_content)
                
            # Generate TCL
            # Project name needs to be unique and simple
            proj_name = f"proj_{func_name}"
            # Absolute paths for TCL
            abs_src = os.path.abspath(src_path)
            abs_tb = os.path.abspath(tb_path)
            
            tcl_content = TCL_TEMPLATE.format(
                proj_name=proj_name,
                top_func=func_name,
                src_file=abs_src,
                tb_file=abs_tb
            )
            
            tcl_path = os.path.join(root, func_name + ".tcl")
            with open(tcl_path, "w") as tclf:
                tclf.write(tcl_content)
            
            print(f"Setup {func_name}: TB={tb_path}, TCL={tcl_path}")

if __name__ == "__main__":
    main()
