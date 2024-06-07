#ifndef __dht11_h
#define __dht11_h

#include "main.h"

void DHT11_REST(void); //��λDHT11
void DHT11_OUT(void);
void DHT11_IN(void );
uint8_t DHT11_Check(void); //DHT11״̬����
uint8_t DHT11_Read_Bit(void); //��DHT11һλ����
uint8_t DHT11_Read_Byte(void); //��DHT11һ�ֽ�����
uint8_t DHT11_Read_Data(uint8_t* humi,uint8_t* temp); //DHT11������ʾ

#endif /*__dht11_h*/
