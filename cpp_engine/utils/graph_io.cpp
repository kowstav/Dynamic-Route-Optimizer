#include "../include/graph_io.h"
#include "../include/graph.h" // Ensure Graph is fully defined
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept> // For runtime_error

// For this example, we'll use a very simple JSON structure.
// A proper JSON library (like nlohmann/json or RapidJSON) is highly recommended for robust parsing.
// This is a manual, fragile parser for demonstration only.

// Helper to trim whitespace
std::string trim(const std::string& str) {
    // Find the first non-whitespace character.
    const auto strBegin = str.find_first_not_of(" \t\n\r");
    // If no non-whitespace character is found, return an empty string.
    if (strBegin == std::string::npos) return "";
    // Find the last non-whitespace character.
    const auto strEnd = str.find_last_not_of(" \t\n\r");
    // Calculate the range of non-whitespace characters.
    const auto strRange = strEnd - strBegin + 1;
    // Return the substring containing only non-whitespace characters.
    return str.substr(strBegin, strRange);
}


// Loads a graph from a JSON file (simplified custom parser).
bool GraphIO::loadGraphFromJson(const std::string& filepath, Graph& graph) {
    // Open the file for reading.
    std::ifstream file(filepath);
    // If the file cannot be opened, return false.
    if (!file.is_open()) {
        // Indicate failure.
        return false;
    }

    // String to hold each line from the file.
    std::string line;
    // Current parsing state (nodes or edges).
    std::string current_section; // "nodes" or "edges"

    // Read the file line by line.
    while (std::getline(file, line)) {
        // Trim whitespace from the current line.
        line = trim(line);
        // Skip empty lines or lines that are just brackets/braces.
        if (line.empty() || line == "{" || line == "}" || line == "[" || line == "]") continue;

        // Check for section headers (e.g., "nodes": [).
        if (line.find("\"nodes\"") != std::string::npos) {
            // Set current section to nodes.
            current_section = "nodes";
            // Continue to the next line.
            continue;
        } else if (line.find("\"edges\"") != std::string::npos) {
            // Set current section to edges.
            current_section = "edges";
            // Continue to the next line.
            continue;
        }
        
        // Process based on current section.
        if (current_section == "nodes") {
            // Example node format: { "id": 1, "x": 10.0, "y": 20.0 }
            // This is a very fragile parser.
            // Find "id":
            size_t id_pos = line.find("\"id\":");
            // Find "x":
            size_t x_pos = line.find("\"x\":");
            // Find "y":
            size_t y_pos = line.find("\"y\":");
            // If "id" tag is found.
            if (id_pos != std::string::npos) {
                // Initialize node properties.
                int id = 0;
                // Initialize x coordinate.
                double x = 0.0;
                // Initialize y coordinate.
                double y = 0.0;
                // Create a string stream from the line.
                std::string temp;
                // Substring after "id": .
                temp = line.substr(id_pos + 5);
                // Find comma after id value.
                size_t comma_pos = temp.find(',');
                // Extract id value.
                id = std::stoi(trim(temp.substr(0, comma_pos)));

                // If "x" tag is found.
                if (x_pos != std::string::npos) {
                    // Substring after "x": .
                    temp = line.substr(x_pos + 4);
                    // Find comma after x value.
                    comma_pos = temp.find(',');
                    // Extract x value.
                    x = std::stod(trim(temp.substr(0, comma_pos)));
                }
                // If "y" tag is found.
                if (y_pos != std::string::npos) {
                    // Substring after "y": .
                    temp = line.substr(y_pos + 4);
                    // Find closing brace after y value.
                    size_t brace_pos = temp.find('}');
                    // Extract y value.
                    y = std::stod(trim(temp.substr(0, brace_pos)));
                }
                // Add the parsed node to the graph.
                graph.addNode(id, x, y);
            }
        } else if (current_section == "edges") {
            // Example edge format: { "from": 1, "to": 2, "weight": 5.0 }
            // Find "from":
            size_t from_pos = line.find("\"from\":");
            // Find "to":
            size_t to_pos = line.find("\"to\":");
            // Find "weight":
            size_t weight_pos = line.find("\"weight\":");

            // If all tags are found.
            if (from_pos != std::string::npos && to_pos != std::string::npos && weight_pos != std::string::npos) {
                // Initialize 'from' node ID.
                int from_node, to_node;
                // Initialize edge weight.
                double weight;
                // Temporary string for parsing.
                std::string temp;
                // Substring after "from": .
                temp = line.substr(from_pos + 7);
                // Find comma after from_node value.
                size_t comma_pos = temp.find(',');
                // Extract from_node value.
                from_node = std::stoi(trim(temp.substr(0, comma_pos)));

                // Substring after "to": .
                temp = line.substr(to_pos + 5); // Check "to": length carefully
                // Find comma after to_node value.
                comma_pos = temp.find(',');
                // Extract to_node value.
                to_node = std::stoi(trim(temp.substr(0, comma_pos)));
                
                // Substring after "weight": .
                temp = line.substr(weight_pos + 9);
                // Find closing brace after weight value.
                size_t brace_pos = temp.find('}');
                // Extract weight value.
                weight = std::stod(trim(temp.substr(0, brace_pos)));
                
                // Add the parsed edge to the graph.
                graph.addEdge(from_node, to_node, weight);
            }
        }
    }
    // Close the file.
    file.close();
    // Indicate successful loading.
    return true;
}

