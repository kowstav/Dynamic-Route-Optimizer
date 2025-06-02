#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "graph.h"
#include <vector>
#include <map>

// Contains functions for various graph algorithms.
namespace Algorithms {
    std::vector<int> dijkstra(const Graph& graph, int startNode, int endNode, double& pathWeight);
    std::vector<int> aStar(const Graph& graph, int startNode, int endNode, double& pathWeight);
    std::map<int, std::map<int, double>> floydWarshall(const Graph& graph, std::map<int, std::map<int, int>>& predecessors);
}

#endif