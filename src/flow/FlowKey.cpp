#include "gatewayforge/flow/FlowKey.h"
#include <sstream>

namespace gatewayforge {

FlowKey FlowKey::Normalized() const {
    FlowKey key = *this;
    if (src_ip > dst_ip || (src_ip == dst_ip && src_port > dst_port)) {
        std::swap(key.src_ip, key.dst_ip);
        std::swap(key.src_port, key.dst_port);
    }
    return key;
}

std::string FlowKey::ToString() const {
    std::ostringstream oss;
    oss << src_ip << ":" << src_port << " -> " << dst_ip << ":" << dst_port
        << " [" << (protocol == TransportProtocol::TCP ? "TCP" : "UDP") << "]";
    return oss.str();
}

} // namespace gatewayforge

namespace std {
size_t hash<gatewayforge::FlowKey>::operator()(const gatewayforge::FlowKey& k) const {
    size_t h = std::hash<std::string>()(k.src_ip);
    h ^= std::hash<uint16_t>()(k.src_port) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= std::hash<std::string>()(k.dst_ip) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= std::hash<uint16_t>()(k.dst_port) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= std::hash<int>()(static_cast<int>(k.protocol)) + 0x9e3779b9 + (h << 6) + (h >> 2);
    return h;
}
}
