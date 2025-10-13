
#include "User.h"
#include "math.h"
#include "stdio.h"
#include "errno.h"
#include "mt6701_port.h"
#include "foc.h"
#include "usart.h"
#include "shell_port.h"
#include <stdint.h>
#include "uart_dbg.h"



void setup(void)
{
    userShellInit();
    mt6701_init(&hmag1, &hmag1_port, MT6701_MODE_SSI);
    HAL_TIM_Base_Start(&htim1);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);

    foc_alignSensor(7,1);
}


#define CH_COUNT (3)
float ch_data[CH_COUNT] = {0.0f};

int foc_pos = 45;
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), foc_pos, &foc_pos, foc angle pos);

int mag_err_count = 0;
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), mag_err_count, &mag_err_count, mt6700 read angle erro count);

// char str[] = "test string";
// SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_STRING), varStr, str, test);

// Log log;
// SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_POINT), log, &log, test);

void loop(void)
{ 

    error_t ret = mt6701_update(&hmag1);
    if (ret) {
        // printf("mt6701_update error: %d\r\n", ret);
        mag_err_count++;
    }   
    velocityOpenloop(20);
    // posCloseloop(foc_pos, 0.033);
    ch_data[0] = Ua;
    ch_data[1] = Ub;
    ch_data[2] = Uc;
    // ch_data[0] = mt6701_read_abs_angle(&hmag1);
    // ch_data[1] = mt6701_read_full_angle(&hmag1);
    // ch_data[2] = mt6701_read_angle_velocity(&hmag1);

    vofa_just_float_send((uint8_t*)ch_data, CH_COUNT*sizeof(float));
}