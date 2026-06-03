#include <iostream>
#include "graph/AdjacencyList.hpp"
#include "graph/IncidenceMatrix.hpp"
#include "algorithms/MST.hpp"
#include "algorithms/SP.hpp"



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

int main() {
    runMSTDemo();
    runSPDemo();
    runBFDemo();
    return 0;
}
