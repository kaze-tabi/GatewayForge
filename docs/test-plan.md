# GatewayForge 测试计划

> 版本: V0.1 MVP | 测试框架: GoogleTest | 语言: C++20

---

## 1. 测试策略总览

### 1.1 测试金字塔

```text
          ╱  E2E  ╲         手动 / Demo 脚本
         ╱   IT   ╲        集成测试: 多模块组合
        ╱    ST   ╲        系统测试: 端到端流程
       ╱═══════════╲
      ╱     UT     ╲       单元测试: 模块级隔离
     ╱═══════════════╲
```

| 层级 | 数量 | 覆盖目标 | 执行时间 |
|------|------|----------|----------|
| **UT** | 78 | 每个类/函数的独立行为 | < 1s |
| **ST** | 18 | 模块级端到端流程 | < 5s |
| **IT** | 12 | 多模块集成链路 | < 10s |

### 1.2 模块测试矩阵

| 模块 | UT | ST | IT | 覆盖率目标 |
|------|:--:|:--:|:--:|:----------:|
| common/ (Buffer, Error, Logger, Result, Time) | 16 | - | - | 90% |
| pcap/ (Packet, PcapReader, PacketParser) | 12 | 3 | 2 | 85% |
| flow/ (FlowKey, FlowTracker, TcpFlow, UdpFlow) | 18 | 4 | 3 | 85% |
| protocol/ (SchemaLoader, BinaryDecoder, FieldType) | 16 | 4 | 3 | 85% |
| replay/ (ReplayEngine, TcpReplayer, UdpReplayer) | 6 | 4 | 3 | 80% |
| timeline/ (TimelineBuilder, ConsolePrinter) | 4 | 2 | 1 | 80% |
| cli/ (CommandLine, Commands) | 6 | 1 | - | 85% |

---

## 2. 单元测试用例 (UT)

### 2.1 common/Buffer — 16 cases

| ID | 用例 | 输入 | 期望输出 |
|----|------|------|----------|
| UT-BUF-001 | 默认构造 Empty | Buffer() | Size()==0, Empty()==true |
| UT-BUF-002 | 从数据构造 | Buffer(data, 10) | Size()==10, Data() 匹配 |
| UT-BUF-003 | 零大小构造 | Buffer(0) | Size()==0 |
| UT-BUF-004 | 拷贝构造 | Buffer a(data,5); Buffer b(a) | b.Size()==5, b.Data()==a.Data() |
| UT-BUF-005 | 拷贝赋值 | b = a | 值相同, 地址不同 |
| UT-BUF-006 | 移动构造 | Buffer b(std::move(a)) | a.Empty(), b.Size()==原值 |
| UT-BUF-007 | 移动赋值 | b = std::move(a) | a.Empty() |
| UT-BUF-008 | operator[] 正常 | buf[3] | 返回第4个字节 |
| UT-BUF-009 | operator[] 越界 | buf[buf.Size()] | 抛出 std::out_of_range |
| UT-BUF-010 | Slice 正常 | buf.Slice(2, 3) | 返回长度为3的子Buffer |
| UT-BUF-011 | Slice 越界 | buf.Slice(8, 5) | 抛出 std::out_of_range |
| UT-BUF-012 | Read<uint16_t> 正常 | buf.Read<uint16_t>(0) | 返回2字节值 |
| UT-BUF-013 | Read<> 越界 | buf.Read<uint64_t>(buf.Size()) | 抛出 std::out_of_range |
| UT-BUF-014 | 大Buffer (>1MB) | Buffer(2MB) | 正常分配和释放 |
| UT-BUF-015 | 空数据指针 | Buffer(nullptr, 0) | Size()==0 |
| UT-BUF-016 | 自赋值 | buf = buf | 无崩溃, 值不变 |

### 2.2 common/Result — 6 cases

| ID | 用例 | 输入 | 期望输出 |
|----|------|------|----------|
| UT-RES-001 | Ok 构造 | Result<int>::Ok(42) | IsOk()==true, Value()==42 |
| UT-RES-002 | Err 构造 | Result<int>::Err("msg") | IsErr()==true, Error()=="msg" |
| UT-RES-003 | Unwrap Ok | Result<int>::Ok(10).Unwrap() | 返回 10 |
| UT-RES-004 | Unwrap Err 抛异常 | Result<int>::Err("fail").Unwrap() | 抛出 std::runtime_error |
| UT-RES-005 | UnwrapOr Ok | Result<int>::Ok(10).UnwrapOr(0) | 返回 10 |
| UT-RES-006 | UnwrapOr Err | Result<int>::Err("e").UnwrapOr(5) | 返回 5 |

### 2.3 common/Time — 4 cases

