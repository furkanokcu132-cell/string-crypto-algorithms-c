#!/usr/bin/env python3
"""
Autograder for COMP 201 – Spring 2026 – Assignment 1
=====================================================
Compiles the student's main.c, runs all 5 tasks against a chosen test case,
and diffs each output against the expected ground truth.

Usage:
    python3 autograder.py                    # runs all 3 test cases
    python3 autograder.py easy               # runs only the easy test case
    python3 autograder.py medium
    python3 autograder.py hard
"""

import os, sys, subprocess, difflib, shutil

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
MAIN_C   = os.path.join(BASE_DIR, "main.c")
ENIGMA   = os.path.join(BASE_DIR, "enigma")

# Points per task
POINTS = {
    "task1": 10,
    "task2a": 5,
    "task2b": 5,
    "task3_decrypted": 10,
    "task3_results": 10,
    "task4_decrypted": 15,
    "task4_results": 15,
    "task5_decrypted": 15,
    "task5_results": 15,
}

# ANSI colors
GREEN  = "\033[92m"
RED    = "\033[91m"
YELLOW = "\033[93m"
CYAN   = "\033[96m"
BOLD   = "\033[1m"
RESET  = "\033[0m"

# ═══════════════════════════════════════════════════════════════════════════
#  Helpers
# ═══════════════════════════════════════════════════════════════════════════

def compile_program():
    """Compile main.c → enigma. Returns True on success."""
    print(f"\n{BOLD}{'='*70}{RESET}")
    print(f"{BOLD}  Compiling main.c ...{RESET}")
    print(f"{BOLD}{'='*70}{RESET}")
    result = subprocess.run(
        ["gcc", MAIN_C, "-o", ENIGMA],
        capture_output=True, text=True
    )
    if result.returncode != 0:
        print(f"{RED}  ✗ Compilation FAILED{RESET}")
        print(result.stderr)
        return False
    print(f"{GREEN}  ✓ Compiled successfully{RESET}")
    return True


def run_enigma(args, label=""):
    """Run ./enigma with given args. Returns (success, stdout, stderr)."""
    cmd = [ENIGMA] + args
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
        if result.returncode != 0:
            print(f"  {RED}✗ {label} — program returned exit code {result.returncode}{RESET}")
            if result.stderr.strip():
                print(f"    stderr: {result.stderr.strip()}")
            return False, result.stdout, result.stderr
        return True, result.stdout, result.stderr
    except subprocess.TimeoutExpired:
        print(f"  {RED}✗ {label} — timed out (>30s){RESET}")
        return False, "", "timeout"


def read_file(path):
    """Read file contents, return empty string if missing."""
    try:
        with open(path, 'r') as f:
            return f.read()
    except FileNotFoundError:
        return None


def compare_files(expected_path, actual_path, label):
    """
    Compare expected vs actual file. Returns (passed: bool, details: str).
    On mismatch, prints a character-level diff showing exactly where they diverge.
    """
    expected = read_file(expected_path)
    actual   = read_file(actual_path)

    if actual is None:
        return False, f"Output file not created: {actual_path}"

    if expected is None:
        return False, f"Expected file missing: {expected_path}"

    if expected == actual:
        return True, ""

    # ── Build detailed mismatch report ────────────────────────────────
    details = []

    # Line-level unified diff
    exp_lines = expected.splitlines(keepends=True)
    act_lines = actual.splitlines(keepends=True)
    diff = list(difflib.unified_diff(
        exp_lines, act_lines,
        fromfile="expected", tofile="actual", lineterm=""
    ))
    if diff:
        details.append("  Line-level diff (first 30 lines):")
        for line in diff[:30]:
            line = line.rstrip('\n')
            if line.startswith('+') and not line.startswith('+++'):
                details.append(f"    {GREEN}{line}{RESET}")
            elif line.startswith('-') and not line.startswith('---'):
                details.append(f"    {RED}{line}{RESET}")
            else:
                details.append(f"    {line}")

    # Character-level: find first mismatch
    min_len = min(len(expected), len(actual))
    first_diff_pos = None
    for i in range(min_len):
        if expected[i] != actual[i]:
            first_diff_pos = i
            break
    if first_diff_pos is None and len(expected) != len(actual):
        first_diff_pos = min_len  # one string is a prefix of the other

    if first_diff_pos is not None:
        # Count line/col of first mismatch
        line_no = expected[:first_diff_pos].count('\n') + 1
        last_nl = expected[:first_diff_pos].rfind('\n')
        col = first_diff_pos - last_nl - 1 if last_nl >= 0 else first_diff_pos

        details.append("")
        details.append(f"  {YELLOW}First mismatch at character position {first_diff_pos} "
                        f"(line {line_no}, col {col}):{RESET}")

        # Show context around the mismatch
        ctx_start = max(0, first_diff_pos - 20)
        ctx_end   = min(max(len(expected), len(actual)), first_diff_pos + 40)

        def show_char(c):
            if c == '\n':   return '\\n'
            if c == '\t':   return '\\t'
            if c == ' ':    return '·'
            return c

        exp_ctx = ''.join(show_char(c) for c in expected[ctx_start:ctx_end])
        act_ctx = ''.join(show_char(c) for c in actual[ctx_start:min(len(actual), ctx_end)])

        # Pointer to mismatch position within context
        pointer_offset = sum(
            len(show_char(c)) for c in expected[ctx_start:first_diff_pos]
        )

        details.append(f"    Expected: ...{exp_ctx}...")
        details.append(f"    Actual:   ...{act_ctx}...")
        details.append(f"              {'':>{pointer_offset + 3}}^")

        if first_diff_pos < len(expected) and first_diff_pos < len(actual):
            exp_ch = repr(expected[first_diff_pos])
            act_ch = repr(actual[first_diff_pos])
            details.append(f"    Expected char: {exp_ch}  |  Actual char: {act_ch}")
        elif first_diff_pos >= len(actual):
            details.append(f"    Actual file is shorter (len={len(actual)}) "
                           f"than expected (len={len(expected)})")
        else:
            details.append(f"    Actual file is longer (len={len(actual)}) "
                           f"than expected (len={len(expected)})")

    if len(expected) != len(actual):
        details.append(f"  File lengths differ: expected={len(expected)}, actual={len(actual)}")

    return False, '\n'.join(details)


