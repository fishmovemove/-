#include "uart.h"
#include <string.h>
#include <rtdevice.h>


static struct rt_serial_device *uart3_dev = RT_NULL;

int uart3_init(rt_uint32_t baudrate)
{
    /* 查找串口设备 */
    uart3_dev = (struct rt_serial_device *)rt_device_find("uart3");
    if (!uart3_dev)
    {
        rt_kprintf("uart3 device not found!\n");
        return -RT_ERROR;
    }

    /* 初始化串口参数 */
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.baud_rate = baudrate;  // 设置为9600波特率
    config.data_bits = DATA_BITS_8;
    config.stop_bits = STOP_BITS_1;
    config.parity    = PARITY_NONE;

    rt_device_control((rt_device_t)uart3_dev, RT_DEVICE_CTRL_CONFIG, &config);

    /* 打开串口设备 */
    if (rt_device_open((rt_device_t)uart3_dev, RT_DEVICE_FLAG_STREAM) != RT_EOK)
    {
        rt_kprintf("uart3 device open failed!\n");
        return -RT_ERROR;
    }

    return RT_EOK;
}

int uart3_send_hex(const rt_uint8_t *data, rt_size_t len)
{
    if (!uart3_dev)
    {
        rt_kprintf("uart3 device not initialized!\n");
        return -RT_ERROR;
    }

    if (!data || len == 0)
    {
        return 0;
    }

    rt_size_t sent = 0;

    /* 直接发送数据 */
    sent = rt_device_write((rt_device_t)uart3_dev, 0, data, len);

    return sent;
}
