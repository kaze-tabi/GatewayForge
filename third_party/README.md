# Third-Party Dependencies

GatewayForge MVP 使用以下第三方库：

| Library | Version | Usage |
|---------|---------|-------|
| libpcap | >= 1.9 | PCAP file reading |
| yaml-cpp | >= 0.7 | YAML schema parsing |
| spdlog | >= 1.10 | Logging |
| nlohmann/json | >= 3.11 | JSON handling |
| GoogleTest | >= 1.12 | Unit testing |

## Installation (Ubuntu/Debian)

```bash
sudo apt-get install -y \
    libpcap-dev \
    libyaml-cpp-dev \
    libspdlog-dev \
    nlohmann-json3-dev \
    libgtest-dev
```
