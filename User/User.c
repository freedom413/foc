
#include "User.h"
#include "math.h"
#include "shell.h"
#include "stdio.h"
#include "errno.h"
#include "mt6701_port.h"
#include "foc.h"
#include "usart.h"
#include "shell_port.h"
#include <stdint.h>
#include "uart_dbg.h"
#include "motor_pwm.h"


void setupTaskFun(void const * argument)
{
    (void)argument;
    setup();
    vTaskDelete(NULL);
}


void setup(void)
{
    userShellInit();
    mt6701_init(&hmag1, &hmag1_port, MT6701_MODE_SSI);
    motor_pwm_init();
    foc_alignSensor(7,1);
}


#define CH_COUNT (3)
float ch_data[CH_COUNT] = {0.0f};

int foc_pos = 45;
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), foc_pos, &foc_pos, foc angle pos);

int foc_vel = 20;
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), foc_vel, &foc_vel, foc angle vel);


static int mag_err_count = 0;
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), mag_err_count, &mag_err_count, mt6700 read angle erro count);


 void foc_task_fun(void *argument)
{

  for(;;)
  {
    error_t ret;

    ret = mt6701_update(&hmag1);
    if (ret) {
        mag_err_count++;
    }
  
    velocityOpenloop(foc_vel);
    osDelay(1);
  }

}


void dbug_info_fun(void *argument)
{

  for(;;)
  {
    ch_data[0] = Ua;
    ch_data[1] = Ub;
    ch_data[2] = Uc;
    // ch_data[0] = mt6701_read_abs_angle(&hmag1);
    // ch_data[1] = mt6701_read_full_angle(&hmag1);
    // ch_data[2] = mt6701_read_angle_velocity(&hmag1);
    // HAL_Delay(1);
    vofa_just_float_send((uint8_t*)ch_data, CH_COUNT*sizeof(float));
    // shellTask(&shell);
    osDelay(50);
  }
}

void loop(void)
{ 

    error_t ret;

    ret = mt6701_update(&hmag1);
    if (ret) {
        mag_err_count++;
    }
  
    velocityOpenloop(foc_vel);
    // posCloseloop(foc_pos, 0.033);
    ch_data[0] = Ua;
    ch_data[1] = Ub;
    ch_data[2] = Uc;
    // ch_data[0] = mt6701_read_abs_angle(&hmag1);
    // ch_data[1] = mt6701_read_full_angle(&hmag1);
    // ch_data[2] = mt6701_read_angle_velocity(&hmag1);
    // HAL_Delay(1);
    vofa_just_float_send((uint8_t*)ch_data, CH_COUNT*sizeof(float));
}