cmake_minimum_required(VERSION 3.16)
project(GatewayForge VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 查找依赖
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED libpcap)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
pkg_check_modules(SPDLOG REQUIRED spdlog)

find_package(nlohmann_json REQUIRED)

# 核心库
add_library(gatewayforge_core STATIC
    # common
    src/common/Buffer.cpp
    src/common/Error.cpp
    src/common/Logger.cpp
    src/common/Result.cpp
    src/common/Time.cpp
    # pcap
    src/pcap/PcapReader.cpp
    src/pcap/Packet.cpp
    src/pcap/PacketParser.cpp
    # flow
    src/flow/FlowKey.cpp
    src/flow/FlowTracker.cpp
    src/flow/TcpFlow.cpp
    src/flow/UdpFlow.cpp
    src/flow/TcpReassembler.cpp
    src/flow/UdpDatagramTracker.cpp
    # protocol
    src/protocol/ProtocolSchema.cpp
    src/protocol/SchemaLoader.cpp
    src/protocol/FieldType.cpp
    src/protocol/FieldValue.cpp
    src/protocol/Framing.cpp
    src/protocol/BinaryDecoder.cpp
    src/protocol/DecodedMessage.cpp
    # replay
    src/replay/ReplayEngine.cpp
    src/replay/TcpReplayer.cpp
    src/replay/UdpReplayer.cpp
    # timeline
    src/timeline/TimelineEvent.cpp
    src/timeline/TimelineBuilder.cpp
    src/timeline/ConsoleTimelinePrinter.cpp
    # cli
    src/cli/CommandLine.cpp
    src/cli/Commands.cpp
)

target_include_directories(gatewayforge_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${PCAP_INCLUDE_DIRS}
    ${YAML_CPP_INCLUDE_DIRS}
    ${SPDLOG_INCLUDE_DIRS}
)

target_link_libraries(gatewayforge_core PUBLIC
    ${PCAP_LIBRARIES}
    ${YAML_CPP_LIBRARIES}
    ${SPDLOG_LIBRARIES}
    nlohmann_json::nlohmann_json
)

# CLI 可执行文件
add_executable(gatewayforge
    src/main.cpp
)

target_link_libraries(gatewayforge PRIVATE gatewayforge_core)

# Demo TCP Server
add_executable(gatewayforge_demo_tcp_server
    examples/demo-server/tcp_demo_server.cpp
)

# Demo UDP Server
add_executable(gatewayforge_demo_udp_server
    examples/demo-server/udp_demo_server.cpp
)

# 测试
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    # ===== 单元测试 (UT) =====
    add_executable(gatewayforge_ut
        # common
        tests/ut/test_buffer.cpp
        tests/ut/test_result.cpp
        tests/ut/test_packet.cpp
        tests/ut/test_command_line.cpp
        tests/ut/test_framing_type.cpp
        # pcap
        tests/ut/test_pcap_reader.cpp
        tests/ut/test_packet_parser.cpp
        # flow
        tests/ut/test_flow_key.cpp
        tests/ut/test_flow_tracker.cpp
        tests/ut/test_tcp_reassembler.cpp
        tests/ut/test_udp_datagram_tracker.cpp
        # protocol
        tests/ut/test_schema_loader.cpp
        tests/ut/test_binary_decoder.cpp
        # replay
        tests/ut/test_replay.cpp
    )

    target_link_libraries(gatewayforge_ut PRIVATE
        gatewayforge_core
        GTest::GTest
        GTest::Main
    )

    # ===== 系统测试 (ST) =====
    add_executable(gatewayforge_st
        tests/st/test_decode_st.cpp
        tests/st/test_replay_st.cpp
    )

    target_link_libraries(gatewayforge_st PRIVATE
        gatewayforge_core
        GTest::GTest
        GTest::Main
    )

    # ===== 集成测试 (IT) =====
    add_executable(gatewayforge_it
        tests/it/test_integration.cpp
    )

    target_link_libraries(gatewayforge_it PRIVATE
        gatewayforge_core
        GTest::GTest
        GTest::Main
    )

    include(GoogleTest)
    gtest_discover_tests(gatewayforge_ut PROPERTIES LABELS "unit")
    gtest_discover_tests(gatewayforge_st PROPERTIES LABELS "system")
    gtest_discover_tests(gatewayforge_it PROPERTIES LABELS "integration")
endif()
