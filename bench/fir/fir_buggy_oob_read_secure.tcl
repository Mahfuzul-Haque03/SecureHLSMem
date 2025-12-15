
open_project proj_fir_buggy_oob_read_secure
set_top fir_buggy_oob_read_secure
add_files /home/rubel/Documents/secureHLSMem/bench/fir/fir_buggy_oob_read_secure.c
add_files -tb /home/rubel/Documents/secureHLSMem/bench/fir/fir_buggy_oob_read_secure_tb.c
open_solution "solution1"
set_part {xczu3eg-sbva484-1-e}
create_clock -period 3.33 -name default
csynth_design
csim_design
close_project
exit
