
open_project proj_pointer_region_buggy_oob_write_secure
set_top pointer_region_buggy_oob_write_secure
add_files /home/rubel/Documents/secureHLSMem/bench/pointer/pointer_region_buggy_oob_write_secure.c
add_files -tb /home/rubel/Documents/secureHLSMem/bench/pointer/pointer_region_buggy_oob_write_secure_tb.c
open_solution "solution1"
set_part {xczu3eg-sbva484-1-e}
create_clock -period 3.33 -name default
csynth_design
csim_design
close_project
exit
