#include "zf_common_headfile.h"
#include "HardwareConfig.h"
#include "Image.h"
#include "Servo.h"
#include "Key.h"
#include "Motor.h"
#include "PID.h"

uint16 FPS = 0;
uint8 FPS_Flag = 0;

uint8 TIM0_ShowTFTData = 0;                     //TFT显示标志位

void Hardware_Debug_Init()
{
    if (TFT_State)
    {
        tft180_show_string(0,70,"Threshold:");
        tft180_show_string(0,87,"FPS:");
    }
}

void Hardware_Init(void)
{
    Key_Init();                             //按键初始化
    Image_Init();                           //图像显示初始化
    tft180_init();                          //tft180初始化
    mt9v03x_init();                         //摄像头初始化：返回值为0则成功、非0则失败（这里不处理）
    Servo_Init();                           //舵机初始化
    Motor_Init();                           //电机初始化
    Encoder_Init();                         //编码器初始化
    Hardware_Debug_Init();                  //硬件调试初始化

    //开启定时器0
    pit_ms_init(TIM0_PIT, 1, pit_TIM0);
}

void TFT_ShowFPS()
{
    if (FPS_Flag)
    {
        tft180_show_uint16(65,87,FPS);
		FPS = 0;
        FPS_Flag = 0;
    }
}

void TFT_ShowData()
{
    if (TIM0_ShowTFTData == 1)
    {
        // tft180_show_uint16(0,134,Angle_Set);
        // tft180_show_float(40,134,Angle_PID.Kp,2,3);
        // tft180_show_float(0,108,Angle_PID.Error0,3,3);
        //tft180_show_int8(0,71,Motor_Temp);
        tft180_show_float(0,87,Angle_PID.Kp,2,4);
        tft180_show_float(0,103,Angle_PID.Kd,2,4);
        tft180_show_float(0,119,Angle_PID.Error0,3,3);
        tft180_show_int16(0,135,Encoder_LeftCnt);
        tft180_show_int16(50,135,Encoder_RightCnt);

        TIM0_ShowTFTData = 0;
    }
}

//定时器0的中断
void pit_TIM0()
{
    static uint16 TIM0_Count_FPS = 0;                   //FPS计时
    static uint16 TIM0_Count_TFTShow = 0;               //编码器计时
    static uint16 TIM0_Count_Encoder = 0;               //编码器计时

    //缓速系统
    if (++Key_Slow >= 10)       Key_Slow = 0;

    //计算帧数
    if (++TIM0_Count_FPS >= 1000)
    {
        FPS_Flag = 1;
		TIM0_Count_FPS = 0;
    }

    //TFT显示数据
    if (++TIM0_Count_TFTShow >= 20)
    {
        TIM0_ShowTFTData = 1;
        TIM0_Count_TFTShow = 0;
    }

    //编码器计数
    if (++TIM0_Count_Encoder >= 20)
    {
        Encoder_LeftCnt = -encoder_get_count(ENCODER_Left);
        Encoder_RightCnt = encoder_get_count(ENCODER_Right);
        encoder_clear_count(ENCODER_Left);
        encoder_clear_count(ENCODER_Right);
        TIM0_Count_Encoder = 0;
    }
}