| ID | 用例 | 输入 | 期望输出 |
|----|------|------|----------|
| UT-TIME-001 | NowUs 单调递增 | 两次连续调用 | t2 >= t1 |
| UT-TIME-002 | NowNs → NowUs 转换 | NowNs() / 1000 ≈ NowUs() | 误差 < 1ms |
| UT-TIME-003 | FormatUs 格式 | FormatUs(1_500_000) | "1.500000s" |
| UT-TIME-004 | FormatNs 格式 | FormatNs(500_000) | "0.000500s" |

### 2.4 pcap/Packet — 6 cases

| ID | 用例 | 输入 | 期望输出 |
|----|------|------|----------|
| UT-PKT-001 | 默认构造 | Packet() | Protocol()==UNKNOWN |
| UT-PKT-002 | Setter/Getter | SetSrcIp("1.2.3.4") | SrcIp()=="1.2.3.4" |
| UT-PKT-003 | Payload 设置 | SetPayload(data, 10) | PayloadSize()==10 |
| UT-PKT-004 | TCP Flags | SetTcpFlags(0x02) | IsSyn()==true |
| UT-PKT-005 | 拷贝构造 | Packet b(pkt) | b.SrcIp()==pkt.SrcIp(), Payload 独立 |
| UT-PKT-006 | 拷贝赋值 | b = pkt | Payload 深拷贝, 地址不同 |

### 2.5 pcap/PacketParser — 6 cases

| ID | 用例 | 输入 | 期望输出 |
|----|------|------|----------|
| UT-PP-001 | nullptr 输入 | Parse(nullptr, 0) | 返回 nullptr |
| UT-PP-002 | 太短数据 | Parse(buf, 14) | 返回 nullptr（无IP头） |
| UT-PP-003 | 非IPv4 | Parse(IPv6数据) | 返回 nullptr |
| UT-PP-004 | TCP包解析 | Parse(合法TCP包) | Protocol()==TCP, 端口正确 |
| UT-PP-005 | UDP包解析 | Parse(合法UDP包) | Protocol()==UDP, 端口正确 |
| UT-PP-006 | payload 提取 | Parse(带payload的包) | PayloadSize() > 0 |

### 2.6 flow/FlowKey — 4 cases

| ID | 用例 | 输入 | 期望输出 |
|----|------|------|----------|
| UT-FK-001 | 相等性 | 两个相同FlowKey | == 为 true |
| UT-FK-002 | 不等性(TCP vs UDP) | 协议不同 | != 为 true |
| UT-FK-003 | 归一化 | {dst大, src小} | Normalized后 src < dst |
| UT-FK-004 | Hash一致性 | key和key.Normalized() | hash值相同 |

### 2.7 flow/TcpReassembler — 6 cases

| ID | 用例 | 输入 | 期望输出 |
|----|------|------|----------|
| UT-TR-001 | 空重组器 | TcpReassembler() | ByteCount()==0 |
| UT-TR-002 | 单段追加 | AddSegment(pkt, true) | ByteCount()==payload_size |
| UT-TR-003 | 顺序两段 | seq=100(4B), seq=104(4B) | ByteCount()==8, NextSeq()==108 |
| UT-TR-004 | 乱序到达(MVP忽略) | seq=108(先), seq=100(后) | byteCount不变(跳过了非顺序) |
| UT-TR-005 | 空payload | AddSegment(payload=0) | ByteCount()不变 |
| UT-TR-006 | 零seq开始 | seq=0 | 正常处理 |

### 2.8 flow/UdpDatagramTracker — 4 cases

| ID | 用例 | 输入 | 期望输出 |
|----|------|------|----------|
| UT-UDT-001 | 空 tracker | UdpDatagramTracker() | Count()==0, ByteCount()==0 |
| UT-UDT-002 | 追加报 | AddDatagram(pkt) | Count()==1 |
| UT-UDT-003 | 多报文追加 | 10个datagram | Count()==10 |
| UT-UDT-004 | ByteCount累计 | 各4字节 | ByteCount()==16 |

### 2.9 flow/FlowTracker — 4 cases

| ID | 用例 | 输入 | 期望输出 |
|----|------|------|----------|
| UT-FT-001 | 空tracker | FlowTracker() | FlowCount()==0 |
| UT-FT-002 | TCP包跟踪 | OnPacket(TCP包) | TcpFlowCount()==1 |
| UT-FT-003 | UDP包跟踪 | OnPacket(UDP包) | UdpFlowCount()==1 |
| UT-FT-004 | 同Flow聚合 | 两个相同四元组TCP包 | FlowCount()==1, PacketCount()==2 |

### 2.10 protocol/FieldType — 4 cases

