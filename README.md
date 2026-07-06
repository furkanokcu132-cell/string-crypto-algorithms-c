[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/ox_qR6m_)
# COMP 201 – Spring 2026 – Assignment 1

## Repository Contents

- **`main.c`** — The code file you will implement. Contains the `main` function and stubs for all five tasks.
- **`test_easy/`**, **`test_medium/`**, **`test_hard/`** — Three test cases of increasing difficulty. Each contains:
  - `input/` — Input files to feed into your program
  - `expected_output/` — Ground truth outputs to compare against
- **`autograder.py`** — Automated grader that compiles, runs, and checks your solution

## Building

```bash
gcc main.c -o enigma
```

## Autograder

The autograder compiles your `main.c`, runs it against the test cases, and compares your output to the expected results. On any mismatch it shows exactly where the outputs differ (line, column, and the mismatched character).

```bash
# Run against all 3 test cases
python3 autograder.py

# Run against a specific difficulty
python3 autograder.py easy
python3 autograder.py medium
python3 autograder.py hard
```

Output is scored out of 100 points per test case, matching the assignment rubric.
