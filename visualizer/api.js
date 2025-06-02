// Base URL for the backend API.
const API_BASE_URL = 'http://127.0.0.1:8000/api/v1';

// Asynchronously fetches graph data from the backend.
async function fetchGraphData() {
    // Try to fetch data from the /graph endpoint.
    try {
        // Await the response from the API.
        const response = await fetch(`${API_BASE_URL}/graph`);
        // If the response is not ok (e.g., 404, 500), throw an error.
        if (!response.ok) {
            // Throw error with status text.
            throw new Error(`HTTP error! status: ${response.status} ${await response.text()}`);
        }
        // Parse the JSON response.
        const data = await response.json();
        // Return the parsed data.
        return data;
    // Catch any errors during fetch or parsing.
    } catch (error) {
        // Log the error to the console.
        console.error("Error fetching graph data:", error);
        // Display error message to the user.
        updateMessage(`Error fetching graph data: ${error.message}`);
        // Return null indicating failure.
        return null;
    }
}

// Asynchronously finds the shortest path between two nodes.
async function findShortestPath(startNode, endNode, algorithm = "dijkstra") {
    // Try to post a request to the /shortest_path endpoint.
    try {
        // Await the response from the API.
        const response = await fetch(`${API_BASE_URL}/shortest_path`, {
            // Specify the HTTP method.
            method: 'POST',
            // Specify the content type.
            headers: {
                'Content-Type': 'application/json',
            },
            // Convert the request payload to a JSON string.
            body: JSON.stringify({ start_node: startNode, end_node: endNode, algorithm: algorithm }),
        });
        // Parse the JSON response.
        const data = await response.json();
        // If the response is not ok, handle the error message from the API.
        if (!response.ok) {
            // Throw an error with the detail message from API or a default message.
            throw new Error(data.detail || `HTTP error! status: ${response.status}`);
        }
        // Return the parsed data.
        return data;
    // Catch any errors during fetch or parsing.
    } catch (error) {
        // Log the error to the console.
        console.error("Error finding shortest path:", error);
        // Display error message to the user.
        updateMessage(`Error finding shortest path: ${error.message}`);
        // Return null indicating failure.
        return null;
    }
}

// Asynchronously adds a new node to the graph.
async function addNode(nodeId, x, y) {
    // Try to post a request to the /add_node endpoint.
    try {
        // Await the response from the API.
        const response = await fetch(`${API_BASE_URL}/add_node`, {
            // Specify the HTTP method.
            method: 'POST',
            // Specify the content type.
            headers: { 'Content-Type': 'application/json' },
            // Convert the request payload to a JSON string.
            body: JSON.stringify({ id: nodeId, x: x, y: y }),
        });
        // Parse the JSON response.
        const data = await response.json();
        // If the response is not ok, throw an error with the detail message.
        if (!response.ok) {
            // Throw error with detail from API response.
            throw new Error(data.detail || `HTTP error! status: ${response.status}`);
        }
        // Update message area with success.
        updateMessage(data.message || "Node added successfully.");
        // Return the response data.
        return data;
    // Catch any errors.
    } catch (error) {
        // Log the error.
        console.error("Error adding node:", error);
        // Display error message.
        updateMessage(`Error adding node: ${error.message}`);
        // Return null.
        return null;
    }
}
// Asynchronously adds a new edge to the graph.
async function addEdge(fromNode, toNode, weight) {
    // Try to post a request to the /add_edge endpoint.
    try {
        // Await the response from the API.
        const response = await fetch(`${API_BASE_URL}/add_edge`, {
            // Specify the HTTP method.
            method: 'POST',
            // Specify the content type.
            headers: { 'Content-Type': 'application/json' },
            // Convert the request payload to a JSON string.
            body: JSON.stringify({ from_node: fromNode, to_node: toNode, weight: weight }),
        });
        // Parse the JSON response.
        const data = await response.json();
        // If the response is not ok, throw an error with the detail message.
        if (!response.ok) {
            // Throw error with detail from API response.
            throw new Error(data.detail || `HTTP error! status: ${response.status}`);
        }
        // Update message area with success.
        updateMessage(data.message || "Edge added successfully.");
        // Return the response data.
        return data;
    // Catch any errors.
    } catch (error) {
        // Log the error.
        console.error("Error adding edge:", error);
        // Display error message.
        updateMessage(`Error adding edge: ${error.message}`);
        // Return null.
        return null;
    }
}

