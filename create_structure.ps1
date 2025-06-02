$root = "dynamic-route-optimizer"

$folders = @(
    "$root/backend/api/routes",
    "$root/backend/api/services",
    "$root/backend/models",
    "$root/cpp_engine/algorithms",
    "$root/cpp_engine/utils",
    "$root/cpp_engine/include",
    "$root/visualizer",
    "$root/data",
    "$root/docs"
)

foreach ($folder in $folders) {
    New-Item -ItemType Directory -Force -Path $folder | Out-Null
}

$files = @{
    "$root/backend/api/main.py" = '# FastAPI main entry'
    "$root/backend/api/routes/graph.py" = '# Graph API routes'
    "$root/backend/api/services/optimizer.py" = '# Python-C++ integration logic'
    "$root/backend/models/schema.py" = '# Pydantic schemas'
    "$root/backend/requirements.txt" = @"
fastapi
uvicorn[standard]
pybind11
"@
    "$root/cpp_engine/algorithms/dijkstra.cpp" = '// Dijkstra algorithm'
    "$root/cpp_engine/algorithms/astar.cpp" = '// A* algorithm'
    "$root/cpp_engine/algorithms/floyd_warshall.cpp" = '// Floyd-Warshall algorithm'
    "$root/cpp_engine/algorithms/union_find.cpp" = '// Union-Find implementation'
    "$root/cpp_engine/utils/graph.cpp" = '// Graph implementation'
    "$root/cpp_engine/utils/segment_tree.cpp" = '// Segment tree for traffic updates'
    "$root/cpp_engine/utils/graph_io.cpp" = '// JSON IO functions'
    "$root/cpp_engine/main.cpp" = '// Entry point for testing graph engine'
    "$root/cpp_engine/CMakeLists.txt" = @"
cmake_minimum_required(VERSION 3.10)
project(RouteOptimizer)
add_executable(main main.cpp)
"@
    "$root/visualizer/index.html" = @"
<!DOCTYPE html>
<html>
<head><title>Route Visualizer</title></head>
<body></body>
</html>
"@
    "$root/visualizer/style.css" = '/* Visualizer styles */'
    "$root/visualizer/app.js" = '// D3.js code'
    "$root/visualizer/api.js" = '// API interactions'
    "$root/data/sample_graph.json" = '{}'
    "$root/data/config.json" = '{}'
    "$root/docs/README.md" = '# Dynamic Route Optimizer'
    "$root/run.sh" = '#!/bin/bash
uvicorn backend.api.main:app --reload'
    "$root/pyproject.toml" = @"
[build-system]
requires = ["setuptools"]
build-backend = "setuptools.build_meta"
"@
}

foreach ($file in $files.Keys) {
    Set-Content -Path $file -Value $files[$file] -Force
}

Write-Host "✅ Project structure created at .\$root"
