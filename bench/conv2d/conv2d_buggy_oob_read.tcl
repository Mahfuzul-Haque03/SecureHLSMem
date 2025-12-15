
open_project proj_conv2d_buggy_oob_read
set_top conv2d_buggy_oob_read
add_files /home/rubel/Documents/secureHLSMem/bench/conv2d/conv2d_buggy_oob_read.c
add_files -tb /home/rubel/Documents/secureHLSMem/bench/conv2d/conv2d_buggy_oob_read_tb.c
open_solution "solution1"
set_part {xczu3eg-sbva484-1-e}
create_clock -period 3.33 -name default
csynth_design
csim_design
close_project
exit
