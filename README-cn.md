# BKV协议
BKV是一种二进制的键值对协议，可以简单理解成[{k:v},{k:v}...]



## 1 协议格式
`bkv = kv + kv + kv + ...`  
`kv = length + [key length] + key + value`  
`length = len(key length + key + value)`   
`[key length] = len(key)`   

BKV由多个KV拼接而成，每个KV由四部分组成：总长度、Key长度、Key值、Value值。总长度和Key长度确定了Key和Value的边界，Key有两种类型，string和number，Value为字节数组

#### 1.1 Length
`length = length(key + value), vary bytes`  

长度字节用每个字节的bit0作为停止位，如果为1则表示下一个字节仍然表示长度，如果为0，则表示终止；例如，如果长度小于128，那么用一个字节就可以表示完整，如果长度大于128，那么用多个字节表示，多个字节中除了最后一个字节的bit0为0，其它字节的bit0均为1。

```
2 -> 0x02
666 -> 0x851A
88888888 -> 0xAAB1AC38
```

#### 1.2 Key Length
键长使用1字节，bit0表示键类型，1表示string，0表示number

#### 1.3 Key
键根据键类型来解析。注意，如果是number型，BKV会对number进行压缩，例如3(uint64_t)会被压缩成一个字节的0x03

#### 1.4 Value
值即为字节数组。

## 2 举例
```c
char *string = "Hello, world";
uint8_t data[60];
memset(data, 0, 60);

uint8_t key[1] = {2};
uint8_t value[3] = {3, 4, 5};

int offset = 0;
offset += bkv_append(data + offset, 60 - offset, key, 1, 0, (uint8_t* )string, strlen(string));
offset += bkv_append(data + offset, 60 - offset, key, 1, 0, value, 3);

uint8_t value_string[3] = {0x30, 0x31, 0x32};
offset += bkv_append_by_string_key(data + offset, 60 - offset, "dd", value_string, 3);

// robust check
int apend_offset = bkv_append_by_number_key(data + offset, 60 - offset, 99, value, 3);
if (apend_offset == -1) {
    LOGE("append fail");
    return;
} else {
    offset += apend_offset;
}
// bkv_dump_buf("encode", data, pos);

int count = bkv_get_count(data, offset);
LOGI("bkv count: %d", count);

// see bkv_dump for iteration
bkv_dump(data, offset);

int contains_number_key = bkv_contains_number_key(data, offset, 2);
LOGI("contains_number_key: %s", contains_number_key ? "true" : "false");
int value_pos_begin = 0;
int value_pos_end = 0;
int result_code = bkv_get_value_by_number_key(data, offset, 2, &value_pos_begin, &value_pos_end);
if (result_code == 0) {
    LOGI("bkv_get_value_by_number_key result: %d", result_code);
    bkv_dump_buf("value", data + value_pos_begin, value_pos_end - value_pos_begin);
}

int contains_string_key = bkv_contains_string_key(data, offset, "dd");
LOGI("contains_string_key: %s", contains_string_key ? "true" : "false");
result_code = bkv_get_value_by_string_key(data, offset, "dd", &value_pos_begin, &value_pos_end);
if (result_code == 0) {
    LOGI("bkv_get_value_by_string_key result: %d", result_code);
    bkv_dump_buf("value", data + value_pos_begin, value_pos_end - value_pos_begin);
}   
```

Example output:
```shell
2019-01-21 18:09:37 INFO: bkv count: 4
4 kv for dump [34]: 0E010248656C6C6F2C20776F726C6405010203040506826464303132050163030405
key-0[n]:               2 -> value[12]: 48656C6C6F2C20776F726C64 (s: Hello, world)
key-1[n]:               2 -> value[3]: 030405
key-2[s]:              dd -> value[3]: 303132 (s: 012)
key-3[n]:              99 -> value[3]: 030405

2019-01-21 18:09:37 INFO: contains_number_key: true
2019-01-21 18:09:37 INFO: bkv_get_value_by_number_key result: 0
value[12]: 48656C6C6F2C20776F726C64 (s: Hello, world)
2019-01-21 18:09:37 INFO: contains_string_key: true
2019-01-21 18:09:37 INFO: bkv_get_value_by_string_key result: 0
value[3]: 303132 (s: 012)
```
Detail:
```
0E010248656C6C6F2C20776F726C6405010203040506826464303132050163030405

kv 0:
0E010248656C6C6F2C20776F726C64
  0E[len] 01[key len] 02[key] 48656C6C6F2C20776F726C64[value:'Hello, world']

kv 1:
050102030405
  05[len] 01[key len] 02[key] 030405[value]

kv 2:
06826464303132
  06[len] 82[key len: string flag + len 2] 6464[key:'dd'] 303132[value:'012']

kv 3:
050163030405
  05[len] 01[key len] 63[key] 030405[value]  
```

## 3 Why BKV
- 动态协议
  
  不需要确定的帧格式layout了，类似json  
  
- 额外开销小  
  
  对于每一个键值对，相较json而言，最小只有3个字节的额外消耗，而json至少需要5个字节，在获得灵活性的同时，又获得了较大的编码效率
  
- 轻量级  
  bkv没有指定多级的内嵌对象的方式，这样极易于encode和decode，例如c语言的BKV库只有几百行代码，时间和空间开销极小 
  
- 用途广
  BKV也可以适用于作为RPC的二进制协议

## 4 下一步计划
- 增加更多工具函数
- value作为子bkv增加更完善的支持
- 支持windows环境编译（mingw/msvc）