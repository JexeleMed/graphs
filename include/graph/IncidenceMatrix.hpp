#ifndef INCIDENCE_MATRIX_HPP
#define INCIDENCE_MATRIX_HPP

#include "IGraph.hpp"
#include "data_structures/Array.hpp"
#include <iostream>
#include <iomanip>
#include <cassert>

class IncidenceMatrix final : public IGraph {
private:
    int** matrix;
    DynamicArray<int> edgeWeights;
    int currentEdgeIndex;

public:
    IncidenceMatrix(int vertices, int expectedEdges, bool directed)
        : IGraph(vertices, expectedEdges, directed),
          edgeWeights(expectedEdges),
          currentEdgeIndex(0) {

        // Matrix allocation V x E
        matrix = new int*[numVertices];
        for (int i = 0; i < numVertices; ++i) {
            matrix[i] = new int[maxEdges];
            for (int j = 0; j < maxEdges; ++j) {
                matrix[i][j] = 0;
            }
        }
    }

    ~IncidenceMatrix() override {
        for (int i = 0; i < numVertices; ++i) {
            delete[] matrix[i];
        }
        delete[] matrix;
    }

    void addEdge(int from, int to, int weight) override {
        assert(currentEdgeIndex < maxEdges && "addEdge: exceeded declared edge count!");

        edgeWeights.append(weight);

        if (isDirected) {
            matrix[from][currentEdgeIndex] = 1;  // Starting node
            matrix[to][currentEdgeIndex] = -1;   // End node
        } else {
            matrix[from][currentEdgeIndex] = 1;  // Undirected graph
            matrix[to][currentEdgeIndex] = 1;    // Both ways
        }

        currentEdgeIndex++;
        numEdges++;
    }

    void getNeighbors(int vertex, DynamicArray<Edge>& neighbors) const override {
        neighbors.clear();

        for (int e = 0; e < currentEdgeIndex; ++e) {
            // Edge from current vertex
            if (matrix[vertex][e] == 1) {
                int weight = edgeWeights[e];

                for (int v = 0; v < numVertices; ++v) {
                    if (v != vertex) {
                        if (isDirected && matrix[v][e] == -1) {
                            neighbors.append({v, weight});
                            break;
                        } else if (!isDirected && matrix[v][e] == 1) {
                            neighbors.append({v, weight});
                            break;
                        }
                    }
                }
            }
        }
    }

    void display() const override {
        std::cout << "Incidence matrix (" << numVertices << "x" << numEdges << "):\n";
        for (int i = 0; i < numVertices; ++i) {
            std::cout << std::setw(3) << i << " | ";
            for (int j = 0; j < currentEdgeIndex; ++j) {
                std::cout << std::setw(3) << matrix[i][j] << " ";
            }
            std::cout << "\n";
        }
    }
};

#endif