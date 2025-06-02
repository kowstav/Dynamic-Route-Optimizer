#!/bin/bash

# Script to build the C++ engine and run the FastAPI backend

# Navigate to the script's directory to ensure relative paths work
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd "$SCRIPT_DIR"

echo "Attempting to build the C++ engine..."
# Check if build directory exists
if [ ! -d "cpp_engine/build" ]; then
    mkdir -p cpp_engine/build
fi

# Navigate to build directory, run cmake and make
cd cpp_engine/build
cmake ..
make

# Check if make was successful
if [ $? -ne 0 ]; then
    echo "C++ engine build failed. Please check CMake/compiler errors."
    cd "$SCRIPT_DIR" # Go back to original script dir
    exit 1
fi
echo "C++ engine built successfully (or was already built)."
cd "$SCRIPT_DIR" # Go back to original script dir


# Check for Python virtual environment
if [ -d "venv" ]; then
    echo "Activating Python virtual environment..."
    source venv/bin/activate
else
    echo "Python virtual environment 'venv' not found."
    echo "Please create it first: python -m venv venv && source venv/bin/activate && pip install -r backend/requirements.txt"
    # Optionally, you could offer to create it here, but for simplicity, we'll just warn.
fi

# Run the FastAPI backend
echo "Starting FastAPI backend server..."
echo "Access the API at http://127.0.0.1:8000 (typically)"
echo "Open visualizer/index.html in your browser."

# Check if uvicorn is available directly or if we need python -m
if command -v uvicorn &> /dev/null; then
    uvicorn backend.api.main:app --host 127.0.0.1 --port 8000 --reload
else
    python -m uvicorn backend.api.main:app --host 127.0.0.1 --port 8000 --reload
fi

# Deactivate virtual environment if it was sourced by this script (optional)
# This part might not be reached if uvicorn runs indefinitely in foreground
if [ -d "venv" ]; then
    echo "To deactivate the virtual environment manually later, type: deactivate"
fi