from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware # Import CORS
import uvicorn
import json
import os

# Import the graph API router.
from backend.api.routes import graph as graph_router # Use fully qualified import
# Import the optimizer service to manage engine initialization.
from backend.services import optimizer as optimizer_service # Use fully qualified import

# Create a FastAPI application instance.
app = FastAPI(title="Dynamic Route Optimizer API")

# Load configuration from config.json.
config_path = os.path.join(os.path.dirname(__file__), '..', '..', 'data', 'config.json')
# Default configuration values.
config = {
    "cpp_engine_path": "cpp_engine/build/dynamic_route_optimizer", # Default if not in config
    "default_graph_data": "data/sample_graph.json",          # Default if not in config
    "api_host": "127.0.0.1",
    "api_port": 8000
}
# Try to load configuration from file.
try:
    # Open and read config file.
    with open(config_path, 'r') as f:
        # Load JSON data and update config dictionary.
        config.update(json.load(f))
# Handle file not found error.
except FileNotFoundError:
    # Print warning if config file not found.
    print(f"Warning: Configuration file {config_path} not found. Using default settings.")
# Handle JSON decoding error.
except json.JSONDecodeError:
    # Print warning if config file is malformed.
    print(f"Warning: Configuration file {config_path} is malformed. Using default settings.")


# Update optimizer service configurations if they were loaded.
if "cpp_engine_path" in config:
    # Set C++ engine executable path in optimizer service.
    optimizer_service.CPP_ENGINE_EXECUTABLE = os.path.join(os.path.dirname(__file__), '..', '..', config["cpp_engine_path"])
# If default graph data path is in config.
if "default_graph_data" in config:
    # Set default graph data path in optimizer service.
    optimizer_service.DEFAULT_GRAPH_DATA_PATH = os.path.join(os.path.dirname(__file__), '..', '..', config["default_graph_data"])


# Configure CORS (Cross-Origin Resource Sharing).
app.add_middleware(
    CORSMiddleware,
    # Allow all origins (adjust in production).
    allow_origins=["*"],
    # Allow all credentials (e.g., cookies, authorization headers).
    allow_credentials=True,
    # Allow all HTTP methods.
    allow_methods=["*"],
    # Allow all HTTP headers.
    allow_headers=["*"],
)


# Event handler for application startup.
@app.on_event("startup")
async def startup_event():
    """
    Actions to perform on application startup.
    For example, initialize the C++ engine with the default graph.
    """
    # Print startup message.
    print("Application startup: Initializing C++ Optimizer Engine...")
    # Ensure the C++ engine is initialized.
    if optimizer_service.ensure_engine_initialized():
        # Print success message.
        print("C++ Optimizer Engine initialized successfully with default graph.")
    else:
        # Print failure message.
        print("Error: C++ Optimizer Engine failed to initialize. API might not function correctly.")


# Include the graph API router with a prefix.
app.include_router(graph_router.router, prefix="/api/v1", tags=["Graph Operations"])

# Root endpoint for basic API check.
@app.get("/")
async def read_root():
    # Return welcome message.
    return {"message": "Welcome to the Dynamic Route Optimizer API"}

# Main entry point to run the FastAPI application using Uvicorn.
if __name__ == "__main__":
    # Run Uvicorn server with host and port from config.
    uvicorn.run("main:app", host=config.get("api_host", "127.0.0.1"), port=config.get("api_port", 8000), reload=True)