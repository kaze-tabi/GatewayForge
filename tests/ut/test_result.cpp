/**
 * @file    tests/ut/test_result.cpp
 * @brief   Result<T> 单元测试 (6 用例)
 * @module  common
 * @level   Unit Test
 *
 * 覆盖场景:
 *   - Ok / Err 构造与状态
 *   - Unwrap: Ok返回 / Err抛异常
 *   - UnwrapOr: Ok返回原值 / Err返回默认值
 *
 * 对应测试计划: UT-RES-001 ~ UT-RES-006
 */

#include <gtest/gtest.h>
#include "gatewayforge/common/Result.h"

using namespace gatewayforge;

/** @test UT-RES-001: Ok — IsOk true, Value 正确 */
TEST(ResultTest, Ok_IsOk_True) {
    auto r = Result<int>::Ok(42);
    EXPECT_TRUE(r.IsOk());
    EXPECT_FALSE(r.IsErr());
    EXPECT_EQ(r.Value(), 42);
}

/** @test UT-RES-002: Err — IsErr true, Error 正确 */
TEST(ResultTest, Err_IsErr_True) {
    auto r = Result<int>::Err("something went wrong");
    EXPECT_TRUE(r.IsErr());
    EXPECT_FALSE(r.IsOk());
    EXPECT_EQ(r.Error(), "something went wrong");
}

/** @test UT-RES-003: Unwrap Ok — 返回内部值 */
TEST(ResultTest, Unwrap_Ok_ReturnsValue) {
    auto r = Result<std::string>::Ok(std::string("hello"));
    EXPECT_EQ(r.Unwrap(), "hello");
}

/** @test UT-RES-004: Unwrap Err — 抛出 runtime_error */
TEST(ResultTest, Unwrap_Err_Throws) {
    auto r = Result<int>::Err("error message");
    EXPECT_THROW(r.Unwrap(), std::runtime_error);
    try {
        r.Unwrap();
    } catch (const std::runtime_error& e) {
        EXPECT_STREQ(e.what(), "error message");
    }
}

/** @test UT-RES-005: UnwrapOr Ok — 返回原值 */
TEST(ResultTest, UnwrapOr_Ok_ReturnsValue) {
    auto r = Result<int>::Ok(10);
    EXPECT_EQ(r.UnwrapOr(0), 10);
}

/** @test UT-RES-006: UnwrapOr Err — 返回默认值 */
TEST(ResultTest, UnwrapOr_Err_ReturnsDefault) {
    auto r = Result<int>::Err("fail");
    EXPECT_EQ(r.UnwrapOr(5), 5);
}
