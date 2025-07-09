#include "spi.h"
#include "stm32f10x.h"                  // Device header

#include "delay.h"

#define SPI_SS   GPIO_Pin_4
#define SPI_SCK  GPIO_Pin_5
#define SPI_MOSI GPIO_Pin_7
#define SPI_MISO GPIO_Pin_6
#define GPIO_PinJ GPIOA

void MySPI_W_SS(uint8_t BitValue) {
    GPIO_WriteBit(GPIO_PinJ, SPI_SS, (BitAction)BitValue);
}

void MySPI_W_SCK(uint8_t BitValue) {
    GPIO_WriteBit(GPIO_PinJ, SPI_SCK, (BitAction)BitValue);
}

void MySPI_W_MOSI(uint8_t BitValue) {
    GPIO_WriteBit(GPIO_PinJ, SPI_MOSI, (BitAction)BitValue);
}

uint8_t MySPI_R_MISO(void) {
    return GPIO_ReadInputDataBit(GPIO_PinJ, SPI_MISO);
}

void SPI_Master_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = SPI_SS | SPI_SCK | SPI_MOSI;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = SPI_MISO;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    MySPI_W_SS(1);
    MySPI_W_SCK(0);
}

uint8_t SPI_TransferByte(uint8_t byte) {
    uint8_t i, received = 0;
    
    for (i = 0; i < 8; i++) {
        MySPI_W_MOSI(byte & (0x80 >> i));
        MySPI_W_SCK(1);
        Delay_us(1);  // 适当延时
        if (MySPI_R_MISO()) received |= (0x80 >> i);
        MySPI_W_SCK(0);
        Delay_us(1);
    }
    
    return received;
}

void SPI_Master_SendData(uint8_t *data, uint16_t length) {
    MySPI_W_SS(0);  // 片选使能
    for (uint16_t i = 0; i < length; i++) {
        SPI_TransferByte(data[i]);
    }
    MySPI_W_SS(1);  // 片选禁用
}


