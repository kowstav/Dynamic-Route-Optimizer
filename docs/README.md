# Dynamic Route Optimizer for Delivery Services

This project implements a dynamic route optimization system featuring a C++ graph engine, a FastAPI backend, and an interactive D3.js frontend visualizer. It supports various graph algorithms like Dijkstra, A*, and Floyd-Warshall, along with zone-based optimizations using Union-Find and live edge weight updates potentially using Segment Trees (core C++ engine supports this concept, API interaction for complex segment tree updates is basic).

## Features

* **Core Graph Engine (C++):**
    * Supports directed, weighted graphs.
    * Algorithms: Dijkstra, A\*, Floyd-Warshall.
    * Data Structures: Union-Find for zone management, Segment Tree concept for dynamic edge weights.
    * JSON import/export for graph data.
    * Command-line interface (CLI) for testing.
* **Backend API (FastAPI):**
    * Exposes C++ engine functionality via subprocess calls to its CLI.
    * Endpoints for:
        * Loading graph data.
        * Adding nodes and edges.
        * Calculating shortest paths (Dijkstra, A\*).
        * Updating edge weights (simulating traffic changes).
        * Union-Find operations (find set, unite sets) for zone management.
        * Retrieving the current graph state.
    * Handles request routing and basic configurations.
* **Frontend Visualizer (D3.js):**
    * Interactive graph view: displays nodes and edges.
    * Live updates: reflects changes made via API calls.
    * Node manipulation: drag-and-drop (visual only, positions not saved back to C++ engine's node properties via API yet).
    * Edge creation: by selecting nodes (basic).
    * Shortest path highlighting.
    * Controls for interacting with API endpoints.

## Project Structure

<pre><code>```bash dynamic-route-optimizer/ ├── backend/ # FastAPI application ├── cpp_engine/ # C++ graph engine ├── visualizer/ # HTML, CSS, JavaScript frontend ├── data/ # Sample graph data and configuration ├── docs/ # Project documentation (this file) ├── run.sh # Script to build C++ and run backend └── pyproject.toml # Python project configuration ``` </code></pre>

## Technologies Used

* **Graph Engine:** C++17, CMake
* **Backend API:** Python 3, FastAPI, Uvicorn
* **Frontend:** HTML, CSS, JavaScript, D3.js
* **Data Format:** JSON (for graph data and API communication)
* **C++/Python Integration:** Subprocess calls to C++ engine's CLI.

## Prerequisites

* C++17 compatible compiler (e.g., GCC, Clang)
* CMake (version 3.10+)
* Python (version 3.8+)
* pip (Python package installer)

## Setup and Installation

1.  **Clone the repository:**
    ```bash
    git clone <repository_url>
    cd dynamic-route-optimizer
    ```

2.  **Build the C++ Graph Engine:**
    ```bash
    cd cpp_engine
    mkdir build
    cd build
    cmake ..
    make
    cd ../..
    ```
    This will create an executable `dynamic_route_optimizer` in `cpp_engine/build/`.
    The `config.json` expects this path.

3.  **Set up the Python Backend:**
    * It's recommended to use a virtual environment:
        ```bash
        python -m venv venv
        source venv/bin/activate  # On Windows: venv\Scripts\activate
        ```
    * Install Python dependencies:
        ```bash
        pip install -r backend/requirements.txt
        ```

## Running the Application

1.  **Ensure the C++ engine is built** (as per Setup step 2).
2.  **Use the `run.sh` script (Linux/macOS):**
    This script attempts to build the C++ engine (if not already done) and then starts the FastAPI backend.
    ```bash
    chmod +x run.sh
    ./run.sh
    ```
    If `run.sh` is not suitable for your OS, follow these manual steps:
    * **Build C++ engine (if not done):** (See Setup step 2)
    * **Start the FastAPI Backend Server:**
        From the project root directory (`dynamic-route-optimizer/`):
        ```bash
        python -m uvicorn backend.api.main:app --host 127.0.0.1 --port 8000 --reload
        ```
        The server will typically run on `http://127.0.0.1:8000`.

3.  **Open the Frontend Visualizer:**
    Open the `visualizer/index.html` file in your web browser.

## Usage

* **Frontend Visualizer:**
    * Use "Load/Refresh Graph" to fetch the current graph from the backend.
    * Input node IDs and click "Find Path" (Dijkstra or A\*) to see the shortest path highlighted.
    * Use "Modify Graph" controls to add nodes/edges or update weights. The graph will refresh after successful operations.
    * "Zone Operations" allow interaction with the Union-Find data structure on the backend.
* **API:**
    * The API is available at `http://127.0.0.1:8000/api/v1/`.
    * Swagger UI documentation is available at `http://127.0.0.1:8000/docs`.
* **C++ Engine CLI:**
    * You can test the C++ engine directly:
        ```bash
        ./cpp_engine/build/dynamic_route_optimizer
        ```
        This starts it in interactive mode. Or, pass commands:
        ```bash
        ./cpp_engine/build/dynamic_route_optimizer load_graph data/sample_graph.json
        ./cpp_engine/build/dynamic_route_optimizer shortest_path dijkstra 1 5
        ```

## Future Enhancements / Limitations

* **Robust C++ JSON Parsing:** The current C++ JSON parser is very basic and fragile. Use a dedicated library like nlohmann/json.
* **Pybind11 Integration:** For tighter and potentially more performant C++/Python integration instead of `subprocess`.
* **Advanced Segment Tree Usage:** Fully integrate segment tree for dynamic "traffic" updates affecting multiple edges efficiently via API.
* **Real-time Frontend Updates:** Use WebSockets for pushing updates from backend to frontend instead of relying on manual refreshes after operations.
* **Persistent Graph Editing:** Save changes made via the frontend (node positions, added elements) back to a persistent store or the JSON file.
* **Error Handling:** Improve error handling and user feedback across all components.
* **Complex Constraints:** Extend beyond simple weights to include time windows, vehicle capacities, etc., for true VRP.
* **Testing:** Add comprehensive unit and integration tests.
* **Deployment:** Containerization (Docker) and deployment strategies.
