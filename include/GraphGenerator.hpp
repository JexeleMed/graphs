#ifndef GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_HPP

#include <string>

enum class GraphType {
    DIRECTED,
    UNDIRECTED
};

class GraphGenerator {
public:
    // Generates a connected random graph and saves it in the project format:
    // first line "V<TAB>E", then one edge per line: "from<TAB>to<TAB>weight".
    // - density: percent of the maximum possible edge count, in (0, 100];
    //   clamped from below so the graph always contains a spanning tree
    // - for DIRECTED graphs every vertex is reachable from vertex 0
    //   (tree edges point away from the root)
    // - weights are drawn uniformly from [1, E*4/5] per project spec
    static void generateAndSave(const std::string& filename, int vertices, double density, GraphType type);
};

#endif
