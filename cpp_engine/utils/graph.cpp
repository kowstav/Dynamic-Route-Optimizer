#include "../include/graph.h"
#include <stdexcept> // For std::out_of_range

// Adds a node to the graph.
void Graph::addNode(int id, double x, double y) {
    // If the node doesn't already exist in the adjacency list, add it.
    if (adj.find(id) == adj.end()) {
        // Initialize an empty vector of edges for the new node.
        adj[id] = std::vector<Edge>();
    }
    // If the node doesn't already exist in the nodes map, add it.
    if (nodes.find(id) == nodes.end()) {
        // Store the node's properties.
        nodes[id] = {id, x, y};
    } else {
        // Update coordinates if node already exists (optional behavior)
        nodes[id].x = x;
        // Update y coordinate
        nodes[id].y = y;
    }
}

// Adds a directed edge to the graph from 'from' node to 'to' node with a given weight.
void Graph::addEdge(int from, int to, double weight) {
    // Ensure 'from' node exists, or add it.
    if (!nodeExists(from)) addNode(from);
    // Ensure 'to' node exists, or add it.
    if (!nodeExists(to)) addNode(to);
    // Add the edge to the adjacency list of the 'from' node.
    // Node coordinates are not directly part of Edge here, but could be looked up from graph.nodes if needed.
    adj[from].push_back({to, weight, 0.0, 0.0}); // x, y in Edge are placeholders for this example
}

// Updates the weight of an existing edge.
bool Graph::updateEdgeWeight(int from, int to, double newWeight) {
    // Check if the 'from' node exists.
    if (adj.count(from)) {
        // Iterate through edges of the 'from' node.
        for (Edge& edge : adj[from]) {
            // If the destination node matches, update the weight.
            if (edge.to == to) {
                // Set the new weight for the edge.
                edge.weight = newWeight;
                // Indicate successful update.
                return true;
            }
        }
    }
    // Indicate failure if edge or 'from' node not found.
    return false;
}


// Returns the list of edges for a given node.
const std::vector<Edge>& Graph::getEdges(int nodeId) const {
    // Attempt to find the node in the adjacency list.
    auto it = adj.find(nodeId);
    // If node is found, return its edges.
    if (it != adj.end()) {
        // Return the vector of edges.
        return it->second;
    }
    // Throw an exception if the node is not found.
    throw std::out_of_range("Node not found in graph");
}

// Returns all node IDs in the graph.
std::vector<int> Graph::getAllNodeIds() const {
    // Initialize a vector to store node IDs.
    std::vector<int> ids;
    // Iterate through the adjacency list map.
    for (const auto& pair : adj) {
        // Add the key (node ID) to the vector.
        ids.push_back(pair.first);
    }
    // Return the vector of node IDs.
    return ids;
}

// Returns the Node object for a given ID.
const Node* Graph::getNode(int id) const {
    // Attempt to find the node in the nodes map.
    auto it = nodes.find(id);
    // If node is found, return a pointer to it.
    if (it != nodes.end()) {
        // Return address of the Node object.
        return &(it->second);
    }
    // Return nullptr if node not found.
    return nullptr;
}

// Checks if a node exists in the graph.
bool Graph::nodeExists(int id) const {
    // Check if the node ID is a key in the adjacency map.
    return adj.count(id) > 0;
}