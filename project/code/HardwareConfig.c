#include "zf_common_headfile.h"
#include "HardwareConfig.h"
#include "Image.h"

uint16 FPS = 0;
uint8 FPS_Flag = 0;

void Hardware_Debug_Init()
{
    tft180_show_string(0,70,"Threshold:");
    tft180_show_string(0,87,"FPS:");
}

void Hardware_Init(void)
{
    Image_Init();                           //图像显示初始化
    tft180_init();                          //tft180初始化
    mt9v03x_init();                         //摄像头初始化：返回值为0则成功、非0则失败（这里不处理）
    Hardware_Debug_Init();                  //硬件调试初始化

    //开启定时器0
    pit_ms_init(TIM0_PIT, 1, pit_handler);
}

//定时器0的中断
void pit_handler()
{
    static uint16 TIM0_Count = 0;

    //计算帧数
    if (++TIM0_Count >= 1000)
    {
        FPS_Flag = 1;
		TIM0_Count = 0;
    }

}

