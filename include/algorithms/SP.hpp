#ifndef SP_ALGORITHMS_HPP
#define SP_ALGORITHMS_HPP

#include "graph/IGraph.hpp"
#include "data_structures/PriorityQueue.hpp"
#include "data_structures/Array.hpp"
#include <limits>
#include <iostream>

struct QueueElement {
    int vertex;
    int distance;

    bool operator>(const QueueElement& other) const {
        return this->distance > other.distance;
    }
};

class SPAlgorithms {
public:
    // Infinity represents lack of paths
    static constexpr int INF = std::numeric_limits<int>::max();

    static void dijkstra(const IGraph& graph, int startVertex, bool showResult = true) {
        int V = graph.getVerticesCount();

        DynamicArray<int> distances(V);
        DynamicArray<int> parents(V);

        // Array initialization
        for (int i = 0; i < V; ++i) {
            distances.append(INF);
            parents.append(-1);
        }

        // Priority queue
        PriorityQueue<QueueElement> pq;

        // Add starting vertex with distance 0
        distances[startVertex] = 0;
        pq.push({startVertex, 0});

        // Reference to neighbours
        DynamicArray<Edge> neighbors(V);

        // Main loop
        while (!pq.isEmpty()) {
            QueueElement current = pq.pop();
            int u = current.vertex;
            int dist_u = current.distance;

            // Safety (Lazy Deletion):
            // If queue had old, worse element, ignore it
            if (dist_u > distances[u]) {
                continue;
            }

            // Get neighbours
            graph.getNeighbors(u, neighbors);

            // Travel through every neighbour (Edge relaxation)
            for (int i = 0; i < neighbors.getSize(); ++i) {
                int v = neighbors[i].to;
                int weight = neighbors[i].weight;

                // If there is a quicker way to "v" through "u"
                if (distances[u] != INF && distances[u] + weight < distances[v]) {
                    distances[v] = distances[u] + weight;
                    parents[v] = u;

                    // Add new way to queue
                    pq.push({v, distances[v]});
                }
            }
        }

        // Print results
        if (showResult) {
            displayResults("Dijkstra", startVertex, distances, parents);
        }
    }

private:
    // Helper for printer
    static void displayResults(const std::string& algoName, int startVertex, const DynamicArray<int>& distances, const DynamicArray<int>& parents) {
        std::cout << "\n--- Output " << algoName << " ---\n";
        std::cout << "Starting edge: " << startVertex << "\n";

        for (int i = 0; i < distances.getSize(); ++i) {
            std::cout << "To node [" << i << "] -> Cost: ";
            if (distances[i] == INF) {
                std::cout << "No paths\n";
            } else {
                std::cout << distances[i] << " | Reverse path: " << i;
                int p = parents[i];
                while (p != -1) {
                    std::cout << " <- " << p;
                    p = parents[p];
                }
                std::cout << "\n";
            }
        }
        std::cout << "-----------------------------------\n";
    }
};

#endif