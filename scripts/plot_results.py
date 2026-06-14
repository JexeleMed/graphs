#!/usr/bin/env python3
"""Plot Study A / Study B results produced by run_research.py.

For each problem (MST / SP / MF) one figure is drawn:
  * Study A: time vs number of vertices (density fixed at 50%).
  * Study B: time vs density (size fixed).
One series per (algorithm, representation). The mean over the iterations is
drawn as a line; the min..max spread is shaded, so a disturbed iteration that
pulls the spread wide stays visible.

Aggregation uses only the standard library; matplotlib is the single external
dependency. PNGs are written to results/plots/.
"""

import csv
import os
import sys
from collections import defaultdict

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
RESULTS_DIR = os.path.join(ROOT, "results")
PLOTS_DIR = os.path.join(RESULTS_DIR, "plots")

STUDY_A_CSV = os.path.join(RESULTS_DIR, "study_a.csv")
STUDY_B_CSV = os.path.join(RESULTS_DIR, "study_b.csv")

PROBLEM_TITLES = {"mst": "MST", "sp": "Shortest path", "mf": "Max flow"}


def load(csv_path, x_column):
    """Return {problem: {(algorithm, structure): {x: [times_us]}}}.

    x_column is "vertices" for the size sweep or "density" for the density sweep.
    """
    data = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))
    with open(csv_path, newline="") as f:
        for row in csv.DictReader(f):
            x = int(row[x_column])
            series = (row["algorithm"], row["structure"])
            data[row["problem"]][series][x].append(int(row["time_us"]))
    return data


def aggregate(by_x):
    """{x: [times]} -> sorted (xs, means, mins, maxes)."""
    xs = sorted(by_x)
    means = [sum(by_x[x]) / len(by_x[x]) for x in xs]
    mins = [min(by_x[x]) for x in xs]
    maxes = [max(by_x[x]) for x in xs]
    return xs, means, mins, maxes


def series_colors(series_map):
    """Pin one color per series so the linear and log figures match exactly.

    Colors come from the active prop cycle in the sorted series order -- the
    same order plot_problem iterates in, so both variants stay in sync.
    """
    cycle = plt.rcParams["axes.prop_cycle"].by_key()["color"]
    return {key: cycle[i % len(cycle)]
            for i, key in enumerate(sorted(series_map))}


def plot_problem(problem, series_map, x_label, title, out_path,
                 colors, log_x=False, log_y=False):
    fig, ax = plt.subplots(figsize=(8, 5))
    for key in sorted(series_map):
        algorithm, structure = key
        xs, means, mins, maxes = aggregate(series_map[key])
        ax.plot(xs, means, marker="o", color=colors[key],
                label=f"{algorithm} / {structure}")
        ax.fill_between(xs, mins, maxes, color=colors[key], alpha=0.15)
    if log_x:
        ax.set_xscale("log")
    if log_y:
        ax.set_yscale("log")
    ax.set_xlabel(x_label)
    ax.set_ylabel("time [us] (mean, shaded = min..max)")
    ax.set_title(title)
    ax.grid(True, which="both", linestyle=":", alpha=0.5)
    ax.legend()
    fig.tight_layout()
    fig.savefig(out_path, dpi=120)
    plt.close(fig)
    print(f"  wrote {os.path.relpath(out_path, ROOT)}")


def plot_study(csv_path, x_column, x_label, name_prefix, study_label, log_x):
    if not os.path.exists(csv_path):
        print(f"skip {study_label}: {os.path.relpath(csv_path, ROOT)} not found")
        return
    print(f"{study_label}:")
    data = load(csv_path, x_column)
    for problem, series_map in data.items():
        colors = series_colors(series_map)
        title = f"{study_label} - {PROBLEM_TITLES.get(problem, problem)}"
        out_path = os.path.join(PLOTS_DIR, f"{name_prefix}_{problem}.png")
        plot_problem(problem, series_map, x_label, title, out_path, colors)


        log_path = os.path.join(PLOTS_DIR, f"{name_prefix}_{problem}_log.png")
        plot_problem(problem, series_map, x_label, f"{title} [log]", log_path,
                     colors, log_x=log_x, log_y=True)


def main():
    os.makedirs(PLOTS_DIR, exist_ok=True)
    plot_study(STUDY_A_CSV, "vertices", "vertices",
               "study_a", "Study A (size @ 50% density)", log_x=True)
    plot_study(STUDY_B_CSV, "density", "density [%]",
               "study_b", "Study B (density sweep)", log_x=False)
    if not os.listdir(PLOTS_DIR):
        sys.exit("No plots produced -- run run_research.py first.")


if __name__ == "__main__":
    main()
