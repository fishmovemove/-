#ifndef __SPI_H_
#define __SPI_H_

void SPI1_W_SS(rt_uint8_t BitValue);
void SPI1_W_SCK(rt_uint8_t BitValue);
void SPI1_W_MOSI(rt_uint8_t BitValue) ;
rt_uint8_t SPI1_R_MISO(void);
void SPI1_Slave_Init(void);
rt_uint8_t SPI1_Slave_ReceiveByte(void);
void SPI1_Slave_ReceiveData(rt_uint8_t *buffer, rt_uint16_t length);


#endif
