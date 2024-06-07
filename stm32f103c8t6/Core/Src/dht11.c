#include "main.h"

void DHT11_REST(void ) //����������ʼ�ź�
{
	DHT11_OUT();
	HAL_GPIO_WritePin (GPIOA ,GPIO_PIN_1 ,GPIO_PIN_RESET );
	HAL_Delay (20);
	HAL_GPIO_WritePin (GPIOA ,GPIO_PIN_1 ,GPIO_PIN_SET );
	delay_us (30);
}

//gpio�˿�Ϊ����ģʽʱ,����Ϊ����������߸�������,�������������,Ĭ��Ϊ�ߵ�ƽ
//���и��ź�����ʱ,gpio�˿�Ϊ1.
//���и��ź����룬���ź��������ӵ�GND���������͵�ƽ�ź�ʱ��GPIO�˿ڻ��⵽�͵�ƽ����ʾΪ0��
//������Ϊ���źŵ����ȼ�������������ĵ�ƽ���á�


uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;

    DHT11_IN();  // ������������Ϊ����ģʽ

    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 1 && retry < 80)
    {
        retry++;
        delay_us(1);  // �ӳ�1΢��
    }

    if (retry >= 80)
        return 1;  // ����ڹ涨ʱ����������Ϊ�ߵ�ƽ����ʾ������δ��Ӧ�����ش������ 1
    else
        retry = 0;

    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 0 && retry < 80)
    {
        retry++;
        delay_us(1);  // �ӳ�1΢��
    }

    if (retry >= 80)
        return 1;  // ����ڹ涨ʱ����������Ϊ�͵�ƽ����ʾ������δ��Ӧ�����ش������ 1

    return 0;  // ��������Ӧ���������سɹ����� 0
}
//��ȡһ��λ,�ο��ߵ͵�ƽ��ʱ��
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
		return 1;// ���ض�ȡ����λΪ�ߵ�ƽ
	else
		return 0;// ���ض�ȡ����λΪ�͵�ƽ
}

//��ȡһ���ֽ�,��������
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
//������������ݸ�ʽ,��������
uint8_t DHT11_Read_Data(uint8_t* humi, uint8_t* temp)
{
    uint8_t buf[5];  // �洢��ȡ����5���ֽ����ݵĻ�����

    DHT11_REST();  // ��ʼ��������ͨ��

    if (DHT11_Check() == 0)  // ��鴫�����Ƿ�������Ӧ
    {
        for (uint8_t i = 0; i < 5; i++)
            buf[i] = DHT11_Read_Byte();  // ����ֽڶ�ȡ���������͵�����

        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])  // У�����ݵ�׼ȷ��
        {
            *humi = buf[0];  // ��ʪ��ֵ�洢��ָ���ı�����
            *temp = buf[2];  // ���¶�ֵ�洢��ָ���ı�����
        }
    }
    else
    {
        return 1;  // ��������Ӧ�쳣�����ش������
    }

    return 0;  // ��ȡ���ݳɹ���������������
}

