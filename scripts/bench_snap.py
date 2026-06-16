#!/usr/bin/env python3
"""Timing benchmark on a SNAP dataset: our implementations vs NetworkX.

The correctness check lives in compare_networkx.py; this script measures *speed*.
For every problem (MST / SP / MF) it times:

  * our binary in benchmark-on-a-file mode (-b -i FILE), which already loops the
    requested iterations and measures only the algorithm (std::chrono, us) for
    both representations (incidence matrix, adjacency list),
  * the matching NetworkX algorithm, timed the same way (build graph once, then
    time only the algorithm call) and appended to the same CSV as a third
    "structure" called "networkx".

A single results/snap.csv (same schema as the study CSVs) feeds a grouped bar
chart per problem comparing the three implementations on a log time axis.

NetworkX is the reference oracle (pure Python), so it is expected to sit between
our two representations rather than to win -- the point is the apples-to-apples
comparison on a real-world graph.
"""

import argparse
import csv
import os
import subprocess
import time
from collections import defaultdict
from datetime import datetime

import networkx as nx
from networkx.algorithms.flow import edmonds_karp

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
RESULTS_DIR = os.path.join(ROOT, "results")
PLOTS_DIR = os.path.join(RESULTS_DIR, "plots")

# problem -> (cli code, ordered algorithm names, directed?)
PROBLEMS = {
    "mst": (0, ["prim", "kruskal"], False),
    "sp":  (1, ["dijkstra", "bellmanFord"], True),
    "mf":  (2, ["fordFulkerson"], True),
}
PROBLEM_TITLES = {"mst": "MST", "sp": "Shortest path", "mf": "Max flow"}

# our binary writes these two; networkx is added by this script
STRUCT_ORDER = ["incidenceMatrix", "adjacencyList", "networkx"]
STRUCT_LABELS = {
    "incidenceMatrix": "incidence matrix (ours)",
    "adjacencyList": "adjacency list (ours)",
    "networkx": "NetworkX",
}

CSV_HEADER = ["timestamp", "problem", "algorithm", "structure",
              "vertices", "density", "iteration", "time_us"]


def build_nx_graph(tsv, directed):
    """Load the same TSV our loader reads into a NetworkX (Di)Graph; return (G, V)."""
    g = nx.DiGraph() if directed else nx.Graph()
    with open(tsv) as f:
        V, _E = map(int, f.readline().split())
        for line in f:
            u, v, w = map(int, line.split())
            g.add_edge(u, v, weight=int(w))
    return g, V


def nx_runner(problem, algorithm, g, source, sink):
    """Return a zero-arg callable that runs the matching NetworkX algorithm.

    Generators (MST edges, path-length dicts) are forced to completion so the
    timing covers the actual work, not just object construction.
    """
    if problem == "mst":
        algo = "prim" if algorithm == "prim" else "kruskal"
        return lambda: list(nx.minimum_spanning_edges(g, algorithm=algo, data=False))
    if problem == "sp":
        if algorithm == "dijkstra":
            return lambda: nx.single_source_dijkstra_path_length(g, source, weight="weight")
        return lambda: nx.single_source_bellman_ford_path_length(g, source, weight="weight")
    # mf
    return lambda: nx.maximum_flow_value(g, source, sink, capacity="weight",
                                         flow_func=edmonds_karp)


def density_pct(V, E, directed):
    """Match the binary's self-describing density column (rounded percent)."""
    max_e = V * (V - 1) if directed else V * (V - 1) // 2
    return round(100 * E / max_e) if max_e else 0


def run_ours(binary, tsv, out_csv, iterations):
    """Drive our binary once per problem; it appends matrix+list rows itself."""
    for problem, (code, _algos, _directed) in PROBLEMS.items():
        print(f"[ours] {problem}: running binary ({iterations} iterations)")
        subprocess.run(
            [binary, "-b", "-i", tsv, "-p", str(code), "-a", "0", "-s", "0",
             "-r", out_csv, "-n", str(iterations)],
            check=True, stdout=subprocess.DEVNULL)


