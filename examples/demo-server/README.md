# GatewayForge Demo Server

示例 TCP/UDP 服务器，用于测试 GatewayForge 的解码和回放功能。

## 构建

```bash
cd GatewayForge
cmake -B build
cmake --build build
```

## 运行

```bash
# TCP Demo Server (默认端口 9000)
./build/gatewayforge_demo_tcp_server

# UDP Demo Server (默认端口 9001)
./build/gatewayforge_demo_udp_server
```

## 协议说明

Demo server 使用 vehicle 协议:
- magic: 0xCAFE (2 bytes)
- version: 1 byte
- msg_type: 1 byte
- seq: 4 bytes (TCP) / device_id 8 bytes + seq 4 bytes (UDP)
- payload_len: 2 bytes