# ═══════════════════════════════════════════════════════════════════════════
#  Grade one test case
# ═══════════════════════════════════════════════════════════════════════════

def get_test_params(difficulty):
    """Return task-specific parameters (keys, known words, etc.)."""
    params = {
        "easy": {
            "task1_known_word": "ATTACK",
            "task2_key": "KEY",
            "task4_key_length": "3",
        },
        "medium": {
            "task1_known_word": "RETREAT",
            "task2_key": "CIPHER",
            "task4_key_length": "6",
        },
        "hard": {
            "task1_known_word": "ENIGMA",
            "task2_key": "BLETCHLEY",
            "task4_key_length": "6",
        },
    }
    return params[difficulty]


def grade_test_case(difficulty):
    """Grade all tasks for a given difficulty. Returns score dict."""
    test_dir = os.path.join(BASE_DIR, f"test_{difficulty}")
    inp_dir  = os.path.join(test_dir, "input")
    exp_dir  = os.path.join(test_dir, "expected_output")
    out_dir  = os.path.join(test_dir, "student_output")

    # Clean & prepare output directory
    if os.path.exists(out_dir):
        shutil.rmtree(out_dir)
    os.makedirs(out_dir)

    params = get_test_params(difficulty)
    scores = {}
    total_possible = 0
    total_earned   = 0

    print(f"\n{BOLD}{CYAN}{'='*70}{RESET}")
    print(f"{BOLD}{CYAN}  Grading: {difficulty.upper()} test case{RESET}")
    print(f"{BOLD}{CYAN}{'='*70}{RESET}")

    # ── Task 1 ────────────────────────────────────────────────────────
    print(f"\n{BOLD}  Task 1: Caesar Known-Word Attack (10 pts){RESET}")
    ok, _, _ = run_enigma([
        "caesar-known",
        os.path.join(inp_dir, "message1_encrypted.txt"),
        params["task1_known_word"],
        os.path.join(out_dir, "message1_decrypted.txt"),
    ], "Task 1")
    if ok:
        passed, details = compare_files(
            os.path.join(exp_dir, "message1_decrypted.txt"),
            os.path.join(out_dir, "message1_decrypted.txt"),
            "Task 1"
        )
        pts = POINTS["task1"] if passed else 0
        scores["task1"] = pts
        if passed:
            print(f"  {GREEN}✓ PASSED — {pts}/{POINTS['task1']} pts{RESET}")
        else:
            print(f"  {RED}✗ FAILED — 0/{POINTS['task1']} pts{RESET}")
            print(details)
    else:
        scores["task1"] = 0

    # ── Task 2a ───────────────────────────────────────────────────────
    print(f"\n{BOLD}  Task 2a: Vigenère Encrypt (5 pts){RESET}")
    ok, _, _ = run_enigma([
        "vigenere-encrypt",
        os.path.join(inp_dir, "message2_plaintext.txt"),
        params["task2_key"],
        os.path.join(out_dir, "message2_encrypted.txt"),
    ], "Task 2a")
    if ok:
        passed, details = compare_files(
            os.path.join(exp_dir, "message2_encrypted.txt"),
            os.path.join(out_dir, "message2_encrypted.txt"),
            "Task 2a"
        )
        pts = POINTS["task2a"] if passed else 0
        scores["task2a"] = pts
        if passed:
            print(f"  {GREEN}✓ PASSED — {pts}/{POINTS['task2a']} pts{RESET}")
        else:
            print(f"  {RED}✗ FAILED — 0/{POINTS['task2a']} pts{RESET}")
            print(details)
    else:
        scores["task2a"] = 0

    # ── Task 2b ───────────────────────────────────────────────────────
    print(f"\n{BOLD}  Task 2b: Vigenère Decrypt (5 pts){RESET}")
    ok, _, _ = run_enigma([
        "vigenere-decrypt",
        os.path.join(inp_dir, "message3_encrypted.txt"),
        params["task2_key"],
        os.path.join(out_dir, "message3_decrypted.txt"),
    ], "Task 2b")
    if ok:
        passed, details = compare_files(
            os.path.join(exp_dir, "message3_decrypted.txt"),
            os.path.join(out_dir, "message3_decrypted.txt"),
            "Task 2b"
        )
        pts = POINTS["task2b"] if passed else 0
        scores["task2b"] = pts
        if passed:
            print(f"  {GREEN}✓ PASSED — {pts}/{POINTS['task2b']} pts{RESET}")
        else:
            print(f"  {RED}✗ FAILED — 0/{POINTS['task2b']} pts{RESET}")
            print(details)
    else:
        scores["task2b"] = 0

    # ── Task 3 ────────────────────────────────────────────────────────
    print(f"\n{BOLD}  Task 3: Caesar Frequency Analysis (20 pts){RESET}")
    ok, _, _ = run_enigma([
        "caesar-freq",
        os.path.join(inp_dir, "message4_encrypted.txt"),
        os.path.join(out_dir, "message4_decrypted.txt"),
        os.path.join(out_dir, "message4_results.txt"),
    ], "Task 3")
    if ok:
        # Decrypted text (10 pts)
        passed_d, details_d = compare_files(
            os.path.join(exp_dir, "message4_decrypted.txt"),
            os.path.join(out_dir, "message4_decrypted.txt"),
            "Task 3 decrypted"
        )
        pts_d = POINTS["task3_decrypted"] if passed_d else 0
        scores["task3_decrypted"] = pts_d
        if passed_d:
            print(f"  {GREEN}✓ Decrypted text PASSED — {pts_d}/{POINTS['task3_decrypted']} pts{RESET}")
        else:
            print(f"  {RED}✗ Decrypted text FAILED — 0/{POINTS['task3_decrypted']} pts{RESET}")
            print(details_d)

        # Results file (10 pts)
        passed_r, details_r = compare_files(
            os.path.join(exp_dir, "message4_results.txt"),
            os.path.join(out_dir, "message4_results.txt"),
            "Task 3 results"
        )
        pts_r = POINTS["task3_results"] if passed_r else 0
        scores["task3_results"] = pts_r
        if passed_r:
            print(f"  {GREEN}✓ Results file PASSED — {pts_r}/{POINTS['task3_results']} pts{RESET}")
        else:
            print(f"  {RED}✗ Results file FAILED — 0/{POINTS['task3_results']} pts{RESET}")
            print(details_r)
    else:
        scores["task3_decrypted"] = 0
        scores["task3_results"]   = 0

    # ── Task 4 ────────────────────────────────────────────────────────
    print(f"\n{BOLD}  Task 4: Vigenère Key Recovery (30 pts){RESET}")
    ok, _, _ = run_enigma([
        "vigenere-freq",
        os.path.join(inp_dir, "message5_encrypted.txt"),
        params["task4_key_length"],
        os.path.join(out_dir, "message5_decrypted.txt"),
        os.path.join(out_dir, "message5_results.txt"),
    ], "Task 4")
    if ok:
        passed_d, details_d = compare_files(
            os.path.join(exp_dir, "message5_decrypted.txt"),
            os.path.join(out_dir, "message5_decrypted.txt"),
            "Task 4 decrypted"
        )
        pts_d = POINTS["task4_decrypted"] if passed_d else 0
        scores["task4_decrypted"] = pts_d
        if passed_d:
            print(f"  {GREEN}✓ Decrypted text PASSED — {pts_d}/{POINTS['task4_decrypted']} pts{RESET}")
        else:
            print(f"  {RED}✗ Decrypted text FAILED — 0/{POINTS['task4_decrypted']} pts{RESET}")
            print(details_d)

        passed_r, details_r = compare_files(
            os.path.join(exp_dir, "message5_results.txt"),
            os.path.join(out_dir, "message5_results.txt"),
            "Task 4 results"
        )
        pts_r = POINTS["task4_results"] if passed_r else 0
        scores["task4_results"] = pts_r
        if passed_r:
            print(f"  {GREEN}✓ Results file PASSED — {pts_r}/{POINTS['task4_results']} pts{RESET}")
        else:
            print(f"  {RED}✗ Results file FAILED — 0/{POINTS['task4_results']} pts{RESET}")
            print(details_r)
    else:
        scores["task4_decrypted"] = 0
        scores["task4_results"]   = 0

    # ── Task 5 ────────────────────────────────────────────────────────
    print(f"\n{BOLD}  Task 5: Caesar Word Scoring (30 pts){RESET}")
    ok, _, _ = run_enigma([
        "caesar-words",
        os.path.join(inp_dir, "message6_encrypted.txt"),
        os.path.join(inp_dir, "common_words.txt"),
        os.path.join(out_dir, "message6_decrypted.txt"),
        os.path.join(out_dir, "message6_results.txt"),
    ], "Task 5")
    if ok:
        passed_d, details_d = compare_files(
            os.path.join(exp_dir, "message6_decrypted.txt"),
            os.path.join(out_dir, "message6_decrypted.txt"),
            "Task 5 decrypted"
        )
        pts_d = POINTS["task5_decrypted"] if passed_d else 0
        scores["task5_decrypted"] = pts_d
        if passed_d:
            print(f"  {GREEN}✓ Decrypted text PASSED — {pts_d}/{POINTS['task5_decrypted']} pts{RESET}")
        else:
            print(f"  {RED}✗ Decrypted text FAILED — 0/{POINTS['task5_decrypted']} pts{RESET}")
            print(details_d)

        passed_r, details_r = compare_files(
            os.path.join(exp_dir, "message6_results.txt"),
            os.path.join(out_dir, "message6_results.txt"),
            "Task 5 results"
        )
        pts_r = POINTS["task5_results"] if passed_r else 0
        scores["task5_results"] = pts_r
        if passed_r:
            print(f"  {GREEN}✓ Results file PASSED — {pts_r}/{POINTS['task5_results']} pts{RESET}")
        else:
            print(f"  {RED}✗ Results file FAILED — 0/{POINTS['task5_results']} pts{RESET}")
            print(details_r)
    else:
        scores["task5_decrypted"] = 0
        scores["task5_results"]   = 0

    # ── Summary ───────────────────────────────────────────────────────
    total_earned   = sum(scores.values())
    total_possible = sum(POINTS.values())
    print(f"\n{BOLD}{'─'*70}{RESET}")
    print(f"{BOLD}  {difficulty.upper()} TOTAL: {total_earned} / {total_possible}{RESET}")

    # Per-task summary
    task_groups = [
        ("Task 1 (Caesar Known-Word)", ["task1"]),
        ("Task 2 (Vigenère Enc+Dec)",  ["task2a", "task2b"]),
        ("Task 3 (Caesar Frequency)",  ["task3_decrypted", "task3_results"]),
        ("Task 4 (Vigenère Recovery)", ["task4_decrypted", "task4_results"]),
        ("Task 5 (Caesar Word Score)", ["task5_decrypted", "task5_results"]),
    ]
    for name, keys in task_groups:
        earned   = sum(scores[k] for k in keys)
        possible = sum(POINTS[k] for k in keys)
        status = f"{GREEN}✓{RESET}" if earned == possible else f"{RED}✗{RESET}"
        print(f"    {status} {name}: {earned}/{possible}")

    print(f"{BOLD}{'─'*70}{RESET}")
    return scores


