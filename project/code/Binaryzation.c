#include "zf_common_headfile.h"

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


uint8 Otsu_GetThreshold(uint16 *Histogram, uint16 PixelNum)
{
    uint32 totalSum = 0;
    uint32 numBack = 0, numFore;
    uint32 sumBack = 0, sumFore;
    uint32 w0, w1;
    uint32 u0, u1;
    uint32 delta, delta2;
    uint32 variance, bestVariance = 0;
    uint16 i;
    uint16 bestLow = 0, bestHigh = 0;
    uint8  bestThreshold = 128;

    if (PixelNum == 0)
    {
        return 128;
    }

    /* 整幅图的灰度总和，只累加一次 */
    for (i = 0; i < 256; i++)
    {
        totalSum += (uint32)Histogram[i] * i;
    }

    /* threshold = i：背景为 [0, i-1]，前景为 [i, 255] */
    for (i = 1; i < 256; i++)
    {
        numBack += Histogram[i - 1];
        sumBack += (uint32)Histogram[i - 1] * (i - 1);

        if ((numBack == 0) || (numBack >= (uint32)PixelNum))
        {
            continue;
        }
        numFore = (uint32)PixelNum - numBack;
        sumFore = totalSum - sumBack;

        /* 类内均值四舍五入到整数（0~255） */
        u0 = (sumBack + (numBack >> 1)) / numBack;
        u1 = (sumFore + (numFore >> 1)) / numFore;

        /* 取绝对差，避免无符号减法回绕 */
        if (u0 > u1)
        {
            delta = u0 - u1;
        }
        else
        {
            delta = u1 - u0;
        }
        delta2 = delta * delta;                 /* 最大 255*255 = 65025 */

        /* 权重 Q8：256 表示 100%，四舍五入 */
        w0 = ((numBack * 256UL) + (PixelNum >> 1)) / PixelNum;
        w1 = 256 - w0;
        if ((w0 == 0) || (w1 == 0))
        {
            continue;
        }

        variance = w0 * w1 * delta2;            /* 最大 16384*65025 ≈ 1.07e9 */
        if (variance > bestVariance)
        {
            bestVariance = variance;
            bestLow = i;
            bestHigh = i;
        }
        else if ((variance == bestVariance) && (i == (uint16)(bestHigh + 1)))
        {
            bestHigh = i;                       /* 连续平台，记录平台右端 */
        }
    }

    /* 双峰之间的方差是平的，取平台中点，阈值更稳、边缘余量更大 */
    if (bestVariance != 0)
    {
        bestThreshold = (uint8)((bestLow + bestHigh) >> 1);
    }

    return bestThreshold;
}
