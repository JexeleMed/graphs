#ifndef MF_ALGORITHMS_HPP
#define MF_ALGORITHMS_HPP

#include "graph/IGraph.hpp"
#include "data_structures/Array.hpp"
#include "data_structures/Queue.hpp"
#include <limits>
#include <iostream>
#include <cassert>

class MFAlgorithms {
public:
    static constexpr int INF = std::numeric_limits<int>::max();

    // Ford-Fulkerson in the Edmonds-Karp variant (BFS augmenting paths), O(V * E^2).
    // GraphT must provide: getVerticesCount(), getNeighbors()
    template <typename GraphT>
    static int fordFulkerson(const GraphT& graph, int source, int sink, bool showResult = true) {
        int V = graph.getVerticesCount();
        assert(source >= 0 && source < V && sink >= 0 && sink < V && source != sink);

        int* capacity = new int[V * V]();  // value-initialized to zero
        int* residual = new int[V * V];

        DynamicArray<Edge> neighbors;
        for (int u = 0; u < V; ++u) {
            graph.getNeighbors(u, neighbors);
            for (int i = 0; i < neighbors.getSize(); ++i) {
                // += merges parallel edges by summing their capacities
                capacity[u * V + neighbors[i].to] += neighbors[i].weight;
            }
        }
        for (int i = 0; i < V * V; ++i) {
            residual[i] = capacity[i];
        }

        DynamicArray<int>  parent(V);
        DynamicArray<bool> visited(V);
        for (int i = 0; i < V; ++i) {
            parent.append(-1);
            visited.append(false);
        }

        int maxFlow = 0;

        // Keep augmenting while BFS finds a path with spare residual capacity
        while (bfs(residual, V, source, sink, parent, visited)) {
            int pathFlow = INF;
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                if (residual[u * V + v] < pathFlow) {
                    pathFlow = residual[u * V + v];
                }
            }

            // Augment: forward edges lose capacity, reverse edges gain it
            // (reverse residual capacity allows later paths to cancel this flow)
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                residual[u * V + v] -= pathFlow;
                residual[v * V + u] += pathFlow;
            }

            maxFlow += pathFlow;
        }

        if (showResult) {
            displayResults(capacity, residual, V, source, sink, maxFlow);
        }

        delete[] capacity;
        delete[] residual;

        return maxFlow;
    }

private:
    // BFS over the residual graph; fills parent[] and reports if sink is reachable
    static bool bfs(const int* residual, int V, int source, int sink,
                    DynamicArray<int>& parent, DynamicArray<bool>& visited) {
        for (int i = 0; i < V; ++i) {
            parent[i] = -1;
            visited[i] = false;
        }

        Queue<int> queue(V);
        queue.enqueue(source);
        visited[source] = true;

        while (!queue.isEmpty()) {
            int u = queue.dequeue();

            for (int v = 0; v < V; ++v) {
                if (!visited[v] && residual[u * V + v] > 0) {
                    visited[v] = true;
                    parent[v] = u;
                    if (v == sink) return true;
                    queue.enqueue(v);
                }
            }
        }
        return false;
    }

    static void displayResults(const int* capacity, const int* residual,
                               int V, int source, int sink, int maxFlow) {
        std::cout << "\n--- Output Ford-Fulkerson (Edmonds-Karp) ---\n";
        std::cout << "Source: " << source << " | Sink: " << sink << "\n";
        std::cout << "Edges carrying flow (flow/capacity):\n";
        for (int u = 0; u < V; ++u) {
            for (int v = 0; v < V; ++v) {
                if (capacity[u * V + v] > 0) {
                    int flow = capacity[u * V + v] - residual[u * V + v];
                    if (flow > 0) {
                        std::cout << "  " << u << " -> " << v
                                  << " | " << flow << "/" << capacity[u * V + v] << "\n";
                    }
                }
            }
        }
        std::cout << "Max flow: " << maxFlow << "\n";
        std::cout << "-----------------------------------\n";
    }
};

#endif
