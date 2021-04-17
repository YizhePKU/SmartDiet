#include "led.h"

/* 
 * LED ��ʼ��
 * BLue PD2
 * Red PC10
 * Green PA5(D13)
 * �豸�������ɫ����ʼ���ɹ�����ɫ������Ϣ��Ҫ�ϴ���ɫ���ɹ���ָ���ɫ
 * Ҫ���ܳ�ʼ��/�ϴ�ʱ�Ǻ����ƾ͸��ÿ���
 */
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ʹ��PD,PC,PA�˿�ʱ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA, ENABLE);

    /* BLue PD2 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);            //�����趨������ʼ��GPIOD2

    /* Red PC10 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);            //�����趨������ʼ��GPIOC10

    /* Green PA5 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);            //�����趨������ʼ��GPIOA5

    LED_RED = 0;
    LED_BLU = 1;
    LED_GRN = 1;
}