#ifndef MST_ALGORITHMS_HPP
#define MST_ALGORITHMS_HPP

#include "graph/IGraph.hpp"
#include "data_structures/PriorityQueue.hpp"
#include "data_structures/Array.hpp"
#include "data_structures/UnionFind.hpp"
#include <limits>
#include <iostream>
#include <algorithm>

struct WeightedEdge {
    int from, to, weight;

    bool operator<(const WeightedEdge& other) const {
        return weight < other.weight;
    }
};

struct PrimElement {
    int vertex;
    int cost;
    int parent;

    bool operator>(const PrimElement& other) const {
        return cost > other.cost;
    }
};

class MSTAlgorithms {
public:
    static constexpr int INF = std::numeric_limits<int>::max();

    // GraphT must provide: getVerticesCount(), getEdgesCount(), getNeighbors()
    template <typename GraphT>
    static void prim(const GraphT& graph, bool showResult = true) {
        int V = graph.getVerticesCount();

        DynamicArray<int>  key(V);    // cheapest known edge cost to reach each vertex
        DynamicArray<int>  parent(V); // which vertex connects to it in the MST
        DynamicArray<bool> inMST(V);  // whether vertex is already part of the MST

        for (int i = 0; i < V; ++i) {
            key.append(INF);
            parent.append(-1);
            inMST.append(false);
        }

        PriorityQueue<PrimElement> pq;
        key[0] = 0;
        pq.push({0, 0, -1});

        DynamicArray<Edge> neighbors;

        while (!pq.isEmpty()) {
            PrimElement curr = pq.pop();
            int u = curr.vertex;

            // Lazy deletion: skip if already settled
            if (inMST[u]) continue;
            inMST[u] = true;
            parent[u] = curr.parent;

            graph.getNeighbors(u, neighbors);
            for (int i = 0; i < neighbors.getSize(); ++i) {
                int v = neighbors[i].to;
                int w = neighbors[i].weight;

                if (!inMST[v] && w < key[v]) {
                    key[v] = w;
                    pq.push({v, w, u});
                }
            }
        }

        if (showResult) {
            displayPrimResults(key, parent, V);
        }
    }

    template <typename GraphT>
    static void kruskal(const GraphT& graph, bool showResult = true) {
        int V = graph.getVerticesCount();
        int E = graph.getEdgesCount();

        DynamicArray<WeightedEdge> edges(E);
        DynamicArray<Edge> neighbors;

        // Collect all edges; use v < u to avoid counting each undirected edge twice
        for (int v = 0; v < V; ++v) {
            graph.getNeighbors(v, neighbors);
            for (int i = 0; i < neighbors.getSize(); ++i) {
                int u = neighbors[i].to;
                int w = neighbors[i].weight;
                if (v < u) {
                    edges.append({v, u, w});
                }
            }
        }

        std::sort(edges.getData(), edges.getData() + edges.getSize());

        UnionFind uf(V);
        DynamicArray<WeightedEdge> mst(V - 1);
        int totalCost = 0;

        for (int i = 0; i < edges.getSize(); ++i) {
            WeightedEdge& e = edges[i];
            if (uf.unite(e.from, e.to)) {
                mst.append(e);
                totalCost += e.weight;
                if (mst.getSize() == V - 1) break;
            }
        }

        if (showResult) {
            displayKruskalResults(mst, totalCost);
        }
    }

private:
    static void displayPrimResults(const DynamicArray<int>& key, const DynamicArray<int>& parent, int V) {
        std::cout << "\n--- Output Prim ---\n";
        int totalCost = 0;
        for (int i = 1; i < V; ++i) {
            std::cout << "Edge: " << parent[i] << " -> " << i << " | Weight: " << key[i] << "\n";
            totalCost += key[i];
        }
        std::cout << "Total MST cost: " << totalCost << "\n";
        std::cout << "-----------------------------------\n";
    }

    static void displayKruskalResults(const DynamicArray<WeightedEdge>& mst, int totalCost) {
        std::cout << "\n--- Output Kruskal ---\n";
        for (int i = 0; i < mst.getSize(); ++i) {
            std::cout << "Edge: " << mst[i].from << " -> " << mst[i].to << " | Weight: " << mst[i].weight << "\n";
        }
        std::cout << "Total MST cost: " << totalCost << "\n";
        std::cout << "-----------------------------------\n";
    }
};

#endif
