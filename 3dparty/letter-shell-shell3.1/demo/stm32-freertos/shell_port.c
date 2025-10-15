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
// #include "cevent.h"
// #include "log.h"


Shell shell;
static char shellBuffer[512];

static SemaphoreHandle_t shellMutex;
static SemaphoreHandle_t txsem;

#define RX_BUF_SIZE      128
static uint8_t rxbuf[RX_BUF_SIZE];
static StreamBufferHandle_t rxstrmbuf;


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (huart->Instance == USART1) {
        xStreamBufferSendFromISR(rxstrmbuf, rxbuf, Size, &xHigherPriorityTaskWoken);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rxbuf, RX_BUF_SIZE);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (huart->Instance == USART1) {
        xSemaphoreGiveFromISR(txsem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

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
    // serialTransmit(&debugSerial, (uint8_t *)data, len, 0x1FF);
    HAL_StatusTypeDef ret = HAL_UART_Transmit_DMA(&huart2, (uint8_t *)data, len);
    if (ret == HAL_OK) {
        if (xSemaphoreTake(txsem, portMAX_DELAY) == pdTRUE) {
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
    // return serialReceive(&debugSerial, (uint8_t *)data, len, 0);
    return xStreamBufferReceive(rxstrmbuf, data, len, portMAX_DELAY);
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
    txsem = xSemaphoreCreateBinary();
    rxstrmbuf = xStreamBufferCreate(RX_BUF_SIZE, 1);

    shell.write = userShellWrite;
    shell.read = userShellRead;
    shell.lock = userShellLock;
    shell.unlock = userShellUnlock;
    shellInit(&shell, shellBuffer, 512);
    if (xTaskCreate(shellTask, "shell", 512, &shell, 5, NULL) != pdPASS)
    {
        printf("shell task creat failed");
    }
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rxbuf,  RX_BUF_SIZE);
}
// CEVENT_EXPORT(EVENT_INIT_STAGE2, userShellInit);

