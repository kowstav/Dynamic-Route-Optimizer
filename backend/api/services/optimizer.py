import subprocess
import json
import os
from typing import List, Tuple, Optional, Dict, Any

# Path to the C++ engine executable, configurable via environment or config file.
CPP_ENGINE_EXECUTABLE = "cpp_engine/build/dynamic_route_optimizer"
# Path to the default graph data.
DEFAULT_GRAPH_DATA_PATH = "data/sample_graph.json"

# Initializes the C++ engine, typically by loading a graph.
def _initialize_engine():
    """
    Ensures the C++ engine is loaded with a graph.
    This might be called at startup or on first request.
    For simplicity, we assume the C++ engine can take a graph file path.
    """
    # Check if the default graph data file exists.
    if not os.path.exists(DEFAULT_GRAPH_DATA_PATH):
        # Print error if graph data not found.
        print(f"Warning: Default graph data {DEFAULT_GRAPH_DATA_PATH} not found.")
        # Return False indicating failure.
        return False
    
    # Command to load the graph in the C++ engine.
    cmd = [CPP_ENGINE_EXECUTABLE, "load_graph", DEFAULT_GRAPH_DATA_PATH]
    # Try to execute the command.
    try:
        # Run the command and capture output.
        result = subprocess.run(cmd, capture_output=True, text=True, check=True, timeout=10)
        # Print stdout from C++ engine.
        print(f"Engine load output: {result.stdout.strip()}")
        # If "Error" is in stdout, something went wrong despite 0 exit code potentially.
        if "Error" in result.stdout or "Could not load graph" in result.stdout :
            # Print error message.
            print(f"Error initializing C++ engine with graph: {result.stdout.strip()}")
            # Return False indicating failure.
            return False
        # Return True indicating success.
        return True
    # Handle exceptions during subprocess execution.
    except subprocess.CalledProcessError as e:
        # Print error message with stderr.
        print(f"Error calling C++ engine for load_graph: {e.stderr}")
        # Return False indicating failure.
        return False
    # Handle timeout exception.
    except subprocess.TimeoutExpired:
        # Print timeout message.
        print("Timeout calling C++ engine for load_graph.")
        # Return False indicating failure.
        return False
    # Handle file not found error.
    except FileNotFoundError:
        # Print executable not found message.
        print(f"Error: C++ executable '{CPP_ENGINE_EXECUTABLE}' not found. Ensure it is compiled and path is correct.")
        # Return False indicating failure.
        return False


# Counter for initialization attempts.
INIT_ATTEMPTS = 0
# Maximum initialization attempts.
MAX_INIT_ATTEMPTS = 1
# Flag indicating if engine is initialized.
ENGINE_INITIALIZED = False

# Ensures the C++ engine is initialized.
def ensure_engine_initialized():
    # Global variables for initialization state.
    global ENGINE_INITIALIZED, INIT_ATTEMPTS
    # If engine is not initialized and attempts are within limit.
    if not ENGINE_INITIALIZED and INIT_ATTEMPTS < MAX_INIT_ATTEMPTS:
        # Attempt to initialize the engine.
        if _initialize_engine():
            # Set initialized flag to True.
            ENGINE_INITIALIZED = True
        # Increment attempt counter.
        INIT_ATTEMPTS +=1
    # Return current initialization state.
    return ENGINE_INITIALIZED


