## 1 概述

为了提高开发效率，保证兼容性、扩展性，降低设备与设备、设备与云之间的对接成本，制定本规范。

在未做特殊说明的情况下，字节序为大端序。

本规范涉及分帧、通讯安全、数据负载格式、数据负载规范四部分。

### 1.1 分帧

在通讯的双方建立的通讯管道为字节流的情况下，比如TCP、串口、蓝牙，需要设计分帧，分帧方案初定为：

**固定帧头（2字节，待定）+ 数据载荷长度（2字节）+ 数据载荷（N字节） + 校验和（2字节 CRC16，具体算法待定）**

在非字节流的情况下，比如UDP，不需要设计分帧。

在基于MQ的通讯方式下，比如MQTT，不需要设计分帧。

### 1.2 通讯安全

视不同的通讯方式以及不同的设备计算能力，采取不同等级的加密方式：

- A：明文
- B：使用AES-128-CBC对数据负载做加密，使用固定密钥，设备密钥需在生产时写入
- C1：使用AES-128-CBC对数据负载做加密，使用固定密钥握手，协商产生会话密钥，会话期间使用会话密钥做加密
- C2：使用AES-128-CBC对数据负载做加密，使用基于时间戳的滚动密钥，对双方的计算能力要求较高
- C3：使用AES-256-GCM对数据负载做加密
- D：使用TLS（双向证书认证）
- E：使用独立的安全芯片

### 1.3 数据负载格式

数据载荷格式，原则上统一使用BKV。

BKV是一种二进制的键值对协议，可以简单理解成`[{K:V},{K:V}...]`。BKV由多个KV拼接而成，每个KV由四部分组成：总长度、Key长度、Key值、Value值。总长度和Key长度确定了Key和Value的边界，Key有两种类型，string和number，Value为字节数组（Byte Array）。

比如将四个属性封装成BKV：
| Key | Value  |
| --- | ------------------------ |
| 2   | 48656C6C6F2C20776F726C64 |
| 2   | 030405 |
| dd  | 303132 |
| 99  | 030405 |

会得到`0E010248656C6C6F2C20776F726C64 050102030405 06826464303132 050163030405`

如图所示：

