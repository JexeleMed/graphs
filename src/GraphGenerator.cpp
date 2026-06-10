#include "GraphGenerator.hpp"
#include "data_structures/Array.hpp"
#include <fstream>
#include <random>
#include <iostream>
#include <cassert>
#include <cmath>

// Anonymous namespace
namespace {
    struct VertexPair {
        int from;
        int to;
    };
}

void GraphGenerator::generateAndSave(const std::string& filename, int vertices, double density, GraphType type) {
    assert(vertices >= 2 && "generateAndSave: at least 2 vertices required");
    assert(density > 0.0 && density <= 100.0 && "generateAndSave: density must be in (0, 100]");

    const bool directed = (type == GraphType::DIRECTED);
    const long long V = vertices;

    const long long maxEdges = directed ? V * (V - 1) : V * (V - 1) / 2;

    long long targetEdges = std::llround(density / 100.0 * static_cast<double>(maxEdges));
    if (targetEdges < V - 1) targetEdges = V - 1;
    if (targetEdges > maxEdges) targetEdges = maxEdges;

    std::mt19937 rng(std::random_device{}());

    DynamicArray<int> perm(vertices);
    for (int i = 0; i < vertices; ++i) {
        perm.append(i);
    }
    for (int i = vertices - 1; i > 0; --i) {
        std::uniform_int_distribution<int> dist(0, i);
        int j = dist(rng);
        int tmp = perm[i];
        perm[i] = perm[j];
        perm[j] = tmp;
    }

    // Flat V*V matrix marking already used pairs (no duplicates / self-loops)
    bool* used = new bool[V * V]();

    DynamicArray<VertexPair> edges(static_cast<int>(targetEdges));

    // Phase 1 — spanning tree
    for (int i = 1; i < vertices; ++i) {
        std::uniform_int_distribution<int> dist(0, i - 1);
        int from = perm[dist(rng)];
        int to = perm[i];

        edges.append({from, to});
        used[from * V + to] = true;
        if (!directed) {
            used[to * V + from] = true;   // undirected: pair blocked both ways
        }
    }

    // Phase 2 — densification
    long long remaining = targetEdges - (V - 1);
    if (remaining > 0) {
        DynamicArray<VertexPair> pool(static_cast<int>(maxEdges - (V - 1)));

        if (directed) {
            for (int u = 0; u < vertices; ++u) {
                for (int v = 0; v < vertices; ++v) {
                    if (u != v && !used[u * V + v]) {
                        pool.append({u, v});
                    }
                }
            }
        } else {
            // u < v: each undirected pair considered exactly once
            for (int u = 0; u < vertices; ++u) {
                for (int v = u + 1; v < vertices; ++v) {
                    if (!used[u * V + v]) {
                        pool.append({u, v});
                    }
                }
            }
        }

        // Partial shuffle
        int poolSize = pool.getSize();
        for (int k = 0; k < remaining; ++k) {
            std::uniform_int_distribution<int> dist(k, poolSize - 1);
            int j = dist(rng);

            VertexPair tmp = pool[k];
            pool[k] = pool[j];
            pool[j] = tmp;

            edges.append(pool[k]);
        }
    }

    delete[] used;

    long long maxWeight = (targetEdges * 4) / 5;
    if (maxWeight < 1) maxWeight = 1;
    std::uniform_int_distribution<int> weightDist(1, static_cast<int>(maxWeight));

    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Error: cannot open file for writing: " << filename << "\n";
        return;
    }

    out << vertices << "\t" << edges.getSize() << "\n";
    for (int i = 0; i < edges.getSize(); ++i) {
        out << edges[i].from << "\t" << edges[i].to << "\t" << weightDist(rng) << "\n";
    }
}
