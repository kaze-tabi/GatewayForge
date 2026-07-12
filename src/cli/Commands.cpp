#include "gatewayforge/cli/Commands.h"
#include "gatewayforge/pcap/PcapReader.h"
#include "gatewayforge/flow/FlowTracker.h"
#include "gatewayforge/flow/TcpFlow.h"
#include "gatewayforge/flow/UdpFlow.h"
#include "gatewayforge/protocol/SchemaLoader.h"
#include "gatewayforge/protocol/BinaryDecoder.h"
#include "gatewayforge/replay/ReplayEngine.h"
#include "gatewayforge/timeline/TimelineBuilder.h"
#include "gatewayforge/timeline/ConsoleTimelinePrinter.h"
#include "gatewayforge/common/Logger.h"
#include <iostream>

namespace gatewayforge {

void Commands::PrintHelp() {
    std::cout << R"(GatewayForge - custom TCP/UDP protocol debugging toolkit

Commands:
  decode       Decode TCP/UDP packets from PCAP
  replay       Replay TCP/UDP traffic from PCAP

Options:
  --pcap <path>         Path to PCAP file
  --schema <path>       Path to YAML protocol schema
  --target <host:port>  Target host:port for replay
  --transport <tcp|udp> Transport protocol
  --speed <value>       Replay speed multiplier (default: 1.0)

Examples:
  gatewayforge decode --pcap sample.pcap --schema schema.yaml
  gatewayforge replay --pcap sample.pcap --target 127.0.0.1:9000 --transport tcp
)";
}

int Commands::Execute(const CommandLine& args) {
    if (args.command == "help" || args.command == "--help" || args.command == "-h") {
        PrintHelp();
        return 0;
    }

    if (args.command == "decode") {
        return CmdDecode(args);
    }

    if (args.command == "replay") {
        return CmdReplay(args);
    }

    std::cerr << "Unknown command: " << args.command << "\n";
    PrintHelp();
    return 1;
}

int Commands::CmdDecode(const CommandLine& args) {
    std::string pcap_path = args.GetOr("pcap", "");
    std::string schema_path = args.GetOr("schema", "");

    if (pcap_path.empty() || schema_path.empty()) {
        std::cerr << "Usage: gatewayforge decode --pcap <path> --schema <path>\n";
        return 1;
    }

    // Load schema
    auto schema_result = SchemaLoader::LoadFromYaml(schema_path);
    if (schema_result.IsErr()) {
        std::cerr << "Failed to load schema: " << schema_result.Error() << "\n";
        return 1;
    }
    auto schema = schema_result.Unwrap();

    // Read PCAP
    PcapReader reader;
    if (!reader.Open(pcap_path)) {
        std::cerr << "Failed to open PCAP: " << pcap_path << "\n";
        return 1;
    }

    // Track flows
    FlowTracker tracker;
    reader.ForEachPacket([&](std::unique_ptr<Packet> packet) {
        if (packet && packet->Protocol() != TransportProtocol::UNKNOWN) {
            tracker.OnPacket(*packet);
        }
    });

    std::cout << "\n=== Flow Summary ===\n";
    std::cout << "Total flows: " << tracker.FlowCount() << "\n";
    std::cout << "TCP flows: " << tracker.TcpFlowCount() << "\n";
    std::cout << "UDP flows: " << tracker.UdpFlowCount() << "\n\n";

    // Build and print timeline
    TimelineBuilder builder(tracker, schema);
    builder.Build();

    ConsoleTimelinePrinter printer;
    printer.Print(builder.Events());

    return 0;
}

int Commands::CmdReplay(const CommandLine& args) {
    std::string pcap_path = args.GetOr("pcap", "");
    std::string target_str = args.GetOr("target", "");
    std::string transport = args.GetOr("transport", "tcp");
    double speed = std::stod(args.GetOr("speed", "1.0"));

    if (pcap_path.empty() || target_str.empty()) {
        std::cerr << "Usage: gatewayforge replay --pcap <path> --target <host:port> [--transport tcp|udp]\n";
        return 1;
    }

    // Parse target host:port
    auto colon_pos = target_str.find(':');
    if (colon_pos == std::string::npos) {
        std::cerr << "Invalid target format. Use host:port\n";
        return 1;
    }

    ReplayOptions options;
    options.pcap_path = pcap_path;
    options.target_host = target_str.substr(0, colon_pos);
    options.target_port = static_cast<uint16_t>(std::stoi(target_str.substr(colon_pos + 1)));
    options.transport = transport;
    options.speed = speed;

    ReplayEngine engine(options);
    engine.Run();

    return 0;
}

} // namespace gatewayforge
