#include "Utils.hpp"
#include <fstream>
#include <iostream>

bool Utils::loadGraphData(const std::string& filename, GraphData& out) {
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Error: cannot open file: " << filename << "\n";
        return false;
    }

    out.edges.clear();

    if (!(in >> out.vertices >> out.declaredEdges)) {
        std::cerr << "Error: invalid header (expected \"V<TAB>E\") in " << filename << "\n";
        return false;
    }
    if (out.vertices < 1 || out.declaredEdges < 0) {
        std::cerr << "Error: nonsensical header values (V=" << out.vertices
                  << ", E=" << out.declaredEdges << ") in " << filename << "\n";
        return false;
    }

    RawEdge e;
    while (in >> e.from >> e.to >> e.weight) {
        if (e.from < 0 || e.from >= out.vertices || e.to < 0 || e.to >= out.vertices) {
            std::cerr << "Error: edge " << e.from << " -> " << e.to
                      << " refers to a vertex outside [0, " << out.vertices << ") in "
                      << filename << "\n";
            return false;
        }
        out.edges.append(e);
    }

    // Loop ended: eofbit - clean end of data, failbit alone - a malformed line
    if (!in.eof()) {
        std::cerr << "Error: malformed edge line (after reading "
                  << out.edges.getSize() << " valid edges) in " << filename << "\n";
        return false;
    }

    // Verify the number of edges actually read
    if (out.edges.getSize() != out.declaredEdges) {
        std::cerr << "Error: header declares " << out.declaredEdges
                  << " edges but file contains " << out.edges.getSize()
                  << " (" << filename << ")\n";
        return false;
    }

    std::cout << "Loaded " << filename << ": " << out.vertices << " vertices, "
              << out.edges.getSize() << "/" << out.declaredEdges
              << " edges (count verified)\n";
    return true;
}
