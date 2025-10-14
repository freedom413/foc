/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */



#include "shell.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "cmsis_os.h"
#include "stdio.h"
Shell shell;
static char shellBuffer[512];


/**
 * @brief 用户shell写
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际写入的数据长度
 */
static short userShellWrite(char *data, unsigned short len)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)data, len, 0x1FF);
    return len;
}

short userShellRead(char *data, unsigned short len)
{
    return HAL_UART_Receive(&huart2, (uint8_t *)data, len, 0x1FF);
}

// static char recv_buf;

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     /* 判断是哪个串口触发的中断 */
//     if(huart ->Instance == USART2)
//     {
//         //调用shell处理数据的接口
//      shellHandler(&shell, recv_buf);
//         //使能串口中断接收
//      HAL_UART_Receive_IT(&huart2, (uint8_t*)&recv_buf, 1);
//     }
// }

/**
 * @brief 用户shell初始化
 * 
 */
void userShellInit(void)
{
    shell.write = userShellWrite; 
    shell.read = userShellRead;
    shellInit(&shell, shellBuffer, 512);
    // HAL_UART_Receive_IT(&huart2, (uint8_t*)&recv_buf, 1);
    if (xTaskCreate(shellTask, "shell", 256, &shell, 5, NULL) != pdPASS)
    {
        printf("shell task creat failed");
    }
}


