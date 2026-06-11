#include <iostream>
#include <fstream>
#include "Parameters.h"
#include "Utils.hpp"
#include "graph/AdjacencyList.hpp"
#include "graph/IncidenceMatrix.hpp"
#include "algorithms/MST.hpp"
#include "algorithms/SP.hpp"
#include "algorithms/MF.hpp"

namespace {

using namespace Parameters;

bool algorithmMatchesProblem(Problems p, Algorithms a) {
    if (a == Algorithms::allAlgorithms) {
        return true;
    }
    switch (p) {
        case Problems::mst:
            return a == Algorithms::prim || a == Algorithms::kruskal;
        case Problems::sp:
            return a == Algorithms::dijkstra || a == Algorithms::bellmanFord;
        case Problems::mf:
            return a == Algorithms::fordFulkerson;
        default:
            return false;
    }
}

// Runs the selected algorithm(s) on one concrete representation.
template <typename GraphT>
void solveOn(const GraphData& data, const char* structureName) {
    const bool directed = (problem != Problems::mst);

    GraphT graph(data.vertices, data.edges.getSize(), directed);
    Utils::fillGraph(data, graph);

    std::cout << "\n=== Structure: " << structureName << " ===\n";
    graph.display();

    const bool all = (algorithm == Algorithms::allAlgorithms);

    switch (problem) {
        case Problems::mst:
            if (all || algorithm == Algorithms::prim) {
                std::cout << "\n[Prim]\n";
                MSTAlgorithms::prim(graph);
            }
            if (all || algorithm == Algorithms::kruskal) {
                std::cout << "\n[Kruskal]\n";
                MSTAlgorithms::kruskal(graph);
            }
            break;
        case Problems::sp:
            if (all || algorithm == Algorithms::dijkstra) {
                std::cout << "\n[Dijkstra from vertex " << vertexStart << "]\n";
                SPAlgorithms::dijkstra(graph, vertexStart);
            }
            if (all || algorithm == Algorithms::bellmanFord) {
                std::cout << "\n[Bellman-Ford from vertex " << vertexStart << "]\n";
                SPAlgorithms::bellmanFord(graph, vertexStart);
            }
            break;
        case Problems::mf:
            if (all || algorithm == Algorithms::fordFulkerson) {
                std::cout << "\n[Ford-Fulkerson from " << vertexStart
                          << " to " << vertexEnd << "]\n";
                MFAlgorithms::fordFulkerson(graph, vertexStart, vertexEnd);
            }
            break;
        default:
            break;
    }
}

int runSingleFile() {
    if (inputFile.empty()) {
        std::cerr << "ERROR! Single file mode requires an input file (-i FILE).\n";
        return 1;
    }
    if (problem == Problems::undefined) {
        std::cerr << "ERROR! No problem selected (-p VAL).\n";
        return 1;
    }
    if (algorithm == Algorithms::undefined) {
        std::cerr << "ERROR! No algorithm selected (-a VAL).\n";
        return 1;
    }
    if (structure == Structures::undefined) {
        std::cerr << "ERROR! No structure selected (-s VAL).\n";
        return 1;
    }
    if (!algorithmMatchesProblem(problem, algorithm)) {
        std::cerr << "ERROR! Algorithm " << static_cast<int>(algorithm)
                  << " does not solve problem " << static_cast<int>(problem)
                  << " (see --help).\n";
        return 1;
    }

    GraphData data;
    if (!Utils::loadGraphData(inputFile, data)) {
        return 1;
    }

    // Vertex parameters make sense only for SP/MF and only within [0, V)
    if (problem == Problems::sp || problem == Problems::mf) {
        if (vertexStart < 0 || vertexStart >= data.vertices) {
            std::cerr << "ERROR! Starting vertex (-c) must be in [0, "
                      << data.vertices << ").\n";
            return 1;
        }
    }
    if (problem == Problems::mf) {
        if (vertexEnd < 0 || vertexEnd >= data.vertices) {
            std::cerr << "ERROR! End vertex (-e) must be in [0, "
                      << data.vertices << ").\n";
            return 1;
        }
        if (vertexEnd == vertexStart) {
            std::cerr << "ERROR! Source and sink must differ for max flow.\n";
            return 1;
        }
    }

    // Swapping cout's buffer sends every display()
    // and algorithm printout to the file without changing their code
    std::ofstream outFileStream;
    std::streambuf* consoleBuf = nullptr;
    if (!outputFile.empty()) {
        outFileStream.open(outputFile);
        if (!outFileStream) {
            std::cerr << "ERROR! Cannot open output file: " << outputFile << "\n";
            return 1;
        }
        consoleBuf = std::cout.rdbuf(outFileStream.rdbuf());
    }

    if (structure == Structures::allStructures || structure == Structures::incidenceMatrix) {
        solveOn<IncidenceMatrix>(data, "Incidence matrix");
    }
    if (structure == Structures::allStructures || structure == Structures::adjacencyList) {
        solveOn<AdjacencyList>(data, "Adjacency list");
    }

    if (consoleBuf != nullptr) {
        std::cout.rdbuf(consoleBuf);
        std::cout << "Solution saved to " << outputFile << "\n";
    }
    return 0;
}

int runBenchmark() {
    std::cerr << "Benchmark mode is not implemented yet.\n";
    return 1;
}

} // namespace

int main(int argc, char** argv) {
    // Skip argv[0]
    if (readParameters(argc - 1, argv + 1) != 0) {
        help();
        return 1;
    }

    switch (runMode) {
        case RunModes::help:
            help();
            return 0;
        case RunModes::singleFile:
            return runSingleFile();
        case RunModes::benchmark:
            return runBenchmark();
        default:
            std::cerr << "ERROR! No run mode selected (-f, -b or -h).\n";
            help();
            return 1;
    }
}