| ID | 用例 | 输入 | 期望输出 |
|----|------|------|----------|
| UT-FTY-001 | 已知类型转换 | FieldTypeFromString("u32") | FieldType::U32 |
| UT-FTY-002 | 所有类型转换 | 10种类型逐一测试 | 全部正确 |
| UT-FTY-003 | 未知类型抛异常 | FieldTypeFromString("foo") | 抛出 runtime_error |
| UT-FTY-004 | Size计算 | FieldTypeSize(U16) | 2 |

### 2.11 protocol/SchemaLoader — 6 cases

| ID | 用例 | 输入 | 期望输出 |
|----|------|------|----------|
| UT-SL-001 | 文件不存在 | LoadFromYaml("nonexistent") | IsErr()==true |
| UT-SL-002 | 无效YAML | LoadFromYaml("bad.yaml") | IsErr()==true |
| UT-SL-003 | TCP schema加载 | vehicle-tcp.yaml | IsOk(), transport=="tcp" |
| UT-SL-004 | UDP schema加载 | vehicle-udp.yaml | IsOk(), transport=="udp" |
| UT-SL-005 | 字段解析 | 含多个字段的schema | fields.size()==期望值 |
| UT-SL-006 | Framing解析 | length_field framing | framing.type==LENGTH_FIELD |

### 2.12 protocol/BinaryDecoder — 8 cases

| ID | 用例 | 输入 | 期望输出 |
|----|------|------|----------|
| UT-BD-001 | 空数据解码 | Decode(nullptr, 0) | IsOk(), Value().empty() |
| UT-BD-002 | UDP单字段解码 | magic字段(2B) | Value()[0]["magic"]==0xCAFE |
| UT-BD-003 | u8解码 | u8字段值 0x42 | 正确解析为66 |
| UT-BD-004 | u32大端解码 | u32 = 0x01020304 | 正确解析 |
| UT-BD-005 | TCP length_field | 多消息TCP流 | 解析出多个消息 |
| UT-BD-006 | const字段校验 | magic字段 const=0xCAFE | 不影响解码 |
| UT-BD-007 | bytes字段 | 变长bytes | 内容正确 |
| UT-BD-008 | DecodeToMessages | 含msg_type的报文 | Message.type正确 |

### 2.13 cli/CommandLine — 6 cases

| ID | 用例 | 输入 | 期望输出 |
|----|------|------|----------|
| UT-CL-001 | 无参数 | Parse(1, ["gw"]) | 返回 nullopt |
| UT-CL-002 | decode命令 | Parse(4, ["gw","decode","--pcap","a.pcap"]) | command=="decode" |
| UT-CL-003 | 选项解析 | --pcap file --speed 2 | Get("pcap")=="file" |
| UT-CL-004 | 无值选项 | --verbose (后无值) | Get("verbose")=="" |
| UT-CL-005 | GetOr默认值 | GetOr("missing", "default") | 返回 "default" |
| UT-CL-006 | help命令 | command="help" | 识别为help |

---

## 3. 系统测试用例 (ST)

### 3.1 完整解码流程 — 6 cases

| ID | 用例 | 步骤 | 检验点 |
|----|------|------|--------|
| ST-DEC-001 | TCP PCAP完整解码 | PcapReader→FlowTracker→TimelineBuilder→ConsolePrinter | 无异常, Events非空 |
| ST-DEC-002 | UDP PCAP完整解码 | 同上UDP版本 | UDP Flows正确识别 |
| ST-DEC-003 | 空PCAP处理 | 打开空PCAP | 无崩溃, FlowCount()==0 |
| ST-DEC-004 | 混合TCP/UDP PCAP | 含TCP和UDP的PCAP | 两种Flow都识别 |
| ST-DEC-005 | 大PCAP (>10K packets) | 打开大PCAP | 无内存泄漏, 能在合理时间完成 |
| ST-DEC-006 | 损坏PCAP | 打开损坏的PCAP | Open()返回false, 不崩溃 |

### 3.2 完整回放流程 — 4 cases

| ID | 用例 | 步骤 | 检验点 |
|----|------|------|--------|
| ST-REP-001 | TCP回放 | 启动demo TCP server→ReplayEngine→停止 | sent_count>0 |
| ST-REP-002 | UDP回放 | 启动demo UDP server→ReplayEngine→停止 | sent_count>0 |
| ST-REP-003 | 回放目标不可达 | replay到不存在的主机 | 不崩溃, 返回错误 |
| ST-REP-004 | 加速回放 | --speed 10.0 | 实际时间 < 原始时间*0.5 |

### 3.3 Demo Server — 4 cases

