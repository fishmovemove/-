#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/* 定义SPI引脚 */
#define SPI1_SS_PIN    GET_PIN(A, 4)
#define SPI1_SCK_PIN   GET_PIN(A, 5)
#define SPI1_MOSI_PIN  GET_PIN(A, 7)
#define SPI1_MISO_PIN  GET_PIN(A, 6)

/* SPI引脚电平操作函数 */
 void SPI1_W_SS(rt_uint8_t BitValue) {
    rt_pin_write(SPI1_SS_PIN, BitValue);
}

void SPI1_W_SCK(rt_uint8_t BitValue) {
    rt_pin_write(SPI1_SCK_PIN, BitValue);
}

void SPI1_W_MOSI(rt_uint8_t BitValue) {
    rt_pin_write(SPI1_MOSI_PIN, BitValue);
}

rt_uint8_t SPI1_R_MISO(void) {
    return rt_pin_read(SPI1_MISO_PIN);
}

/* SPI1从机初始化 */
void SPI1_Slave_Init(void) {
    /* 初始化SS、SCK、MOSI为输入模式 */
    rt_pin_mode(SPI1_SS_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(SPI1_SCK_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(SPI1_MOSI_PIN, PIN_MODE_INPUT_PULLUP);

    /* 初始化MISO为输出模式 */
    rt_pin_mode(SPI1_MISO_PIN, PIN_MODE_OUTPUT);

    /* 初始化为高电平 */
    SPI1_W_MOSI(1);
}

/* 接收一个字节 */
rt_uint8_t SPI1_Slave_ReceiveByte(void) {
    rt_uint8_t i, received = 0;

    /* 等待片选信号拉低 */
    while(rt_pin_read(SPI1_SS_PIN) == PIN_HIGH);

    for (i = 0; i < 8; i++) {
        /* 等待时钟上升沿 */
        while(rt_pin_read(SPI1_SCK_PIN) == PIN_LOW);

        /* 读取MOSI数据 */
        if (rt_pin_read(SPI1_MOSI_PIN)) {
            received |= (0x80 >> i);
        }

        /* 等待时钟下降沿 */
        while(rt_pin_read(SPI1_SCK_PIN) == PIN_HIGH);
    }

    return received;
}

/* 接收指定长度的数据 */
void SPI1_Slave_ReceiveData(rt_uint8_t *buffer, rt_uint16_t length) {
    for (rt_uint16_t i = 0; i < length; i++) {
        buffer[i] = SPI1_Slave_ReceiveByte();
    }
}