// Asynchronously updates the weight of an existing edge.
async function updateEdgeWeight(fromNode, toNode, newWeight) {
    // Try to put a request to the /update_weight endpoint.
    try {
        // Await the response from the API.
        const response = await fetch(`${API_BASE_URL}/update_weight`, {
            // Specify the HTTP method.
            method: 'PUT',
            // Specify the content type.
            headers: { 'Content-Type': 'application/json' },
            // Convert the request payload to a JSON string.
            body: JSON.stringify({ from_node: fromNode, to_node: toNode, new_weight: newWeight }),
        });
        // Parse the JSON response.
        const data = await response.json();
        // If the response is not ok, throw an error with the detail message.
        if (!response.ok) {
            // Throw error with detail from API response.
            throw new Error(data.detail || `HTTP error! status: ${response.status}`);
        }
        // Update message area with success.
        updateMessage(data.message || "Edge weight updated successfully.");
        // Return the response data.
        return data;
    // Catch any errors.
    } catch (error) {
        // Log the error.
        console.error("Error updating edge weight:", error);
        // Display error message.
        updateMessage(`Error updating edge weight: ${error.message}`);
        // Return null.
        return null;
    }
}

// Asynchronously finds the set representative for a node (Union-Find).
async function findSet(nodeId) {
    // Try to post a request to the /zones/find_set endpoint.
    try {
        // Await the response from the API.
        const response = await fetch(`${API_BASE_URL}/zones/find_set`, {
            // Specify the HTTP method.
            method: 'POST',
            // Specify the content type.
            headers: { 'Content-Type': 'application/json' },
            // Convert the request payload to a JSON string.
            body: JSON.stringify({ node_id: nodeId }),
        });
        // Parse the JSON response.
        const data = await response.json();
        // If the response is not ok, throw an error with the detail message.
        if (!response.ok) {
            // Throw error with detail from API response.
            throw new Error(data.detail || `HTTP error! status: ${response.status}`);
        }
        // Display the result in a dedicated area or message area.
        const resultArea = document.getElementById('findSetResult');
        // If result area exists.
        if (resultArea) {
            // Update result area text.
            resultArea.textContent = `Set for node ${data.node_id}: ${data.set_representative}`;
        }
        // Update general message area as well.
        updateMessage(data.message || `Set for node ${data.node_id} is ${data.set_representative}.`);
        // Return the response data.
        return data;
    // Catch any errors.
    } catch (error) {
        // Log the error.
        console.error("Error finding set:", error);
        // Display error message.
        updateMessage(`Error finding set: ${error.message}`);
        // Clear result area on error.
        const resultArea = document.getElementById('findSetResult');
        // If result area exists.
        if (resultArea) resultArea.textContent = '';
        // Return null.
        return null;
    }
}

// Asynchronously unites the sets of two nodes (Union-Find).
async function uniteSets(nodeId1, nodeId2) {
    // Try to post a request to the /zones/unite_sets endpoint.
    try {
        // Await the response from the API.
        const response = await fetch(`${API_BASE_URL}/zones/unite_sets`, {
            // Specify the HTTP method.
            method: 'POST',
            // Specify the content type.
            headers: { 'Content-Type': 'application/json' },
            // Convert the request payload to a JSON string.
            body: JSON.stringify({ node_id1: nodeId1, node_id2: nodeId2 }),
        });
        // Parse the JSON response.
        const data = await response.json();
        // If the response is not ok, throw an error with the detail message.
        if (!response.ok) {
            // Throw error with detail from API response.
            throw new Error(data.detail || `HTTP error! status: ${response.status}`);
        }
        // Update message area with success.
        updateMessage(data.message || "Sets united successfully.");
        // Return the response data.
        return data;
    // Catch any errors.
    } catch (error) {
        // Log the error.
        console.error("Error uniting sets:", error);
        // Display error message.
        updateMessage(`Error uniting sets: ${error.message}`);
        // Return null.
        return null;
    }
}


// Helper function to update the message area on the HTML page.
function updateMessage(message, isError = false) {
    // Get the message area element by its ID.
    const messageArea = document.getElementById('messageArea');
    // If the message area element exists.
    if (messageArea) {
        // Set the text content of the message area.
        messageArea.textContent = message;
        // Set text color to red if it's an error, otherwise to green or default.
        messageArea.style.color = isError ? 'red' : 'green';
    }
}