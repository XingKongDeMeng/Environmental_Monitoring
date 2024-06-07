#include "main.h"

void DHT11_REST(void ) //主机发出起始信号
{
	DHT11_OUT();
	HAL_GPIO_WritePin (GPIOA ,GPIO_PIN_1 ,GPIO_PIN_RESET );
	HAL_Delay (20);
	HAL_GPIO_WritePin (GPIOA ,GPIO_PIN_1 ,GPIO_PIN_SET );
	delay_us (30);
}

//gpio端口为输入模式时,配置为上拉输入或者浮空输入,因外接上拉电阻,默认为高电平
//当有负信号输入时,gpio端口为1.
//若有负信号输入，当信号引脚连接到GND或者其他低电平信号时，GPIO端口会检测到低电平并显示为0。
//这是因为负信号的优先级高于上拉电阻的电平设置。


uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;

    DHT11_IN();  // 设置数据引脚为输入模式

    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 1 && retry < 80)
    {
        retry++;
        delay_us(1);  // 延迟1微秒
    }

    if (retry >= 80)
        return 1;  // 如果在规定时间内引脚仍为高电平，表示传感器未响应，返回错误代码 1
    else
        retry = 0;

    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 0 && retry < 80)
    {
        retry++;
        delay_us(1);  // 延迟1微秒
    }

    if (retry >= 80)
        return 1;  // 如果在规定时间内引脚仍为低电平，表示传感器未响应，返回错误代码 1

    return 0;  // 传感器响应正常，返回成功代码 0
}
//读取一个位,参考高低电平的时序
uint8_t DHT11_Read_Bit(void)
{
	uint8_t retry=0;
	while((HAL_GPIO_ReadPin (GPIOA ,GPIO_PIN_1)==1) && (retry <100))
	{
		retry ++;
		delay_us (1);
	}
	retry=0;
	while((HAL_GPIO_ReadPin(GPIOA ,GPIO_PIN_1)==0) && (retry <100))
	{
		retry ++;
		delay_us (1);
	}
	delay_us (40);
	if(HAL_GPIO_ReadPin (GPIOA ,GPIO_PIN_1 )==1)
		return 1;// 返回读取到的位为高电平
	else
		return 0;// 返回读取到的位为低电平
}

//读取一个字节,接收数据
uint8_t DHT11_Read_Byte(void)
{
	uint8_t dat=0;
	for(uint8_t i=0;i<8;i++)
	{
		dat <<= 1;
		dat |= DHT11_Read_Bit();
	}
	return dat;

}
//更据上面的数据格式,解码数据
uint8_t DHT11_Read_Data(uint8_t* humi, uint8_t* temp)
{
    uint8_t buf[5];  // 存储读取到的5个字节数据的缓冲区

    DHT11_REST();  // 初始化传感器通信

    if (DHT11_Check() == 0)  // 检查传感器是否正常响应
    {
        for (uint8_t i = 0; i < 5; i++)
            buf[i] = DHT11_Read_Byte();  // 逐个字节读取传感器发送的数据

        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])  // 校验数据的准确性
        {
            *humi = buf[0];  // 将湿度值存储到指定的变量中
            *temp = buf[2];  // 将温度值存储到指定的变量中
        }
    }
    else
    {
        return 1;  // 传感器响应异常，返回错误代码
    }

    return 0;  // 读取数据成功，返回正常代码
}

