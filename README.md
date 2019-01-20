# BKV
Binary key-value tuples protocol, c implementation

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
Pack:
```c
bkv* tb = bkv_new();

u_int8_t value[2] = {2, 3};
bkv_add_by_number_key(tb, 1, buffer_new(value, 2));
bkv_add_by_string_key(tb, "version", buffer_new_from_number(515));

bkv_add_by_string_key(tb, "test", buffer_new_from_string("hello"));

buffer* b = bkv_pack(tb);
```

Unpack:
```c
bkv_unpack_result* r = bkv_unpack(b->buf, b->size);
if (r->code != 0) {
    printf("unpack fail");
    return;
}
char* test = bkv_get_string_value_from_string_key(r->bkv, "test");
u_int64_t version = bkv_get_number_value_from_string_key(r->bkv, "version");
```

Example output:
```shell
pack result:[27]: 04010102030A8776657273696F6E02030A847465737468656C6C6F

unpack result code:            0
unpack kv size:                3

key[n]:               1 -> value[2]: 0203
key[s]:         version -> value[2]: 0203
key[s]:            test -> value[5]: 68656C6C6F (s: hello)
```
Detail:
```
04010102030A8776657273696F6E02030A847465737468656C6C6F

kv 1:
0401010203
  04[len] 01[key len] 01[key] 0203[value]

kv 2:
0A8776657273696F6E0203
  0A[len] 87[key len: string flag + len 7] + 76657273696F6E[key:'version'] 0203[value]

kv 3:
0A847465737468656C6C6F
  0A[len] 84[key len: string flag + len 4] + 74657374[key:'test'] 68656C6C6F[value:'hello']
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
