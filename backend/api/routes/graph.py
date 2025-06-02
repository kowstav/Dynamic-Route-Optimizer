# Graph API routes
from fastapi import APIRouter, HTTPException, Depends
from typing import List, Dict, Any

from backend.models import schema # Use fully qualified import
from backend.services import optimizer as optimizer_service # Use fully qualified import


# Create an API router instance for graph operations.
router = APIRouter()

# Dependency to ensure C++ engine is initialized before handling requests.
async def check_engine_initialized():
    # If the optimizer service's engine is not initialized.
    if not optimizer_service.ENGINE_INITIALIZED:
        # Attempt to initialize it.
        if not optimizer_service.ensure_engine_initialized():
            # If initialization fails, raise an HTTPException.
            raise HTTPException(status_code=503, detail="C++ Optimizer Engine not initialized or failed to load graph.")

# API endpoint to get the current graph data.
@router.get("/graph", response_model=schema.GraphResponse, dependencies=[Depends(check_engine_initialized)])
async def get_graph_data():
    """
    Retrieves the current graph structure (nodes and edges) from the C++ engine.
    """
    # Call service to get graph data.
    graph_data_dict = optimizer_service.get_graph_data_service()
    # If graph data is not retrieved successfully.
    if graph_data_dict is None:
        # Raise 404 Not Found error.
        raise HTTPException(status_code=404, detail="Graph data not found or error retrieving it.")
    # Try to parse the graph data into Pydantic models.
    try:
        # Parse nodes from the dictionary.
        nodes = [schema.Node(**n) for n in graph_data_dict.get("nodes", [])]
        # Parse edges from the dictionary.
        edges_raw = graph_data_dict.get("edges", [])
        # Edges list to store parsed Edge objects.
        edges = []
        # Iterate through raw edge data.
        for e_raw in edges_raw:
            # Create Edge object, mapping "from" to "from_node".
            edges.append(schema.Edge(from_node=e_raw["from"], to_node=e_raw["to"], weight=e_raw["weight"]))
        # Return graph data in response model.
        return schema.GraphResponse(nodes=nodes, edges=edges, message="Graph data retrieved successfully.")
    # Handle exceptions during parsing.
    except Exception as e:
        # Raise 500 Internal Server Error if parsing fails.
        raise HTTPException(status_code=500, detail=f"Error parsing graph data from engine: {str(e)}")


# API endpoint to add a node to the graph.
@router.post("/add_node", response_model=schema.MessageResponse, dependencies=[Depends(check_engine_initialized)])
async def add_node(request: schema.AddNodeRequest):
    """
    Adds a new node to the graph.
    """
    # Call service to add a node.
    result = optimizer_service.add_node_service(request.id, request.x, request.y)
    # If the result contains details (implies an error or specific feedback).
    if result.get("details"):
        # Raise 400 Bad Request with details.
        raise HTTPException(status_code=400, detail=result["details"])
    # Return success message.
    return schema.MessageResponse(message=result["message"])

# API endpoint to add an edge to the graph.
@router.post("/add_edge", response_model=schema.MessageResponse, dependencies=[Depends(check_engine_initialized)])
async def add_edge(request: schema.AddEdgeRequest):
    """
    Adds a new directed edge to the graph.
    """
    # Call service to add an edge.
    result = optimizer_service.add_edge_service(request.from_node, request.to_node, request.weight)
    # If the result contains details.
    if result.get("details"):
        # Raise 400 Bad Request with details.
        raise HTTPException(status_code=400, detail=result["details"])
    # Return success message.
    return schema.MessageResponse(message=result["message"])

# API endpoint to find the shortest path between two nodes.
@router.post("/shortest_path", response_model=schema.ShortestPathResponse, dependencies=[Depends(check_engine_initialized)])
async def shortest_path(request: schema.ShortestPathRequest):
    """
    Calculates the shortest path between two nodes using Dijkstra or A*.
    """
    # Check if the requested algorithm is valid.
    if request.algorithm not in ["dijkstra", "astar"]:
        # Raise 400 Bad Request for invalid algorithm.
        raise HTTPException(status_code=400, detail="Invalid algorithm. Choose 'dijkstra' or 'astar'.")
    # Call service to find the shortest path.
    result = optimizer_service.shortest_path_service(request.start_node, request.end_node, request.algorithm)
    # If the path list is empty and there's a message (likely error or no path).
    if not result["path"] and "message" in result and "found" not in result["message"].lower() : # Check if it's not a "No path found" message
        # If the message suggests an error rather than just "no path".
        if "error" in result["message"].lower() or "could not parse" in result["message"].lower() or "failed" in result["message"].lower():
            # Raise 500 Internal Server Error.
             raise HTTPException(status_code=500, detail=result["message"])
    # Return the shortest path response.
    return schema.ShortestPathResponse(path=result["path"], weight=result["weight"], message=result.get("message", "Path calculation processed."))


# API endpoint to update the weight of an edge.
@router.put("/update_weight", response_model=schema.MessageResponse, dependencies=[Depends(check_engine_initialized)])
async def update_weight(request: schema.UpdateWeightRequest):
    """
    Updates the weight of an existing edge, e.g., due to traffic changes.
    """
    # Call service to update edge weight.
    result = optimizer_service.update_weight_service(request.from_node, request.to_node, request.new_weight)
    # If the result contains details.
    if result.get("details"):
        # Raise 400 Bad Request if update fails (e.g., edge not found).
        raise HTTPException(status_code=400, detail=result["details"])
    # Return success message.
    return schema.MessageResponse(message=result["message"])

# API endpoint for Union-Find: find the representative of a node's set.
@router.post("/zones/find_set", response_model=schema.FindSetResponse, dependencies=[Depends(check_engine_initialized)])
async def find_set(request: schema.FindSetRequest):
    """
    Finds the representative of the set (zone) containing the given node.
    """
    # Call service for find_set operation.
    result = optimizer_service.find_set_service(request.node_id)
    # If the message indicates an error.
    if "error" in result.get("message", "").lower() or result.get("set_representative", -1) == -1 :
        # Raise 400 Bad Request or 404 Not Found depending on message.
        status_code = 404 if "not found" in result.get("message", "").lower() else 400
        # Raise HTTPException with appropriate status code and detail.
        raise HTTPException(status_code=status_code, detail=result.get("message", "Failed to find set."))
    # Return the find_set response.
    return schema.FindSetResponse(node_id=result["node_id"], set_representative=result["set_representative"], message=result.get("message"))

# API endpoint for Union-Find: unite the sets of two nodes.
@router.post("/zones/unite_sets", response_model=schema.MessageResponse, dependencies=[Depends(check_engine_initialized)])
async def unite_sets(request: schema.UniteSetsRequest):
    """
    Unites the sets (zones) containing the two given nodes.
    """
    # Call service to unite sets.
    result = optimizer_service.unite_sets_service(request.node_id1, request.node_id2)
    # If the result contains details (implies an error).
    if result.get("details"):
        # Raise 400 Bad Request with details.
        raise HTTPException(status_code=400, detail=result["details"])
    # Return success message.
    return schema.MessageResponse(message=result["message"])