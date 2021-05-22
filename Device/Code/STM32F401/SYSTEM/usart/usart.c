#include "sys.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "stm32f4xx_tim.h"
unsigned char uart1_getok;
_SaveData Save_Data;

/* 
 * �������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
 */
#if 1
#pragma import(__use_no_semihosting)

/* ��׼����Ҫ��֧�ֺ��� */
struct __FILE
{
    int handle;
};
FILE __stdout;

/* ����_sys_exit()�Ա���ʹ�ð�����ģʽ */
void _sys_exit(int x)
{
    x = x;
}

/* �ض���fputc����  */
int fputc(int ch, FILE *f)
{
    while ((USART2->SR & 0X40) == 0)
        ; //ѭ������,ֱ���������
    USART2->DR = (u8)ch;
    return ch;
}
#endif

void usart1_send_byte(char data)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;
    USART_SendData(USART1, data);
}

void usart2_send_byte(char data)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        ;
    USART_SendData(USART2, data);
}

void usart6_send_byte(char data)
{
    while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET)
        ;
    USART_SendData(USART6, data);
}

//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
//�����������ѭ��д�ģ�д��ĩβ�ֻص���ͷ
char RxCounter, RxBuffer[200];   //���ջ���,���USART_REC_LEN���ֽ�.
char RxCounter1, RxBuffer1[100]; //���ջ���,���USART_REC_LEN���ֽ�.
u16 point2 = 0;
char RxBuffer2[200];
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0; //����״̬���
extern u8 Timeout;

/**
 * ��ʼ�� USART1
 * TX PA9 D8
 * RX PA10 D2
 * bound: ������
 */
void usart1_init(u32 bound)
{
    /* GPIO�˿����� */
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* ʹ��USART1��GPIOAʱ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* ���ڶ�Ӧ���Ÿ���ӳ�� */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);  //GPIOA9����ΪUSART1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //GPIOA10����ΪUSART1

    /* �˿����� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //����
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //��ʼ��PA9��PA10

    /* USART ��ʼ������ */
    USART_InitStructure.USART_BaudRate = bound;                                     //����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //�շ�ģʽ
    USART_Init(USART1, &USART_InitStructure);                                       //��ʼ������

/* �ж����� */
#if EN_USART1_RX
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //��������ж�

    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;         //����1�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //����ָ���Ĳ�����ʼ��NVIC�Ĵ�����
#endif

    USART_Cmd(USART1, ENABLE); //ʹ�ܴ���
    USART_ClearFlag(USART1, USART_FLAG_TC);
}

/**
 * ��ʼ�� USART2
 * TX PA2 D1
 * RX PA3 D0
 * bound: ������
 */
void usart2_init(u32 bound)
{
    /* GPIO�˿����� */
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* ʹ��USART2��GPIOAʱ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* ���ڶ�Ӧ���Ÿ���ӳ�� */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //GPIOA2����ΪUSART2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //GPIOA3����ΪUSART2

    /* �˿����� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //����
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //��ʼ��PA2��PA3

    /* USART ��ʼ������ */
    USART_InitStructure.USART_BaudRate = bound;                                     //����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //�շ�ģʽ
    USART_Init(USART2, &USART_InitStructure);                                       //��ʼ������

/* �ж����� */
#if EN_USART2_RX
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //��������ж�

    //Usart2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;         //����2�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //����ָ���Ĳ�����ʼ��NVIC�Ĵ�����
#endif

    USART_Cmd(USART2, ENABLE); //ʹ�ܴ���
    USART_ClearFlag(USART2, USART_FLAG_TC);
}

/**
 * ��ʼ�� USART6
 * TX PC6 J4_3
 * RX PC7 J4_2(D9)
 * bound: ������
 */
void usart6_init(u32 bound)
{
    /* GPIO�˿����� */
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* ʹ��USART6��GPIOCʱ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    /* ���ڶ�Ӧ���Ÿ���ӳ�� */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6); //GPIOC11����ΪUSART6
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6); //GPIOC12����ΪUSART6

    /* �˿����� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOA2��GPIOA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //����
    GPIO_Init(GPIOC, &GPIO_InitStructure);                 //��ʼ��PC6��PC7

    /* USART ��ʼ������ */
    USART_InitStructure.USART_BaudRate = bound;                                     //����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //�շ�ģʽ
    USART_Init(USART6, &USART_InitStructure);                                       //��ʼ������

/* �ж����� */
#if EN_USART6_RX
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //��������ж�

    //Usart6 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;         //����6�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //����ָ���Ĳ�����ʼ��NVIC�Ĵ�����
#endif

    USART_Cmd(USART6, ENABLE); //ʹ�ܴ���
    USART_ClearFlag(USART6, USART_FLAG_TC);
}

/* ����1��ӡ���� */
void usart1_send_str(char *SendBuf)
{
    while (*SendBuf)
    {
        while ((USART1->SR & 0X40) == 0)
            ; //�ȴ��������
        USART1->DR = (u8)*SendBuf;
        SendBuf++;
    }
}

/* ����2��ӡ���� */
void usart2_send_str(char *SendBuf)
{
    while (*SendBuf)
    {
        while ((USART2->SR & 0X40) == 0)
            ; //�ȴ��������
        USART2->DR = (u8)*SendBuf;
        SendBuf++;
    }
}

/* ����6��ӡ���� */
void usart6_send_str(char *SendBuf)
{
    while (*SendBuf)
    {
        while ((USART6->SR & 0X40) == 0)
            ; //�ȴ��������
        USART6->DR = (u8)*SendBuf;
        SendBuf++;
    }
}

/* ����1�жϷ������ */
void USART1_IRQHandler(void)
{
    char Res;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //�����жϣ�������չ������
    {
        Res = USART_ReceiveData(USART1);                   //����ģ�������;
        RxBuffer[RxCounter++] = USART_ReceiveData(USART1); //����ģ�������
        if (RxCounter > 199)                               //���������趨
            RxCounter = 0;
    }
}

/* ����2�жϷ������ */
void USART2_IRQHandler(void)
{
    u8 Res;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        Res = USART_ReceiveData(USART2); //(USART1->DR);	//��ȡ���յ�������
    }
}

/* ����6�жϷ������ */
void USART6_IRQHandler(void)
{
    char Res;
    if (USART_GetITStatus(USART6, USART_IT_RXNE) != RESET) //����ģ�鷵�ص�����
    {
        Res = USART_ReceiveData(USART6); //����ģ�������;
    }
}