| ID | 用例 | 步骤 | 检验点 |
|----|------|------|--------|
| ST-SRV-001 | TCP Server启动 | 启动 tcp_demo_server | port绑定成功, 接受连接 |
| ST-SRV-002 | UDP Server启动 | 启动 udp_demo_server | port绑定成功, 接收报文 |
| ST-SRV-003 | TCP Server多客户端 | 3个并发连接 | 全部接受, 不阻塞 |
| ST-SRV-004 | TCP Server接收协议报文 | 发送vehicle TCP包 | 正确解析并响应ACK |

### 3.4 CLI 端到端 — 4 cases

| ID | 用例 | 步骤 | 检验点 |
|----|------|------|--------|
| ST-CLI-001 | decode命令完整 | gatewayforge decode --pcap ... --schema ... | exit code 0, 有时间线输出 |
| ST-CLI-002 | replay命令完整 | gatewayforge replay ... | exit code 0 |
| ST-CLI-003 | 未知命令 | gatewayforge unknown_cmd | exit code!=0, 输出帮助 |
| ST-CLI-004 | 缺少必需参数 | gatewayforge decode (无--pcap) | exit code!=0, 提示用法 |

---

## 4. 集成测试用例 (IT)

### 4.1 数据流集成 — 4 cases

| ID | 用例 | 组合模块 | 检验点 |
|----|------|----------|--------|
| IT-DS-001 | PCAP→Flow→Timeline 链路 | PcapReader+FlowTracker+TimelineBuilder | 时间线事件按序排列 |
| IT-DS-002 | PCAP→Flow→Decode 链路 | PcapReader+FlowTracker+BinaryDecoder | 每个Flow的消息正确解析 |
| IT-DS-003 | Schema→Decode→Timeline | SchemaLoader+BinaryDecoder+TimelineBuilder | schema驱动正确解析 |
| IT-DS-004 | TCP Reassembler→Decoder 链路 | TcpReassembler+BinaryDecoder | 重组流被正确解码 |

### 4.2 回放集成 — 3 cases

| ID | 用例 | 组合模块 | 检验点 |
|----|------|----------|--------|
| IT-RP-001 | PCAP→Replay→Server 链路 | PcapReader+ReplayEngine+DemoServer | 服务端收到回放流量 |
| IT-RP-002 | 多Flow并发回放 | FlowTracker+ReplayEngine | 所有Flow都被回放 |
| IT-RP-003 | TCP重连回放 | TcpReplayer多次发送 | 连接管理正确 |

### 4.3 协议集成 — 3 cases

| ID | 用例 | 组合模块 | 检验点 |
|----|------|----------|--------|
| IT-PR-001 | YAML→Schema→Decode TCP | SchemaLoader+BinaryDecoder | 完整解析 vehicle-tcp 协议 |
| IT-PR-002 | YAML→Schema→Decode UDP | SchemaLoader+BinaryDecoder | 完整解析 vehicle-udp 协议 |
| IT-PR-003 | 多协议schema切换 | 加载两个不同schema | 各自独立, 无状态污染 |

### 4.4 端到端 — 2 cases

| ID | 用例 | 步骤 | 检验点 |
|----|------|------|--------|
| IT-E2E-001 | 全链路TCP | 启动Server→Generate Traffic→Capture→Decode→Replay | 全流程无错误 |
| IT-E2E-002 | 全链路UDP | 同上UDP版本 | 全流程无错误 |

---

## 5. 测试数据

### 5.1 构造数据

- **合法TCP包**: 自构造Ethernet+IPv4+TCP+payload
- **合法UDP包**: 自构造Ethernet+IPv4+UDP+payload
- **Vehicle TCP协议包**: magic=0xCAFE, version=1, msg_type=1, seq=1, payload_len=0
- **Vehicle UDP协议包**: magic=0xCAFE, version=1, msg_type=1, device_id=10001, seq=1, timestamp=xxx, payload_len=0

### 5.2 生成脚本

```bash
# 使用 python3 生成测试用 PCAP
python3 -c "
import struct

def make_tcp_pkt():
    # 构造合法TCP包用于测试
    ...
"
```

---

## 6. 执行方式

```bash
# 编译并运行所有测试
cmake -B build -DBUILD_TESTS=ON
cmake --build build
cd build && ctest --output-on-failure

# 按标签运行
ctest -L unit         # 仅 UT
ctest -L system       # 仅 ST
ctest -L integration  # 仅 IT

# 单独运行
./build/gatewayforge_tests --gtest_filter=BufferTest.*
```

---

## 7. 覆盖率目标

| 模块 | 行覆盖 | 分支覆盖 |
|------|:------:|:--------:|
| common | 90% | 85% |
| pcap | 85% | 80% |
| flow | 85% | 80% |
| protocol | 85% | 80% |
| replay | 80% | 75% |
| timeline | 80% | 75% |
| cli | 85% | 80% |
| **总计** | **85%** | **80%** |

---

*文档版本: 1.0 | 更新日期: 2026-06-13*
