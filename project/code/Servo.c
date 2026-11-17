#include "zf_common_headfile.h"
#include "Servo.h"
#include "PID.h"

uint8 Angle_Set = 95;

void Servo_Init()                               //舵机初始化
{
    pwm_init(SERVO_PWM,SERVO_FREQ,0);
}

void Servo_Ctrl_Angle(uint8 Servo_Angle)                               //舵机控制
{
    pwm_set_duty(SERVO_PWM,SERVO_DUTY(Servo_Angle));
}

void Servo_Ctrl_Duty(uint16 Servo_Duty)                               //舵机控制
{
    pwm_set_duty(SERVO_PWM,Servo_Duty);
}

void Servo_AngleRestriction()
{
    if (Angle_Set >= 111)
        Angle_Set = 111;
    if (Angle_Set <= 79)
        Angle_Set = 79;
}

void Servo_Process()
{
    Angle_PID_Process();
    Servo_AngleRestriction();
    Servo_Ctrl_Angle(Angle_Set);
}