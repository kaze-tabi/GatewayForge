#!/bin/bash
set -e

echo "[GatewayForge] Build script"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

BUILD_DIR="${PROJECT_DIR}/build"
echo "[1/3] Configuring CMake..."
cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release

echo "[2/3] Building..."
cmake --build "$BUILD_DIR" -j$(nproc)

echo "[3/3] Build complete!"
echo ""
echo "Binaries:"
echo "  ${BUILD_DIR}/gatewayforge"
echo "  ${BUILD_DIR}/gatewayforge_demo_tcp_server"
echo "  ${BUILD_DIR}/gatewayforge_demo_udp_server"
