// D3 selections for SVG container and graph elements.
let svg, simulation, linkElements, nodeElements, nodeLabels, edgeWeightLabels;
// Dimensions for the SVG canvas.
const width = document.getElementById('graphContainer').clientWidth || 800;
// Height of the SVG canvas.
const height = document.getElementById('graphContainer').clientHeight || 500;

// Store graph data (nodes and links).
let graphNodes = [];
// Array to store graph links (edges).
let graphLinks = [];

// Variables to store selected nodes for edge creation.
let selectedNode1 = null;
// Second selected node for creating an edge.
let selectedNode2 = null;

// Initializes the D3 simulation and SVG elements.
function initializeGraph() {
    // Select the SVG element by ID and set its dimensions.
    svg = d3.select("#graphSvg")
        .attr("width", width)
        .attr("height", height)
        .on("click", handleSvgClick); // Handle clicks on SVG background

    // Clear previous graph elements if any.
    svg.selectAll("*").remove();

    // Create a group for links, to draw them under nodes.
    const linkGroup = svg.append("g").attr("class", "links");
    // Create a group for edge weights.
    const edgeWeightGroup = svg.append("g").attr("class", "edge-weights");
    // Create a group for nodes.
    const nodeGroup = svg.append("g").attr("class", "nodes");
    // Create a group for node labels.
    const nodeLabelGroup = svg.append("g").attr("class", "node-labels");


    // Initialize D3 force simulation.
    simulation = d3.forceSimulation(graphNodes)
        // Force to keep nodes within the viewing area.
        .force("center", d3.forceCenter(width / 2, height / 2))
        // Force defining attraction/repulsion between nodes.
        .force("charge", d3.forceManyBody().strength(-100)) // Negative strength for repulsion
        // Force defining link strength and distance.
        .force("link", d3.forceLink(graphLinks).id(d => d.id).distance(70)) // Distance between linked nodes
        // Collision force to prevent nodes from overlapping.
        .force("collide", d3.forceCollide().radius(20)) // Radius for collision detection
        // Callback function for each simulation tick.
        .on("tick", ticked);

    // Select or create link elements (lines).
    linkElements = linkGroup.selectAll(".link");
    // Select or create node elements (circles).
    nodeElements = nodeGroup.selectAll(".node");
    // Select or create node label elements (text).
    nodeLabels = nodeLabelGroup.selectAll(".node-label");
    // Select or create edge weight label elements.
    edgeWeightLabels = edgeWeightGroup.selectAll(".edge-weight");

    // Load initial graph data.
    loadAndDrawGraph();
}

// Handles clicks on the SVG background to deselect nodes.
function handleSvgClick(event) {
    // Check if the click target is the SVG itself (not a node).
    if (event.target.tagName === 'svg') {
        // If a first node was selected for creating an edge.
        if (selectedNode1) {
            // Deselect the first node.
            d3.select(`#node-${selectedNode1.id}`).classed("selected", false);
            // Reset selectedNode1.
            selectedNode1 = null;
            // Clear the 'from' input field.
            document.getElementById('edgeFrom').value = '';
        }
        // If a second node was selected.
        if (selectedNode2) {
            // Deselect the second node.
            d3.select(`#node-${selectedNode2.id}`).classed("selected", false);
            // Reset selectedNode2.
            selectedNode2 = null;
            // Clear the 'to' input field.
            document.getElementById('edgeTo').value = '';
        }
        // Update message area.
        updateMessage("Selection cleared.");
    }
}


// Fetches graph data from the API and updates the D3 visualization.
async function loadAndDrawGraph() {
    // Fetch graph data.
    const data = await fetchGraphData();
    // If data is successfully fetched.
    if (data && data.nodes && data.edges) {
        // Clear existing shortest path highlights.
        clearShortestPath();

        // Map API nodes to D3 compatible nodes. Ensure x, y are present.
        graphNodes = data.nodes.map(n => ({
            // Node ID.
            id: n.id,
            // X coordinate, default if not provided.
            x: n.x || Math.random() * width, // Provide default if not set
            // Y coordinate, default if not provided.
            y: n.y || Math.random() * height, // Provide default if not set
            // Fixed x position (null initially).
            fx: null, // For D3 fixed position after drag
            // Fixed y position (null initially).
            fy: null
        }));

        // Map API edges to D3 compatible links (source and target are node objects or IDs).
        graphLinks = data.edges.map(e => ({
            // Source node ID.
            source: e.from_node, // D3 expects source/target to be IDs that match node IDs
            // Target node ID.
            target: e.to_node,
            // Edge weight.
            weight: e.weight
        }));

        // Update the D3 simulation with new data.
        updateSimulation();
        // Update message area.
        updateMessage("Graph loaded successfully.");
    } else {
        // Update message area if graph loading failed.
        updateMessage("Failed to load graph data or graph is empty.", true);
        // Reset graph arrays if loading fails.
        graphNodes = [];
        // Reset graph links.
        graphLinks = [];
        // Update simulation with empty data to clear visuals.
        updateSimulation();
    }
}


