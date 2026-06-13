#pragma once

#include "gatewayforge/cli/CommandLine.h"

namespace gatewayforge {

class Commands {
public:
    static int Execute(const CommandLine& args);
    static void PrintHelp();

private:
    static int CmdDecode(const CommandLine& args);
    static int CmdReplay(const CommandLine& args);
};

} // namespace gatewayforge
