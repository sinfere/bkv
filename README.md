# BKV / [中文介绍](README-cn.md)
Binary key-value tuples protocol, c implementation, no malloc version for embedded device !!!

## 1 Protocol
`bkv = kv + kv + kv + ...`  
`kv = length + [key length] + key + value`  
`length = len(key length + key + value)`   
`[key length] = len(key)`   

#### 1.1 Length
`length = length(key + value), vary bytes`  
Length byte uses first bit for stopping bit, indicating whether length ends, 0 stands for ending, 1 stands for continuing. 7 bits are used for actual value    
For example, if length is small than 128, one byte is enough; if length is larger than 128, using multiple bytes, the first high bit of every byte is 1, the first high bit of last byte is 0.
```
2 -> 0x02
666 -> 0x851A
88888888 -> 0xAAB1AC38
```

#### 1.2 Key Length
`Key length` use 1 byte, the first bit stands for key type, 0 stands for number key, 1 stands for strings, thus max string `key length` is 128

#### 1.3 Key
Key is either string or number, BKV will use as less bytes to stands for number as possible, for example, if number is u_int64_t 3, it will be encoded to byte 0x03

#### 1.4 Value
Value is just raw bytes, there is no type info for value, the parsing work is left for application level according to key. when we define a key, we should know the responding value bytes layout

## 2 Example
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
- **no need to parse custom frame for iot device any more**  
  for every new device to integerate, there is no need to define new frame layout, just define args, like rpc
- **small overhead**  
  normally, for each kv, there is only two bytes overhead, while json has 5 or more, and hard to parse; it's good for iot or rpc protocol
- **light weight**  
  there is no nested object in bkv, bkv is just a list of kvs, so it's very easy to pack and unpack, so is implementation 
- **more "reasonable"**  
  in RPC scenario, bkv can be used as function name + function params, it looks reasonably
