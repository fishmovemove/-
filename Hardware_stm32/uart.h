#ifndef __uart_H_
#define __uart_H_

#include "stm32f10x.h"
#include <stdarg.h>
#include <stdio.h>

extern uint8_t Serial2_RxData;     // 定义串口2接收的数据变量
extern uint8_t Serial2_RxFlag;     // 定义串口2接收的标志位变量

void Serial2_Init(void);           // 串口2初始化
void Serial2_SendByte(uint8_t Byte); // 串口2发送一个字节
void Serial2_SendArray(uint8_t *Array, uint16_t Length); // 串口2发送一个数组
void Serial2_SendString(char *String); // 串口2发送一个字符串
void Serial2_SendNumber(uint32_t Number, uint8_t Length); // 串口2发送数字
void Serial2_Printf(char *format, ...); // 串口2格式化输出
uint8_t Serial2_GetRxFlag(void);    // 获取串口2接收标志位
uint8_t Serial2_GetRxData(void);    // 获取串口2接收的数据

#endif


