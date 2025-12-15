import sys
import os
import argparse
import glob
from securehls.static import analyze_file
from securehls.instrument import instrument_file

BENCH_DIR = "bench"

def cmd_check(args):
    print("Running SecureHLS Static Analyzer...")
    # Find all .c files
    files = glob.glob(f"{BENCH_DIR}/**/*.c", recursive=True)
    files = [f for f in files if "tb.c" not in f and "main_test.c" not in f and "secure" not in f]
    
    tp = 0
    fp = 0
    fn = 0
    
    # Ground truth knowledge hardcoded for Prototype evaluation
    # In production, this would read ground_truth.json
    BUGGY_FILES = [
        "fir_buggy", "gemm_buggy", "conv2d_buggy", 
        "maxpool2x2_buggy", "pointer_region_buggy", "null_deref_buggy"
    ]
    
    for f in files:
        bugs = analyze_file(f)
        is_buggy_file = any(b in f for b in BUGGY_FILES)
        
        if bugs:
            print(f"[BUG FOUND] {f}:")
            for b in bugs:
                print(f"  Line {b['line']}: {b['msg']}")
            
            if is_buggy_file:
                tp += 1
            else:
                fp += 1
        else:
            if is_buggy_file:
                print(f"[MISS] {f}")
                fn += 1
            else:
                pass # True Negative
                
    # Metrics
    precision = tp / (tp + fp) if (tp + fp) > 0 else 0
    recall = tp / (tp + fn) if (tp + fn) > 0 else 0
    
    print("\n=== SecureHLS Static Results ===")
    print(f"TP: {tp}, FP: {fp}, FN: {fn}")
    print(f"Precision: {precision:.2f}")
    print(f"Recall: {recall:.2f}")

def cmd_instrument(args):
    print("Running SecureHLS Instrumentation (BCU Injection)...")
    # Only instrument buggy files for the experiment
    files = glob.glob(f"{BENCH_DIR}/**/*.c", recursive=True)
    files = [f for f in files if "buggy" in f and "tb.c" not in f and "secure" not in f]
    
    for f in files:
        # Create output filename: e.g. fir_buggy_oob_read_secure.c
        out_f = f.replace(".c", "_secure.c")
        instrument_file(f, out_f)

def main():
    parser = argparse.ArgumentParser(description="SecureHLS Toolchain")
    subparsers = parser.add_subparsers(dest="command")
    
    # check command
    p_check = subparsers.add_parser("check", help="Run static analysis")
    
    # instrument command
    p_inst = subparsers.add_parser("instrument", help="Instrument code with BCU checks")
    p_inst.add_argument("--mode", default="bcu", help="Instrumentation mode")
    
    args = parser.parse_args()
    
    if args.command == "check":
        cmd_check(args)
    elif args.command == "instrument":
        cmd_instrument(args)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
