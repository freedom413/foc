#include "main.h"
#include "usart.h"
#include <stdint.h>
#include "i2c.h"
#include "stdio.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"

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


static char CPU_RunInfo[512];
static void get_rtos_mem(void)
{
    memset(CPU_RunInfo, 0, 512);
    vTaskList((char *)&CPU_RunInfo); //获取任务运行时间信息
    printf("---------------------------------------------\r\n");
    printf("任务名  \t状态\t优先级\t剩余栈\t序号\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n");
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_PARAM_NUM(0),
cpumem, get_rtos_mem, cpu memory info);


static void get_rtos_runtime(void const *argument)
{
    memset(CPU_RunInfo,0,512);
    vTaskGetRunTimeStats((char *)&CPU_RunInfo);
    printf("---------------------------------------------\r\n");
    printf("任务名  \t运行计数\t使用率\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n\n");
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_PARAM_NUM(0),
cpuload, get_rtos_runtime, cpu memory info);

volatile long long FreeRTOSRunTimeTicks;

void configureTimerForRunTimeStats(void)
{
    FreeRTOSRunTimeTicks = 0;
}

unsigned long getRunTimeCounterValue(void)
{
    return FreeRTOSRunTimeTicks;
}


#if 0
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
#endif