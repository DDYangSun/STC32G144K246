#include "zf_common_headfile.h"
#include "HardwareConfig.h"
#include "Image.h"
#include "Servo.h"
#include "Key.h"

void main(void)
{
    clock_init(SYSTEM_CLOCK_96M);                   // 时钟配置及系统初始化<务必保留>
    debug_init();                                   // 调试串口信息初始化
    Hardware_Init();

    while(1)
    {
        Servo_Process();
        Key_Process();
        if (FPS_Flag)
        {
            tft180_show_uint16(65,87,FPS);
			FPS = 0;
            FPS_Flag = 0;
        }

        if (mt9v03x_finish_flag)
        {
            Image_Process();
            mt9v03x_finish_flag = 0;
            FPS++;
        }
    }
}

