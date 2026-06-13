#include <iostream>
#include "gatewayforge/cli/CommandLine.h"
#include "gatewayforge/cli/Commands.h"
#include "gatewayforge/common/Logger.h"

int main(int argc, char** argv) {
    gatewayforge::Logger::Init("info");

    auto cmd = gatewayforge::CommandLine::Parse(argc, argv);
    if (!cmd) {
        gatewayforge::Commands::PrintHelp();
        return 1;
    }

    return gatewayforge::Commands::Execute(*cmd);
}
