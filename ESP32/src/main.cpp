#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>  


/* 连接WIFI SSID和密码 */
#define WIFI_SSID         "HUAWEI"
#define WIFI_PASSWD       "123456789"

/* 设备的三元组信息*/
#define PRODUCT_KEY       "ikisIfcCOWL"
#define DEVICE_NAME       "Demo"
#define DEVICE_SECRET     "5a72dbc4d819e24143709a2dd0628a45"
#define REGION_ID         "cn-shanghai"


#define MQTT_SERVER       "iot-06z00a1z31yx71h.mqtt.iothub.aliyuncs.com"
#define MQTT_PORT          1883
#define MQTT_USRNAME      "Demo&ikisIfcCOWL"

#define CLIENT_ID         "ikisIfcCOWL.Demo|securemode=2,signmethod=hmacsha256,timestamp=1714986740259|"
#define MQTT_PASSWD       "d1eab4431ed22b522d82ca42be945c7aaf5a66320f823c0e9fee099d69a6b4c3"

//订阅主题与发布主题
const char SubTopic[]  =   "sys/ikisIfcCOWL/${deviceName}/thing/service/property/set" ;//订阅
const char PubTopic[]  =   "/sys/ikisIfcCOWL/Demo/thing/event/property/post" ;//发布

char param[100];
char jsonBuf[128];
unsigned long lastMs = 0;


WiFiClient espClient;
PubSubClient  client(espClient);

//测试向主题发送的消息

String payload = "{\"id\":\"Demo\",\"version\":\"1.0\",\"method\":\"thing.event.property.post\",\"params\":%s}";

//连接wifi
void wifiInit()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("WiFi not Connect");
    }
}

//mqtt连接
void mqttCheckConnect()
{
    while (!client.connected())
    {
        Serial.println("Connecting to MQTT Server ...");
        if(client.connect(CLIENT_ID, MQTT_USRNAME, MQTT_PASSWD))
        {
          Serial.println("MQTT Connected!");
        }
        else{
           Serial.print("MQTT Connect err:");
            Serial.println(client.state());
            delay(5000);
          }
        
    }
}

void setup()
{
    Serial2.begin(115200); // 设置串口2的波特率为115200
    wifiInit();
    client.setServer(MQTT_SERVER, MQTT_PORT); /* 连接MQTT服务器 */
}

void loop()
{
    /*每5秒钟检查一次MQTT连接状态*/
    if (millis() - lastMs >= 5000)
    {
        lastMs = millis();
        mqttCheckConnect();
    }
    client.loop(); // 维护MQTT客户端的活动


    if (Serial2.available()){
        String input = Serial2.readStringUntil('\n'); // 读取串口数据到 String
        sprintf(param, input.c_str());
        sprintf(jsonBuf, payload.c_str(), param);
        client.publish(PubTopic, jsonBuf);// 使用 c_str() 转换为 const char*
    }
        
    delay(5000);

 
}