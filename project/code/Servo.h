#ifndef __SERVO_H__
#define __SERVO_H__

#define SERVO_PWM                   PWME_CH1P_PA0
#define SERVO_FREQ                  (250)

//#define SERVO_DUTY(x)           PWM_DUTY_MAX * (0.5 + 1 / 90 * x) * SERVO_Freq / 1000                 防止浮点运算
#define SERVO_DUTY(x)           ((uint32)PWM_DUTY_MAX * (45 + (x)) * SERVO_FREQ / 1000 / 90)                    //扩大90倍再除以90

#define SERVO_MiddlePos             95

void Servo_Init(void);
void Servo_Ctrl_Angle(uint8 Servo_Angle);
void Servo_Ctrl_Duty(uint16 Servo_Duty);
void Servo_Process();

extern uint8 Angle_Set;

#endif