/**
 * @file shell_port.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#ifndef __SHELL_PORT_H__
#define __SHELL_PORT_H__

#include "shell.h"
#include "semphr.h"
#include "stream_buffer.h"

#define RX_BUF_SIZE      128

extern Shell shell;
extern SemaphoreHandle_t shell_tx_sem;
extern uint8_t shell_rxbuf[RX_BUF_SIZE];
extern StreamBufferHandle_t shell_rx_strmbuf;

void userShellInit(void);
#endif
