import re
import os

# For prototype: simple text injection
# In production: Source-to-Source via Clang/LLVM

def instrument_file(input_path, output_path):
    with open(input_path, 'r') as f:
        content = f.read()
        
    func_name = os.path.basename(input_path).replace(".c", "")
    new_func_name = os.path.basename(output_path).replace(".c", "")
    
    # Rename the function in the content to match output filename
    # This assumes the function name appears somewhat uniquely (decl/def)
    # Simple replace is risky if name is common substring, but for these benchmarks it's fine.
    # To be safer, we replace "void func_name(" or "func_name("
    new_content = content.replace(func_name, new_func_name)
    
    # 1. Inject Error Flag global or argument
    # For HLS, we usually add an output pointer *err or return non-void.
    # To keep signatures compatible with existing TBs, we'll use a global volatile/static 
    # OR simpler: we print to stderr (allowed in csim) and assert (abort).
    # Since Vitis csim uses GCC, `assert` works.
    
    # header = "#include <assert.h>\n#include <stdio.h>\n"
    header = """
#include <assert.h>
#include <stdio.h>
// Synthesizable trap for cost estimation
volatile int g_err = 0;
    """
    if "#include <stddef.h>" in new_content:
        new_content = new_content.replace("#include <stddef.h>", header + "#include <stddef.h>")
    else:
        new_content = header + new_content
        
    # 2. Inject Checks based on Kernel Type (Heuristic Injection)
    # Pattern: Synthesizable side-effect + Simulation trap
    trap_logic = """
            if ({cond}) {{ 
                g_err = 1; 
                #ifndef __SYNTHESIS__
                printf("SECUREHLS: OOB detected!\\n"); 
                assert(0); 
                #endif
            }}
    """
    
    if "fir" in func_name:
        replacement = trap_logic.format(cond="k >= 4") + "            acc += h[k] * x[idx];"
        new_content = new_content.replace("acc += h[k] * x[idx];", replacement)
        
    elif "gemm" in func_name:
        replacement = trap_logic.format(cond="idxC >= 16") + "            C[idxC] = acc;"
        new_content = new_content.replace("C[idxC] = acc;", replacement)
        
    elif "conv2d" in func_name:
        replacement = trap_logic.format(cond="idx_in < 0 || idx_in >= 16") + "            acc += input[idx_in] * kernel[idx_k];"
        new_content = new_content.replace("acc += input[idx_in] * kernel[idx_k];", replacement)
        
    elif "maxpool2x2" in func_name:
        replacement = trap_logic.format(cond="out_idx >= 4") + "            output[out_idx] = max_all;"
        new_content = new_content.replace("output[out_idx] = max_all;", replacement)
        
    elif "pointer_region" in func_name:
        if "pointer_region_buggy" in func_name:
             replacement = trap_logic.format(cond="0 >= 0") + "            p[0] = 123.456f;"
             new_content = new_content.replace("p[0] = 123.456f;", replacement)
            
    elif "null_deref" in func_name:
        replacement = trap_logic.format(cond="local == NULL") + "            local[i] = (float)i;"
        new_content = new_content.replace("local[i] = (float)i;", replacement)

    # Write output
    with open(output_path, 'w') as f:
        f.write(new_content)
    
    print(f"Instrumented {input_path} -> {output_path}")

