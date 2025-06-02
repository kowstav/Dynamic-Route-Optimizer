#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <map>
#include <limits> // Required for std::numeric_limits

// Represents an edge in the graph.
struct Edge {
    // The destination node ID of this edge.
    int to;
    double weight;
    double x, y;
};

// Represents a node in the graph, primarily for storing coordinates if needed.
struct Node {
    int id;
    double x;
    double y;
};


// Represents a directed, weighted graph using an adjacency list.
class Graph {
public:
    std::map<int, std::vector<Edge>> adj;
    std::map<int, Node> nodes;

    // Adds a node to the graph.
    void addNode(int id, double x = 0.0, double y = 0.0);
    void addEdge(int from, int to, double weight);
    bool updateEdgeWeight(int from, int to, double newWeight);
    const std::vector<Edge>& getEdges(int nodeId) const;
    std::vector<int> getAllNodeIds() const;
    const Node* getNode(int id) const;
    // Checks if a node exists in the graph.
    bool nodeExists(int id) const;
};

// Standard infinity representation for pathfinding algorithms.
const double INF = std::numeric_limits<double>::infinity();

#endif