// Updates D3 elements based on current graphNodes and graphLinks.
function updateSimulation() {
    // Update node data join.
    nodeElements = nodeElements.data(graphNodes, d => d.id);
    // Remove old nodes.
    nodeElements.exit().remove();
    // Add new nodes.
    nodeElements = nodeElements.enter().append("circle")
        .attr("class", "node")
        .attr("r", 10) // Node radius
        .attr("id", d => `node-${d.id}`)
        .style("fill", d => getColor(d.id)) // Color nodes based on ID (example)
        .on("click", handleNodeClick) // Handle node click for selection
        .call(d3.drag() // Enable dragging for nodes
            .on("start", dragstarted)
            .on("drag", dragged)
            .on("end", dragended))
        .merge(nodeElements);

    // Update node label data join.
    nodeLabels = nodeLabels.data(graphNodes, d => d.id);
    // Remove old labels.
    nodeLabels.exit().remove();
    // Add new labels.
    nodeLabels = nodeLabels.enter().append("text")
        .attr("class", "node-label")
        .text(d => d.id)
        .attr("dx", 0)
        .attr("dy", -15) // Position label above the node
        .style("text-anchor", "middle")
        .merge(nodeLabels);

    // Update link data join.
    linkElements = linkElements.data(graphLinks, d => `${d.source.id || d.source}-${d.target.id || d.target}`); // Use IDs if source/target are objects
    // Remove old links.
    linkElements.exit().remove();
    // Add new links.
    linkElements = linkElements.enter().append("line")
        .attr("class", "link")
        .attr("stroke-width", 2)
        .merge(linkElements);

    // Update edge weight label data join.
    edgeWeightLabels = edgeWeightLabels.data(graphLinks, d => `${d.source.id || d.source}-${d.target.id || d.target}`);
    // Remove old weight labels.
    edgeWeightLabels.exit().remove();
    // Add new weight labels.
    edgeWeightLabels = edgeWeightLabels.enter().append("text")
        .attr("class", "edge-weight")
        .text(d => d.weight.toFixed(1))
        .attr("dy", -5) // Position above the link
        .merge(edgeWeightLabels);

    // Restart the simulation with new nodes and links.
    simulation.nodes(graphNodes);
    // Update links in simulation.
    simulation.force("link").links(graphLinks);
    // Alpha target restarts simulation slightly, alpha > 0 means simulation is running.
    simulation.alpha(1).restart();
}


// Node click handler for selecting nodes for edge creation or other actions.
function handleNodeClick(event, d) {
    // Stop event propagation to prevent svg click handler from deselecting immediately.
    event.stopPropagation();

    // Remove selection from previously selected nodes.
    nodeElements.classed("selected", false);

    // If no node is selected yet, or if the same node is clicked again.
    if (!selectedNode1 || selectedNode1.id === d.id) {
        // Select this node as the first node.
        selectedNode1 = d;
        // Clear second selected node.
        selectedNode2 = null;
        // Add "selected" class for visual feedback.
        d3.select(this).classed("selected", true);
        // Set 'from' input field value.
        document.getElementById('edgeFrom').value = d.id;
        // Clear 'to' input field.
        document.getElementById('edgeTo').value = '';
        // Update message area.
        updateMessage(`Node ${d.id} selected as FROM node.`);
    // If a first node is already selected and a different node is clicked.
    } else if (selectedNode1 && selectedNode1.id !== d.id) {
        // Select this node as the second node.
        selectedNode2 = d;
        // Add "selected" class for visual feedback.
        d3.select(this).classed("selected", true);
        // Also re-highlight selectedNode1.
        d3.select(`#node-${selectedNode1.id}`).classed("selected", true);
        // Set 'to' input field value.
        document.getElementById('edgeTo').value = d.id;
        // Update message area.
        updateMessage(`Node ${d.id} selected as TO node. Ready to add edge.`);
    }
}


// D3 drag event handlers.
function dragstarted(event, d) {
    // If simulation is not active, reactivate it.
    if (!event.active) simulation.alphaTarget(0.3).restart();
    // Fix node's x position during drag start.
    d.fx = d.x;
    // Fix node's y position during drag start.
    d.fy = d.y;
}
// Handles dragging of nodes.
function dragged(event, d) {
    // Update fixed x position to current mouse x.
    d.fx = event.x;
    // Update fixed y position to current mouse y.
    d.fy = event.y;
}
// Handles end of drag operation for nodes.
function dragended(event, d) {
    // If simulation is not active, stop targeting alpha.
    if (!event.active) simulation.alphaTarget(0);
    // Unfix node's position (fx, fy) if you want it to be affected by forces again.
    // Comment out if you want nodes to stay where dragged.
    d.fx = null;
    // Unfix node's y position.
    d.fy = null;
}

