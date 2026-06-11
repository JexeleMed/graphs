#ifndef GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_HPP

#include "Utils.hpp"
#include <string>

enum class GraphType {
    DIRECTED,
    UNDIRECTED
};

class GraphGenerator {
public:
    // Generates a connected random graph into memory:
    // - density: percent of the maximum possible edge count, in (0, 100];
    //   clamped from below so the graph always contains a spanning tree
    // - for DIRECTED graphs every vertex is reachable from vertex 0
    //   (tree edges point away from the root)
    // - weights are drawn uniformly from [1, E*4/5]
    static void generate(GraphData& out, int vertices, double density, GraphType type);

    // First line "V<TAB>E", then one edge per line: "from<TAB>to<TAB>weight".
    static void generateAndSave(const std::string& filename, int vertices, double density, GraphType type);
};

#endif
