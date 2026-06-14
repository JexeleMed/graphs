#!/usr/bin/env python3
"""Compare our MST implementation against NetworkX on a real-world dataset.

Why compare the total cost and not the edge set: a minimum spanning tree is not
unique when edge weights repeat, but every MST of a graph has the same total
weight. The cost is therefore the invariant worth comparing.

Timing note: wall-clock time is dominated by I/O, not by
the algorithm. The reported times are a rough indication only; the real
algorithm timings live in the benchmark study (results/study_*.csv).
"""

import argparse
import os
import re
import subprocess
import tempfile
import time

import networkx as nx

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)

STRUCT_RE = re.compile(r"=== Structure:\s*(.+?)\s*===")
ALGO_RE = re.compile(r"^\[(Prim|Kruskal)\]")
COST_RE = re.compile(r"Total MST cost:\s*(\d+)")


def run_ours(binary, tsv, structure):
    """Run our MST (Prim + Kruskal) and return [(structure, algo, cost)], seconds.

    Single-file mode dumps the full graph too so we send it to a throwaway temp file and delete it after
    parsing instead of leaving it next to the dataset.
    """
    fd, out_path = tempfile.mkstemp(prefix="mst_compare_", suffix=".txt")
    os.close(fd)
    cmd = [binary, "-f", "-p", "0", "-a", "0", "-s", str(structure),
           "-i", tsv, "-o", out_path]
    try:
        t = time.perf_counter()
        subprocess.run(cmd, check=True, stdout=subprocess.DEVNULL)
        dt = time.perf_counter() - t

        results = []
        cur_struct, cur_algo = "?", "?"
        with open(out_path) as f:
            for line in f:
                m = STRUCT_RE.search(line)
                if m:
                    cur_struct = m.group(1)
                    continue
                m = ALGO_RE.match(line)
                if m:
                    cur_algo = m.group(1)
                    continue
                m = COST_RE.search(line)
                if m:
                    results.append((cur_struct, cur_algo, int(m.group(1))))
    finally:
        os.remove(out_path)
    return results, dt


def run_networkx(tsv):
    """Compute the MST weight with NetworkX; return cost, seconds, (V, E)."""
    g = nx.Graph()
    with open(tsv) as f:
        V, E = map(int, f.readline().split())
        for line in f:
            u, v, w = map(int, line.split())
            g.add_edge(u, v, weight=w)
    t = time.perf_counter()
    mst = nx.minimum_spanning_tree(g, algorithm="kruskal")
    dt = time.perf_counter() - t
    cost = sum(d["weight"] for _, _, d in mst.edges(data=True))
    return cost, dt, (g.number_of_nodes(), g.number_of_edges())


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--tsv", default=os.path.join(ROOT, "data", "ego-facebook.tsv"),
                    help="graph file produced by fetch_snap.py")
    ap.add_argument("--binary", default=os.path.join(ROOT, "build", "graphs"),
                    help="our compiled program")
    ap.add_argument("--structure", type=int, default=2, choices=(0, 1, 2),
                    help="0=both, 1=incidence matrix, 2=adjacency list (default)")
    args = ap.parse_args()

    ref_cost, ref_dt, (V, E) = run_networkx(args.tsv)
    ours, ours_dt = run_ours(args.binary, args.tsv, args.structure)

    print(f"dataset: {os.path.relpath(args.tsv, ROOT)}  (V={V}, E={E})\n")
    print(f"{'implementation':<32}{'MST cost':>14}{'match':>8}")
    print("-" * 54)
    print(f"{'NetworkX (kruskal, reference)':<32}{ref_cost:>14}{'--':>8}")
    all_ok = True
    for struct, algo, cost in ours:
        ok = (cost == ref_cost)
        all_ok &= ok
        label = f"ours: {algo} / {struct}"
        print(f"{label:<32}{cost:>14}{('OK' if ok else 'FAIL'):>8}")

    print(f"\ntiming (rough -- see docstring):")
    print(f"  NetworkX MST algorithm : {ref_dt * 1000:8.1f} ms")
    print(f"  our binary (load+solve+dump): {ours_dt * 1000:8.1f} ms")

    print(f"\nRESULT: {'PASS -- all costs match NetworkX' if all_ok else 'FAIL -- mismatch above'}")
    return 0 if all_ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
