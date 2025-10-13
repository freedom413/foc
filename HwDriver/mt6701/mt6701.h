#ifndef __MT6701_H__
#define __MT6701_H__

#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"
#include "stdbool.h"
#include "math.h"
/*
 * MT6701 磁性角度编码器驱动头文件
 * 
 * MT6701 是一款基于各向异性磁阻(AMR)技术的角度位置传感器，
 * 可以测量0°到360°的旋转角度，具有高精度和高可靠性。
 */

/* MT6701 I2C地址 */
#define MT6701_I2C_ADDR                  0x06        /* 7位I2C地址 */
#define MT6701_USE_CRC

typedef struct {
    error_t (*pfn_mt6701_init)(void);
    error_t (*pfn_mt6701_deinit)(void);
    error_t (*pfn_mt6701_us_tick)(uint32_t *us);
    error_t (*pfn_mt6701_ssi_read)(uint8_t *data, uint8_t len);
    error_t (*pfn_mt6701_i2c_write)(uint8_t reg, uint8_t *data, uint8_t len);
    error_t (*pfn_mt6701_i2c_read)(uint8_t reg, uint8_t *data, uint8_t len);
} mt6701_port_t;

typedef union {
    uint32_t value;
    struct {
        uint32_t crc_value : 6;
        uint32_t field_status : 2;
        uint32_t button_detected : 1;
        uint32_t over_speed : 1;
        uint32_t raw_angle : 14;
        uint32_t reserved : 8; // 高8位，保留
    } bits;
} mt6701_frame_t;

typedef enum {
    MT6701_MODE_SSI,
    MT6701_MODE_I2C,
} mt6701_mode_t;

/*
 * MT6701 句柄结构体
 */
typedef struct {
    mt6701_port_t      *p_port;
    mt6701_frame_t      frame;
    float               angle;                  /* 当前角度值 (度) */
    float               angle_prev;             /* 上次角度值 (度) */
    uint32_t            angle_tick_us;          /* 上次调用时间 (微秒) */
    int                 angle_rotationCount;    /* 转过的圈数 */
    float               vel_angle_prev;         /* 上次角度值 (度) */
    uint32_t            vel_tick_us_prev;       /* 上次调用时间 (微秒) */
    int                 vel_rotationCount_prev; /* 上次转过的圈数 */
    bool                is_init;                /* 是否初始化 */
    mt6701_mode_t       mode;                   /* 工作模式 */
} mt6701_handle_t;


/* 初始化MT6701设备 */
error_t mt6701_init(mt6701_handle_t *hmag, mt6701_port_t *port, mt6701_mode_t mode);
/* 释放MT6701设备 */
error_t mt6701_delete(mt6701_handle_t *hmag);
/* 更新MT6701角度值 */
error_t mt6701_update(mt6701_handle_t *hmag);
/* 读取角度 */
float_t mt6701_read_abs_angle(mt6701_handle_t *hmag);
/* 读取完整角度值 */
float_t mt6701_read_full_angle(mt6701_handle_t *hmag);
/* 读取角度速度 */
float_t mt6701_read_angle_velocity(mt6701_handle_t *hmag);



#endif /* __MT6701_H__ */