# Calls the C++ engine with a given command and arguments.
def call_cpp_engine(command_args: List[str]) -> Tuple[Optional[str], Optional[str]]:
    """
    Calls the C++ executable with the given list of command arguments.
    Returns a tuple (stdout, stderr).
    """
    # If engine is not initialized, try to initialize it.
    if not ensure_engine_initialized():
        # If initialization fails, return error.
        return None, "C++ engine could not be initialized with graph data."

    # Prepend the executable path to the command arguments.
    cmd = [CPP_ENGINE_EXECUTABLE] + command_args
    # Try to execute the command.
    try:
        # Run the command and capture output.
        result = subprocess.run(cmd, capture_output=True, text=True, check=False, timeout=15) # check=False to get output even on error
        # If C++ process returned an error code (non-zero).
        if result.returncode != 0:
            # Combine stdout and stderr for error message.
            error_message = f"C++ engine error (code {result.returncode}): {result.stdout.strip()} {result.stderr.strip()}"
            # Return None for stdout and the error message for stderr.
            return None, error_message
        # Return stdout and stderr (which should be empty on success).
        return result.stdout.strip(), result.stderr.strip() if result.stderr else None
    # Handle subprocess execution errors.
    except subprocess.CalledProcessError as e:
        # Return None for stdout and error details for stderr.
        return None, f"CalledProcessError: {e.stderr or e.stdout or str(e)}"
    # Handle timeout exception.
    except subprocess.TimeoutExpired:
        # Return None for stdout and timeout message for stderr.
        return None, "Timeout calling C++ engine."
    # Handle file not found error (engine executable).
    except FileNotFoundError:
        # Return None for stdout and file not found message for stderr.
        return None, f"Error: C++ executable '{CPP_ENGINE_EXECUTABLE}' not found."

# Service function to add a node.
def add_node_service(node_id: int, x: Optional[float] = None, y: Optional[float] = None) -> Dict[str, Any]:
    # Prepare command arguments for adding a node.
    args = ["add_node", str(node_id)]
    # If x coordinate is provided.
    if x is not None:
        # Add x coordinate to arguments.
        args.append(str(x))
    # If y coordinate is provided.
    if y is not None:
        # Add y coordinate to arguments.
        args.append(str(y))
    # Call the C++ engine.
    stdout, stderr = call_cpp_engine(args)
    # If an error occurred.
    if stderr:
        # Return error message.
        return {"message": "Failed to add node", "details": stderr}
    # Return success message from stdout.
    return {"message": stdout or "Node added successfully (no output from engine)."}

# Service function to add an edge.
def add_edge_service(from_node: int, to_node: int, weight: float) -> Dict[str, Any]:
    # Prepare command arguments for adding an edge.
    args = ["add_edge", str(from_node), str(to_node), str(weight)]
    # Call the C++ engine.
    stdout, stderr = call_cpp_engine(args)
    # If an error occurred.
    if stderr:
        # Return error message.
        return {"message": "Failed to add edge", "details": stderr}
    # Return success message from stdout.
    return {"message": stdout or "Edge added successfully (no output from engine)."}

# Service function to find the shortest path.
def shortest_path_service(start_node: int, end_node: int, algorithm: str) -> Dict[str, Any]:
    # Prepare command arguments for finding the shortest path.
    args = ["shortest_path", algorithm, str(start_node), str(end_node)]
    # Call the C++ engine.
    stdout, stderr = call_cpp_engine(args)
    # If an error occurred or no stdout.
    if stderr or not stdout:
        # Return error message or "No path found" based on stderr.
        return {"path": [], "weight": float('inf'), "message": stderr or "No path found or error in engine."}
    # Try to parse the output from the C++ engine.
    try:
        # Initialize path and weight.
        path_nodes = []
        # Initialize path weight.
        path_weight = float('inf')
        # Split stdout into lines.
        lines = stdout.splitlines()
        # If "No path found" is in the output.
        if "No path found" in stdout:
             # Return no path found message.
            return {"path": [], "weight": float('inf'), "message": f"No path found from {start_node} to {end_node}."}

        # Iterate through lines to find "Path:" and "Weight:".
        for line in lines:
            # If line starts with "Path:".
            if line.startswith("Path:"):
                # Extract path string.
                path_str = line.replace("Path:", "").strip()
                # Split path string by " -> " to get node IDs.
                path_nodes = [int(p.strip()) for p in path_str.split("->")]
            # Else if line starts with "Weight:".
            elif line.startswith("Weight:"):
                # Extract and convert weight to float.
                path_weight = float(line.replace("Weight:", "").strip())
        
        # If path nodes were extracted.
        if path_nodes:
            # Return the found path and weight.
            return {"path": path_nodes, "weight": path_weight, "message": "Path found successfully."}
        else:
            # If parsing failed, return an error.
            return {"path": [], "weight": float('inf'), "message": f"Could not parse path from engine output: {stdout}"}

    # Handle exceptions during parsing.
    except Exception as e:
        # Return parsing error message.
        return {"path": [], "weight": float('inf'), "message": f"Error parsing C++ engine output: {str(e)}. Raw output: {stdout}"}

