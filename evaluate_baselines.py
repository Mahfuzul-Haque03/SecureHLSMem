#!/usr/bin/env python3
import json
import os
import re
from collections import defaultdict, namedtuple

ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
LOG_DIR = os.path.join(ROOT_DIR, "baseline_logs")

Benchmark = namedtuple("Benchmark", ["id", "file", "function", "bugs"])

# Simple mapping of substrings to bug types for classification
BUG_KEYWORDS = {
    "OOB_READ":  ["out of bounds", "array index out of bounds", "out-of-bounds", "buffer overrun"],
    "OOB_WRITE": ["out of bounds", "buffer overflow", "array index out of bounds", "out-of-bounds"],
    "NULL_DEREF": ["null pointer", "dereference of null", "NULL dereference"]
}

TOOLS = ["clang", "cppcheck", "scanbuild"]


def load_ground_truth(path):
    with open(path, "r") as f:
        data = json.load(f)

    benches = {}
    for b in data["benchmarks"]:
        benches[b["file"]] = Benchmark(
            id=b["id"],
            file=b["file"],
            function=b.get("function", ""),
            bugs=b.get("bugs", [])
        )
    return benches


def classify_bug_type(message):
    msg_lower = message.lower()
    for bug_type, keywords in BUG_KEYWORDS.items():
        for kw in keywords:
            if kw in msg_lower:
                return bug_type
    return None


def parse_clang_log(path):
    """
    Very simple parser: each warning/error line usually contains:
      file:line:col: warning: message
    We just grab the message to classify bug type.
    """
    reports = []
    with open(path, "r") as f:
        for line in f:
            if "warning:" in line or "error:" in line:
                reports.append(line.strip())
    return reports


def parse_cppcheck_log(path):
    """
    We used template: {id}::{file}::{line}::{message}
    So we can split on '::'.
    """
    reports = []
    with open(path, "r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split("::", 3)
            if len(parts) == 4:
                _, file_path, line_no, message = parts
                reports.append(message.strip())
            else:
                reports.append(line)
    return reports


def parse_scanbuild_log(path):
    """
    scan-build prints diagnostics similar to clang.
    We can reuse the clang parser here.
    """
    return parse_clang_log(path)


PARSERS = {
    "clang": parse_clang_log,
    "cppcheck": parse_cppcheck_log,
    "scanbuild": parse_scanbuild_log,
}


def evaluate_tool(tool, benches):
    tp = 0
    fp = 0
    fn = 0

    # Build a quick lookup: file -> set of bug types present
    file_to_bugtypes = {}
    for b in benches.values():
        if not b.bugs:
            file_to_bugtypes[b.file] = set()
        else:
            file_to_bugtypes[b.file] = set(bug["bug_type"] for bug in b.bugs)

    for rel_path, bench in benches.items():
        safe_bugtypes = file_to_bugtypes[rel_path]
        # log file name: replace '/' with '_' to match run_baselines.sh
        log_name = rel_path.replace("/", "_") + ".log"
        log_path = os.path.join(LOG_DIR, tool, log_name)

        if not os.path.exists(log_path):
            # No log = no warnings; any bugs in this file are FN
            fn += len(safe_bugtypes)
            continue

        parser = PARSERS[tool]
        messages = parser(log_path)

        detected_bugtypes = set()
        for msg in messages:
            bug_type = classify_bug_type(msg)
            if bug_type is None:
                # unrelated warning -> FP
                fp += 1
            else:
                detected_bugtypes.add(bug_type)

        # For each known bug type in this file:
        for bug_type in safe_bugtypes:
            if bug_type in detected_bugtypes:
                tp += 1
            else:
                fn += 1

        # Any detected bug type not actually present in this file is also FP
        for bug_type in detected_bugtypes:
            if bug_type not in safe_bugtypes:
                fp += 1

        # For safe benchmarks (no bugs), all detections are FP.
        if not safe_bugtypes:
            # All detections in this file were already counted as FP above
            pass

    precision = tp / (tp + fp) if (tp + fp) > 0 else 0.0
    recall = tp / (tp + fn) if (tp + fn) > 0 else 0.0

    return {
        "tp": tp,
        "fp": fp,
        "fn": fn,
        "precision": precision,
        "recall": recall
    }


def cleanup_artifacts():
    print("\nCleaning up logs and temporary files...")
    if os.path.exists(LOG_DIR):
        import shutil
        shutil.rmtree(LOG_DIR)
        print(f"Removed {LOG_DIR}")
    
    # Remove .o files in root dir
    for f in os.listdir(ROOT_DIR):
        if f.endswith(".o"):
            os.remove(os.path.join(ROOT_DIR, f))
            print(f"Removed {f}")


def main():
    gt_path = os.path.join(ROOT_DIR, "ground_truth.json")
    benches = load_ground_truth(gt_path)

    print("Evaluating baseline tools...")
    for tool in TOOLS:
        res = evaluate_tool(tool, benches)
        print(f"\n=== {tool} ===")
        print(f"TP = {res['tp']}")
        print(f"FP = {res['fp']}")
        print(f"FN = {res['fn']}")
        print(f"Precision = {res['precision']:.3f}")
        print(f"Recall    = {res['recall']:.3f}")

    cleanup_artifacts()


if __name__ == "__main__":
    main()
