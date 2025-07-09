#ifndef __uart_H_
#define __uart_H_

#include "stm32f10x.h"
#include <stdarg.h>
#include <stdio.h>

extern uint8_t Serial2_RxData;     // ���崮��2���յ����ݱ���
extern uint8_t Serial2_RxFlag;     // ���崮��2���յı�־λ����

void Serial2_Init(void);           // ����2��ʼ��
void Serial2_SendByte(uint8_t Byte); // ����2����һ���ֽ�
void Serial2_SendArray(uint8_t *Array, uint16_t Length); // ����2����һ������
void Serial2_SendString(char *String); // ����2����һ���ַ���
void Serial2_SendNumber(uint32_t Number, uint8_t Length); // ����2��������
void Serial2_Printf(char *format, ...); // ����2��ʽ�����
uint8_t Serial2_GetRxFlag(void);    // ��ȡ����2���ձ�־λ
uint8_t Serial2_GetRxData(void);    // ��ȡ����2���յ�����

#endif


