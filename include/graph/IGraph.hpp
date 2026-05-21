#ifndef IGRAPH_HPP
#define IGRAPH_HPP

#include "data_structures/Array.hpp"
#include <iostream>
#include <iomanip>

struct Edge {
    int to;
    int weight;
};

class IGraph {
protected:
    int numVertices;
    int numEdges;
    int maxEdges;
    bool isDirected;

public:
    IGraph(int vertices, int expectedEdges, bool directed)
        : numVertices(vertices), numEdges(0), maxEdges(expectedEdges), isDirected(directed) {}

    virtual ~IGraph() = default;

    virtual void addEdge(int from, int to, int weight) = 0;

    virtual void getNeighbors(int vertex, DynamicArray<Edge>& neighbors) const = 0;

    virtual void display() const = 0;

    [[nodiscard]] int getVerticesCount() const { return numVertices; }
    [[nodiscard]] int getEdgesCount() const { return numEdges; }
    [[nodiscard]] bool getIsDirected() const { return isDirected; }
};

#endif