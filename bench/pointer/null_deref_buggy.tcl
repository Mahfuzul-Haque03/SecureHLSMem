
open_project proj_null_deref_buggy
set_top null_deref_buggy
add_files /home/rubel/Documents/secureHLSMem/bench/pointer/null_deref_buggy.c
add_files -tb /home/rubel/Documents/secureHLSMem/bench/pointer/null_deref_buggy_tb.c
open_solution "solution1"
set_part {xczu3eg-sbva484-1-e}
create_clock -period 3.33 -name default
csynth_design
csim_design
close_project
exit
