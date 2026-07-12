#!/bin/bash
set -e

echo "==============================================="
echo "  GatewayForge MVP Demo"
echo "==============================================="
echo ""

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_DIR}/build"

# Step 1: Build
echo "[1/5] Build project..."
cmake -B "$BUILD_DIR" -DBUILD_TESTS=OFF > /dev/null 2>&1
cmake --build "$BUILD_DIR" -j$(nproc) > /dev/null 2>&1
echo "  Build complete."

# Step 2: Start demo servers
echo "[2/5] Start demo servers..."
"$BUILD_DIR/gatewayforge_demo_tcp_server" 19000 &
TCP_PID=$!
"$BUILD_DIR/gatewayforge_demo_udp_server" 19001 &
UDP_PID=$!
sleep 1
echo "  TCP server on :19000, UDP server on :19001"

# Step 3: Generate traffic
echo "[3/5] Generate sample traffic..."
python3 -c "
import socket, struct, time

# TCP traffic
try:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(2)
    sock.connect(('127.0.0.1', 19000))
    for seq in range(10):
        pkt = struct.pack('>HBBIIH', 0xCAFE, 1, (seq % 3) + 1, seq, 10, 0)
        sock.send(pkt)
        time.sleep(0.02)
    sock.close()
except Exception as e:
    print(f'  TCP traffic gen: {e}')

# UDP traffic
try:
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    for seq in range(15):
        ts = int(time.time() * 1000)
        pkt = struct.pack('>HBBQIQH', 0xCAFE, 1, 1, 10001, seq, ts, 10, 0)
        sock.sendto(pkt, ('127.0.0.1', 19001))
        time.sleep(0.02)
    sock.close()
except Exception as e:
    print(f'  UDP traffic gen: {e}')
" 2>/dev/null || echo "  Traffic generation requires python3. Skipping."
echo "  Traffic generated."

# Step 4: Capture pcap
echo "[4/5] Capture sample pcap..."
if command -v tcpdump &> /dev/null; then
    sudo tcpdump -i lo port 19000 or port 19001 -w "$PROJECT_DIR/examples/sample-pcap/vehicle_tcp_sample.pcap" -c 30 2>/dev/null &
    TCPDUMP_PID=$!
    sleep 3
    wait $TCPDUMP_PID 2>/dev/null || true
    echo "  PCAP captured."
else
    echo "  tcpdump not available. Skipping PCAP capture."
fi

# Step 5: Decode
echo "[5/5] Decode sample pcap..."
if [ -f "$PROJECT_DIR/examples/sample-pcap/vehicle_tcp_sample.pcap" ]; then
    "$BUILD_DIR/gatewayforge" decode \
        --pcap "$PROJECT_DIR/examples/sample-pcap/vehicle_tcp_sample.pcap" \
        --schema "$PROJECT_DIR/examples/schemas/vehicle-tcp.yaml" 2>/dev/null || true
else
    "$BUILD_DIR/gatewayforge" decode \
        --pcap "$PROJECT_DIR/examples/sample-pcap/vehicle_tcp_sample.pcap" \
        --schema "$PROJECT_DIR/examples/schemas/vehicle-tcp.yaml" 2>/dev/null || \
    echo "  No PCAP file available. Run with real pcap:"
    echo "    build/gatewayforge decode --pcap <file> --schema examples/schemas/vehicle-tcp.yaml"
fi

# Cleanup
kill $TCP_PID $UDP_PID 2>/dev/null || true

echo ""
echo "==============================================="
echo "  Demo complete!"
echo "==============================================="
