#include "btn.h"
#include "usart.h"
/*
 * ��ʼ�� BTN_1 �ж�
 * PC13
 * �м�İ�ť
 */
void btn1_exti_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* ����PC13Ϊ���� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //ʹ��PC�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;            //�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;          //����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //����
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;      //����Ƶ��
    GPIO_Init(GPIOC, &GPIO_InitStructure);                //�����趨������ʼ��GPIOC

    /* ����EXTI13 */
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_EXTIT, ENABLE);  //ʹ��EXTITʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    /* ������õ������0�ţ���ô����������GPIO_PinSource0 ������õ������3�ţ���ô����������GPIO_PinSource3 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
    EXTI_InitStructure.EXTI_Line = EXTI_Line13; //���õľ���EXTI_Line0-EXTI_Line015����gpio�ܽŵ��Ǽ���
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); //��ʼ���ж�

    /* ����NVIC */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;      //ʹ���ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //��ռ���ȼ�2  ��ΪΪ����Ϊ2 �����������Ϊ0-3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //��Ӧ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);                           //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

/* �жϷ����� */
void EXTI15_10_IRQHandler(void)
{
    //�ж��ⲿ�ж�13�Ƿ���
    if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        usart2_send_str("��⵽����\r\n");
        Save_Data.isUsefull = 1;
        Save_Data.counter = (Save_Data.counter + 1) % 200;
        //����жϱ�־λ
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
}