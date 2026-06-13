#include <gtest/gtest.h>
#include "gatewayforge/replay/ReplayOptions.h"

using namespace gatewayforge;

TEST(ReplayOptionsTest, DefaultValues) {
    ReplayOptions options;
    EXPECT_DOUBLE_EQ(options.speed, 1.0);
    EXPECT_FALSE(options.randomize_source_port);
}
