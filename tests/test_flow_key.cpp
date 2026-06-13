#include <gtest/gtest.h>
#include "gatewayforge/flow/FlowKey.h"

TEST(FlowKeyTest, Equality) {
    gatewayforge::FlowKey a{"10.0.0.1", 1234, "10.0.0.2", 5678, gatewayforge::TransportProtocol::TCP};
    gatewayforge::FlowKey b{"10.0.0.1", 1234, "10.0.0.2", 5678, gatewayforge::TransportProtocol::TCP};
    EXPECT_EQ(a, b);
}

TEST(FlowKeyTest, Inequality) {
    gatewayforge::FlowKey a{"10.0.0.1", 1234, "10.0.0.2", 5678, gatewayforge::TransportProtocol::TCP};
    gatewayforge::FlowKey b{"10.0.0.1", 1234, "10.0.0.2", 5678, gatewayforge::TransportProtocol::UDP};
    EXPECT_NE(a, b);
}

TEST(FlowKeyTest, Normalization) {
    gatewayforge::FlowKey key{"10.0.0.2", 5678, "10.0.0.1", 1234, gatewayforge::TransportProtocol::UDP};
    auto normalized = key.Normalized();
    EXPECT_EQ(normalized.src_ip, "10.0.0.1");
    EXPECT_EQ(normalized.src_port, 1234);
}

TEST(FlowKeyTest, HashSameForKeyAndNormalized) {
    gatewayforge::FlowKey a{"10.0.0.1", 1234, "10.0.0.2", 5678, gatewayforge::TransportProtocol::UDP};
    gatewayforge::FlowKey b{"10.0.0.2", 5678, "10.0.0.1", 1234, gatewayforge::TransportProtocol::UDP};
    EXPECT_EQ(std::hash<gatewayforge::FlowKey>{}(a.Normalized()),
              std::hash<gatewayforge::FlowKey>{}(b.Normalized()));
}
