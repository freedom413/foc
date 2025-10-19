
#include "FreeRTOS.h"
#include "main.h"
#include "mt6701.h"
#include "semphr.h"
#include "mt6701_port.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "stream_buffer.h"
#include "user.h"
#include "shell_port.h"
#include "uart_dbg.h"
#include "foc.h"

/*
 * @brief 串口空闲中断接收完成事件回调
 * 
 * @param huart 串口句柄
 * @param Size 接收数据长度
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (huart->Instance == USART2) {
        xStreamBufferSendFromISR(shell_rx_strmbuf, shell_rxbuf, Size, &xHigherPriorityTaskWoken);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, shell_rxbuf, RX_BUF_SIZE);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


/**
 * @brief 串口发送完成回调
 * 
 * @param huart 串口句柄
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (huart->Instance == USART2) {
        xSemaphoreGiveFromISR(shell_tx_sem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


int foc_mod = 1;
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), foc_mod, &foc_mod, 1:vel 2:pos);

int foc_pos = 45;
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), foc_pos, &foc_pos, foc angle pos);

int foc_vel = 20;
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), foc_vel, &foc_vel, foc angle vel);
static int mag_err_count = 0;
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), mag_err_count, &mag_err_count, mt6700 read angle erro count);

/**
 * @brief 定时器周期结束回调
 * 
 * @param htim 定时器句柄
 */
void TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{   
    if (htim->Instance == TIM1) {
        extern volatile long long FreeRTOSRunTimeTicks;
        FreeRTOSRunTimeTicks++;
    }
    else if (htim->Instance == TIM7) {
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);
    error_t ret = 0;
    ret = mt6701_update(&hmag1);
    if (ret) {
        mag_err_count++;
    }
    if (foc_mod == 1) {
        velocityOpenloop(foc_vel);
    }else {
        posCloseloop(foc_pos, 0.088f);
    }


    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET);
    }
}


void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI3) {

    }
}