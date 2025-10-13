#ifndef __FOC_H__
#define __FOC_H__

int posCloseloop(float pos, float kp);
int velocityOpenloop(float target_velocity);
void foc_alignSensor(int _PP,int _DIR);
extern float Ua,Ub,Uc;
#endif