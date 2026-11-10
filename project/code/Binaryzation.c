#include "zf_common_headfile.h"
#include "Image.h"

#pragma OPTIMIZE(3,SPEED)           //加了这个有质的飞跃，没加20帧，加了33帧

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     统计灰度直方图
// 参数说明     GrayImage       灰度图像首地址
// 参数说明     Length          像素个数（MT9V03X 为 188*120 = 22560）
// 参数说明     PixelValueNum   直方图数组，长度 256
// 返回参数     void
// 备注信息     每帧统计前必须清零；uint16 计数最大 65535，当前分辨率不会溢出
//-------------------------------------------------------------------------------------------------------------------
void DrawPixelChart(uint8 *GrayImage, uint16 Length, uint16 *PixelValueNum)
{
    uint16 i;

    for (i = 0; i < 256; i++)
    {
        PixelValueNum[i] = 0;
    }

    for (i = 0; i < Length; i++)
    {
        PixelValueNum[*GrayImage]++;
        GrayImage++;
    }
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     计算像素总数
// 参数说明     Wide            图像宽度
// 参数说明     Height          图像高度
// 返回参数     uint16          像素总数
// 备注信息     MT9V03X 为 188*120 = 22560，未超过 uint16 范围
//-------------------------------------------------------------------------------------------------------------------
uint16 Get_PixelNum(uint16 Wide, uint16 Height)
{
    return Wide * Height;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     大津法（Otsu）计算二值化阈值
// 参数说明     Histogram       256 级灰度直方图
// 参数说明     PixelNum        像素总数
// 返回参数     uint8           阈值 1~255；直方图退化时返回 128
// 备注信息     前缀和实现，复杂度 O(256)；权重用 Q8 定点（256 = 100%），
//              最大类间方差约 1.07e9，uint32 不会溢出；
//              空类直接跳过，避免除零；双峰平台取中点，阈值更稳
//-------------------------------------------------------------------------------------------------------------------
uint8 Otsu_GetThreshold(uint16 *PixelValueNum,uint16 PixelNum)       //灰度直方图
{
	uint8 Otsu_Threshold = 0;
	uint32 Threshold_BackgroundNum = 0,Thereshold_ForegroundNum = 0;			            	//当前灰度值的像素数量
	uint32 Threshold_BackgroundSum = 0,Threshold_ForegroundSum = 0,GraySum = 0;		            //当前灰度值的灰度值总数
	int32 w0,w1,u0,u1;
	int32 Variance = 0,Variance_Temp = 0;						//方差
    uint8 low = 0,high = 0;                                     //取平均数的前后阈值
	uint16 i;

    /*
        遍历，算灰度值总和

        +=背景像素和
        +=背景灰度值
        w0 = 每一次从左到右+=的背景像素 / 总像素
        w1 = 1 - w0（根据放大缩小后关系来定1）

        u0 = 每一次从左到右+=的背景灰度值 / 背景的像素和
        u1 = （总灰度值 - 背景灰度值）/ （总像素和 - 背景像素和）

        公式：Variance = w0 * w1 * (u0 - u1) * (u0 - u1)
    */

    //计算灰度值之和
    for (i = 0;i < 256;i++)
    {
        GraySum += PixelValueNum[i] * i;
    }

    for (i = 1;i < 255;i++)
    {
        Threshold_BackgroundNum += PixelValueNum[i];
        Threshold_BackgroundSum += PixelValueNum[i] * i;
        Thereshold_ForegroundNum = PixelNum - Threshold_BackgroundNum;
        Threshold_ForegroundSum = GraySum - Threshold_BackgroundSum;

        //四舍五入
        w0 = ((Threshold_BackgroundNum * 256UL) + (PixelNum >> 1)) / PixelNum;
        w1 = 256 - w0;

        //防止一瞬间的阈值错乱
        if (w0 == 0 || w1 == 0)
            continue;

        u0 = (Threshold_BackgroundSum + (Threshold_BackgroundNum >> 1)) / Threshold_BackgroundNum;
        u1 = (Threshold_ForegroundSum + (Thereshold_ForegroundNum >> 1)) / Thereshold_ForegroundNum;
 
        //防止一瞬间的阈值错乱
        if (u0 == 0 || u1 == 0)
            continue;
        
        Variance_Temp = w0 * w1 * (u0 - u1) * (u0 - u1);

        if (Variance_Temp > Variance)
        {
            Variance = Variance_Temp;
            low = high = i;
        }
        else if (Variance_Temp == Variance && i == (uint16)(high + 1))
        {
            high = i;
        }  
    }

    //取平均数
    Otsu_Threshold = (low + high) >> 1;

    return Otsu_Threshold;
}

//二值化
void ImageBinaryzation(uint8 GrayImage[GrayImage_Height][GrayImage_Wide],uint8 Threshold,uint8 GrayImage_Bina[GrayImage_Height][GrayImage_Wide])
{
	uint16 i,j;

    for (i = 0;i < GrayImage_Height;i++)
    {
        for (j = 0;j < GrayImage_Wide;j++)
        {
            if (GrayImage[i][j] < Threshold)
                GrayImage_Bina[i][j] = 0;
            else
                GrayImage_Bina[i][j] = 0xFF;
        }
    }
}
