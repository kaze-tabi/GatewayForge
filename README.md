# GatewayForge

> 面向自研 TCP/UDP/IoT 协议的 C++ 高性能调试、回放与压测平台

GatewayForge 是一个面向自研网络协议的高性能 C++ 工具平台，主要用于解决 IoT、车联网、游戏网关、边缘网关、自研 TCP/UDP 协议系统中的协议调试、流量复现和高并发压测问题。

## 功能概述

- **PCAP 解析** — 读取 pcap 文件，解析链路层/IP 层/传输层头部
- **Flow 追踪** — TCP Session 和 UDP Flow 识别与跟踪
- **TCP 重组** — TCP 有序字节流重组，粘包/半包处理
- **UDP 分析** — UDP datagram 聚合，丢包/乱序/重复包检测
- **协议解析** — 基于 YAML schema 的自定义二进制协议解析
- **时间线输出** — 控制台时间线可视化
- **流量回放** — TCP/UDP 流量回放到目标服务

## 快速开始

### 依赖安装

```bash
sudo apt-get install -y build-essential cmake \
    libpcap-dev libyaml-cpp-dev libspdlog-dev \
    nlohmann-json3-dev libgtest-dev
```

### 构建

```bash
cd GatewayForge
cmake -B build
cmake --build build
```

### 运行 Demo

```bash
./scripts/run_demo.sh
```

### 命令示例

```bash
# 解码 PCAP
./build/gatewayforge decode \
    --pcap examples/sample-pcap/vehicle_tcp_sample.pcap \
    --schema examples/schemas/vehicle-tcp.yaml

# TCP 回放
./build/gatewayforge replay \
    --pcap examples/sample-pcap/vehicle_tcp_sample.pcap \
    --target 127.0.0.1:9000 \
    --transport tcp

# UDP 回放
./build/gatewayforge replay \
    --pcap examples/sample-pcap/vehicle_udp_sample.pcap \
    --target 127.0.0.1:9001 \
    --transport udp
```

## 项目结构

```
gatewayforge/
├── CMakeLists.txt
├── README.md
├── docs/              # 设计文档
├── include/           # 头文件
├── src/               # 源文件
├── examples/          # 示例文件
├── tests/             # 单元测试
└── scripts/           # 脚本
```

## 技术栈

- **C++20** — 现代 C++ 标准
- **epoll** — Linux I/O 多路复用
- **libpcap** — PCAP 文件解析
- **yaml-cpp** — YAML 配置解析
- **nlohmann/json** — JSON 处理
- **spdlog** — 日志输出
- **GoogleTest** — 单元测试

## 版本路线图

- **V0.1** — PCAP 解码与协议解析（当前 MVP）
- **V0.2** — 完整 replay 与 chaos proxy
- **V0.3** — 高并发压测与 drain test
- **V0.4** — TLS/mTLS 支持
- **V0.5** — DPDK 引擎与 Web UI

## License

MIT