// Called on each tick of the D3 simulation to update element positions.
function ticked() {
    // Update link positions.
    linkElements
        .attr("x1", d => d.source.x)
        .attr("y1", d => d.source.y)
        .attr("x2", d => d.target.x)
        .attr("y2", d => d.target.y);
    // Update node positions.
    nodeElements
        .attr("cx", d => d.x)
        .attr("cy", d => d.y);
    // Update node label positions.
    nodeLabels
        .attr("x", d => d.x)
        .attr("y", d => d.y);
    // Update edge weight label positions (midpoint of the edge).
    edgeWeightLabels
        .attr("x", d => (d.source.x + d.target.x) / 2)
        .attr("y", d => (d.source.y + d.target.y) / 2);
}

// Generates a color based on node ID (for visual distinction).
function getColor(nodeId) {
    // Simple color cycling based on ID.
    const colors = ["#66c2a5", "#fc8d62", "#8da0cb", "#e78ac3", "#a6d854", "#ffd92f"];
    // Return color based on modulo of ID.
    return colors[nodeId % colors.length];
}

// Highlights the shortest path on the graph.
function highlightShortestPath(pathNodeIds) {
    // Clear previous highlights.
    clearShortestPath();
    // If no path or path is too short, do nothing.
    if (!pathNodeIds || pathNodeIds.length < 2) return;

    // Iterate through the path to highlight edges.
    for (let i = 0; i < pathNodeIds.length - 1; i++) {
        // Current node ID in path.
        const sourceId = pathNodeIds[i];
        // Next node ID in path.
        const targetId = pathNodeIds[i + 1];
        // Select link elements and filter for the current path segment.
        linkElements
            .filter(d => (d.source.id === sourceId && d.target.id === targetId) || (d.source.id === targetId && d.target.id === sourceId) ) // Check both directions if graph undirected
            .classed("shortest-path", true);
    }
}

// Clears any existing shortest path highlights from links.
function clearShortestPath() {
    // Remove "shortest-path" class from all link elements.
    linkElements.classed("shortest-path", false);
}