# Service function to update an edge's weight.
def update_weight_service(from_node: int, to_node: int, new_weight: float) -> Dict[str, Any]:
    # Prepare command arguments for updating edge weight.
    args = ["update_edge_weight", str(from_node), str(to_node), str(new_weight)]
    # Call the C++ engine.
    stdout, stderr = call_cpp_engine(args)
    # If an error occurred.
    if stderr:
        # Return error message.
        return {"message": "Failed to update weight", "details": stderr}
    # Return success message from stdout.
    return {"message": stdout or "Weight updated successfully (no output from engine)."}

# Service function for Union-Find 'find' operation.
def find_set_service(node_id: int) -> Dict[str, Any]:
    # Prepare command arguments for find_set.
    args = ["find_set", str(node_id)]
    # Call the C++ engine.
    stdout, stderr = call_cpp_engine(args)
    # If an error occurred or no stdout.
    if stderr or not stdout:
        # Return error message.
        return {"node_id": node_id, "set_representative": -1, "message": stderr or "Error in engine or node not found."}
    # Try to parse the output.
    try:
        # Example output: "Set for node 1: 1"
        if "Set for node" in stdout and ":" in stdout:
            # Extract representative part.
            parts = stdout.split(":")
            # Convert representative to int.
            representative = int(parts[1].strip())
            # Return success response.
            return {"node_id": node_id, "set_representative": representative, "message": "Set found successfully."}
        # If output format is unexpected.
        return {"node_id": node_id, "set_representative": -1, "message": f"Could not parse find_set output: {stdout}"}
    # Handle exceptions during parsing.
    except Exception as e:
        # Return parsing error message.
        return {"node_id": node_id, "set_representative": -1, "message": f"Error parsing C++ engine output: {str(e)}"}

# Service function for Union-Find 'unite' operation.
def unite_sets_service(node_id1: int, node_id2: int) -> Dict[str, Any]:
    # Prepare command arguments for unite_sets.
    args = ["unite_sets", str(node_id1), str(node_id2)]
    # Call the C++ engine.
    stdout, stderr = call_cpp_engine(args)
    # If an error occurred.
    if stderr:
        # Return error message.
        return {"message": "Failed to unite sets", "details": stderr}
    # Return success message from stdout.
    return {"message": stdout or "Sets united successfully (no output from engine)."}

# Service function to get the current graph data as JSON.
def get_graph_data_service() -> Optional[Dict[str, Any]]:
    # Command to dump graph data as JSON from C++ engine.
    args = ["dump_graph_json"]
    # Call C++ engine.
    stdout, stderr = call_cpp_engine(args)
    # If an error occurred or no stdout.
    if stderr or not stdout:
        # Print error message.
        print(f"Error getting graph data from C++ engine: {stderr or 'No output'}")
        # Return None.
        return None
    # Try to parse the JSON string from stdout.
    try:
        # Parse JSON string to Python dictionary.
        graph_data = json.loads(stdout)
        # Return parsed graph data.
        return graph_data
    # Handle JSON decoding errors.
    except json.JSONDecodeError as e:
        # Print JSON decoding error message.
        print(f"Error decoding JSON from C++ engine: {str(e)}. Raw output: {stdout}")
        # Return None.
        return None

# On module load, try to initialize the C++ engine with the default graph.
# This makes the first API call faster if successful.
# ensure_engine_initialized() # Call this in main.py's startup event instead for FastAPI context