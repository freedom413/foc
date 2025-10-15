#ifndef __UART_DBG_H__
#define __UART_DBG_H__

void vofa_just_float_send(uint8_t * data, uint8_t len);
void i2c_addr_check(I2C_HandleTypeDef *hi2c);
void task_info_inc_Callback(void);
#endif