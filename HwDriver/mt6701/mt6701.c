#include "mt6701.h"

/* MT6701 寄存器地址定义 */
#define MT6701_REG_ANGLE_H_REG          0x03  /* 角度值高字节寄存器 */
#define MT6701_REG_ANGLE_H_POS          0x00  /* 角度值高字节位置 */
#define MT6701_REG_ANGLE_H_MASK         0xFF  /* 角度值高字节掩码 */

#define MT6701_REG_ANGLE_L_REG          0x04  /* 角度值低字节寄存器 */
#define MT6701_REG_ANGLE_L_POS          0x02  /* 角度值低字节位置 */
#define MT6701_REG_ANGLE_L_MASK         0xFC  /* 角度值低字节掩码 */


#define MT6701_SSI_BYTE                 0x03  /* SSI读取字节数 */
#define MT6701_I2C_BYTE                 0x02  /* I2C读取字节数 */

#define __PI                              (3.1415926f)    /* 圆周率（弧度或者角度） */
#define __2PI                             (6.2831853f)    /* 2倍圆周率（弧度或者角度） */

#ifdef MT6701_USE_CRC
/**
  * @brief  用于 MT6701 的 CRC-6 校验
  * @param  data: 待计算CRC的数据数组
  * @param  length: 数据长度
  * @retval 6位的CRC校验值
  */
static uint8_t __crc6_itu(uint8_t *data, uint32_t length)
{  
    uint8_t i;  
    uint8_t crc = 0;        // 初始值  
    while(length--) {  
        crc ^= *data++;     // crc ^= *data; data++;  
        for (i = 6; i > 0; --i) { 
            if (crc & 0x20) {
                crc = (crc << 1) ^ 0x03; // 多项式: 0x03 (x^6 + x + 1)
            }
            else {
                crc = (crc << 1);
            }
        }
    }
    return (crc & 0x3f);     // 返回6位的CRC值
}
#endif

/* SSI读取角度函数 */
static error_t __mt6701_read_angle(mt6701_handle_t *hmag)
{
    if (!hmag->is_init) {
        return -EINVAL;
    }
    if (hmag->mode == MT6701_MODE_SSI) {
        uint8_t data[MT6701_SSI_BYTE] = {0};
        if (hmag->p_port->pfn_mt6701_ssi_read(data, MT6701_SSI_BYTE)) {
            return -EIO;
        }
        uint32_t raw_data = ((uint32_t)data[0] << 16 | (uint32_t)data[1] << 8 | (uint32_t)data[2]);
#ifdef MT6701_USE_CRC
        uint8_t  crc_data [3];
        uint8_t  crc_res;
        uint8_t  crc;
        crc_data[0] = raw_data >> 18; 
        crc_data[1] = raw_data >> 12;
        crc_data[2] = raw_data >> 6;
        crc_res = raw_data & 0x3f;

        crc = __crc6_itu(crc_data, MT6701_SSI_BYTE);
        if (crc != crc_res) {
            return -ENOEXEC;
        }
#endif
        hmag->frame.value = raw_data;
    }
    else if (hmag->mode == MT6701_MODE_I2C) {
        uint8_t data[MT6701_I2C_BYTE] = {0};
        if (hmag->p_port->pfn_mt6701_i2c_read(MT6701_REG_ANGLE_H_REG, data, MT6701_I2C_BYTE)) {
            return -EIO;
        }
        uint16_t raw_data = ((uint32_t)data[0] << 8 | (uint32_t)data[1]);
        hmag->frame.bits.raw_angle = raw_data >> 2;
    }
    else {
        return -EINVAL;
    }

    hmag->angle = (float)hmag->frame.bits.raw_angle / 16383.0f * __2PI;

    return 0;
}



error_t mt6701_update(mt6701_handle_t *hmag)
{
    error_t ret;
    ret = __mt6701_read_angle(hmag);
    if (ret) {
        return ret;
    }
    ret = hmag->p_port->pfn_mt6701_us_tick(&hmag->angle_tick_us);
    if (ret) {
        return ret;
    }
    float deltaAngle = hmag->angle - hmag->angle_prev;
    if (fabs(deltaAngle) > (0.8f * __2PI)) {
        hmag->angle_rotationCount += (deltaAngle > 0) ? -1 : 1;
    }

    hmag->angle_prev = hmag->angle;
    return 0;
}

float_t mt6701_read_abs_angle(mt6701_handle_t *hmag)
{
    return hmag->angle_prev;
}

float_t mt6701_read_full_angle(mt6701_handle_t *hmag)
{
    return (float)hmag->angle_rotationCount * __2PI + hmag->angle_prev;
}


float_t mt6701_read_angle_velocity(mt6701_handle_t *hmag)
{
    float Ts = (hmag->angle_tick_us - hmag->vel_tick_us_prev)*1e-6;
    if(Ts <= 0) {
        Ts = 1e-3f;
    }
    float vel = (((hmag->angle_rotationCount - hmag->vel_rotationCount_prev) * __2PI) +  (hmag->angle_prev - hmag->vel_angle_prev)) / Ts;
    hmag->vel_rotationCount_prev = hmag->angle_rotationCount;
    hmag->vel_angle_prev = hmag->angle_prev;
    hmag->vel_tick_us_prev = hmag->angle_tick_us;
    return vel;
}


error_t mt6701_init(mt6701_handle_t *hmag, mt6701_port_t *port, mt6701_mode_t mode)
{   
    error_t ret;
    if (   hmag == NULL 
        || port == NULL 
        || port->pfn_mt6701_init == NULL
        || port->pfn_mt6701_deinit == NULL
        || port->pfn_mt6701_us_tick == NULL) {
        return -EINVAL;
    }
    if (mode == MT6701_MODE_SSI && port->pfn_mt6701_ssi_read == NULL) {
            return -EINVAL;
    }

    if (mode == MT6701_MODE_I2C && (port->pfn_mt6701_i2c_read == NULL || port->pfn_mt6701_i2c_write == NULL)) {
            return -EINVAL;
    }

    memset(hmag, 0, sizeof(mt6701_handle_t));
    hmag->p_port = port;
    hmag->mode = mode;
    ret = hmag->p_port->pfn_mt6701_init();
    if (ret) {
        return ret;
    }
    hmag->is_init = true;
    ret = hmag->p_port->pfn_mt6701_us_tick(&hmag->vel_tick_us_prev);
    if (ret) {
        return ret;
    }
    hmag->angle_tick_us = hmag->vel_tick_us_prev;
    ret = __mt6701_read_angle(hmag);
    if (ret) {
        return ret;
    }
    hmag->vel_angle_prev = hmag->angle;
    hmag->angle_prev = hmag->angle;

    return 0;
}

error_t mt6701_delete(mt6701_handle_t *hmag)
{
    if (hmag == NULL || hmag->p_port->pfn_mt6701_deinit == NULL) {
        return -EINVAL;
    }
    if (hmag->p_port->pfn_mt6701_deinit()) {
        return -EIO;
    }
    memset(hmag, 0, sizeof(mt6701_handle_t));
    hmag->is_init = false;
    return 0;
}