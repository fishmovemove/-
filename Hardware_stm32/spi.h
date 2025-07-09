#ifndef __SPI_H_
#define __SPI_H_
#include "stm32f10x.h"                  // Device header


void SPI_Master_Init(void);
void SPI_Master_SendData(uint8_t *data, uint16_t length);
void MySPI_W_SS(uint8_t BitValue);
void MySPI_W_SCK(uint8_t BitValue);
void MySPI_W_MOSI(uint8_t BitValue);
uint8_t MySPI_R_MISO(void);
uint8_t SPI_TransferByte(uint8_t byte) ;


#endif
