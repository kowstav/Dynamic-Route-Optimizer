#ifndef GRAPH_IO_H
#define GRAPH_IO_H

#include "graph.h"
#include <string>

// Contains functions for reading and writing graph data.
namespace GraphIO {
    bool loadGraphFromJson(const std::string& filepath, Graph& graph);
    std::string saveGraphToJson(const Graph& graph);
}

#endif