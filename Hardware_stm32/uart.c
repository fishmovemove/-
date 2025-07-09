#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

uint8_t Serial2_RxData;            // ���崮��2���յ����ݱ���
uint8_t Serial2_RxFlag;            // ���崮��2���յı�־λ����

/**
  * ��    ��������2��ʼ��
  * ��    ������
  * �� �� ֵ����
  */
void Serial2_Init(void)
{
    /*����ʱ��*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);    // ����USART2��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);     // ����GPIOA��ʱ��
    
    /*GPIO��ʼ��*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                    // ��PA2���ų�ʼ��Ϊ�����������
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                    // ��PA3���ų�ʼ��Ϊ��������
    
    /*USART��ʼ��*/
    USART_InitTypeDef USART_InitStructure;                    // ����ṹ�����
    USART_InitStructure.USART_BaudRate = 9600;               // ������
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // Ӳ�������ƣ�����Ҫ
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // ģʽ������ģʽ�ͽ���ģʽ��ѡ��
    USART_InitStructure.USART_Parity = USART_Parity_No;       // ��żУ�飬����Ҫ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;    // ֹͣλ��ѡ��1λ
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // �ֳ���ѡ��8λ
    USART_Init(USART2, &USART_InitStructure);                 // ���ṹ���������USART_Init������USART2
    
    /*�ж��������*/
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);            // �������ڽ������ݵ��ж�
                        
    /*NVIC�жϷ���*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);           // ����NVICΪ����2
    
    /*NVIC����*/
    NVIC_InitTypeDef NVIC_InitStructure;                      // ����ṹ�����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;         // ѡ������NVIC��USART2��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // ָ��NVIC��·ʹ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // ָ��NVIC��·����ռ���ȼ�Ϊ1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        // ָ��NVIC��·����Ӧ���ȼ�Ϊ1
    NVIC_Init(&NVIC_InitStructure);                           // ���ṹ���������NVIC_Init������NVIC����
    
    /*USARTʹ��*/
    USART_Cmd(USART2, ENABLE);                                // ʹ��USART2�����ڿ�ʼ����
}

/**
  * ��    ��������2����һ���ֽ�
  * ��    ����Byte Ҫ���͵�һ���ֽ�
  * �� �� ֵ����
  */
void Serial2_SendByte(uint8_t Byte)
{
    USART_SendData(USART2, Byte);     // ���ֽ�����д�����ݼĴ�����д���USART�Զ�����ʱ����
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); // �ȴ��������
    /*�´�д�����ݼĴ������Զ����������ɱ�־λ���ʴ�ѭ�������������־λ*/
}

/**
  * ��    ��������2����һ������
  * ��    ����Array Ҫ����������׵�ַ
  * ��    ����Length Ҫ��������ĳ���
  * �� �� ֵ����
  */
void Serial2_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i ++)    // ��������
    {
        Serial2_SendByte(Array[i]);   // ���ε���Serial2_SendByte����ÿ���ֽ�����
    }
}

/**
  * ��    ��������2����һ���ַ���
  * ��    ����String Ҫ�����ַ������׵�ַ
  * �� �� ֵ����
  */
void Serial2_SendString(char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i ++) // �����ַ����飨�ַ������������ַ���������־λ��ֹͣ
    {
        Serial2_SendByte(String[i]);     // ���ε���Serial2_SendByte����ÿ���ֽ�����
    }
}

/**
  * ��    �����η��������ڲ�ʹ�ã�
  * �� �� ֵ������ֵ����X��Y�η�
  */
uint32_t Serial2_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;    // ���ý����ֵΪ1
    while (Y --)            // ִ��Y��
    {
        Result *= X;        // ��X�۳˵����
    }
    return Result;
}

/**
  * ��    ��������2��������
  * ��    ����Number Ҫ���͵����֣���Χ��0~4294967295
  * ��    ����Length Ҫ�������ֵĳ��ȣ���Χ��0~10
  * �� �� ֵ����
  */
void Serial2_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i ++)       // �������ֳ��ȱ������ֵ�ÿһλ
    {
        Serial2_SendByte(Number / Serial2_Pow(10, Length - i - 1) % 10 + '0'); // ���ε���Serial2_SendByte����ÿλ����
    }
}

/**
  * ��    ����ʹ��printf��Ҫ�ض���ĵײ㺯��
  * ��    ��������ԭʼ��ʽ���ɣ�����䶯
  * �� �� ֵ������ԭʼ��ʽ���ɣ�����䶯
  */
int fputc2(int ch, FILE *f)
{
    Serial2_SendByte(ch);            // ��printf�ĵײ��ض����Լ��ķ����ֽں���
    return ch;
}

/**
  * ��    �����Լ���װ��prinf����
  * ��    ����format ��ʽ���ַ���
  * ��    ����... �ɱ�Ĳ����б�
  * �� �� ֵ����
  */
void Serial2_Printf(char *format, ...)
{
    char String[100];               // �����ַ�����
    va_list arg;                    // ����ɱ�����б��������͵ı���arg
    va_start(arg, format);          // ��format��ʼ�����ղ����б�arg����
    vsprintf(String, format, arg);  // ʹ��vsprintf��ӡ��ʽ���ַ����Ͳ����б��ַ�������
    va_end(arg);                    // ��������arg
    Serial2_SendString(String);     // ���ڷ����ַ����飨�ַ�����
}

/**
  * ��    ������ȡ����2���ձ�־λ
  * ��    ������
  * �� �� ֵ�����ڽ��ձ�־λ����Χ��0~1�����յ����ݺ󣬱�־λ��1����ȡ���־λ�Զ�����
  */
uint8_t Serial2_GetRxFlag(void)
{
    if (Serial2_RxFlag == 1)        // �����־λΪ1
    {
        Serial2_RxFlag = 0;
        return 1;                   // �򷵻�1�����Զ������־λ
    }
    return 0;                       // �����־λΪ0���򷵻�0
}

/**
  * ��    ������ȡ����2���յ�����
  * ��    ������
  * �� �� ֵ�����յ����ݣ���Χ��0~255
  */
uint8_t Serial2_GetRxData(void)
{
    return Serial2_RxData;          // ���ؽ��յ����ݱ���
}

/**
  * ��    ����USART2�жϺ���
  * ��    ������
  * �� �� ֵ����
  * ע������˺���Ϊ�жϺ�����������ã��жϴ������Զ�ִ��
  *           ������ΪԤ����ָ�����ƣ����Դ������ļ�����
  *           ��ȷ����������ȷ���������κβ��죬�����жϺ��������ܽ���
  */
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)        // �ж��Ƿ���USART2�Ľ����¼��������ж�
    {
        Serial2_RxData = USART_ReceiveData(USART2);             // ��ȡ���ݼĴ���������ڽ��յ����ݱ���
        Serial2_RxFlag = 1;                                    // �ý��ձ�־λ����Ϊ1
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);        // ���USART2��RXNE��־λ
                                                                // ��ȡ���ݼĴ������Զ�����˱�־λ
                                                                // ����Ѿ���ȡ�����ݼĴ�����Ҳ���Բ�ִ�д˴���
    }
}


