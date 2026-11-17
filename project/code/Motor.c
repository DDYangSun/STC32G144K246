#include <zf_common_headfile.h>
#include "Motor.h"
#include "Key.h"

// -------------------------------------------电机---------------------------------------------------------
void Motor_Init()
{
    pwm_init(PWM_L, 17000, 0);               	// PWM 通道 L1 初始化频率 17KHz 占空比初始为 0
    pwm_init(PWM_R, 17000, 0);               	// PWM 通道 L1 初始化频率 17KHz 占空比初始为 0
    gpio_init(IO_P76, GPO, 1, GPO_PUSH_PULL);   //右轮方向IO初始化
    gpio_init(IO_P74, GPO, 0, GPO_PUSH_PULL);   //左轮方向IO初始化

    Motor_SetDuty(PWM_L,Motor_Temp);            
    Motor_SetDuty(PWM_R,Motor_Temp);

}

//设置占空比
void Motor_SetDuty(pwm_channel_enum pin,int8 duty)
{
    pwm_set_duty(pin, (duty) * (PWM_DUTY_MAX / 100));
}


// -------------------------------------------编码器---------------------------------------------------------
int16 Encoder_LeftCnt = 0,Encoder_RightCnt = 0;

void Encoder_Init()
{
    encoder_dir_init(ENCODER_Left, Left_CH1, Left_CH2);   // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_dir_init(ENCODER_Right, Right_CH1, Right_CH2);   // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_clear_count(ENCODER_Left);
    encoder_clear_count(ENCODER_Right);
}
