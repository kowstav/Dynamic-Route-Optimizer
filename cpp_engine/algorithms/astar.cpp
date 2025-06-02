#include "../include/algorithms.h"
#include "../include/graph.h"
#include <vector>
#include <queue>
#include <map>
#include <cmath> // For std::sqrt, std::pow (heuristic)


// Heuristic function (Euclidean distance) for A*. Requires node coordinates.
double heuristic(const Node* node1, const Node* node2) {
    // If either node is null, return 0 (no heuristic).
    if (!node1 || !node2) return 0.0;
    // Calculate difference in x coordinates.
    double dx = node1->x - node2->x;
    // Calculate difference in y coordinates.
    double dy = node1->y - node2->y;
    // Return Euclidean distance.
    return std::sqrt(dx*dx + dy*dy);
}

// Computes the shortest path from a start node to an end node using A* algorithm.
std::vector<int> Algorithms::aStar(const Graph& graph, int startNode, int endNode, double& pathWeight) {
    // Priority queue storing (f_score, nodeId), where f_score = g_score + h_score.
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<std::pair<double, int>>> openSet;
    // Map to store the predecessor of each node in the path.
    std::map<int, int> cameFrom;
    // g_score[n] is the cost of the cheapest path from start to n currently known.
    std::map<int, double> gScore;
    // f_score[n] = g_score[n] + h(n). It represents our current best guess as to
    // how short a path from start to finish can be if it goes through n.
    std::map<int, double> fScore;

    // Get the Node object for the end node for heuristic calculation.
    const Node* endNodeObj = graph.getNode(endNode);

    // Initialize scores for all nodes.
    for (int nodeId : graph.getAllNodeIds()) {
        // Set gScore to infinity.
        gScore[nodeId] = INF;
        // Set fScore to infinity.
        fScore[nodeId] = INF;
    }

    // gScore of startNode is 0.
    gScore[startNode] = 0;
    // Get the Node object for the start node.
    const Node* startNodeObj = graph.getNode(startNode);
    // fScore of startNode is its heuristic cost to endNode.
    fScore[startNode] = heuristic(startNodeObj, endNodeObj);
    // Add startNode to the open set.
    openSet.push({fScore[startNode], startNode});

    // Main loop of A* algorithm.
    while (!openSet.empty()) {
        // Get the node in openSet having the lowest fScore value.
        int current = openSet.top().second;
        // Remove current from openSet.
        openSet.pop();

        // If the current node is the end node, path is found.
        if (current == endNode) {
            // Path weight is the gScore of the end node.
            pathWeight = gScore[endNode];
            // Reconstruct the path.
            std::vector<int> path;
            // Start from the end node.
            int temp = current;
            // Traverse backwards using cameFrom map.
            while (cameFrom.count(temp)) {
                // Add current node to path.
                path.push_back(temp);
                // Move to the predecessor.
                temp = cameFrom[temp];
            }
            // Add the start node to the path.
            path.push_back(startNode);
            // Reverse the path to get it from start to end.
            std::reverse(path.begin(), path.end());
            // Return the reconstructed path.
            return path;
        }

        // Iterate over neighbors of the current node.
        if (graph.nodeExists(current)) {
            // Get the Node object for the current node.
            const Node* currentNodeObj = graph.getNode(current);
            // For each edge from current to neighbor.
            for (const auto& edge : graph.getEdges(current)) {
                // Neighbor node ID.
                int neighbor = edge.to;
                // Cost from current to neighbor.
                double tentative_gScore = gScore[current] + edge.weight;

                // If this path to neighbor is better than any previous one.
                if (tentative_gScore < gScore[neighbor]) {
                    // Record this path.
                    cameFrom[neighbor] = current;
                    // Update gScore for neighbor.
                    gScore[neighbor] = tentative_gScore;
                    // Get the Node object for the neighbor.
                    const Node* neighborNodeObj = graph.getNode(neighbor);
                    // Update fScore for neighbor.
                    fScore[neighbor] = gScore[neighbor] + heuristic(neighborNodeObj, endNodeObj);
                    // Add neighbor to openSet if not already present (priority queue handles this implicitly by potentially adding duplicates, effectively an update).
                    openSet.push({fScore[neighbor], neighbor});
                }
            }
        }
    }

    // Open set is empty but goal was never reached.
    pathWeight = INF;
    // Return an empty path indicating failure.
    return {};
}