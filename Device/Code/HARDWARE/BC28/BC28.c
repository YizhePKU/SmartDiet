#include "bc28.h"
#include "string.h"
#include "usart.h"
#include "wdg.h"
#include "led.h"
char *strx, *extstrx;
extern char RxBuffer[200], RxCounter;
BC28 BC28_Status;
unsigned char socketnum = 0; //��ǰ��socket����

/* ��ջ��� */
void Clear_Buffer(void)
{
    u8 i;
    usart2_send_str(RxBuffer);
    for (i = 0; i < 200; i++)
        RxBuffer[i] = 0; //����
    RxCounter = 0;
    IWDG_Feed(); //ι��
}

void BC28_Init(void)
{
    printf("AT\r\n");
    delay_ms(300);
    strx = strstr((const char *)RxBuffer, (const char *)"OK"); //����OK
    Clear_Buffer();
    while (strx == NULL)
    {
        Clear_Buffer();
        printf("AT\r\n");
        delay_ms(300);
        strx = strstr((const char *)RxBuffer, (const char *)"OK"); //����OK
    }
    printf("AT+CFUN=1\r\n"); //��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
    delay_ms(300);
    printf("AT+CIMI\r\n"); //��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
    delay_ms(300);
    strx = strstr((const char *)RxBuffer, (const char *)"460"); //��460������ʶ�𵽿���
    Clear_Buffer();
    while (strx == NULL)
    {
        Clear_Buffer();
        printf("AT+CIMI\r\n"); //��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
        delay_ms(300);
        strx = strstr((const char *)RxBuffer, (const char *)"460"); //����OK,˵�����Ǵ��ڵ�
    }
    printf("AT+CGATT=1\r\n"); //�������磬PDP
    delay_ms(300);
    strx = strstr((const char *)RxBuffer, (const char *)"OK"); //��OK
    Clear_Buffer();
    printf("AT+CGATT?\r\n"); //��ѯ����״̬
    delay_ms(300);
    strx = strstr((const char *)RxBuffer, (const char *)"+CGATT:1"); //��1
    Clear_Buffer();
    while (strx == NULL)
    {
        Clear_Buffer();
        printf("AT+CGATT?\r\n"); //��ȡ����״̬
        delay_ms(300);
        strx = strstr((const char *)RxBuffer, (const char *)"+CGATT:1"); //����1,����ע���ɹ�
    }
    printf("AT+CESQ\r\n"); //�鿴��ȡCSQֵ
    delay_ms(300);
    strx = strstr((const char *)RxBuffer, (const char *)"+CESQ"); //����CSQ
    if (strx)
    {
        BC28_Status.CSQ = (strx[7] - 0x30) * 10 + (strx[8] - 0x30); //��ȡCSQ
        if ((BC28_Status.CSQ == 99) || ((strx[7] - 0x30) == 0))     //˵��ɨ��ʧ��
        {
            while (1)
            {
                BC28_Status.netstatus = 0;
                usart2_send_str("�ź�����ʧ�ܣ���鿴ԭ��!\r\n");
                //RESET=1;//����
                /* ��ʱ��ATָ�λ */
                printf("AT+NRB\r\n");
                delay_ms(300);
                delay_ms(300);
                //RESET=0;//��λģ��
                delay_ms(300); //û���źž͸�λ
            }
        }
        else
        {
            BC28_Status.netstatus = 1;
        }
    }
    Clear_Buffer();
}

void BC28_CreateUDPSokcet(void) //����sokcet
{
    u8 i;
    for (i = 0; i < 5; i++)
    {
        printf("AT+NSOCL=%c\r\n", i + 0x30); //�ر���һ��socekt����
        delay_ms(300);
    }
    Clear_Buffer();
    printf("AT+NSOCR=DGRAM,17,3005,1\r\n"); //����SOCKET����
    delay_ms(300);
    socketnum = RxBuffer[2]; //��ȡ��ǰ��socket����
}

