#ifndef GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_HPP

#include <string>

enum class GraphType {
    DIRECTED,
    UNDIRECTED
};

class GraphGenerator {
public:
    // Parameters:
    // - filename
    // - vertices
    // - density
    // - type
    static void generateAndSave(const std::string& filename, int vertices, double density, GraphType type);

private:
    static int getRandomWeight(int maxEdges);
};

#endif