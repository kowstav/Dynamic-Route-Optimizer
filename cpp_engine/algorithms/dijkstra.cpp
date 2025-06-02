#include "../include/algorithms.h"
#include "../include/graph.h"
#include <vector>
#include <queue>
#include <map>

// Computes the shortest path from a start node to an end node using Dijkstra's algorithm.
std::vector<int> Algorithms::dijkstra(const Graph& graph, int startNode, int endNode, double& pathWeight) {
    // Map to store the minimum distance found so far from startNode to each other node.
    std::map<int, double> dist;
    // Map to store the predecessor of each node in the shortest path from startNode.
    std::map<int, int> prev;
    // Priority queue to efficiently select the node with the smallest distance. Stores pairs of (distance, nodeId).
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<std::pair<double, int>>> pq;

    // Initialize distances for all nodes.
    for (int nodeId : graph.getAllNodeIds()) {
        // Set distance to infinity.
        dist[nodeId] = INF;
        // Set predecessor to -1 (undefined).
        prev[nodeId] = -1;
    }

    // Distance from startNode to itself is 0.
    dist[startNode] = 0;
    // Push the start node into the priority queue with distance 0.
    pq.push({0, startNode});

    // Main loop of Dijkstra's algorithm.
    while (!pq.empty()) {
        // Get the node with the smallest distance from the priority queue.
        double d = pq.top().first;
        // Current node being processed.
        int u = pq.top().second;
        // Remove the processed node from the priority queue.
        pq.pop();

        // If a shorter path to u has already been found, skip this iteration.
        if (d > dist[u]) continue;
        // If the destination node is reached, stop.
        if (u == endNode) break;

        // Iterate over all neighbors v of node u.
        if (graph.nodeExists(u)) {
            // For each edge (u,v)
            for (const auto& edge : graph.getEdges(u)) {
                // Neighbor node ID.
                int v = edge.to;
                // Weight of the edge (u,v).
                double weight = edge.weight;
                // If a shorter path to v is found through u.
                if (dist[u] + weight < dist[v]) {
                    // Update distance to v.
                    dist[v] = dist[u] + weight;
                    // Set u as the predecessor of v.
                    prev[v] = u;
                    // Push v into the priority queue with its new distance.
                    pq.push({dist[v], v});
                }
            }
        }
    }

    // Reconstruct the shortest path from endNode back to startNode.
    std::vector<int> path;
    // If endNode is unreachable or is the startNode itself without a path (should not happen if dist[endNode] < INF).
    if (dist[endNode] == INF) {
        // Set path weight to infinity if no path found.
        pathWeight = INF;
        // Return empty path.
        return path; // No path found
    }
    
    // Set the total weight of the found path.
    pathWeight = dist[endNode];
    // Start from the end node.
    int curr = endNode;
    // Traverse backwards using predecessors until startNode or no predecessor.
    while (curr != -1) {
        // Add current node to the path.
        path.push_back(curr);
        // Move to the predecessor of the current node.
        curr = prev[curr];
    }
    // Reverse the path to get it from startNode to endNode.
    std::reverse(path.begin(), path.end());
    // Return the reconstructed path.
    return path;
}