def run_networkx(tsv, out_csv, iterations):
    """Time the matching NetworkX algorithms and append them as 'networkx' rows."""
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    with open(out_csv, "a", newline="") as f:
        writer = csv.writer(f)
        for problem, (_code, algos, directed) in PROBLEMS.items():
            g, V = build_nx_graph(tsv, directed)
            E = g.number_of_edges()
            dens = density_pct(V, E, directed)
            source, sink = 0, V - 1
            for algorithm in algos:
                run = nx_runner(problem, algorithm, g, source, sink)
                print(f"[nx]   {problem}/{algorithm}: timing ({iterations} iterations)")
                for it in range(iterations):
                    t0 = time.perf_counter()
                    run()
                    us = int((time.perf_counter() - t0) * 1e6)
                    writer.writerow([timestamp, problem, algorithm, "networkx",
                                     V, dens, it, us])


def load_csv(csv_path):
    """{problem: {algorithm: {structure: [times_us]}}}."""
    data = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))
    with open(csv_path, newline="") as f:
        for row in csv.DictReader(f):
            data[row["problem"]][row["algorithm"]][row["structure"]].append(
                int(row["time_us"]))
    return data


def plot_comparison(csv_path):
    data = load_csv(csv_path)
    colors = plt.rcParams["axes.prop_cycle"].by_key()["color"]
    for problem, (_code, algos_all, _directed) in PROBLEMS.items():
        if problem not in data:
            continue
        algos = [a for a in algos_all if a in data[problem]]
        structs = [s for s in STRUCT_ORDER
                   if any(s in data[problem][a] for a in algos)]
        bar_w = 0.8 / len(structs)

        fig, ax = plt.subplots(figsize=(8, 5))
        for si, struct in enumerate(structs):
            xs, heights, lowers, uppers = [], [], [], []
            for ai, algo in enumerate(algos):
                times = data[problem][algo].get(struct)
                if not times:
                    continue
                avg = sum(times) / len(times)
                xs.append(ai + (si - (len(structs) - 1) / 2) * bar_w)
                heights.append(avg)
                lowers.append(avg - min(times))
                uppers.append(max(times) - avg)
            ax.bar(xs, heights, bar_w, label=STRUCT_LABELS[struct],
                   color=colors[si % len(colors)], yerr=[lowers, uppers], capsize=3)

        ax.set_yscale("log")
        ax.set_xticks(range(len(algos)))
        ax.set_xticklabels(algos)
        ax.set_ylabel("time [us] (mean, error bars = min..max)")
        ax.set_title(f"SNAP ego-Facebook - {PROBLEM_TITLES[problem]} (ours vs NetworkX)")
        ax.grid(True, which="both", axis="y", linestyle=":", alpha=0.5)
        ax.legend()
        fig.tight_layout()
        out_path = os.path.join(PLOTS_DIR, f"snap_{problem}.png")
        fig.savefig(out_path, dpi=120)
        plt.close(fig)
        print(f"  wrote {os.path.relpath(out_path, ROOT)}")


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--tsv", default=os.path.join(ROOT, "data", "ego-facebook.tsv"),
                    help="graph file produced by fetch_snap.py")
    ap.add_argument("--binary", default=os.path.join(ROOT, "build", "graphs"),
                    help="our compiled program")
    ap.add_argument("--out", default=os.path.join(RESULTS_DIR, "snap.csv"),
                    help="results CSV (same schema as the study CSVs)")
    ap.add_argument("-n", "--iterations", type=int, default=50,
                    help="iterations per (algorithm, implementation)")
    ap.add_argument("--plot-only", action="store_true",
                    help="skip the runs, just redraw plots from an existing CSV")
    args = ap.parse_args()

    os.makedirs(PLOTS_DIR, exist_ok=True)

    if not args.plot_only:
        if os.path.exists(args.out):
            os.remove(args.out)  # fresh run; results are cheap to regenerate
        run_ours(args.binary, args.tsv, args.out, args.iterations)
        run_networkx(args.tsv, args.out, args.iterations)
        print(f"\nresults written to {os.path.relpath(args.out, ROOT)}")

    print("\nplots:")
    plot_comparison(args.out)


if __name__ == "__main__":
    main()