// Event listeners for UI controls.
document.addEventListener('DOMContentLoaded', () => {
    // Initialize graph visualization on page load.
    initializeGraph();

    // Listener for "Load/Refresh Graph" button.
    document.getElementById('loadGraphBtn').addEventListener('click', loadAndDrawGraph);

    // Listener for "Find Path" (Dijkstra) button.
    document.getElementById('findPathBtn').addEventListener('click', async () => {
        // Get start node ID from input.
        const startNode = parseInt(document.getElementById('startNode').value);
        // Get end node ID from input.
        const endNode = parseInt(document.getElementById('endNode').value);
        // If start or end node is not a number, show error.
        if (isNaN(startNode) || isNaN(endNode)) {
            // Update message area with error.
            updateMessage("Please enter valid Start and End Node IDs for Dijkstra.", true);
            // Exit function.
            return;
        }
        // Call API to find shortest path.
        const result = await findShortestPath(startNode, endNode, "dijkstra");
        // If result and path exist.
        if (result && result.path) {
            // Highlight the found path.
            highlightShortestPath(result.path);
            // Update message area with path details.
            updateMessage(`Dijkstra Path: ${result.path.join(' -> ')}, Weight: ${result.weight.toFixed(2)}`);
        } else if (result && result.message) {
            // If no path found or error, display message.
            updateMessage(result.message, true);
            // Clear previous path highlights.
            clearShortestPath();
        }
    });
    
    // Listener for "Find Path A*" button.
    document.getElementById('findPathAstarBtn').addEventListener('click', async () => {
        // Get start node ID for A*.
        const startNode = parseInt(document.getElementById('startNodeAstar').value);
        // Get end node ID for A*.
        const endNode = parseInt(document.getElementById('endNodeAstar').value);
        // If start or end node is not a number.
        if (isNaN(startNode) || isNaN(endNode)) {
            // Display error message.
            updateMessage("Please enter valid Start and End Node IDs for A*.", true);
            // Exit function.
            return;
        }
        // Call API to find shortest path using A*.
        const result = await findShortestPath(startNode, endNode, "astar");
        // If result and path exist.
        if (result && result.path) {
            // Highlight the A* path.
            highlightShortestPath(result.path);
            // Display A* path details.
            updateMessage(`A* Path: ${result.path.join(' -> ')}, Weight: ${result.weight.toFixed(2)}`);
        } else if (result && result.message) {
            // If no path or error, display message.
            updateMessage(result.message, true);
            // Clear previous path highlights.
            clearShortestPath();
        }
    });

    // Listener for "Add Node" button.
    document.getElementById('addNodeBtn').addEventListener('click', async () => {
        // Get node ID to add.
        const nodeId = parseInt(document.getElementById('nodeIdAdd').value);
        // Get x coordinate for new node.
        const nodeX = parseFloat(document.getElementById('nodeXAdd').value);
        // Get y coordinate for new node.
        const nodeY = parseFloat(document.getElementById('nodeYAdd').value);
        // If node ID is not a number.
        if (isNaN(nodeId)) {
            // Display error for invalid node ID.
            updateMessage("Please enter a valid Node ID to add.", true);
            // Exit function.
            return;
        }
        // Call API to add node.
        const result = await addNode(nodeId, isNaN(nodeX) ? null : nodeX, isNaN(nodeY) ? null : nodeY);
        // If node added successfully.
        if (result && !result.detail) { // Assuming error responses have a 'detail' field
            // Reload and draw the graph to reflect changes.
            loadAndDrawGraph();
        }
    });

    // Listener for "Add Edge" button.
    document.getElementById('addEdgeBtn').addEventListener('click', async () => {
        // Get 'from' node ID for edge.
        const fromNode = parseInt(document.getElementById('edgeFrom').value);
        // Get 'to' node ID for edge.
        const toNode = parseInt(document.getElementById('edgeTo').value);
        // Get weight for the new edge.
        const weight = parseFloat(document.getElementById('edgeWeight').value);
        // If 'from', 'to', or weight is invalid.
        if (isNaN(fromNode) || isNaN(toNode) || isNaN(weight)) {
            // Display error for invalid input.
            updateMessage("Please select valid FROM and TO nodes and enter a valid Weight.", true);
            // Exit function.
            return;
        }
        // Call API to add edge.
        const result = await addEdge(fromNode, toNode, weight);
        // If edge added successfully.
        if (result && !result.detail) {
            // Reload and draw graph.
            loadAndDrawGraph();
            // Clear selection and input fields.
            selectedNode1 = null; selectedNode2 = null;
            // Reset 'from' input.
            document.getElementById('edgeFrom').value = '';
            // Reset 'to' input.
            document.getElementById('edgeTo').value = '';
            // Reset node visual selection.
            nodeElements.classed("selected", false);
        }
    });

    // Listener for "Update Edge Weight" button.
    document.getElementById('updateEdgeWeightBtn').addEventListener('click', async () => {
        // Get 'from' node ID for edge update.
        const fromNode = parseInt(document.getElementById('updateEdgeFrom').value);
        // Get 'to' node ID for edge update.
        const toNode = parseInt(document.getElementById('updateEdgeTo').value);
        // Get new weight for the edge.
        const newWeight = parseFloat(document.getElementById('updateEdgeWeightVal').value);
        // If any input is invalid.
        if (isNaN(fromNode) || isNaN(toNode) || isNaN(newWeight)) {
            // Display error for invalid input.
            updateMessage("Please enter valid FROM node, TO node, and New Weight.", true);
            // Exit function.
            return;
        }
        // Call API to update edge weight.
        const result = await updateEdgeWeight(fromNode, toNode, newWeight);
        // If update successful.
        if (result && !result.detail) {
            // Reload and draw graph.
            loadAndDrawGraph();
        }
    });
    
    // Listener for "Find Set" (Union-Find) button.
    document.getElementById('findSetBtn').addEventListener('click', async () => {
        // Get node ID for find_set operation.
        const nodeId = parseInt(document.getElementById('findSetNodeId').value);
        // If node ID is invalid.
        if (isNaN(nodeId)) {
            // Display error.
            updateMessage("Please enter a valid Node ID for Find Set.", true);
            // Reset result area.
            document.getElementById('findSetResult').textContent = '';
            // Exit function.
            return;
        }
        // Call API for find_set.
        await findSet(nodeId); // Message is updated within findSet function
    });

    // Listener for "Unite Sets" (Union-Find) button.
    document.getElementById('uniteSetsBtn').addEventListener('click', async () => {
        // Get first node ID for unite_sets.
        const nodeId1 = parseInt(document.getElementById('uniteNodeId1').value);
        // Get second node ID for unite_sets.
        const nodeId2 = parseInt(document.getElementById('uniteNodeId2').value);
        // If any node ID is invalid.
        if (isNaN(nodeId1) || isNaN(nodeId2)) {
            // Display error.
            updateMessage("Please enter valid Node ID 1 and Node ID 2 for Unite Sets.", true);
            // Exit function.
            return;
        }
        // Call API for unite_sets.
        const result = await uniteSets(nodeId1, nodeId2);
        // If unite operation successful.
        if (result && !result.detail) {
            // Optionally, you might want to visually represent zones/sets if your graph nodes have set info.
            // For now, just a message update.
            loadAndDrawGraph(); // Reload graph if zones affect visual representation or for consistency
        }
    });
});