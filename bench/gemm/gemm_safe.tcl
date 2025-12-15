
open_project proj_gemm_safe
set_top gemm_safe
add_files /home/rubel/Documents/secureHLSMem/bench/gemm/gemm_safe.c
add_files -tb /home/rubel/Documents/secureHLSMem/bench/gemm/gemm_safe_tb.c
open_solution "solution1"
set_part {xczu3eg-sbva484-1-e}
create_clock -period 3.33 -name default
csynth_design
csim_design
close_project
exit
