#!/usr/bin/env python3
"""
Study A: effect of graph size at fixed 50% density, 6 sizes.
Study B: effect of density at one fixed size, 4 densities.

Each (problem, size, density) cell is run for ITERATIONS repetitions. The binary
is invoked with -a 0 -s 0 so every algorithm and both representations are timed
on the SAME generated graph per iteration. Every single
iteration is appended as its own CSV row.

The work is split into small batches (BATCH_SIZE iterations per process call) so
the output file is flushed on every process exit. A multi-hour run can therefore
be interrupted with Ctrl-C without losing the data already written, and re-running
the script resumes from where it stopped.
"""

import csv
import os
import subprocess
import sys
import time

# --- Configuration -----------------------------------------------------------

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
BINARY = os.path.join(ROOT, "build", "graphs")
RESULTS_DIR = os.path.join(ROOT, "results")

STUDY_A_CSV = os.path.join(RESULTS_DIR, "study_a.csv")
STUDY_B_CSV = os.path.join(RESULTS_DIR, "study_b.csv")

ITERATIONS = 200       # repetitions per cell
BATCH_SIZE = 25        # iterations per binary invocation

# problem code
PROBLEMS = {
    0: ("mst", ["prim", "kruskal"]),
    1: ("sp", ["dijkstra", "bellmanFord"]),
    2: ("mf", ["fordFulkerson"]),
}
STRUCTURES = ["incidenceMatrix", "adjacencyList"]

# Study A: 6 sizes at 50% density. Largest size kept under the 16 GB ceiling
# (incidence matrix is V x E ~ 2*V^3 bytes at 50%: V=1400 ~ 5.5 GB).
STUDY_A_SIZES = [100, 200, 400, 800, 1100, 1400]
STUDY_A_DENSITY = 50

# Study B: one fixed size across four densities.
STUDY_B_SIZE = 1000
STUDY_B_DENSITIES = [25, 50, 75, 99]


# --- Resume support ----------------------------------------------------------

def rows_per_iteration(problem):
    """One iteration writes one row per (algorithm x structure) it ran."""
    _, algorithms = PROBLEMS[problem]
    return len(algorithms) * len(STRUCTURES)


def completed_iterations(csv_path, problem, vertices, density):
    """Count iterations already recorded for a cell, so a re-run can resume.

    Batching repeats the per-process iteration index (0..BATCH_SIZE-1), so we
    cannot trust that column -- instead we count matching rows and divide by the
    number of rows a single iteration produces.
    """
    if not os.path.exists(csv_path):
        return 0
    label, _ = PROBLEMS[problem]
    matched = 0
    with open(csv_path, newline="") as f:
        for row in csv.DictReader(f):
            if (row["problem"] == label
                    and int(row["vertices"]) == vertices
                    and int(row["density"]) == density):
                matched += 1
    return matched // rows_per_iteration(problem)


# --- Running -----------------------------------------------------------------

def run_batch(problem, vertices, density, count, csv_path):
    """Invoke the binary for `count` iterations of one cell, appending to CSV."""
    cmd = [BINARY, "-b",
           "-p", str(problem), "-a", "0", "-s", "0",
           "-l", str(vertices), "-d", str(density),
           "-n", str(count), "-r", csv_path]
    # Discard the binary's own console summary; the CSV is what we keep.
    subprocess.run(cmd, check=True, stdout=subprocess.DEVNULL)


def run_cell(problem, vertices, density, csv_path):
    """Run one cell up to ITERATIONS, in batches, skipping done iterations."""
    label, _ = PROBLEMS[problem]
    done = completed_iterations(csv_path, problem, vertices, density)
    remaining = ITERATIONS - done
    header = f"  {label:>3} V={vertices:<5} d={density:>2}%"
    if remaining <= 0:
        print(f"{header}  already complete ({done}/{ITERATIONS}) -- skipped")
        return
    if done:
        print(f"{header}  resuming ({done}/{ITERATIONS} done)")

    started = time.time()
    while remaining > 0:
        batch = min(BATCH_SIZE, remaining)
        run_batch(problem, vertices, density, batch, csv_path)
        remaining -= batch
        progress = ITERATIONS - remaining
        elapsed = time.time() - started
        rate = (progress - done) / elapsed if elapsed > 0 else 0
        eta = (remaining / rate) if rate > 0 else 0
        print(f"{header}  {progress:>3}/{ITERATIONS}"
              f"  ({elapsed:6.1f}s elapsed, ~{eta:6.1f}s left)", flush=True)


def run_study(name, csv_path, cells):
    print(f"\n=== {name} -> {os.path.relpath(csv_path, ROOT)} ===")
    for problem, vertices, density in cells:
        run_cell(problem, vertices, density, csv_path)


def main():
    if not os.path.exists(BINARY):
        sys.exit(f"Binary not found: {BINARY}\nBuild it first: cmake --build build")
    os.makedirs(RESULTS_DIR, exist_ok=True)

    study_a_cells = [(p, v, STUDY_A_DENSITY)
                     for p in PROBLEMS for v in STUDY_A_SIZES]
    study_b_cells = [(p, STUDY_B_SIZE, d)
                     for p in PROBLEMS for d in STUDY_B_DENSITIES]

    wall_start = time.time()
    try:
        run_study("Study A (size sweep @ 50%)", STUDY_A_CSV, study_a_cells)
        run_study("Study B (density sweep @ V=%d)" % STUDY_B_SIZE,
                  STUDY_B_CSV, study_b_cells)
    except KeyboardInterrupt:
        print("\nInterrupted -- data written so far is preserved; "
              "re-run to resume.", file=sys.stderr)
        sys.exit(130)

    total = time.time() - wall_start
    print(f"\nDone. Total wall time: {total/60:.1f} min")


if __name__ == "__main__":
    main()