![](file:////Users/zhousijia/Library/Containers/com.kingsoft.wpsoffice.mac/Data/tmp/wps-zhousijia/ksohtml//wps3.jpg) 

网页工具解析如下：

![](file:////Users/zhousijia/Library/Containers/com.kingsoft.wpsoffice.mac/Data/tmp/wps-zhousijia/ksohtml//wps4.jpg) 

**一般协议，本质上是Value值的组装。BKV协议会有一定的冗余信息（带有额外的长度、Key信息），在不使用BKV库的情况下，没有额外的计算损耗，本质上是一种牺牲部分空间获得时间（便利性、扩展性）的协议。**

在设备对内存要求极端苛刻的情况下，可以不使用BKV，使用自定协议来节省内存。

BKV协议的好处在于：

 - 不需要单独定义帧格式，客户端封装和服务端解析都会极其方便，而且数据值的位置可以随意组合，兼容性极佳，类似JSON
 - 额外开销小，对于每一个键值对，相较json而言，最小只有3个字节的额外消耗，而json至少需要5个字节，在获得灵活性的同时，又获得了较大的编码效率
 - BKV没有指定多级的内嵌对象的方式，这样极易于encode和decode，例如C语言的BKV库只有几百行代码，时间和空间开销极小，在不使用库的情况下，没有计算时间开销
 - BKV支持C/JAVA/GO/JS等主流语言，有独立的解析工具辅助解析，提升开发和调试效率

### 1.4 数据负载规范

原则上每个数据负载必须携带流水号信息，一般有两种通讯方式：

 -  客户端发送Request，等待服务端返回Response
 -  客户端周期性或突发性发送Report

流水号信息即Request ID概念，返回的Response也携带Request ID，使一组Request与Response对应。这样使得信道中可以存在多组Request-Response，并且请求端可以根据ID来做精准响应匹配。这种机制也类似于TCP的Sequence与ACK Number机制。

大量的协议设计时未考虑此机制，故在此做要求。

### 1.5 CRC16

```C++
uint16_t const crc16_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

static inline uint16_t crc16_byte(uint16_t crc, const uint8_t data)
{
    return (crc >> 8) ^ crc16_table[(crc ^ data) & 0xff];
}

uint16_t crc16(uint8_t const *buffer, size_t len)
{
    uint16_t crc = 0;
    while (len--)
        crc = crc16_byte(crc, *buffer++);
    return crc;
}
```



### 1.6 相关SDK与工具

BKV SDK：[https://github.com/sinfere/bkv](https://github.com/sinfere/bkv)

BKV封包与解包测试工具：[http://bkv-js-tool.sinfere.com/](http://bkv-js-tool.sinfere.com/)

## 2 协议制定举例

### 2.1 车辆与服务端通讯协议

### 2.1.1 KV总表

KEY均为整数。
| KEY描述                    | KEY值 | VALUE类型   | VALUE描述                                                                                                                              |
| ------------------------ | ---- | --------- | ------------------------------------------------------------------------------------------------------------------------------------ |
| 基本                       |      |           |                                                                                                                                      |
| type                     | 0x01 | uint8     | bit7: 0-上行，1-下行<br><br>bit4~bit0:<br><br>1=登录帧，<br><br>2=车辆状态上报，<br><br>3=GPS定位<br><br>4=报警<br><br>5=检测<br><br>6=BMS信息<br><br>7=远程控制 |
| time                     | 0x02 | uint64    | 时间戳                                                                                                                                  |
| packet_id                | 0x03 | uint32    | 包流水号                                                                                                                                 |
| 模块信息                     |      |           |                                                                                                                                      |
| vcu_version              | 0x11 | uint64    |                                                                                                                                      |
| iccid                    | 0x12 | bytes(20) |                                                                                                                                      |
| bms_version              | 0x13 | bytes(28) |                                                                                                                                      |
| 位置信息                     |      |           |                                                                                                                                      |
| gps_status               | 0x21 | uint8     | 定位状态<br><br>1=有效，0=无效                                                                                                                |
| gps_signal_strength      | 0x22 |           |                                                                                                                                      |
| location_status          | 0x23 | uint8     | 位置状态                                                                                                                                 |
| location_navigate        | 0x24 | uint8     | 位置朝向<br><br>bit0:1=北半球，0=南半球<br><br>bit1：1=东半球，0=西半球                                                                                 |
| location_longitude       | 0x25 | uint32    | 经度，放大1000000倍上报                                                                                                                      |
| location_latitude        | 0x26 | uint32    | 纬度，放大1000000倍上报                                                                                                                      |
| location_speed           | 0x27 | uint16    | 速度，放大100倍上报                                                                                                                          |
| location_angle           | 0x28 | uint16    | 角度，放大100倍上报                                                                                                                          |
| location_satellite_count | 0x29 | uint8     | 卫星数                                                                                                                                  |
| location_time            | 0x30 | bytes(6)  | 2019/08/28 12:34:23(年取后两位 19) ->0x13081c0c2217                                                                                       |
| location_report_type     | 0x31 | uint8     | 位置上报类型                                                                                                                               |

#### 2.1.2 登录数据包(type=0x01)包含的键值对：
| KEY | KEY值 |
| ----------- | ---- |
| type   | 0x01 |
| time   | 0x02 |
| packet_id   | 0x03 |
| vcu_version | 0x11 |
| iccid  | 0x12 |
| bms_version | 0x13 |

### 2.1.3 位置上报数据包(type=0x03)包含的键值对：
| KEY    | KEY值 |
| ------------------------ | ---- |
| type   | 0x01 |
| time   | 0x02 |
| packet_id   | 0x03 |
| gps_status  | 0x21 |
| gps_signal_strength | 0x22 |
| location_status  | 0x23 |
| location_navigate   | 0x24 |
| location_longitude  | 0x25 |
| location_latitude   | 0x26 |
| location_speed   | 0x27 |
| location_angle   | 0x28 |
| location_satellite_count | 0x29 |
| location_time  | 0x30 |
| location_report_type  | 0x31 |
