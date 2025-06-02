#include "include/graph.h"
#include "include/algorithms.h"
#include "include/union_find.h"
// SegmentTree not directly used in CLI for this basic version, but could be for "update_traffic"
#include "include/segment_tree.h"
#include "include/graph_io.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream> // For parsing command arguments
#include <iomanip> // For std::fixed and std::setprecision


// Global graph object for the CLI.
Graph g;
// Global UnionFind object, initialized when graph is loaded.
UnionFind* uf = nullptr;
// Placeholder for segment tree if we map edges to an array for dynamic updates.
// SegmentTree* st = nullptr; // This would require a more complex setup.

// Helper function to split string by delimiter
std::vector<std::string> split(const std::string& s, char delimiter) {
   // Vector to store tokens.
   std::vector<std::string> tokens;
   // Current token being built.
   std::string token;
   // String stream from input string.
   std::istringstream tokenStream(s);
   // Read tokens separated by delimiter.
   while (std::getline(tokenStream, token, delimiter)) {
      // Add token to the vector.
      tokens.push_back(token);
   }
   // Return vector of tokens.
   return tokens;
}

// Main function for the C++ engine's command-line interface.
int main(int argc, char* argv[]) {
    // If arguments are provided directly to main (e.g. for single command execution).
    if (argc > 1) {
        // Concatenate all arguments into a single command string.
        std::string command_str;
        // Start from the first argument (skip program name).
        for (int i = 1; i < argc; ++i) {
            // Append argument to command string.
            command_str += argv[i];
            // Add space if not the last argument.
            if (i < argc - 1) {
                // Add space separator.
                command_str += " ";
            }
        }
        // Parse the command string.
        std::vector<std::string> args = split(command_str, ' ');
        // Command is the first argument.
        std::string command = args[0];

        // Command to load a graph from a JSON file.
        if (command == "load_graph" && args.size() > 1) {
            // Load graph from specified file path.
            if (GraphIO::loadGraphFromJson(args[1], g)) {
                // Print success message.
                std::cout << "Graph loaded successfully from " << args[1] << std::endl;
                // Initialize UnionFind with node IDs from the loaded graph.
                delete uf; // Delete old instance if any
                // Create new UnionFind instance.
                uf = new UnionFind(g.getAllNodeIds());
            } else {
                // Print error message.
                std::cout << "Error: Could not load graph from " << args[1] << std::endl;
                // Return error code.
                return 1;
            }
        }
        // Command to add a node.
        else if (command == "add_node" && args.size() >= 2) {
            // Parse node ID.
            int id = std::stoi(args[1]);
            // Default x coordinate.
            double x = 0.0, y = 0.0;
            // If x coordinate is provided.
            if (args.size() >= 3) x = std::stod(args[2]);
            // If y coordinate is provided.
            if (args.size() >= 4) y = std::stod(args[3]);
            // Add node to the graph.
            g.addNode(id, x, y);
            // If UnionFind is initialized, add node to it as well.
            if (uf) uf->makeSet(id); // Ensure new nodes are part of UF
            // Print success message.
            std::cout << "Node " << id << " added." << std::endl;
        }
        // Command to add an edge.
        else if (command == "add_edge" && args.size() == 4) {
            // Parse 'from' node ID.
            int from = std::stoi(args[1]);
            // Parse 'to' node ID.
            int to = std::stoi(args[2]);
            // Parse edge weight.
            double weight = std::stod(args[3]);
            // Add edge to the graph.
            g.addEdge(from, to, weight);
            // Print success message.
            std::cout << "Edge from " << from << " to " << to << " with weight " << weight << " added." << std::endl;
        }
        // Command to find the shortest path.
        else if (command == "shortest_path" && args.size() == 4) {
            // Algorithm type (dijkstra or astar).
            std::string algo_type = args[1];
            // Parse start node ID.
            int start = std::stoi(args[2]);
            // Parse end node ID.
            int end = std::stoi(args[3]);
            // Vector to store the path.
            std::vector<int> path;
            // Variable to store path weight.
            double pathWeight = 0;

            // If algorithm is Dijkstra.
            if (algo_type == "dijkstra") {
                // Compute shortest path using Dijkstra.
                path = Algorithms::dijkstra(g, start, end, pathWeight);
            // Else if algorithm is A*.
            } else if (algo_type == "astar") {
                // Compute shortest path using A*.
                path = Algorithms::aStar(g, start, end, pathWeight);
            } else {
                // Print error for unknown algorithm.
                std::cout << "Error: Unknown algorithm " << algo_type << ". Use 'dijkstra' or 'astar'." << std::endl;
                // Return error code.
                return 1;
            }

            // If a path is found.
            if (!path.empty()) {
                // Print path found message.
                std::cout << "Path: ";
                // Iterate through nodes in the path.
                for (size_t i = 0; i < path.size(); ++i) {
                    // Print node ID.
                    std::cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
                }
                // Set output precision for weight.
                std::cout << std::fixed << std::setprecision(2);
                // Print path weight.
                std::cout << "\nWeight: " << pathWeight << std::endl;
            } else {
                // Print message if no path found.
                std::cout << "No path found from " << start << " to " << end << "." << std::endl;
            }
        }
        // Command to update edge weight (simulates traffic update).
        else if (command == "update_edge_weight" && args.size() == 4) {
            // Parse 'from' node ID.
            int from = std::stoi(args[1]);
            // Parse 'to' node ID.
            int to = std::stoi(args[2]);
            // Parse new weight.
            double new_weight = std::stod(args[3]);
            // Update edge weight in the graph.
            if (g.updateEdgeWeight(from, to, new_weight)) {
                // Print success message.
                std::cout << "Weight of edge from " << from << " to " << to << " updated to " << new_weight << std::endl;
                // Here you would also update the segment tree if it's being used for these edges.
            } else {
                // Print error if edge not found.
                std::cout << "Error: Edge from " << from << " to " << to << " not found for update." << std::endl;
            }
        }
        // Command to get all-pairs shortest paths using Floyd-Warshall.
        else if (command == "get_all_pairs_shortest_paths" && args.size() == 1) {
            // Map to store predecessors for path reconstruction (not fully utilized in this CLI output).
            std::map<int, std::map<int, int>> predecessors;
            // Compute all-pairs shortest paths.
            auto distances = Algorithms::floydWarshall(g, predecessors);
            // Print distances header.
            std::cout << "All-pairs shortest paths (Floyd-Warshall):\n";
            // Set output precision.
            std::cout << std::fixed << std::setprecision(2);
            // Iterate through source nodes.
            for (const auto& pair_u : distances) {
                // Iterate through destination nodes.
                for (const auto& pair_v : pair_u.second) {
                    // Print path information.
                    std::cout << "From " << pair_u.first << " to " << pair_v.first << ": ";
                    // If distance is infinity.
                    if (pair_v.second == INF) {
                        // Print infinity symbol.
                        std::cout << "INF\n";
                    } else {
                        // Print distance.
                        std::cout << pair_v.second << "\n";
                    }
                }
            }
        }
        // Command to find the set (representative) of a node in Union-Find.
        else if (command == "find_set" && args.size() == 2) {
            // If UnionFind is not initialized.
            if (!uf) { std::cout << "Error: Graph not loaded, UnionFind not initialized." << std::endl; return 1; }
            // Parse node ID.
            int node_id = std::stoi(args[1]);
            // If node does not exist in graph (and thus UF).
            if (!g.nodeExists(node_id)) {
                // Print error.
                std::cout << "Error: Node " << node_id << " not found in graph." << std::endl;
                // Return error code.
                return 1;
            }
            // Print the representative of the set.
            std::cout << "Set for node " << node_id << ": " << uf->findSet(node_id) << std::endl;
        }
        // Command to unite the sets of two nodes in Union-Find.
        else if (command == "unite_sets" && args.size() == 3) {
            // If UnionFind is not initialized.
            if (!uf) { std::cout << "Error: Graph not loaded, UnionFind not initialized." << std::endl; return 1; }
            // Parse first node ID.
            int node_id1 = std::stoi(args[1]);
            // Parse second node ID.
            int node_id2 = std::stoi(args[2]);
             // If either node does not exist in graph.
            if (!g.nodeExists(node_id1) || !g.nodeExists(node_id2)) {
                // Print error.
                std::cout << "Error: One or both nodes not found in graph for unite operation." << std::endl;
                // Return error code.
                return 1;
            }
            // Unite the sets.
            uf->uniteSets(node_id1, node_id2);
            // Print success message.
            std::cout << "United sets containing node " << node_id1 << " and " << node_id2 << "." << std::endl;
            // Print new representatives for verification.
            std::cout << "New set for node " << node_id1 << ": " << uf->findSet(node_id1) << std::endl;
            // Print new representative for node2.
            std::cout << "New set for node " << node_id2 << ": " << uf->findSet(node_id2) << std::endl;
        }
        // Command to dump the current graph to JSON (stdout).
        else if (command == "dump_graph_json" && args.size() == 1) {
            // Save graph to JSON string and print it.
            std::cout << GraphIO::saveGraphToJson(g) << std::endl;
        }
        // Handle unknown commands.
        else {
            // Print usage instructions.
            std::cerr << "Usage:\n"
                      << "  dynamic_route_optimizer load_graph <filepath.json>\n"
                      << "  dynamic_route_optimizer add_node <id> [x] [y]\n"
                      << "  dynamic_route_optimizer add_edge <from_id> <to_id> <weight>\n"
                      << "  dynamic_route_optimizer shortest_path <dijkstra|astar> <start_id> <end_id>\n"
                      << "  dynamic_route_optimizer update_edge_weight <from_id> <to_id> <new_weight>\n"
                      << "  dynamic_route_optimizer get_all_pairs_shortest_paths\n"
                      << "  dynamic_route_optimizer find_set <node_id>\n"
                      << "  dynamic_route_optimizer unite_sets <node_id1> <node_id2>\n"
                      << "  dynamic_route_optimizer dump_graph_json\n"
                      << "If no arguments, runs in interactive mode." << std::endl;
            // Return error code.
            return 1;
        }
        // Clean up UnionFind if allocated.
        delete uf;
        // Return success code.
        return 0;
    }


    // Interactive mode if no arguments are passed
    std::cout << "Dynamic Route Optimizer CLI (Interactive Mode)" << std::endl;
    // String to hold user input line.
    std::string line;
    // Loop to read commands interactively.
    while (true) {
        // Print command prompt.
        std::cout << "> ";
        // Read a line of input from the user.
        if (!std::getline(std::cin, line)) break; // Exit on EOF or error
        // If input is empty, continue to next iteration.
        if (line.empty()) continue;
        // If command is "exit", break the loop.
        if (line == "exit") break;

        // Prepare arguments for main function call (simulating command line args)
        std::vector<std::string> args_str = split(line, ' ');
        // Vector to hold char* arguments.
        std::vector<char*> c_args;
        // Program name placeholder.
        std::string prog_name = "dynamic_route_optimizer";
        // Add program name to char* arguments.
        c_args.push_back(&prog_name[0]);
        // Convert string arguments to char* for main.
        for(std::string& s : args_str) {
            // Add argument to char* vector.
            c_args.push_back(&s[0]);
        }
        // Call main recursively with the parsed command (this is a simplified way to reuse parsing logic).
        // A better approach would be a dedicated command parsing function.
        main(c_args.size(), c_args.data()); // This recursive call is a bit of a hack for simplicity
                                            // A proper loop with a command handler function is better.
                                            // For now, it demonstrates how single commands are processed.
                                            // In a real interactive CLI, you'd parse and execute directly here.
                                            // The current structure implies each interactive command re-evaluates `if (argc > 1)` logic.
                                            // For a true interactive loop, the parsing and action logic inside `if (argc > 1)`
                                            // should be refactored into a separate function called here.
                                            // Example for direct handling:
                                            // std::vector<std::string> args = split(line, ' ');
                                            // std::string command = args[0];
                                            // if (command == "load_graph" && args.size() > 1) { ... } else if ... etc.
    }

    // Clean up UnionFind object before exiting.
    delete uf;
    // Indicate successful termination.
    return 0;
}