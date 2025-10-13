#include "main.h"
#include "usart.h"
#include <stdint.h>
#include "i2c.h"
#include "stdio.h"

#ifdef __GNUC__
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

#ifdef __GNUC__
int _write(int fd, char *ptr, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}
#endif


static const uint8_t tail[4] = {0x00, 0x00, 0x80, 0x7f};

void vofa_just_float_send(uint8_t * data, uint8_t len)
{
    HAL_UART_Transmit(&huart1, data, len, 0x1fff);
    HAL_UART_Transmit(&huart1, tail, sizeof(tail), 0x1fff);
}

void i2c_addr_check(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef result;
    // 遍历所有可能的7位I2C地址 (0x08 to 0x77 是常用范围，但理论上可扫 1-127)
    for (uint16_t i = 1; i < 128; i++) {
        // 将7位地址左移1位，形成8位地址基础，读写位由HAL库处理
        uint16_t target_address = i << 1;
        // 检查设备是否就绪，重试2次，超时2毫秒
        result = HAL_I2C_IsDeviceReady(hi2c, target_address, 2, 2);
        
        if (result == HAL_OK) {
            printf("I2C device found at address: 0x%02X (7-bit address: 0x%02X)\r\n", target_address, i);
        }
    }

}