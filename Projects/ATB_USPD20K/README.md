# USPD20K

## Protocol
MCU emulates I2C EEPROM memory with address 0x50 (see AT24C512C)

## Memory mapping
### 0x0000 - 0x07FF (read only)
From address 0x0000 till 0x07FF holds ADC conversions table

|   32b   |   32b   |   32b   |   32b   |   32b   |   32b   |    32b    |    32b     | 
| ------- | ------- | ------- | ------- | ------- | ------- | --------- | ---------- |
| Port2: ADC CH1 | Port3: ADC CH2 | Port4: ADC CH3 | AVDD: ADC CH4 | 5V: ADC CH5 | Port1: ADC CH6 | Timestamp(ms) | Validation: 0xCAFEFEED|

First 6 32-bits double words holds raw ADC values converted for each corresponding channels. Next is timestamp when conversion was completed for the last channel. And validation number shuld have 0xCAFEFEED value, in other case row is invalid. Raw ADC value equals to 0xFFFFFFFF also invalid. Table consists of 64 rows.

### 0x1000 - 0x100B (read), 0x1004 - 0x100B (read/write)
From address 0x1000 till 0x100B holds analog ports input circuit configuration.

|    Mnemonic      | Offset | Width | Access |          Description            |
| ---------------  | ------ | ----- | ------ | ------------------------------- |
| ACTIVE_CNF_PORT1 |      0 |    8b | R  | Current circuit configuration for ain 1 |
| ACTIVE_CNF_PORT2 |      1 |    8b | R  | Current circuit configuration for ain 2 |
| ACTIVE_CNF_PORT3 |      2 |    8b | R  | Current circuit configuration for ain 3 |
| ACTIVE_CNF_PORT4 |      3 |    8b | R  | Current circuit configuration for ain 4 |
| USER_CNF_PORT1   |      4 |    8b | RW | New user circuit configuration for ain 1 |
| USER_CNF_PORT2   |      5 |    8b | RW | New user circuit configuration for ain 2 |
| USER_CNF_PORT3   |      6 |    8b | RW | New user circuit configuration for ain 3 |
| USER_CNF_PORT4   |      7 |    8b | RW | New user circuit configuration for ain 4 |
| SYNC_KEY         |      8 |   32b | RW | Write 0x12345678 to synchronize user config, resets to 0 when finish |

#### ACTIVE_CNF_PORT/USER_CNF_PORT bit map

| 7  | 6  |   5   |    4    |   3    |      2      |      1       |   0    | 
| -- | -- | ----- | ------- | ------ | ----------- | ------------ | ------ |
| NA | NA | 1K_PD | 150R_PD | 4K3_PD | CUR_SRC_LOW | CUR_SRC_HIGH | NTC_PU |
(see schematic)

### 0x1100 - 0x113B (read), 0x111C - 0x113B (read/write)
|    Mnemonic        | Offset | Width | Access |          Description            |
| ------------------ | ------ | ----- | ------ | ------------------------------- |
| ACTIVE_ADC_CNF_CH1 |      0 |   16b | R  | Current ADC config register value for CH1 |
| ACTIVE_ADC_OPT_CH1 |      2 |    8b | R  | Current ADC options for CH1 |
| Reserved           |      3 |    8b | R  |  |
| ACTIVE_ADC_CNF_CH2 |      4 |   16b | R  | (see ACTIVE_ADC_CNF_CH1) |
| ACTIVE_ADC_OPT_CH2 |      6 |    8b | R  | (see ACTIVE_ADC_OPT_CH1) |
| Reserved           |      7 |    8b | R  |  |
| ACTIVE_ADC_CNF_CH3 |      8 |   16b | R  | (see ACTIVE_ADC_CNF_CH1) |
| ACTIVE_ADC_OPT_CH3 |     10 |    8b | R  | (see ACTIVE_ADC_OPT_CH1) |
| Reserved           |     11 |    8b | R  |  |
| ACTIVE_ADC_CNF_CH4 |     12 |   16b | R  | (see ACTIVE_ADC_CNF_CH1) |
| ACTIVE_ADC_OPT_CH4 |     14 |    8b | R  | (see ACTIVE_ADC_OPT_CH1) |
| Reserved           |     15 |    8b | R  |  |
| ACTIVE_ADC_CNF_CH5 |     16 |   16b | R  | (see ACTIVE_ADC_CNF_CH1) |
| ACTIVE_ADC_OPT_CH5 |     18 |    8b | R  | (see ACTIVE_ADC_OPT_CH1) |
| Reserved           |     19 |    8b | R  |  |
| ACTIVE_ADC_CNF_CH6 |     20 |   16b | R  | (see ACTIVE_ADC_CNF_CH1) |
| ACTIVE_ADC_OPT_CH6 |     22 |    8b | R  | (see ACTIVE_ADC_OPT_CH1) |
| Reserved           |     23 |    8b | R  |  |
| ACTIVE_ADC_IO      |     24 |    8b | R  | Current ADC IO register value |
| Reserved           |     25 |    8b | R  |  |
| ACTIVE_ADC_MODE    |     26 |   16b | R  | Current ADC Mode register value |
| USER_ADC_CNF_CH1   |     28 |   16b | RW | New user  ADC config register value for CH1 |
| USER_ADC_OPT_CH1   |     30 |    8b | RW | New user  ADC options for CH1 |
| Reserved           |     31 |    8b | RW |  |
| USER_ADC_CNF_CH2   |     32 |   16b | RW | (see USER_ADC_CNF_CH1) |
| USER_ADC_OPT_CH2   |     34 |    8b | RW | (see USER_ADC_OPT_CH1) |
| Reserved           |     35 |    8b | RW |  |
| USER_ADC_CNF_CH3   |     36 |   16b | RW | (see USER_ADC_CNF_CH1) |
| USER_ADC_OPT_CH3   |     38 |    8b | RW | (see USER_ADC_OPT_CH1) |
| Reserved           |     39 |    8b | RW |  |
| USER_ADC_CNF_CH4   |     40 |   16b | RW | (see USER_ADC_CNF_CH1) |
| USER_ADC_OPT_CH4   |     42 |    8b | RW | (see USER_ADC_OPT_CH1) |
| Reserved           |     43 |    8b | RW |  |
| USER_ADC_CNF_CH5   |     44 |   16b | RW | (see USER_ADC_CNF_CH1) |
| USER_ADC_OPT_CH5   |     46 |    8b | RW | (see USER_ADC_OPT_CH1) |
| Reserved           |     47 |    8b | RW |  |
| USER_ADC_CNF_CH6   |     48 |   16b | RW | (see USER_ADC_CNF_CH1) |
| USER_ADC_OPT_CH6   |     50 |    8b | RW | (see USER_ADC_OPT_CH1) |
| Reserved           |     51 |    8b | RW |  |
| USER_ADC_IO        |     52 |    8b | RW | Current ADC IO register value |
| Reserved           |     53 |    8b | RW |  |
| USER_ADC_MODE      |     54 |   16b | RW | Current ADC Mode register value |
| SYNC_KEY           |     56 |   32b | RW | Write 0xABCDEFAB to synchronize user config, resets to 0 when finish |

