#include "gatewayforge/common/Error.h"
#include <cstring>
#include <netdb.h>

namespace gatewayforge {

std::string Error::FormatErrno(int err) {
    return std::string(std::strerror(err));
}

std::string Error::FormatGaiError(int err) {
    return std::string(gai_strerror(err));
}

} // namespace gatewayforge
