# 协议 DSL 设计文档

GatewayForge 通过 YAML 描述自研二进制协议。

## 基础结构

```yaml
name: protocol_name
transport: tcp|udp
endian: big|little

framing:
  type: length_field|datagram
  length_field: field_name    # for length_field type
  header_len: N               # for length_field type
  length_includes_header: bool

message:
  - name: field_name
    type: u8|u16|u32|u64|i8|i16|i32|i64|bytes|string
    const: <value>            # optional constant value

payload:
  length_from: field_name

analysis:
  sequence_field: field_name
  timestamp_field: field_name
  timestamp_unit: ms|us|ns
  device_id_field: field_name
```

## MVP 支持的字段类型

- u8, u16, u32, u64 — 无符号整数
- i8, i16, i32, i64 — 有符号整数
- bytes — 原始字节
- string — 字符串

## 示例

见 `examples/schemas/vehicle-tcp.yaml` 和 `examples/schemas/vehicle-udp.yaml`
