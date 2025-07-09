#ifndef __UART_H__
#define __UART_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

int uart3_init(rt_uint32_t baudrate);

/**
 * @brief 通过串口3发送16进制数组
 * @param data 要发送的数据数组
 * @param len 数据长度
 * @return 实际发送的字节数
 */
int uart3_send_hex(const rt_uint8_t *data, rt_size_t len);

#endif



