from pydantic import BaseModel
from typing import List, Optional, Union, Tuple

# Represents a node for API requests and responses.
class Node(BaseModel):
    # Unique identifier for the node.
    id: int
    # Optional x-coordinate of the node.
    x: Optional[float] = None
    # Optional y-coordinate of the node.
    y: Optional[float] = None

# Represents an edge for API requests and responses.
class Edge(BaseModel):
    # Source node ID of the edge.
    from_node: int # Pydantic uses 'from_node' as 'from' is a keyword
    # Destination node ID of the edge.
    to_node: int
    # Weight of the edge.
    weight: float

# Represents the full graph structure for API communication.
class GraphData(BaseModel):
    # List of nodes in the graph.
    nodes: List[Node]
    # List of edges in the graph.
    edges: List[Edge]

# Request model for adding a new node.
class AddNodeRequest(BaseModel):
    # ID of the node to be added.
    id: int
    # Optional x-coordinate.
    x: Optional[float] = None
    # Optional y-coordinate.
    y: Optional[float] = None

# Request model for adding a new edge.
class AddEdgeRequest(BaseModel):
    # Source node ID.
    from_node: int
    # Destination node ID.
    to_node: int
    # Weight of the new edge.
    weight: float

# Request model for calculating the shortest path.
class ShortestPathRequest(BaseModel):
    # ID of the starting node.
    start_node: int
    # ID of the ending node.
    end_node: int
    # Algorithm to use ("dijkstra" or "astar").
    algorithm: str = "dijkstra"

# Response model for the shortest path calculation.
class ShortestPathResponse(BaseModel):
    # List of node IDs forming the path.
    path: List[int]
    # Total weight of the path.
    weight: float
    # Message indicating success or failure.
    message: Optional[str] = None

# Request model for updating an edge's weight.
class UpdateWeightRequest(BaseModel):
    # Source node ID of the edge.
    from_node: int
    # Destination node ID of the edge.
    to_node: int
    # New weight for the edge.
    new_weight: float

# Request model for Union-Find 'find' operation.
class FindSetRequest(BaseModel):
    # ID of the node whose set representative is to be found.
    node_id: int

# Response model for Union-Find 'find' operation.
class FindSetResponse(BaseModel):
    # ID of the node queried.
    node_id: int
    # Representative of the set containing the node.
    set_representative: int
    # Optional message.
    message: Optional[str] = None

# Request model for Union-Find 'unite' operation.
class UniteSetsRequest(BaseModel):
    # ID of the first node.
    node_id1: int
    # ID of the second node.
    node_id2: int

# Generic message response for operations like add, update, unite.
class MessageResponse(BaseModel):
    # Message detailing the result of the operation.
    message: str
    # Optional details or status.
    details: Optional[str] = None

# Response model for getting the entire graph.
class GraphResponse(BaseModel):
    # List of nodes in the current graph.
    nodes: List[Node]
    # List of edges in the current graph.
    edges: List[Edge]
    # Optional message.
    message: Optional[str] = None