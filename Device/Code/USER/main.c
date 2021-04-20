/*
  BC28_ONENET_LWM2M Read & Respond Mode
  �Զ�ע�� Object �� resource
  ���� ONENET �Ķ� resource ������Ӧ
*/
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "math.h"
#include "stdio.h"
#include "stm32f4xx_flash.h"
#include "stdlib.h"
#include "string.h"
#include "wdg.h"
#include "timer.h"
#include "stm32f4xx_tim.h"
#include "bc28.h"
#include "math.h"
#include "btn.h"
#include "led.h"

extern char RxBuffer[200], RxCounter;
u8 timeout;

int main(void)
{
    delay_init();           //��ʱ������ʼ��
    NVIC_Configuration();   //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    LED_Init();             //LED��ʼ��
    usart1_init(9600);      //����1--BC28
    usart2_init(9600);      //����2--PC
    //usart6_init(9600);    //����3
    btn1_exti_init();       //��ť�жϳ�ʼ��
    BC28_Init();            //��BC28�豸��ʼ��
    BC28_CreateInstance();  //�������Ӷ���ONENET
    Save_Data.counter = 0;
    while (1)
    {
        LED_RED=1; LED_BLU=1; LED_GRN=0;
        ONENET_Readdata();//����ONENET�������·�ָ������
        /* �п��������������ϴ� */
        // if (Save_Data.isUsefull)
        // {
        //     LED_RED=0; LED_BLU=1; LED_GRN=0;
        //     Save_Data.isUsefull = 0;
        //     BC28_NotifyResource();
        //     timeout++;
        //     delay_ms(1000);
        // }
        if (timeout >= 120) //2����ˢ��һ�����ӷ�����
        {
            printf("AT+MIPLUPDATE=0,600,0\r\n"); //10��������ʱ��
            RxCounter = 0;
            timeout = 0;
        }
        //delay_ms(500);
    }
    
}
