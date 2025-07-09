#include "stm32f10x.h"          // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "spi.h"

#define DATA_COUNT 10 // 连续接收10次数据

uint8_t RxData;			//定义用于接收串口数据的变量
uint8_t receivebyte;//用于转换openmv发过来的asc||码值
uint8_t rx_data[DATA_COUNT];  // 存储接收的5次数据
uint8_t spi_tx_data[1] = {0};//0-->物品
uint8_t i;
 //统计数组中出现次数最多的值（多数表决）————>过滤函数
uint8_t get_most_frequent(uint8_t *arr, uint8_t len)
{
    uint8_t max_count = 0;
    uint8_t result = 0;
    
    for (uint8_t i = 0; i < len; i++) {
        uint8_t count = 0;
        // 统计当前元素出现的次数
        for (uint8_t j = 0; j < len; j++) {
            if (arr[j] == arr[i]) {
                count++;
            }
        }
        // 更新出现次数最多的值
        if (count > max_count) {
            max_count = count;
            result = arr[i];
        }
    }
    return result;
}

int main(void)
{

	OLED_Init();		      //OLED初始化
	OLED_ShowString(1, 1, "RxData:");        /*显示静态字符串*/

	Serial_Init();	                   /*串口初始化*/
	SPI_Master_Init();                //软件模拟spi初始化
	while (1)
	{
		if (Serial_GetRxFlag() == 1)			//检查串口接收数据的标志位        主机接收
		{
			RxData = Serial_GetRxData();		//获取串口接收的数据-openmv
			receivebyte = RxData - '0';			//转换asc||码值为10进制							
			for(i=0;i<DATA_COUNT;i++)      //接收5次传来的数据，并将其打包
			{
				rx_data[i]=receivebyte;
			}		
			spi_tx_data[0]=get_most_frequent(rx_data,DATA_COUNT);

			OLED_ShowNum(2, 1, spi_tx_data[0],4);	//显示过滤后的数据	
			
			SPI_Master_SendData(spi_tx_data, sizeof(spi_tx_data));//spi发送数据到星火一号
			Delay_ms(2000);      // 每1000ms发送一次
		}
	}
}





