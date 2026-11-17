#include "zf_common_headfile.h"
#include "Key.h"
#include "Servo.h"
#include "PID.h"
#include "Image.h"
#include "Motor.h"

/*
    蓝桥杯大弟子来也！！！！
*/

#define Key_1               IO_PB0
#define Key_2               IO_PB1
#define Key_3               IO_PB2
#define Key_4               IO_PB3
#define Key_5               IO_PB4

uint8 Key_Slow = 0;                 //按键缓速

uint8 Key_Value = 0,Key_Old = 0;
uint8 Key_Down = 0,Key_Up = 0;

void Key_Init()
{
    gpio_init(Key_1,GPI,1,GPI_PULL_UP);
    gpio_init(Key_2,GPI,1,GPI_PULL_UP);
    gpio_init(Key_3,GPI,1,GPI_PULL_UP);
    gpio_init(Key_4,GPI,1,GPI_PULL_UP);
    gpio_init(Key_5,GPI,1,GPI_PULL_UP);
}

uint8 Key_Read()
{
    uint8 Key_Temp = 0x00;

    if (gpio_get_level(Key_1) == 0)
        Key_Temp = 1;
    if (gpio_get_level(Key_2) == 0)
        Key_Temp = 2;
    if (gpio_get_level(Key_3) == 0)
        Key_Temp = 3;
    if (gpio_get_level(Key_4) == 0)
        Key_Temp = 4;
    if (gpio_get_level(Key_5) == 0)
        Key_Temp = 5;

    return Key_Temp;
}

int8 Motor_Temp = 10;

void Key_Process()
{
    if (Key_Slow != 0)      return;
    Key_Slow = 1;

    Key_Value = Key_Read();
    Key_Down = Key_Value & (Key_Value ^ Key_Old);                   //任何数异或0都为自己，因此括号内得到自己，“与”上自己或者对0“取反”（1111 1111）也为自己，有没有~纯看Key_Value的值
    Key_Up = ~Key_Value & (Key_Value ^ Key_Old);
    Key_Old = Key_Value;

    if (Key_Down == 1)
    {
        Angle_PID.Kp += 0.1;
        //Motor_Temp += 5;
        //Angle_PID.Kd += 1;
    }

    if (Key_Down == 2)
    {
        Angle_PID.Kp -= 0.1;
        //Motor_Temp -= 5;
        //Angle_PID.Kd -= 1;
    }

    if (Key_Down == 3)
    {
        //Angle_PID.Kp += 0.1;
        //Motor_Temp += 1;
        Angle_PID.Kd += 0.01;
    }

    if (Key_Down == 4)
    {
        //Angle_PID.Kp -= 0.1;
        //Motor_Temp -= 1;
        Angle_PID.Kd += 0.001;
    }

    if (Key_Down == 5)
    {
        /*
        if (TFT_State)
            TFT_State = 0;
        else
            TFT_State = 1;
        */
        Motor_Temp += 1;
        Motor_SetDuty(PWM_L,Motor_Temp);
         Motor_SetDuty(PWM_R,Motor_Temp);
    }

        
}

