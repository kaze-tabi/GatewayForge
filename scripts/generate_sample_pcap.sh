#!/bin/bash
set -e

echo "[GatewayForge] Generate sample PCAP files"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
PCAP_DIR="${PROJECT_DIR}/examples/sample-pcap"

mkdir -p "$PCAP_DIR"

# Check if tcpdump is available
if ! command -v tcpdump &> /dev/null; then
    echo "Warning: tcpdump not available. Skip PCAP generation."
    echo "Install tcpdump: sudo apt-get install tcpdump"
    exit 0
fi

echo "[1/3] Starting demo TCP server for capture..."
cd "$PROJECT_DIR"
if [ -f build/gatewayforge_demo_tcp_server ]; then
    build/gatewayforge_demo_tcp_server 19000 &
    TCP_PID=$!
    sleep 1

    echo "[2/3] Generating TCP traffic..."
    # Send some sample packets via TCP
    python3 -c "
import socket, struct, time
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(('127.0.0.1', 19000))
for seq in range(10):
    # vehicle TCP protocol packet
    pkt = struct.pack('>HBBIIH', 0xCAFE, 1, (seq % 3) + 1, seq, 10, 0)
    sock.send(pkt)
    time.sleep(0.01)
sock.close()
" 2>/dev/null || echo "Python3 not available for traffic generation"

    # Capture with tcpdump
    echo "[3/3] Capturing PCAP..."
    sudo tcpdump -i lo port 19000 -w "$PCAP_DIR/vehicle_tcp_sample.pcap" -c 50 2>/dev/null &
    TCPDUMP_PID=$!

    sleep 2
    # Send more packets for capture
    python3 -c "
import socket, struct, time
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(('127.0.0.1', 19000))
for seq in range(20):
    pkt = struct.pack('>HBBIIH', 0xCAFE, 1, (seq % 3) + 1, seq, 10, 0)
    sock.send(pkt)
    time.sleep(0.01)
sock.close()
" 2>/dev/null || true

    wait $TCPDUMP_PID 2>/dev/null || true
    kill $TCP_PID 2>/dev/null || true
else
    echo "Build first: ./scripts/build.sh"
fi

echo "Done. PCAP files in ${PCAP_DIR}/"
