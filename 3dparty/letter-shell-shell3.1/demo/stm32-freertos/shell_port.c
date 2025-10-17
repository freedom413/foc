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
#include "usart.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "shell.h"
#include <stdint.h>
#include <stdio.h>
#include "shell_port.h"

static char shellBuffer[512];
static SemaphoreHandle_t shellMutex;

Shell shell;
SemaphoreHandle_t shell_tx_sem;
uint8_t shell_rxbuf[RX_BUF_SIZE];
StreamBufferHandle_t shell_rx_strmbuf;

/**
 * @brief 用户shell写
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际写入的数据长度
 */
short userShellWrite(char *data, unsigned short len)
{
    HAL_StatusTypeDef ret = HAL_UART_Transmit_DMA(&huart2, (uint8_t *)data, len);
    if (ret == HAL_OK) {
        if (xSemaphoreTake(shell_tx_sem, portMAX_DELAY) == pdTRUE) {
            return len;
        }
    }
    return 0;
}


/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际读取到
 */
short userShellRead(char *data, unsigned short len)
{
    return xStreamBufferReceive(shell_rx_strmbuf, data, len, portMAX_DELAY);
}

/**
 * @brief 用户shell上锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellLock(Shell *shell)
{
    xSemaphoreTakeRecursive(shellMutex, portMAX_DELAY);
    return 0;
}

/**
 * @brief 用户shell解锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellUnlock(Shell *shell)
{
    xSemaphoreGiveRecursive(shellMutex);
    return 0;
}

/**
 * @brief 用户shell初始化
 * 
 */
void userShellInit(void)
{
    shellMutex = xSemaphoreCreateRecursiveMutex();
    shell_tx_sem = xSemaphoreCreateBinary();
    shell_rx_strmbuf = xStreamBufferCreate(RX_BUF_SIZE, 1);

    shell.write = userShellWrite;
    shell.read = userShellRead;
    shell.lock = userShellLock;
    shell.unlock = userShellUnlock;
    shellInit(&shell, shellBuffer, 512);
    if (xTaskCreate(shellTask, "shell", 512, &shell, 5, NULL) != pdPASS)
    {
        printf("shell task creat failed");
    }
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, shell_rxbuf,  RX_BUF_SIZE);
}

