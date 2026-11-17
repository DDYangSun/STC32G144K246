#ifndef __MOTOR_H__
#define __MOTOR_H__

#define PWM_L               ( PWMB_CH2_P75 )
#define PWM_R               ( PWMB_CH4_P77 )

#define ENCODER_Right                 	(PWMA_ENCODER)              // 带方向编码器对应使用的编码器接口 
#define Right_CH1                    	(PWMA_ENCODER_CH1P_P60)     // PULSE 对应的引脚
#define Right_CH2                       (PWMA_ENCODER_CH2P_P62)     // DIR 对应的引脚

#define ENCODER_Left                 	(PWMC_ENCODER)              // 带方向编码器对应使用的编码器接口
#define Left_CH1   		                (PWMC_ENCODER_CH1P_P40)     // PULSE 对应的引脚
#define Left_CH2       	                (PWMC_ENCODER_CH2P_P42)     // DIR 对应的引脚

extern int16 Encoder_LeftCnt,Encoder_RightCnt;

void Motor_Init();
void Motor_SetDuty(pwm_channel_enum pin,int8 duty);
void Encoder_Init();

#endif

