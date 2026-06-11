#include <iostream>
#include "graph/AdjacencyList.hpp"
#include "graph/IncidenceMatrix.hpp"
#include "algorithms/MST.hpp"
#include "algorithms/SP.hpp"
#include "algorithms/MF.hpp"
#include "GraphGenerator.hpp"
#include "Utils.hpp"



static void runMSTDemo() {
    std::cout << "========================================\n";
    std::cout << "  MST DEMO — undirected weighted graph  \n";
    std::cout << "========================================\n\n";

    std::cout << "Graph:\n";
    std::cout << "  Vertices: 0 1 2 3 4\n";
    std::cout << "  Edges: 0-1(2), 0-3(6), 1-2(3), 1-3(8), 1-4(5), 2-4(7), 3-4(9)\n\n";

    const int V = 5, E = 7;

    // --- Adjacency List ---
    AdjacencyList al(V, E, false);
    al.addEdge(0, 1, 2);
    al.addEdge(0, 3, 6);
    al.addEdge(1, 2, 3);
    al.addEdge(1, 3, 8);
    al.addEdge(1, 4, 5);
    al.addEdge(2, 4, 7);
    al.addEdge(3, 4, 9);

    std::cout << "--- Representation: Adjacency List ---\n";
    al.display();

    std::cout << "\n[Prim]\n";
    MSTAlgorithms::prim(al);

    std::cout << "\n[Kruskal]\n";
    MSTAlgorithms::kruskal(al);

    // --- Incidence Matrix ---
    IncidenceMatrix im(V, E, false);
    im.addEdge(0, 1, 2);
    im.addEdge(0, 3, 6);
    im.addEdge(1, 2, 3);
    im.addEdge(1, 3, 8);
    im.addEdge(1, 4, 5);
    im.addEdge(2, 4, 7);
    im.addEdge(3, 4, 9);

    std::cout << "\n--- Representation: Incidence Matrix ---\n";
    im.display();

    std::cout << "\n[Prim]\n";
    MSTAlgorithms::prim(im);

    std::cout << "\n[Kruskal]\n";
    MSTAlgorithms::kruskal(im);
}

// Directed graph used for Dijkstra:
//
// Vertices: 0 1 2 3 4
// Edges (directed):
//   0->1 weight 10
//   0->2 weight 3
//   1->3 weight 2
//   2->1 weight 4
//   2->3 weight 8
//   2->4 weight 2
//   3->4 weight 5
//   4->3 weight 1
//
// Shortest paths from 0:
//   to 0: 0  (itself)
//   to 1: 7  (0->2->1)
//   to 2: 3  (0->2)
//   to 3: 9  (0->2->4->3)
//   to 4: 5  (0->2->4)

static void runSPDemo() {
    std::cout << "\n========================================\n";
    std::cout << "   SP DEMO — directed weighted graph    \n";
    std::cout << "========================================\n\n";

    std::cout << "Graph:\n";
    std::cout << "  Vertices: 0 1 2 3 4\n";
    std::cout << "  Edges: 0->1(10), 0->2(3), 1->3(2), 2->1(4),\n";
    std::cout << "         2->3(8), 2->4(2), 3->4(5), 4->3(1)\n\n";

    const int V = 5, E = 8;

    AdjacencyList al(V, E, true);
    al.addEdge(0, 1, 10);
    al.addEdge(0, 2, 3);
    al.addEdge(1, 3, 2);
    al.addEdge(2, 1, 4);
    al.addEdge(2, 3, 8);
    al.addEdge(2, 4, 2);
    al.addEdge(3, 4, 5);
    al.addEdge(4, 3, 1);

    std::cout << "--- Representation: Adjacency List ---\n";
    al.display();

    std::cout << "\n[Dijkstra from vertex 0]\n";
    SPAlgorithms::dijkstra(al, 0);

    IncidenceMatrix im(V, E, true);
    im.addEdge(0, 1, 10);
    im.addEdge(0, 2, 3);
    im.addEdge(1, 3, 2);
    im.addEdge(2, 1, 4);
    im.addEdge(2, 3, 8);
    im.addEdge(2, 4, 2);
    im.addEdge(3, 4, 5);
    im.addEdge(4, 3, 1);

    std::cout << "\n--- Representation: Incidence Matrix ---\n";
    im.display();

    std::cout << "\n[Dijkstra from vertex 0]\n";
    SPAlgorithms::dijkstra(im, 0);
}

// Directed graph with negative weight used for Bellman-Ford:
//
// Vertices: 0 1 2 3 4
// Edges (directed):
//   0->1 weight  6
//   0->2 weight  7
//   1->2 weight  8
//   1->3 weight -4
//   1->4 weight  5
//   2->4 weight -3
//   3->0 weight  2
//   4->3 weight  7
//
// Shortest paths from 0:
//   to 0: 0  (itself)
//   to 1: 6  (0->1)
//   to 2: 7  (0->2)
//   to 3: 2  (0->1->3, cost 6+(-4)=2)
//   to 4: 4  (0->2->4, cost 7+(-3)=4)

