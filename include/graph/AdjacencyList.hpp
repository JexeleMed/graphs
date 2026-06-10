#ifndef ADJACENCY_LIST_HPP
#define ADJACENCY_LIST_HPP

#include "IGraph.hpp"
#include "data_structures/List.hpp"
#include <iostream>
#include <cassert>

// "final" lets the compiler devirtualize calls when the concrete type is known
class AdjacencyList final : public IGraph {
private:
    // Index is a number of vertex
    // List contains all his exiting edges (target edge and weight)
    SinglyLinkedList<Edge>* listArray;

public:
    AdjacencyList(int vertices, int expectedEdges, bool directed)
        : IGraph(vertices, expectedEdges, directed) {

        // Allocate array
        listArray = new SinglyLinkedList<Edge>[numVertices];
    }

    ~AdjacencyList() override {
        // Free array of lists
        delete[] listArray;
    }

    void addEdge(int from, int to, int weight) override {
        assert(numEdges < maxEdges && "addEdge: exceeded declared edge count!");

        // Add exiting edge
        listArray[from].append({to, weight});

        // For Undirected graph add return connection
        if (!isDirected) {
            listArray[to].append({from, weight});
        }

        numEdges++;
    }

    void getNeighbors(int vertex, DynamicArray<Edge>& neighbors) const override {
        neighbors.clear();

        for (auto& edge : listArray[vertex]) {
            neighbors.append(edge);
        }
    }

    void display() const override {
        std::cout << "Adjacency List (" << numVertices << " vertices):\n";
        for (int i = 0; i < numVertices; ++i) {
            std::cout << "[" << i << "] -> ";
            for (auto& edge : listArray[i]) {
                std::cout << "(" << edge.to << ", w:" << edge.weight << ") ";
            }
            std::cout << "\n";
        }
    }
};

#endif