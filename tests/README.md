# GatewayForge 测试用例运行指南

## 1. 测试体系概览

```
tests/
├── README.md          ← 本文件
├── ut/                ← 单元测试 (Unit Test)
│   ├── test_buffer.cpp              16 用例
│   ├── test_result.cpp               6 用例
│   ├── test_packet.cpp               6 用例
│   ├── test_command_line.cpp         6 用例
│   ├── test_framing_type.cpp        10 用例
│   ├── test_pcap_reader.cpp          2 用例
│   ├── test_packet_parser.cpp        2 用例
│   ├── test_flow_key.cpp             4 用例
│   ├── test_flow_tracker.cpp         2 用例
│   ├── test_tcp_reassembler.cpp      2 用例
│   ├── test_udp_datagram_tracker.cpp 2 用例
│   ├── test_schema_loader.cpp        4 用例
│   ├── test_binary_decoder.cpp       3 用例
│   └── test_replay.cpp              1 用例
│                                     ----
│                          UT 合计   66 用例
├── st/                ← 系统测试 (System Test)
│   ├── test_decode_st.cpp            6 用例
│   └── test_replay_st.cpp            4 用例
│                                     ----
│                          ST 合计   10 用例
└── it/                ← 集成测试 (Integration Test)
    └── test_integration.cpp          5 用例
                                     ----
                          IT 合计    5 用例
                          ==============
                          总计       81 用例
```

## 2. 环境准备

### 安装依赖

```bash
sudo apt-get install -y build-essential cmake \
    libpcap-dev libyaml-cpp-dev libspdlog-dev \
    nlohmann-json3-dev libgtest-dev
```

### 构建测试

```bash
cd GatewayForge
cmake -B build -DBUILD_TESTS=ON
cmake --build build -j$(nproc)
```

## 3. 运行测试

### 3.1 全部测试

```bash
cd build
ctest
```

输出示例：

```
100% tests passed, 0 tests failed out of 81

Label Time Summary:
integration    =   0.02 sec*proc (5 tests)
system         =   0.03 sec*proc (10 tests)
unit           =   0.22 sec*proc (66 tests)

Total Test time (real) =   0.29 sec
```

### 3.2 按层级运行

```bash
# 仅运行单元测试
ctest -L unit

# 仅运行系统测试
ctest -L system

# 仅运行集成测试
ctest -L integration
```

### 3.3 按模块过滤

```bash
# 运行某个模块的全部测试
./build/gatewayforge_ut --gtest_filter='BufferTest.*'
./build/gatewayforge_ut --gtest_filter='ResultTest.*'
./build/gatewayforge_ut --gtest_filter='PacketTest.*'
./build/gatewayforge_ut --gtest_filter='FlowKey*.*'
./build/gatewayforge_ut --gtest_filter='*Decoder*.*'

# 系统测试
./build/gatewayforge_st --gtest_filter='Decode*.*'
./build/gatewayforge_st --gtest_filter='Replay*.*'

# 集成测试
./build/gatewayforge_it --gtest_filter='Integration*.*'
```

### 3.4 运行单个用例

```bash
# UT 单个用例
./build/gatewayforge_ut --gtest_filter='BufferTest.DefaultConstruct_Empty'
./build/gatewayforge_ut --gtest_filter='ResultTest.Unwrap_Err_Throws'

# ST 单个用例
./build/gatewayforge_st --gtest_filter='DecodeSystemTest.TcpPacketFullDecode'

# IT 单个用例
./build/gatewayforge_it --gtest_filter='IntegrationTest.SchemaDecodePipeline_TCP'
```

### 3.5 详细输出

```bash
# 显示失败用例的详细信息
ctest --output-on-failure

# 显示所有用例的输出
ctest -V

# GoogleTest 详细输出
./build/gatewayforge_ut --gtest_print_time=1
```

### 3.6 重复运行（排查偶发失败）

```bash
# 重复运行 10 次
./build/gatewayforge_ut --gtest_repeat=10

# 重复运行直到失败
./build/gatewayforge_ut --gtest_repeat=-1 --gtest_break_on_failure
```

## 4. 直接执行测试二进制

```bash
# UT
./build/gatewayforge_ut
./build/gatewayforge_ut --gtest_list_tests    # 列出全部用例
./build/gatewayforge_ut --gtest_shuffle        # 随机顺序

# ST
./build/gatewayforge_st

# IT
./build/gatewayforge_it
```

## 5. 用例命名规范

每个测试命名格式：`模块_用例描述`

```
TEST(ClassName, MethodName_ExpectedBehavior)

示例:
TEST(BufferTest, CopyConstruct_DeepCopy)
TEST(FlowKeyTest, Normalization)
TEST(IntegrationTest, PcapFlowTimelinePipeline_TCP)
```

## 6. 常用参数

| 参数 | 说明 |
|------|------|
| `--gtest_filter=PATTERN` | 按模式过滤用例 |
| `--gtest_list_tests` | 列出所有用例名 |
| `--gtest_repeat=N` | 重复运行 N 次 (-1 无限) |
| `--gtest_shuffle` | 随机顺序执行 |
| `--gtest_break_on_failure` | 首个失败时中断 |
| `--gtest_print_time=1` | 打印每个用例耗时 |
| `--gtest_output=xml:report.xml` | 输出 XML 报告 |

## 7. 覆盖率（可选）

```bash
# 安装 lcov
sudo apt-get install -y lcov

# CMake 启用覆盖率
cmake -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="--coverage"

# 构建并运行
cmake --build build
cd build && ctest

# 生成覆盖率报告
lcov --capture --directory build --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

## 8. CI 集成示例

```yaml
# .github/workflows/test.yml
- name: Build and Test
  run: |
    sudo apt-get install -y libpcap-dev libyaml-cpp-dev libspdlog-dev nlohmann-json3-dev libgtest-dev
    cmake -B build -DBUILD_TESTS=ON
    cmake --build build -j$(nproc)
    cd build && ctest --output-on-failure
```
