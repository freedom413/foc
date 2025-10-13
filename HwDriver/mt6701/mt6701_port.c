#include "mt6701_port.h"
#include "main.h"
#include "shell_port.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"
#include <errno.h>
#include <stdint.h>


static int mag_err_count = 0;
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), mag_err_count, &mag_err_count, mt6700 read angle erro count);


static error_t __hmag1_ssi_read(uint8_t *data, uint8_t len)
{   
    uint32_t timeoutcount = 200;
    while (HAL_SPI_GetState(&hspi3) != HAL_SPI_STATE_READY)
    {
        if (timeoutcount-- == 0)
        {   
            mag_err_count++;
            return -EBUSY; // 在超时时直接返回，避免继续执行后续代码
        }
    }

    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef ret = HAL_SPI_Receive(&hspi3, data, len, 100);
    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);
    if (ret != HAL_OK) {
        return -EIO;
    }
    return 0;
}

static error_t __hmag1_i2c_read(uint8_t reg, uint8_t *data, uint8_t len)
{
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(&hi2c1, MT6701_I2C_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return -EIO;
    }
    return 0;
}

static error_t __hmag1_i2c_write(uint8_t reg, uint8_t *data, uint8_t len)
{
   HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(&hi2c1, MT6701_I2C_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return -EIO;
    }
    return 0;
}

static error_t __hmag1_us_tick(uint32_t *us)
{
    *us = HAL_GetTick() * 1000u;
    return 0;
}

static error_t __hmag1_init(void)
{
    return 0;
}

static error_t __hmag1_deinit(void)
{
    return 0;
}

mt6701_port_t hmag1_port = {
    .pfn_mt6701_init = __hmag1_init,
    .pfn_mt6701_deinit = __hmag1_deinit,
    .pfn_mt6701_us_tick = __hmag1_us_tick,
    .pfn_mt6701_ssi_read = __hmag1_ssi_read,
    .pfn_mt6701_i2c_read = __hmag1_i2c_read,
    .pfn_mt6701_i2c_write = __hmag1_i2c_write,
};

mt6701_handle_t hmag1;

