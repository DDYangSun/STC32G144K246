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
        Key_Process();                              //按键控制
        Servo_Process();                            //角度环PID舵机控制

        TFT_ShowFPS();                              //TFT显示帧率
        TFT_ShowData();                 //TFT显示其他数据

        if (mt9v03x_finish_flag)
        {
            Image_Process();                        //图像处理
            mt9v03x_finish_flag = 0;
            FPS++;
        }
    }
}

