#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <limits>
#include "Parameters.h"
#include "Utils.hpp"
#include "GraphGenerator.hpp"
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

// Checks shared by single-file and benchmark modes
bool validateCommonParameters() {
    if (problem == Problems::undefined) {
        std::cerr << "ERROR! No problem selected (-p VAL).\n";
        return false;
    }
    if (algorithm == Algorithms::undefined) {
        std::cerr << "ERROR! No algorithm selected (-a VAL).\n";
        return false;
    }
    if (structure == Structures::undefined) {
        std::cerr << "ERROR! No structure selected (-s VAL).\n";
        return false;
    }
    if (!algorithmMatchesProblem(problem, algorithm)) {
        std::cerr << "ERROR! Algorithm " << static_cast<int>(algorithm)
                  << " does not solve problem " << static_cast<int>(problem)
                  << " (see --help).\n";
        return false;
    }
    return true;
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
    if (!validateCommonParameters()) {
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

const char* problemName(Problems p) {
    switch (p) {
        case Problems::mst: return "mst";
        case Problems::sp:  return "sp";
        case Problems::mf:  return "mf";
        default:            return "?";
    }
}

const char* algorithmName(Algorithms a) {
    switch (a) {
        case Algorithms::prim:          return "prim";
        case Algorithms::kruskal:       return "kruskal";
        case Algorithms::dijkstra:      return "dijkstra";
        case Algorithms::bellmanFord:   return "bellmanFord";
        case Algorithms::fordFulkerson: return "fordFulkerson";
        default:                        return "?";
    }
}

// Expands "-a 0" into the concrete algorithm list for the chosen problem
int algorithmsForRun(Algorithms list[2]) {
    const bool all = (algorithm == Algorithms::allAlgorithms);
    int n = 0;
    switch (problem) {
        case Problems::mst:
            if (all || algorithm == Algorithms::prim)        list[n++] = Algorithms::prim;
            if (all || algorithm == Algorithms::kruskal)     list[n++] = Algorithms::kruskal;
            break;
        case Problems::sp:
            if (all || algorithm == Algorithms::dijkstra)    list[n++] = Algorithms::dijkstra;
            if (all || algorithm == Algorithms::bellmanFord) list[n++] = Algorithms::bellmanFord;
            break;
        case Problems::mf:
            if (all || algorithm == Algorithms::fordFulkerson) list[n++] = Algorithms::fordFulkerson;
            break;
        default:
            break;
    }
    return n;
}

struct Stats {
    long long sum = 0;
    long long mn = std::numeric_limits<long long>::max();
    long long mx = 0;
    int n = 0;

    void add(long long t) {
        sum += t;
        if (t < mn) mn = t;
        if (t > mx) mx = t;
        ++n;
    }
};

std::string currentTimestamp() {
    const std::time_t now =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return buf;
}

template <typename GraphT>
long long timeAlgorithm(const GraphT& graph, Algorithms alg) {
    using Clock = std::chrono::steady_clock;

    const auto t0 = Clock::now();
    switch (alg) {
        case Algorithms::prim:          MSTAlgorithms::prim(graph, false); break;
        case Algorithms::kruskal:       MSTAlgorithms::kruskal(graph, false); break;
        case Algorithms::dijkstra:      SPAlgorithms::dijkstra(graph, vertexStart, false); break;
        case Algorithms::bellmanFord:   SPAlgorithms::bellmanFord(graph, vertexStart, false); break;
        case Algorithms::fordFulkerson: MFAlgorithms::fordFulkerson(graph, vertexStart, vertexEnd, false); break;
        default: break;
    }
    const auto t1 = Clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}

template <typename GraphT>
void benchStructure(const GraphData& data, const char* structName,
                    std::ofstream& csv, const std::string& timestamp, int iteration,
                    const Algorithms* algs, int algCount, Stats* stats) {
    const bool directed = (problem != Problems::mst);
    GraphT graph(data.vertices, data.edges.getSize(), directed);
    Utils::fillGraph(data, graph);

    for (int i = 0; i < algCount; ++i) {
        const long long us = timeAlgorithm(graph, algs[i]);

        csv << timestamp << ',' << problemName(problem) << ','
            << algorithmName(algs[i]) << ',' << structName << ','
            << data.vertices << ',' << density << ',' << iteration << ','
            << us << '\n';

        stats[i].add(us);
    }
}

int runBenchmark() {
    if (!validateCommonParameters()) {
        return 1;
    }
    if (resultsFile.empty()) {
        std::cerr << "ERROR! Benchmark mode requires a results file (-r FILE).\n";
        return 1;
    }
    if (vertexCount < 2) {
        std::cerr << "ERROR! Vertex count (-l) must be at least 2.\n";
        return 1;
    }
    if (density <= 0 || density > 100) {
        std::cerr << "ERROR! Density (-d) must be in (0, 100].\n";
        return 1;
    }
    if (iterations < 1) {
        std::cerr << "ERROR! Iteration count (-n) must be at least 1.\n";
        return 1;
    }

    // SP/MF need endpoints; the generator guarantees reachability from 0,
    // so default to source 0 and sink V-1 when not given explicitly
    if (problem == Problems::sp || problem == Problems::mf) {
        if (vertexStart < 0) vertexStart = 0;
        if (vertexStart >= vertexCount) {
            std::cerr << "ERROR! Starting vertex (-c) must be in [0, " << vertexCount << ").\n";
            return 1;
        }
    }
    if (problem == Problems::mf) {
        if (vertexEnd < 0) vertexEnd = vertexCount - 1;
        if (vertexEnd >= vertexCount || vertexEnd == vertexStart) {
            std::cerr << "ERROR! End vertex (-e) must be in [0, " << vertexCount
                      << ") and differ from the source.\n";
            return 1;
        }
    }

    bool writeHeader;
    {
        std::ifstream probe(resultsFile);
        writeHeader = !probe || probe.peek() == std::ifstream::traits_type::eof();
    }
    std::ofstream csv(resultsFile, std::ios::app);
    if (!csv) {
        std::cerr << "ERROR! Cannot open results file: " << resultsFile << "\n";
        return 1;
    }
    if (writeHeader) {
        csv << "timestamp,problem,algorithm,structure,vertices,density,iteration,time_us\n";
    }

    Algorithms algs[2];
    const int algCount = algorithmsForRun(algs);
    const bool useMatrix = (structure == Structures::allStructures ||
                            structure == Structures::incidenceMatrix);
    const bool useList   = (structure == Structures::allStructures ||
                            structure == Structures::adjacencyList);

    const GraphType type = (problem == Problems::mst) ? GraphType::UNDIRECTED
                                                      : GraphType::DIRECTED;
    const std::string timestamp = currentTimestamp();

    // stats[0] = incidence matrix, stats[1] = adjacency list
    Stats stats[2][2];

    std::cout << "Benchmark: problem=" << problemName(problem)
              << ", V=" << vertexCount << ", density=" << density
              << "%, iterations=" << iterations << "\n";

    for (int iter = 0; iter < iterations; ++iter) {
        GraphData data;
        GraphGenerator::generate(data, vertexCount, static_cast<double>(density), type);

        if (useMatrix) {
            benchStructure<IncidenceMatrix>(data, "incidenceMatrix", csv, timestamp,
                                            iter, algs, algCount, stats[0]);
        }
        if (useList) {
            benchStructure<AdjacencyList>(data, "adjacencyList", csv, timestamp,
                                          iter, algs, algCount, stats[1]);
        }
    }

    std::cout << "Results appended to " << resultsFile << "\n\n";
    for (int s = 0; s < 2; ++s) {
        const bool used = (s == 0) ? useMatrix : useList;
        if (!used) continue;
        const char* structName = (s == 0) ? "incidenceMatrix" : "adjacencyList";
        for (int i = 0; i < algCount; ++i) {
            const Stats& st = stats[s][i];
            std::cout << structName << " / " << algorithmName(algs[i])
                      << ": avg=" << (st.sum / static_cast<double>(st.n))
                      << " us, min=" << st.mn << " us, max=" << st.mx
                      << " us (n=" << st.n << ")\n";
        }
    }
    return 0;
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
