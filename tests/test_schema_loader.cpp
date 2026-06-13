#include <gtest/gtest.h>
#include "gatewayforge/protocol/SchemaLoader.h"
#include <fstream>

using namespace gatewayforge;

TEST(SchemaLoaderTest, LoadNonExistentFile) {
    auto result = SchemaLoader::LoadFromYaml("nonexistent.yaml");
    EXPECT_TRUE(result.IsErr());
}

TEST(SchemaLoaderTest, FieldTypeConversion) {
    EXPECT_EQ(FieldTypeFromString("u8"), FieldType::U8);
    EXPECT_EQ(FieldTypeFromString("u16"), FieldType::U16);
    EXPECT_EQ(FieldTypeFromString("u32"), FieldType::U32);
    EXPECT_EQ(FieldTypeFromString("u64"), FieldType::U64);
    EXPECT_EQ(FieldTypeFromString("i8"), FieldType::I8);
    EXPECT_EQ(FieldTypeFromString("i16"), FieldType::I16);
    EXPECT_EQ(FieldTypeFromString("i32"), FieldType::I32);
    EXPECT_EQ(FieldTypeFromString("i64"), FieldType::I64);
    EXPECT_EQ(FieldTypeFromString("bytes"), FieldType::BYTES);
    EXPECT_EQ(FieldTypeFromString("string"), FieldType::STRING);
}

TEST(SchemaLoaderTest, UnknownFieldType) {
    EXPECT_THROW(FieldTypeFromString("unknown"), std::runtime_error);
}
