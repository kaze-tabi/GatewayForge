# GatewayForge MVP 设计文档

## 版本

V0.1 - PCAP 解码版

## MVP 功能范围

1. PCAP 文件读取
2. TCP Flow 识别
3. UDP Flow 识别
4. TCP 基础有序流重组
5. UDP datagram 解析
6. YAML 协议 schema 加载
7. 自定义二进制协议解析
8. 控制台时间线输出
9. TCP/UDP 简单 replay
10. sample pcap + demo server + README

## 不包含的内容

- DPDK
- Web UI
- TLS/mTLS
- Chaos Proxy
- 大规模压测
- 完整 Metrics
- 复杂 TCP 乱序重组
- 完整 TCP 重传去重