//���ݷ��ͺ���
void BC28_UDPSend(uint8_t *len, uint8_t *data)
{
    printf("AT+NSOST=%c,114.115.148.172,9999,%s,%s\r\n", socketnum, len, data); //����0socketIP�Ͷ˿ں������Ӧ���ݳ����Լ�����,
    delay_ms(300);
    strx = strstr((const char *)RxBuffer, (const char *)"OK"); //����OK
    while (strx == NULL)
    {
        strx = strstr((const char *)RxBuffer, (const char *)"OK"); //����OK
    }
    Clear_Buffer();
}

void BC28_RECData(void)
{

    strx = strstr((const char *)RxBuffer, (const char *)"+QSONMI"); //����+QSONMI���������յ�UDP���������ص�����
    if (strx)
    {

        Clear_Buffer();
    }
}

//�����������ӵ�ƽ̨
void BC28_CreateInstance(void)
{
    u8 i = 0;
    printf("AT+MIPLCREATE\r\n");
    delay_ms(300);
    strx = strstr((const char *)RxBuffer, (const char *)"+MIPLCREATE:0"); //���󴴽��ɹ�
    while (strx == NULL)
    {
        strx = strstr((const char *)RxBuffer, (const char *)"+MIPLCREATE:0");
    }
    Clear_Buffer();

    /* 
     * AT+MIPLADDOBJ
     * ���ã�������ģ������һ�������ĵ�object���������instance
     * �����ʽ��AT+MIPLADDOBJ=<ref>,<objid>,<inscount>,<bitmap>,<atts>,<acts>
     * a. ref���豸ʵ��ID
     * b. objid��Object ID
     * c. inscount��ʵ������
     * d. bitmap��ʵ��λͼ���ַ�����ʽ��ÿһ���ַ���ʾΪһ��ʵ����1��ʾ���ã�0��ʾ�����á�
     * e. atts�����Ը�����Ĭ������Ϊ0���ɡ�
     * f. acts������������Ĭ������Ϊ0���ɡ�
     */
    // printf("AT+MIPLADDOBJ=0,3311,2,\"11\",4,2\r\n");//����3311��2������
    printf("AT+MIPLADDOBJ=0,3322,1,\"1\",9,0\r\n"); //3322 Load 1 object
    delay_ms(300);
    strx = strstr((const char *)RxBuffer, (const char *)"OK"); //���󴴽��ɹ�
    while (strx == NULL)
    {
        strx = strstr((const char *)RxBuffer, (const char *)"OK");
    }
    Clear_Buffer();
    printf("AT+MIPLOPEN=0,600,60\r\n"); //����ƽ̨��Դ
    delay_ms(300);

    /* ��ȡobserve���� */
    strx = strstr((const char *)RxBuffer, (const char *)"+MIPLOBSERVE:"); //��ȡobserve����
    while (strx == NULL)
    {
        strx = strstr((const char *)RxBuffer, (const char *)"+MIPLOBSERVE:"); //��ȡobserve����
    }
    delay_ms(100);
    while (strx[16 + i] != ',') //û���������ž���observe ID
    {
        BC28_Status.Observe_ID[i] = strx[16 + i];
        i++;
    }
    Clear_Buffer(); //���ܹ�����while֮ǰ����Ϊstrx������RxBuffer��ָ�룬�����˾�û��
    i = 0;

    /* ��ȡ��Դ���� */
    strx = strstr((const char *)RxBuffer, (const char *)"+MIPLDISCOVER:"); //��ȡ��Դ����
    while (strx == NULL)
    {
        strx = strstr((const char *)RxBuffer, (const char *)"+MIPLDISCOVER:"); //��ȡ��Դ����
    }
    delay_ms(100);
    while (strx[17 + i] != ',') //û���������ž�����ԴID
    {
        BC28_Status.Resource_ID[i] = strx[17 + i];
        i++;
    }
    Clear_Buffer();
    delay_ms(1000);

    /* ���е����Զ���ע�ᵽƽ̨�� */
    //44 �ַ���λ��
    printf("AT+MIPLDISCOVERRSP=0,%s,1,44,\"5601;5602;5603;5604;5605;5700;5701;5750;5821\"\r\n", BC28_Status.Resource_ID);
    delay_ms(300);
    strx = strstr((const char *)RxBuffer, (const char *)"OK");
    while (strx == NULL)
    {
        strx = strstr((const char *)RxBuffer, (const char *)"OK");
    }
    Clear_Buffer();
}

