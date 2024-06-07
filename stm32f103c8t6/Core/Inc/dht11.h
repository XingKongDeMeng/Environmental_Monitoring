#ifndef __dht11_h
#define __dht11_h

#include "main.h"

void DHT11_REST(void); //复位DHT11
void DHT11_OUT(void);
void DHT11_IN(void );
uint8_t DHT11_Check(void); //DHT11状态反馈
uint8_t DHT11_Read_Bit(void); //读DHT11一位数据
uint8_t DHT11_Read_Byte(void); //读DHT11一字节数据
uint8_t DHT11_Read_Data(uint8_t* humi,uint8_t* temp); //DHT11数据显示

#endif /*__dht11_h*/