// Saves a graph to a JSON string (could be adapted to save to file).
std::string GraphIO::saveGraphToJson(const Graph& graph) {
    // Initialize a string stream to build the JSON string.
    std::stringstream ss;
    // Start JSON object.
    ss << "{\n";
    // Start "nodes" array.
    ss << "  \"nodes\": [\n";
    // Get all node IDs from the graph.
    auto nodeIds = graph.getAllNodeIds();
    // Iterate through node IDs to serialize nodes.
    for (size_t i = 0; i < nodeIds.size(); ++i) {
        // Get the current node ID.
        int id = nodeIds[i];
        // Get the Node object.
        const Node* node_ptr = graph.getNode(id);
        // Start node object.
        ss << "    { \"id\": " << id;
        // If node details (coordinates) are available.
        if (node_ptr) {
            // Add x coordinate to JSON.
            ss << ", \"x\": " << node_ptr->x;
            // Add y coordinate to JSON.
            ss << ", \"y\": " << node_ptr->y;
        }
        // End node object.
        ss << " }";
        // If not the last node, add a comma.
        if (i < nodeIds.size() - 1) {
            // Add comma separator.
            ss << ",";
        }
        // Add newline.
        ss << "\n";
    }
    // End "nodes" array.
    ss << "  ],\n";
    // Start "edges" array.
    ss << "  \"edges\": [\n";
    // Flag to track if it's the first edge (for comma handling).
    bool firstEdge = true;
    // Iterate through all nodes to get their edges.
    for (int from_id : nodeIds) {
        // Check if the node exists and has outgoing edges.
        if (graph.adj.count(from_id)) {
            // For each edge starting from from_id.
            for (const auto& edge : graph.getEdges(from_id)) {
                // If not the first edge, add a comma.
                if (!firstEdge) {
                    // Add comma separator.
                    ss << ",\n";
                }
                // Start edge object.
                ss << "    { \"from\": " << from_id << ", \"to\": " << edge.to << ", \"weight\": " << edge.weight << " }";
                // Set firstEdge to false after processing the first edge.
                firstEdge = false;
            }
        }
    }
    // Add newline if any edges were written.
    if(!firstEdge) ss << "\n";
    // End "edges" array.
    ss << "  ]\n";
    // End JSON object.
    ss << "}\n";
    // Return the JSON string.
    return ss.str();
}