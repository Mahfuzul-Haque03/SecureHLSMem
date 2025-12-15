import subprocess
import os

tcl_file = "bench/fir/fir_buggy_oob_read_secure.tcl"

# Run vitis_hls
cmd = ["vitis_hls", "-f", tcl_file]
print(f"Running debug Vitis for {tcl_file}...")
res = subprocess.run(cmd, capture_output=True, text=True)

print("Vitis Exit Code:", res.returncode)
print("STDOUT TAIL:")
print("\n".join(res.stdout.splitlines()[-20:]))

# Check report existence
func_name = "fir_buggy_oob_read_secure"
proj_name = f"proj_{func_name}"
report_path = f"{proj_name}/solution1/syn/report/{func_name}_csynth.xml"

if os.path.exists(report_path):
    print(f"Report FOUND at {report_path}")
    # Print content snippet?
else:
    print(f"Report NOT FOUND at {report_path}")
    # List directory components
    if os.path.exists(proj_name):
        print(f"Project dir {proj_name} exists.")
        print("Contents:", os.listdir(proj_name))
        if os.path.exists(f"{proj_name}/solution1"):
             print("solution1 contents:", os.listdir(f"{proj_name}/solution1"))
    else:
        print(f"Project dir {proj_name} DOES NOT EXIST.")
