# Project 2 — Graphs (MST / SP / MF)

Hand-written implementation of graph structures (incidence matrix, adjacency
list) and algorithms: Prim, Kruskal, Dijkstra, Bellman–Ford, Ford–Fulkerson.

## Requirements
- C++17 compiler (gcc 15.2.0 works)
- CMake ≥ 3.10

## Building
```bash
cmake -S . -B build
cmake --build build
```
Produces the `build/graphs` binary (defaults to `Release`).

**Parameters library:** if a compiled `libparameters.so` is present in
`third_party/`, CMake links it (message `Parameters: linking prebuilt …`);
otherwise it compiles `third_party/Parameters.cpp` from source. After swapping
the `.so`, reconfigure: `rm -rf build && cmake -S . -B build`.

## Running
Three modes (exactly one required); full parameter list: `./build/graphs -h`.

**Single-file mode** (`-f`): load a graph, solve, print (`-o` = save to a file)
```bash
# MST, both algorithms, both structures
./build/graphs -f -p 0 -a 0 -s 0 -i graph.tsv
# Shortest path with Dijkstra from vertex 0, adjacency list
./build/graphs -f -p 1 -a 3 -s 2 -i graph.tsv -c 0
# Max flow from source 0 to sink 3
./build/graphs -f -p 2 -a 5 -s 0 -i graph.tsv -c 0 -e 3
```

**Benchmark mode** (`-b`): generates a random graph each iteration, measures
time, appends to a CSV
```bash
# 50 repetitions: V=35, 75% density, MST, all algorithms and structures
./build/graphs -b -p 0 -a 0 -s 0 -l 35 -d 75 -n 50 -r results.csv
```

### Parameters
| flag | meaning | values |
|---|---|---|
| `-p` | problem | 0=MST, 1=SP, 2=MF |
| `-a` | algorithm | 0=all for the problem, 1=Prim, 2=Kruskal, 3=Dijkstra, 4=Bellman–Ford, 5=Ford–Fulkerson |
| `-s` | structure | 0=both, 1=incidence matrix, 2=adjacency list |
| `-i`/`-o` | input / output file | single-file mode |
| `-c`/`-e` | start / end vertex | SP, MF |
| `-r` | results CSV file | benchmark mode |
| `-l`/`-d`/`-n` | vertex count / density % / repetitions | benchmark mode |

### Graph file format (TSV)
First line: `vertex_count` `edge_count`. Following lines:
`source_vertex` `target_vertex` `weight` (tab-separated, vertices numbered from
0). MST treats edges as undirected, SP/MF as directed.
```
4	6
0	1	10
0	2	10
1	3	5
2	3	12
3	0	1
3	2	7
```
