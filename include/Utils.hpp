#ifndef UTILS_HPP
#define UTILS_HPP

#include "data_structures/Array.hpp"
#include <string>

struct RawEdge {
    int from;
    int to;
    int weight;
};

// Note: DynamicArray is non-copyable, so GraphData's copy operations are
// implicitly deleted too — it can only be passed by reference.
struct GraphData {
    int vertices = 0;
    int declaredEdges = 0;
    DynamicArray<RawEdge> edges;
};

namespace Utils {

    bool loadGraphData(const std::string& filename, GraphData& out);

    template <typename GraphT>
    void fillGraph(const GraphData& data, GraphT& graph) {
        for (int i = 0; i < data.edges.getSize(); ++i) {
            const RawEdge& e = data.edges[i];
            graph.addEdge(e.from, e.to, e.weight);
        }
    }
}

#endif
