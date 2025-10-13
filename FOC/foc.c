#include "foc.h"
#include "User.h"
#include "arm_math.h"
#include "mt6701.h"
#include "mt6701_port.h"
#include "stm32f4xx_hal.h"
#include <math.h>
#include <sys/_intsup.h>


#define _PI    (3.1415926f)
#define _2PI  (6.28318530718f)
#define _3PI_2 4.71238898038f
#define _constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

float voltage_power_supply=12.6;
float shaft_angle=0,open_loop_timestamp=0;
float zero_electric_angle=0,Ualpha,Ubeta=0,Ua=0,Ub=0,Uc=0,dc_a=0,dc_b=0,dc_c=0;
int DIR=1,PP=7;

// 归一化角度到 [0,2PI]
float _normalizeAngle(float angle){
    float a = fmodf(angle, _2PI);
    return a >= 0 ? a : (a + _2PI);
}

float _electricalAngle(void) {
    return  _normalizeAngle((float)(DIR *  PP) * mt6701_read_abs_angle(&hmag1) - zero_electric_angle);
}


// 设置PWM到控制器输出
void setPwm(float Ua, float Ub, float Uc) {

  // 计算占空比
  // 限制占空比从0到1
  dc_a = _constrain(Ua / voltage_power_supply, 0.f,  1.f);
  dc_b = _constrain(Ub / voltage_power_supply, 0.f , 1.f );
  dc_c = _constrain(Uc / voltage_power_supply, 0.f , 1.f );

  //写入PWM到PWM 0 1 2 通道
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, dc_a * 8400);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, dc_b * 8400);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, dc_c * 8400);
}

void setPhaseVoltage(float Uq, float angle_el) {

  Uq=_constrain(Uq,-voltage_power_supply * 0.5f,voltage_power_supply * 0.5f);
  angle_el = _normalizeAngle(angle_el);

  float32_t sin_val, cos_val;
  static const float32_t sqrt_3 = 1.73205080757f;
  float32_t temp1, temp2;
    
  // 一次调用同时计算sin和cos - 最大优化点
  arm_sin_cos_f32(angle_el * 57.2957795f, &sin_val, &cos_val);
  
  // 帕克逆变换
  Ualpha = -Uq * sin_val;
  Ubeta = Uq * cos_val;
  
  // 克拉克逆变换 - 直接计算，无函数调用
  temp1 = sqrt_3 * Ubeta;
  temp2 = voltage_power_supply * 0.5f;  // 用乘法代替除法
  
  Ua = Ualpha + temp2;
  Ub = (temp1 - Ualpha) * 0.5f + temp2;
  Uc = (-Ualpha - temp1) * 0.5f + temp2;

  setPwm(Ua,Ub,Uc);
}

// void setPhaseVoltage(float Uq, float angle_el) {

//   Uq=_constrain(Uq,-voltage_power_supply/2,voltage_power_supply/2);
//   angle_el = _normalizeAngle(angle_el);
//   // 帕克逆变换
//   Ualpha =  -Uq*sin(angle_el); 
//   Ubeta =   Uq*cos(angle_el); 

//   // 克拉克逆变换
//   Ua = Ualpha + voltage_power_supply/2;
//   Ub = (sqrt(3)*Ubeta-Ualpha)/2 + voltage_power_supply/2;
//   Uc = (-Ualpha-sqrt(3)*Ubeta)/2 + voltage_power_supply/2;
//   setPwm(Ua,Ub,Uc);
// }


void foc_alignSensor(int _PP,int _DIR)
{ 
  DIR=_DIR;
  PP=_PP;
  setPhaseVoltage(3, _3PI_2);
  HAL_Delay(1000);
  mt6701_update(&hmag1);
  zero_electric_angle=_electricalAngle();
  setPhaseVoltage(0, _3PI_2);
}

//开环速度函数
int velocityOpenloop(float target_velocity)
{   
    if (target_velocity > 100.0f || target_velocity < 0.0f) {
        return -1;
    }
    float pars = target_velocity / 100;
    float Uq = (voltage_power_supply / 2) * pars;
  
  setPhaseVoltage(Uq, _electricalAngle());
  
  return 0;
}


int posCloseloop(float pos, float kp)
{   
    float_t full_angle = mt6701_read_full_angle(&hmag1) * 180 / _PI;
    setPhaseVoltage(kp*(pos - DIR*full_angle), _electricalAngle());
    return 0;
}
