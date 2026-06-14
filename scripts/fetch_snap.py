#!/usr/bin/env python3
"""Download a SNAP dataset and convert it to our TSV graph format.

We run the implementations on a large
real-world dataset and compare them against a ready-made tool (NetworkX).
SNAP edge lists are plain "u v" pairs: no header line and no weights, while our
loader expects a "V<TAB>E" header and a weight on every edge. We adapt the input file to our format.
Changes made:

  1. keep only the largest connected component (our generator guarantees
     connected graphs; the loader/algorithms assume it too),
  2. relabel vertices to a contiguous 0..V-1 range,
  3. add synthetic edge weights -- reproducible (fixed seed), drawn from
     1..E*4/5 just like GraphGenerator.
  4. prepend the "V<TAB>E" header line.

Default dataset: ego-Facebook (4039 vertices, 88234 undirected edges, a single
connected component). Both representations still build in memory.
"""

import argparse
import gzip
import os
import random
import sys
import urllib.request

import networkx as nx

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
DATA_DIR = os.path.join(ROOT, "data")

DATASETS = {
    "ego-facebook": {
        "url": "https://snap.stanford.edu/data/facebook_combined.txt.gz",
        "directed": False,
    },
    "ca-grqc": {
        "url": "https://snap.stanford.edu/data/ca-GrQc.txt.gz",
        "directed": False,
    },
}


def download(url, cache_path):
    """Fetch url into cache_path once; reuse the local copy afterwards."""
    if os.path.exists(cache_path):
        print(f"cache hit: {os.path.relpath(cache_path, ROOT)}")
        return
    print(f"downloading {url}")
    urllib.request.urlretrieve(url, cache_path)
    print(f"saved {os.path.relpath(cache_path, ROOT)} "
          f"({os.path.getsize(cache_path)} bytes)")


def read_edges(gz_path):
    """Yield (u, v) int pairs from a gzipped SNAP edge list (# = comment)."""
    with gzip.open(gz_path, "rt") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            a, b = line.split()[:2]
            yield int(a), int(b)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("dataset", nargs="?", default="ego-facebook",
                    choices=sorted(DATASETS), help="SNAP dataset key")
    ap.add_argument("--seed", type=int, default=42,
                    help="RNG seed for synthetic weights (reproducible)")
    args = ap.parse_args()

    spec = DATASETS[args.dataset]
    os.makedirs(DATA_DIR, exist_ok=True)
    gz_path = os.path.join(DATA_DIR, f"{args.dataset}.txt.gz")
    out_path = os.path.join(DATA_DIR, f"{args.dataset}.tsv")

    download(spec["url"], gz_path)

    # Build undirected graph
    raw = nx.Graph()
    raw.add_edges_from(read_edges(gz_path))
    print(f"raw graph: {raw.number_of_nodes()} vertices, "
          f"{raw.number_of_edges()} edges, "
          f"{nx.number_connected_components(raw)} connected component(s)")

    # 1) largest connected component
    largest = max(nx.connected_components(raw), key=len)
    g = raw.subgraph(largest).copy()

    # 2) contiguous relabeling 0..V-1
    g = nx.convert_node_labels_to_integers(g, first_label=0, ordering="sorted")

    V = g.number_of_nodes()
    E = g.number_of_edges()

    # 3) reproducible synthetic weights in 1..E*4/5
    rng = random.Random(args.seed)
    w_max = max(1, E * 4 // 5)

    with open(out_path, "w") as out:
        out.write(f"{V}\t{E}\n")
        for u, v in g.edges():
            out.write(f"{u}\t{v}\t{rng.randint(1, w_max)}\n")

    print(f"\nwrote {os.path.relpath(out_path, ROOT)}")
    print("--- modifications applied (for the report) ---")
    print(f"  dataset      : {args.dataset} ({spec['url']})")
    print(f"  component    : largest CC kept "
          f"({V}/{raw.number_of_nodes()} vertices, "
          f"{E}/{raw.number_of_edges()} edges)")
    print(f"  relabeling   : vertices renumbered to contiguous 0..{V - 1}")
    print(f"  weights      : synthetic, seed={args.seed}, range 1..{w_max} "
          f"(= E*4/5")
    print(f"  header       : prepended \"{V}\\t{E}\"")


if __name__ == "__main__":
    main()
