/**
 * @file    tests/ut/test_buffer.cpp
 * @brief   Buffer 类单元测试 (16 用例)
 * @module  common
 * @level   Unit Test
 *
 * 覆盖场景:
 *   - 构造: 默认 / 数据 / 零大小 / 拷贝 / 移动
 *   - 访问: operator[] 正常 / 越界
 *   - 操作: Slice / Read<T>
 *   - 边界: 大内存 / 空指针 / 自赋值
 *
 * 对应测试计划: UT-BUF-001 ~ UT-BUF-016
 */

#include <gtest/gtest.h>
#include "gatewayforge/common/Buffer.h"

using namespace gatewayforge;

// ============================================================
// 构造
// ============================================================

/** @test UT-BUF-001: 默认构造 — Empty */
TEST(BufferTest, DefaultConstruct_Empty) {
    Buffer buf;
    EXPECT_EQ(buf.Size(), 0);
    EXPECT_TRUE(buf.Empty());
    EXPECT_EQ(buf.Data(), nullptr);
}

/** @test UT-BUF-002: 从数据构造 — 大小和内容正确 */
TEST(BufferTest, FromData_CorrectSize) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    Buffer buf(data, 5);
    EXPECT_EQ(buf.Size(), 5);
    EXPECT_FALSE(buf.Empty());
    EXPECT_EQ(buf[0], 0x01);
    EXPECT_EQ(buf[4], 0x05);
}

/** @test UT-BUF-003: 零大小构造 — 不崩溃 */
TEST(BufferTest, ZeroSizeConstruct) {
    Buffer buf(static_cast<size_t>(0));
    EXPECT_EQ(buf.Size(), 0);
}

/** @test UT-BUF-004: 拷贝构造 — 深拷贝, 地址不同 */
TEST(BufferTest, CopyConstruct_DeepCopy) {
    uint8_t data[] = {0xAA, 0xBB};
    Buffer a(data, 2);
    Buffer b(a);
    EXPECT_EQ(b.Size(), 2);
    EXPECT_EQ(b[0], 0xAA);
    EXPECT_NE(a.Data(), b.Data());
}

/** @test UT-BUF-005: 拷贝赋值 — 深拷贝, 地址不同 */
TEST(BufferTest, CopyAssignment_DeepCopy) {
    uint8_t data[] = {0x11, 0x22};
    Buffer a(data, 2);
    Buffer b = a;
    EXPECT_EQ(b.Size(), 2);
    EXPECT_EQ(b[0], 0x11);
    EXPECT_NE(a.Data(), b.Data());
}

/** @test UT-BUF-006: 移动构造 — 所有权转移 */
TEST(BufferTest, MoveConstruct) {
    uint8_t data[] = {0x01, 0x02, 0x03};
    Buffer a(data, 3);
    const uint8_t* old_ptr = a.Data();
    Buffer b(std::move(a));
    EXPECT_EQ(b.Size(), 3);
    EXPECT_EQ(b.Data(), old_ptr);
    EXPECT_TRUE(a.Empty());
}

/** @test UT-BUF-007: 移动赋值 — 所有权转移 */
TEST(BufferTest, MoveAssignment) {
    uint8_t data[] = {0x10, 0x20};
    Buffer a(data, 2);
    const uint8_t* old_ptr = a.Data();
    Buffer b;
    b = std::move(a);
    EXPECT_EQ(b.Size(), 2);
    EXPECT_EQ(b.Data(), old_ptr);
    EXPECT_TRUE(a.Empty());
}

// ============================================================
// 元素访问
// ============================================================

/** @test UT-BUF-008: operator[] — 正常访问 */
TEST(BufferTest, OperatorBracket_Valid) {
    uint8_t data[] = {0x10, 0x20, 0x30, 0x40};
    Buffer buf(data, 4);
    EXPECT_EQ(buf[0], 0x10);
    EXPECT_EQ(buf[2], 0x30);
    EXPECT_EQ(buf[3], 0x40);
}

/** @test UT-BUF-009: operator[] — 越界抛出 out_of_range */
TEST(BufferTest, OperatorBracket_OutOfRange) {
    uint8_t data[] = {0x01, 0x02};
    Buffer buf(data, 2);
    EXPECT_THROW(buf[2], std::out_of_range);
    EXPECT_THROW(buf[100], std::out_of_range);
}

// ============================================================
// 操作
// ============================================================

/** @test UT-BUF-010: Slice — 正常切片 */
TEST(BufferTest, Slice_Normal) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    Buffer buf(data, 6);
    Buffer sub = buf.Slice(2, 3);
    EXPECT_EQ(sub.Size(), 3);
    EXPECT_EQ(sub[0], 0x03);
    EXPECT_EQ(sub[1], 0x04);
    EXPECT_EQ(sub[2], 0x05);
}

/** @test UT-BUF-011: Slice — 越界抛出异常 */
TEST(BufferTest, Slice_OutOfRange) {
    uint8_t data[] = {0x01, 0x02, 0x03};
    Buffer buf(data, 3);
    EXPECT_THROW(buf.Slice(2, 5), std::out_of_range);
    EXPECT_THROW(buf.Slice(4, 1), std::out_of_range);
}

/** @test UT-BUF-012: Read<uint16_t> */
TEST(BufferTest, ReadU16_Normal) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    Buffer buf(data, 4);
    uint16_t val = buf.Read<uint16_t>(0);
    EXPECT_EQ(val, 0x0201); // little-endian
}

/** @test UT-BUF-013: Read — 越界抛出异常 */
TEST(BufferTest, Read_OutOfRange) {
    uint8_t data[] = {0x01, 0x02};
    Buffer buf(data, 2);
    EXPECT_THROW(buf.Read<uint64_t>(0), std::out_of_range);
}

// ============================================================
// 边界
// ============================================================

/** @test UT-BUF-014: 1MB 大 Buffer */
TEST(BufferTest, LargeBuffer) {
    Buffer buf(1 << 20);
    EXPECT_EQ(buf.Size(), 1 << 20);
}

/** @test UT-BUF-015: nullptr+0 构造 */
TEST(BufferTest, NullDataPtr) {
    Buffer buf(nullptr, 0);
    EXPECT_EQ(buf.Size(), 0);
}

/** @test UT-BUF-016: 自赋值安全 */
TEST(BufferTest, SelfAssignment) {
    uint8_t data[] = {0x01, 0x02};
    Buffer buf(data, 2);
    buf = buf;
    EXPECT_EQ(buf.Size(), 2);
    EXPECT_EQ(buf[0], 0x01);
}