static void runBFDemo() {
    std::cout << "\n========================================\n";
    std::cout << " SP DEMO — Bellman-Ford (negative weights)\n";
    std::cout << "========================================\n\n";

    std::cout << "Graph:\n";
    std::cout << "  Edges: 0->1(6), 0->2(7), 1->2(8), 1->3(-4),\n";
    std::cout << "         1->4(5), 2->4(-3), 3->0(2), 4->3(7)\n\n";

    const int V = 5, E = 8;

    AdjacencyList al(V, E, true);
    al.addEdge(0, 1,  6);
    al.addEdge(0, 2,  7);
    al.addEdge(1, 2,  8);
    al.addEdge(1, 3, -4);
    al.addEdge(1, 4,  5);
    al.addEdge(2, 4, -3);
    al.addEdge(3, 0,  2);
    al.addEdge(4, 3,  7);

    std::cout << "--- Representation: Adjacency List ---\n";
    al.display();

    std::cout << "\n[Bellman-Ford from vertex 0]\n";
    SPAlgorithms::bellmanFord(al, 0);

    std::cout << "\n[Dijkstra from vertex 0 — same graph, for comparison]\n";
    SPAlgorithms::dijkstra(al, 0);

    IncidenceMatrix im(V, E, true);
    im.addEdge(0, 1,  6);
    im.addEdge(0, 2,  7);
    im.addEdge(1, 2,  8);
    im.addEdge(1, 3, -4);
    im.addEdge(1, 4,  5);
    im.addEdge(2, 4, -3);
    im.addEdge(3, 0,  2);
    im.addEdge(4, 3,  7);

    std::cout << "\n--- Representation: Incidence Matrix ---\n";
    im.display();

    std::cout << "\n[Bellman-Ford from vertex 0]\n";
    SPAlgorithms::bellmanFord(im, 0);
}

// Directed flow network (CLRS, chapter on maximum flow):
//
// Vertices: 0 (source) 1 2 3 4 5 (sink)
// Edges (directed, capacities):
//   0->1 16, 0->2 13, 1->3 12, 2->1 4, 2->4 14,
//   3->2 9, 3->5 20, 4->3 7, 4->5 4
//
// Known maximum flow from 0 to 5: 23

static void runMFDemo() {
    std::cout << "\n========================================\n";
    std::cout << "  MF DEMO — directed flow network        \n";
    std::cout << "========================================\n\n";

    std::cout << "Graph:\n";
    std::cout << "  Edges: 0->1(16), 0->2(13), 1->3(12), 2->1(4), 2->4(14),\n";
    std::cout << "         3->2(9), 3->5(20), 4->3(7), 4->5(4)\n";
    std::cout << "  Expected max flow 0 -> 5: 23\n\n";

    const int V = 6, E = 9;

    AdjacencyList al(V, E, true);
    al.addEdge(0, 1, 16);
    al.addEdge(0, 2, 13);
    al.addEdge(1, 3, 12);
    al.addEdge(2, 1, 4);
    al.addEdge(2, 4, 14);
    al.addEdge(3, 2, 9);
    al.addEdge(3, 5, 20);
    al.addEdge(4, 3, 7);
    al.addEdge(4, 5, 4);

    std::cout << "--- Representation: Adjacency List ---\n";
    al.display();

    std::cout << "\n[Ford-Fulkerson from 0 to 5]\n";
    MFAlgorithms::fordFulkerson(al, 0, 5);

    IncidenceMatrix im(V, E, true);
    im.addEdge(0, 1, 16);
    im.addEdge(0, 2, 13);
    im.addEdge(1, 3, 12);
    im.addEdge(2, 1, 4);
    im.addEdge(2, 4, 14);
    im.addEdge(3, 2, 9);
    im.addEdge(3, 5, 20);
    im.addEdge(4, 3, 7);
    im.addEdge(4, 5, 4);

    std::cout << "\n--- Representation: Incidence Matrix ---\n";
    im.display();

    std::cout << "\n[Ford-Fulkerson from 0 to 5]\n";
    MFAlgorithms::fordFulkerson(im, 0, 5);
}

// Round trip: generator -> TSV file -> loader -> both representations -> MST.
static void runFileDemo() {
    std::cout << "\n========================================\n";
    std::cout << " FILE DEMO — generate, save, load, solve \n";
    std::cout << "========================================\n\n";

    const std::string filename = "demo_graph.tsv";
    GraphGenerator::generateAndSave(filename, 6, 60.0, GraphType::UNDIRECTED);

    GraphData data;
    if (!Utils::loadGraphData(filename, data)) {
        return;
    }

    AdjacencyList al(data.vertices, data.edges.getSize(), false);
    Utils::fillGraph(data, al);

    std::cout << "\n--- Representation: Adjacency List ---\n";
    al.display();
    std::cout << "\n[Prim]\n";
    MSTAlgorithms::prim(al);

    IncidenceMatrix im(data.vertices, data.edges.getSize(), false);
    Utils::fillGraph(data, im);

    std::cout << "\n--- Representation: Incidence Matrix ---\n";
    im.display();
    std::cout << "\n[Prim]\n";
    MSTAlgorithms::prim(im);
}

int main() {
    runMSTDemo();
    runSPDemo();
    runBFDemo();
    runMFDemo();
    runFileDemo();
    return 0;
}
