#include "zf_common_headfile.h"
#include "HardwareConfig.h"
#include "Image.h"
#include "Servo.h"
#include "Key.h"
#include "PID.h"

uint16 FPS = 0;
uint8 FPS_Flag = 0;

uint8 TIM0_ShowTFT = 0;                     //TFT显示标志位

void Hardware_Debug_Init()
{
    tft180_show_string(0,70,"Threshold:");
    tft180_show_string(0,87,"FPS:");
}

void Hardware_Init(void)
{
    Key_Init();                             //按键初始化
    Image_Init();                           //图像显示初始化
    tft180_init();                          //tft180初始化
    mt9v03x_init();                         //摄像头初始化：返回值为0则成功、非0则失败（这里不处理）
    Servo_Init();                           //舵机初始化
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

void TFT_ShowData(uint8 TIM0_ShowTFT)
{
    if (TIM0_ShowTFT)
    {
        tft180_show_uint16(0,134,Angle_Set);
        tft180_show_float(40,134,Angle_PID.Kp,2,3);
        tft180_show_float(0,108,Angle_PID.Error0,3,3);
        TIM0_ShowTFT = 0;
    }
}

//定时器0的中断
void pit_TIM0()
{
    static uint16 TIM0_Count = 0;
    
    //缓速系统
    if (++Key_Slow >= 10)       Key_Slow = 0;

    //计算帧数
    if (++TIM0_Count >= 1000)
    {
        TIM0_ShowTFT = 1;
        FPS_Flag = 1;
		TIM0_Count = 0;
    }
}

