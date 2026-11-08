#include "zf_common_headfile.h"
#include "zf_device_tft180.h"

//制作像素值数量图表
void DrawPixelChart(uint8 *GrayImage,uint16 Length,uint16 *PixelValueNum)
{
	uint16 i;

	for(i = 0;i < 256;i++)		//每帧统计前必须先清零，否则直方图会一帧帧累加，uint16 几帧就溢出
	{
		PixelValueNum[i] = 0;
	}

	for(i = 0;i < Length;i++)
	{
		PixelValueNum[*GrayImage]++;
		GrayImage++;
	}
}

uint16 Get_PixelNum(uint16 Wide,uint16 Height)
{
    return Wide * Height;
}

uint8 Otsu_GetThreshold(uint16 *GrayImage,uint16 PixelNum)       //灰度直方图
{
	uint8 Otsu_Threshold = 0;
	uint32 Threshold_BackgroundNum = 0,Thereshold_ForegroundNum = 0;				//前景背景的像素数量
	uint32 Threshold_BackgroundSum = 0,Threshold_ForegroundSum = 0;		//灰度值
	int32 w0,w1,u0,u1;
	int32 Variance = 0,Variance_Temp = 0;						//方差

	uint16 i,j;

	for (i = 1;i < 255;i++)
	{
		for (j = 0;j < 256;j++)
		{
			if (j < i)
			{
				Threshold_BackgroundNum += GrayImage[j];			//计算背景灰度值数量之和，用于计算背景权重
				Threshold_BackgroundSum += GrayImage[j] * j;
			}
			else
			{
				Thereshold_ForegroundNum += GrayImage[j];			//计算前景灰度值数量之和，用于计算前景权重				
				Threshold_ForegroundSum += GrayImage[j] * j;
			}

		}
		w0 = (Threshold_BackgroundNum * 100) / PixelNum;
		w1 = 100 - w0;	

		u0 = Threshold_BackgroundSum / Threshold_BackgroundNum;
		u1 = Threshold_ForegroundSum / Thereshold_ForegroundNum;

		Variance_Temp = w0 * w1 * (u0 - u1) * (u0 - u1);

		if (Variance_Temp > Variance)
		{
			Otsu_Threshold = i;
            
			Variance = Variance_Temp;
		}

		Threshold_BackgroundNum = Thereshold_ForegroundNum = Threshold_BackgroundSum = Threshold_ForegroundSum = 0;
	}

    tft180_show_uint8(0,70,Otsu_Threshold);
	return Otsu_Threshold;
}

