# stm32f103zet6

作为主机接收从机发送过来的数据，并连接有oled显示屏，显示温度、湿度、光照强度、烟雾浓度。并设置有报警装置，当超过一定的阈值后触发蜂鸣器报警。

```shell


主机接线图：
oled：
VCC---VCC
GND---GND
SCL---PB6
SDA---PB7

HC-08:
VCC---VCC
GND---GND
TXD---RX2
RXD---TX2

ESP32:
VIN---VCC
GND---GND
TX2---RX3
RX2---TX3

蜂鸣器：PB8

从机：
GY-302:
VCC---VCC
GND---GND
SCL---PB6
SDA---PB7
ADDR---GND

DHT11:
VCC---VCC
GND---GND
DAT---PA1

HC-08:
VCC---VCC
GND---GND
RXD---TX1
TXD---RX1

烟雾传感器：
VCC---VCC
GND---GND
D0不接
A0---A0


```



# stm32c8t6

作为从机接收主机采集的信息



# ESP32

- 连接阿里云MQTT通讯，并将采集到的数据上传到阿里云，在物模型中实时显示出来

- 使用vscode打开，用platformIO IDE插件，还需要安装PubSUbClient库，依赖PubSubClient，在使用前，请务必修改PubSubClient的连接参数，否则无法使用，具体修改如下：
- PubSubClient中的MQTT_MAX_PACKET_SIZE修改为1024
- PubSubClient中的MQTT_KEEPALIVE修改为60
- 掉线后会一直尝试重新连接，可能会触发阿里云的一些限流规则，并且会导致挤掉其他同设备 ID 的设备
- 默认 5000ms 检测一次连接状态，可以通过 CHECK_INTERVAL 修改此值
