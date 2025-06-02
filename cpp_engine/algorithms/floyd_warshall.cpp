#include "../include/algorithms.h"
#include "../include/graph.h"
#include <vector>
#include <map>

// Computes all-pairs shortest paths using Floyd-Warshall algorithm.
std::map<int, std::map<int, double>> Algorithms::floydWarshall(const Graph& graph, std::map<int, std::map<int, int>>& predecessors) {
    // Map to store shortest distances between pairs of nodes. dist[i][j] is distance from i to j.
    std::map<int, std::map<int, double>> dist;
    // Get all node IDs from the graph.
    std::vector<int> nodeIds = graph.getAllNodeIds();
    // Get the number of nodes.
    int numNodes = nodeIds.size();
    // Map node IDs to 0-based indices for easier array-like access if preferred, but direct map usage is fine.
    std::map<int, int> nodeToIndex;
    // Map 0-based indices back to node IDs.
    std::vector<int> indexToNode(numNodes);

    // Populate mapping from node ID to index and vice versa.
    for (int i = 0; i < numNodes; ++i) {
        // Map node ID to its index.
        nodeToIndex[nodeIds[i]] = i;
        // Map index back to its node ID.
        indexToNode[i] = nodeIds[i];
    }

    // Initialize distances and predecessors.
    for (int u_id : nodeIds) {
        // For each node v_id.
        for (int v_id : nodeIds) {
            // If u_id is the same as v_id, distance is 0.
            if (u_id == v_id) {
                // Distance from a node to itself is 0.
                dist[u_id][v_id] = 0;
                // Predecessor of v_id in path from u_id is u_id itself.
                predecessors[u_id][v_id] = u_id;
            } else {
                // Initialize distance to infinity.
                dist[u_id][v_id] = INF;
                // Initialize predecessor to -1 (no path yet).
                predecessors[u_id][v_id] = -1;
            }
        }
    }

    // Initialize distances with direct edge weights.
    for (int u_id : nodeIds) {
        // Check if node u_id exists in graph (should always be true here).
        if (graph.nodeExists(u_id)) {
            // For each edge starting from u_id.
            for (const auto& edge : graph.getEdges(u_id)) {
                // The destination node of the edge.
                int v_id = edge.to;
                // The weight of the edge.
                double weight = edge.weight;
                // Set the direct distance.
                dist[u_id][v_id] = weight;
                // Predecessor of v_id in path from u_id is u_id.
                predecessors[u_id][v_id] = u_id;
            }
        }
    }

    // Main Floyd-Warshall algorithm loops.
    for (int k_id : nodeIds) { // Intermediate node
        // For each source node u_id.
        for (int u_id : nodeIds) { // Source node
            // For each destination node v_id.
            for (int v_id : nodeIds) { // Destination node
                // If path through k_id is shorter.
                if (dist[u_id][k_id] != INF && dist[k_id][v_id] != INF &&
                    dist[u_id][k_id] + dist[k_id][v_id] < dist[u_id][v_id]) {
                    // Update shortest distance from u_id to v_id.
                    dist[u_id][v_id] = dist[u_id][k_id] + dist[k_id][v_id];
                    // Update predecessor of v_id to be the same as predecessor of v_id in path from k_id.
                    predecessors[u_id][v_id] = predecessors[k_id][v_id];
                }
            }
        }
    }
    // Return the map of all-pairs shortest distances.
    return dist;
}