# ═══════════════════════════════════════════════════════════════════════════
#  Main
# ═══════════════════════════════════════════════════════════════════════════

if __name__ == "__main__":
    if not compile_program():
        sys.exit(1)

    if len(sys.argv) > 1:
        targets = list(sys.argv[1:])
    else:
        targets = ["easy", "medium", "hard"]

    all_scores = {}
    for t in targets:
        if t not in ("easy", "medium", "hard"):
            print(f"Unknown difficulty: {t}")
            sys.exit(1)
        if not os.path.isdir(os.path.join(BASE_DIR, f"test_{t}")):
            print(f"Test case directory test_{t}/ not found. "
                  f"Run generate_ground_truth.py first.")
            sys.exit(1)
        all_scores[t] = grade_test_case(t)

    # Overall summary
    if len(targets) > 1:
        print(f"\n{BOLD}{CYAN}{'='*70}{RESET}")
        print(f"{BOLD}{CYAN}  OVERALL SUMMARY{RESET}")
        print(f"{BOLD}{CYAN}{'='*70}{RESET}")
        for t in targets:
            earned = sum(all_scores[t].values())
            possible = sum(POINTS.values())
            pct = earned / possible * 100
            color = GREEN if pct == 100 else (YELLOW if pct >= 50 else RED)
            print(f"    {color}{t.upper():8s}: {earned:3d}/{possible} ({pct:.0f}%){RESET}")
        print()