void ONENET_Readdata(void) //�ȴ��������Ķ�����
{
    u8 i = 0, count = 0;
    strx = strstr((const char *)RxBuffer, (const char *)"+MIPLREAD:"); //�ж��������,������ݲ�ͬ�������Ͳ�ͬ��ָ������
    if (strx)                                                          //��ȡ��ָ������֮�󣬻�ȡ��������
    {
        LED_RED=0; LED_BLU=1; LED_GRN=0;
        delay_ms(100);
        while (1)
        {
            if (strx[11 + i] == ',')
                count++;
            BC28_Status.ReadSource[i] = strx[11 + i];
            if (count >= 2) //����2�������ˣ���Ҫ����
            {
                BC28_Status.ReadSource[i + 1] = '1';
                break; //����
            }
            i++;
        }
        while (strx[11 + i] != 0x0D) //û������������
        {

            BC28_Status.ReadSource[i + 2] = strx[11 + i]; //ָ���ƶ�������ƶ�����λ�ã������������������
            usart2_send_str(RxBuffer);
            i++;
        }
        strx = strchr(strx + 1, ',');
        Clear_Buffer();
        
        /* 
         * AT+MIPLREADRSP
         * ���ã�MCU�����Ӧ��Read��������ƽ̨�ظ�Read�������
         * AT+MIPLREADRSP=<ref>,<msgid>,<result>[,<objid>,<insid>,<resid>,<type>,<len>,<value>,<index>,<flag>]
         * result
         */
        // printf("AT+MIPLREADRSP=%s,4,4,2.35,0,0\r\n", BC28_Status.ReadSource); //�ظ����󣬽����ݴ�����ȥ
        printf("AT+MIPLREADRSP=%s,4,4,2.35,0,0\r\n", BC28_Status.ReadSource); //���� 2.35 4-float 4-����λ��
        delay_ms(300);
        strx = strstr((const char *)RxBuffer, (const char *)"OK"); //
        while (strx == NULL)
        {
            strx = strstr((const char *)RxBuffer, (const char *)"OK"); //
        }
        Clear_Buffer();
    }
}

void BC28_NotifyResource(void)
{
    /*
     * AT+MIPLNOTIFY
     * ���ã�������ģ�����OneNET ƽ̨�ϱ�ָ����Դ������
     * AT+MIPLNOTIFY=<ref>,<msgid>,<objid>,<insid>,<resid>,<type>,<len>,<value>,<index>,<flag>[,<ackid>]
     * a. ref���豸ʵ��ID
     * b. msgid����resource������instance observe����ʱ�·���msgid
     * c. objid��Object ID
     * d. insid��Instance ID
     * e. resid��Resource ID
     * f. type���ϱ���Դ���������ͣ�1-string��2-opaque��3-integer��4-float��5-bool��6-hex_str��
     * g. len��valueֵ�ĳ���
     * h. index��ָ����š����Է�N�����ģ���N-1��0�����ţ�0��ʾ����Notifyָ�����
     * i. value���ϱ����ݡ�
     * j. flag����Ϣ��ʶ��ָʾ��һ�����м�����һ�����ġ�
     */
    printf("AT+MIPLNOTIFY=0,%s,3322,0,5700,4,4,2.33,0,0\r\n", BC28_Status.Observe_ID); //������������2.33 ����λ��Ϊ4
    delay_ms(300);
    strx = strstr((const char *)RxBuffer, (const char *)"OK");
    while (strx == NULL)
    {
        strx = strstr((const char *)RxBuffer, (const char *)"OK");
    }
    Clear_Buffer();
}