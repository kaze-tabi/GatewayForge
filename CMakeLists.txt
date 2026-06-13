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

    add_executable(gatewayforge_tests
        tests/test_pcap_reader.cpp
        tests/test_packet_parser.cpp
        tests/test_flow_key.cpp
        tests/test_flow_tracker.cpp
        tests/test_tcp_reassembler.cpp
        tests/test_udp_datagram_tracker.cpp
        tests/test_schema_loader.cpp
        tests/test_binary_decoder.cpp
        tests/test_replay.cpp
    )

    target_link_libraries(gatewayforge_tests PRIVATE
        gatewayforge_core
        GTest::GTest
        GTest::Main
    )

    include(GoogleTest)
    gtest_discover_tests(gatewayforge_tests)
endif()