#### ACTIVE_ADC_CNF_CH/USER_ADC_CNF_CH bit map
(see AD7794/AD7795 CONFIGURATION REGISTER)

#### ACTIVE_ADC_OPT_CH/USER_ADC_OPT_CH bit map
| 7  | 6  | 5  | 4  | 3  |   2    |   1    |   0    | 
| -- | -- | -- | -- | -- | ------ | ------ | ------ |
| NA | NA | NA | NA | NA | CAL_FS | CAL_ZS | ENABLE |

ENABLE - enable channel

CAL_ZS - enable zero scale calibration

CAL_FS - enable full scale calibration

#### ACTIVE_ADC_IO/USER_ADC_IO bit map
(see AD7794/AD7795 IO REGISTER)

#### ACTIVE_ADC_MODE/USER_ADC_MODE bit map
(see AD7794/AD7795 MODE REGISTER)

## Interaction example
#### Check if driver up:
```
# modprobe i2c-dev
# i2cdetect -y 4
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:                         -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: UU -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
```

#### Read all ADC records:
```
# for i in {0..63};do echo -n "$((i+1));" && dd if=/sys/bus/i2c/devices/4-0050/eeprom bs=1 count=32 skip=$((i*32)) status=none | hexdump -v -e '1/4 "%u;"' && echo "";done

1;235;213;205;9258480;13960969;205;5345343;3405709037;
2;210;193;199;9258732;13960975;193;5345599;3405709037;
3;204;197;199;9258683;13960928;206;5345855;3405709037;
4;215;177;221;9258607;13960970;214;5346112;3405709037;
5;207;200;206;9258250;13960904;200;5346368;3405709037;
6;202;166;207;9258489;13960950;228;5346624;3405709037;
7;216;179;206;9258559;13960912;192;5346880;3405709037;
8;191;223;181;9258371;13960962;204;5347136;3405709037;
9;188;200;179;9258529;13960863;209;5347393;3405709037;
...
```

#### Dump circuit configuration:
```
# dd if=/sys/bus/i2c/devices/4-0050/eeprom bs=1 count=12 skip=4096 | hexdump -C -v
00000000  08 08 08 08 08 08 08 08  00 00 00 00              |............|
0000000c
12+0 records in
12+0 records out
12 bytes copied, 0.00731899 s, 1.6 kB/s
```
#### Dump ADC configuration:
```
# dd if=/sys/bus/i2c/devices/4-0050/eeprom bs=1 count=60 skip=4352 | hexdump -C -v
00000000  10 10 07 00 11 10 07 00  12 10 07 00 13 10 07 00  |................|
00000010  14 10 07 00 15 10 07 00  00 00 05 60 10 10 07 00  |...........`....|
00000020  11 10 07 00 12 10 07 00  13 10 07 00 14 10 07 00  |................|
00000030  15 10 07 00 00 00 05 60  00 00 00 00              |.......`....|
0000003c
60+0 records in
60+0 records out
60 bytes copied, 0.0367524 s, 1.6 kB/s
```

#### Set new circuit configuration:
```
# echo -en \\x16\\x26\\x36\\x30\\x78\\x56\\x34\\x12 | dd of=/sys/bus/i2c/devices/4-0050/eeprom bs=1 count=8 seek=4100
```

#### Set new ADC configuration:
```
# echo -en \\x10\\x10\\x07\\x00\\x11\\x10\\x07\\x00\\x12\\x10\\x00\\x00\\x13\\x10\\x00\\x00\\x14\\x10\\x00\\x00\\x15\\x10\\x00\\x00\\x00\\x00\\x05\\x60\\xAB\\xEF\\xCD\\xAB | dd of=/sys/bus/i2c/devices/4-0050/eeprom bs=1 count=32 seek=4380
```

#### Set new ADC preset configuration:
```
# echo -en \\x04\\x04\\x00\\x00\\x21\\x43\\x65\\x87 | dd of=/sys/bus/i2c/devices/4-0050/eeprom bs=1 count=8 seek=4608
```

dd if=/sys/bus/i2c/devices/4-0050/eeprom bs=1 count=8 skip=4608 | hexdump -C -v