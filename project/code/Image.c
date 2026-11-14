#include "zf_common_headfile.h"
#include "Binaryzation.h"
#include "Image.h"

#pragma OPTIMIZE(3,SPEED)

uint16 PixelNum;                                                            //像素总数
uint16 PixelValueNum[256];                                                  //灰度直方图

uint8 mt9v03x_image_Compress[GrayImage_Height][GrayImage_Wide];             //二值化/压缩后像素数组

uint8 LeftLinePoint[GrayImage_Height][2];                                   //左边线：按行存 x/y
uint8 RightLinePoint[GrayImage_Height][2];                                  //右边线：按行存 x/y
uint16 LeftLineCount = 0;                                                   //左边线有效行数
uint16 RightLineCount = 0;                                                  //右边线有效行数
uint8 LineValid = 0;                                                        //本帧线是否有效
uint16 LastCenter = 0;                                                      //上一帧底线中心，作为本帧起始搜索点
uint8 LeftLinePos = 0,RightLinePos = 255;                                   //底线左右边界（调试用）

void Image_Init(void)
{
    PixelNum = Get_PixelNum(MT9V03X_W, MT9V03X_H);                          // 分辨率固定，只算一次
    LastCenter = GrayImage_Wide / 2;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     将摄像头传输图像压缩至63 * 40分辨率
// 参数说明     GrayImage               灰度图像首地址
// 参数说明     GrayImage_Compress      压缩后灰度图像首地址
// 返回参数     void
// 备注信息     188 * 120 变成 63 * 40
//-------------------------------------------------------------------------------------------------------------------
void GrayImage_Compress_63_40(uint8 GrayImage[MT9V03X_H][MT9V03X_W], uint8 GrayImage_Compress[MT9V03X_H / 3][MT9V03X_W / 3 + 1])
{
    uint16 i,j;
    uint16 u = 0,v = 0;

    for (i = 0;i < MT9V03X_H;i += 3)
    {
        for (j  = 0;j < MT9V03X_W;j += 3)
        {
            GrayImage_Compress[u][v] = GrayImage[i][j];
            v++;
        }
        v = 0;
        u++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     将摄像头传输图像压缩至94 * 60分辨率
// 参数说明     GrayImage               灰度图像首地址
// 参数说明     GrayImage_Compress      压缩后灰度图像首地址
// 返回参数     void
// 备注信息     188 * 120 变成 94 * 60
//-------------------------------------------------------------------------------------------------------------------
void GrayImage_Compress_94_60(uint8 GrayImage[MT9V03X_H][MT9V03X_W], uint8 GrayImage_Compress[MT9V03X_H / 2][MT9V03X_W / 2])
{
    uint16 i,j;
    uint16 u = 0,v = 0;

    for (i = 0;i < MT9V03X_H;i += 2)
    {
        for (j  = 0;j < MT9V03X_W;j += 2)
        {
            GrayImage_Compress[u][v] = GrayImage[i][j];
            v++;
        }
        v = 0;
        u++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     在一行里找“离 center 最近、宽度合法”的赛道线游程
// 参数说明     image       二值图
// 参数说明     y           行号
// 参数说明     center      期望中心（上一行/上一帧）
// 参数说明     left/right  输出游程左右边界
// 返回参数     uint8       1-找到 0-没有
// 备注信息     只做行内扫描，不跨行跟边界，因此不会沿着横线跑
//-------------------------------------------------------------------------------------------------------------------
static uint8 FindRunInRow(uint8 image[GrayImage_Height][GrayImage_Wide], uint16 y, uint16 center, uint16 *left, uint16 *right)
{
    uint16 x = 0, x0, x1, width, runCenter, dist, bestDist = 0xFFFF;
    uint8 found = 0;

    while (x < GrayImage_Wide)
    {
        if (image[y][x] == TRACK_LINE_COLOR)
        {
            x0 = x;
            while ((x + 1 < GrayImage_Wide) && (image[y][x + 1] == TRACK_LINE_COLOR))
            {
                x++;
            }
            x1 = x;
            width = x1 - x0 + 1;

            // 太窄是噪点，太宽是背景/整行白，都不算赛道线
            if ((width >= TRACK_LINE_MIN_WIDTH) && (width <= TRACK_LINE_MAX_WIDTH))
            {
                runCenter = (x0 + x1) >> 1;
                dist = (runCenter > center) ? (runCenter - center) : (center - runCenter);
                if (dist < bestDist)
                {
                    bestDist = dist;
                    *left = x0;
                    *right = x1;
                    found = 1;
                }
            }
        }
        x++;
    }

    return found;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     按行扫描赛道线
// 参数说明     image       二值图
// 返回参数     void
// 备注信息     底行找线，然后逐行往上找最近的合法游程；找不到就停，不再填垃圾点
//-------------------------------------------------------------------------------------------------------------------
static void FindLine(uint8 image[GrayImage_Height][GrayImage_Wide])
{
    uint16 y, left, right, center;

    LineValid = 0;
    LeftLineCount = 0;
    RightLineCount = 0;

    // 底行：从上一帧中心附近找线
    if (!FindRunInRow(image, GrayImage_Height - 1, LastCenter, &left, &right))
    {
        return;
    }

    LeftLinePos = (uint8)left;
    RightLinePos = (uint8)right;
    LeftLinePoint[GrayImage_Height - 1][0] = (uint8)left;
    LeftLinePoint[GrayImage_Height - 1][1] = GrayImage_Height - 1;
    RightLinePoint[GrayImage_Height - 1][0] = (uint8)right;
    RightLinePoint[GrayImage_Height - 1][1] = GrayImage_Height - 1;
    LeftLineCount = 1;
    RightLineCount = 1;
    center = (left + right) >> 1;
    LastCenter = center;

    // 逐行往上
    for (y = GrayImage_Height - 1; y > 0; y--)
    {
        if (!FindRunInRow(image, y - 1, center, &left, &right))
        {
            break;
        }

        LeftLinePoint[y - 1][0] = (uint8)left;
        LeftLinePoint[y - 1][1] = y - 1;
        RightLinePoint[y - 1][0] = (uint8)right;
        RightLinePoint[y - 1][1] = y - 1;
        LeftLineCount++;
        RightLineCount++;
        center = (left + right) >> 1;
    }

    if (LeftLineCount < TRACK_LINE_MIN_ROWS)
    {
        return;
    }

    LineValid = 1;
}

void Image_Process(void)
{
    uint8 ImageThreshold;
    uint16 Temp;

    // 压缩图像（0 表示不压缩；这里必须用 #elif，不能用 #else if）
#if CompressImage == 2
    GrayImage_Compress_63_40(mt9v03x_image[0],GrayImageArray);
#elif CompressImage == 1
    GrayImage_Compress_94_60(mt9v03x_image[0],GrayImageArray);
#endif

    DrawPixelChart(GrayImageArray, GrayImage_Wide * GrayImage_Height, PixelValueNum);                        //转换成灰度直方图
    ImageThreshold = Otsu_GetThreshold(PixelValueNum, GrayImage_Wide * GrayImage_Height);                    //获取大津法阈值
    ImageBinaryzation(GrayImageArray,ImageThreshold,mt9v03x_image_Compress[0]);                              //二值化

    FindLine(mt9v03x_image_Compress);                                                                         //按行扫描左右边线

    //显示二值化图像
#if CompressImage
    tft180_show_gray_image(0, 0, mt9v03x_image_Compress[0], GrayImage_Wide, GrayImage_Height, GrayImage_Wide, GrayImage_Height, ImageThreshold);
#else
    tft180_show_gray_image(0, 0, mt9v03x_image_Compress[0], GrayImage_Wide, GrayImage_Height, GrayImage_Wide / 2, GrayImage_Height / 2, ImageThreshold);
#endif

    //调试：只画有效点，且画之前做坐标检查，避免 tft180_draw_point 的 zf_assert 死循环
    if (LineValid)
    {
        uint16 drawX, drawY;

        for (Temp = GrayImage_Height - LeftLineCount; Temp < GrayImage_Height; Temp++)
        {
            drawX = LeftLinePoint[Temp][0];
            drawY = LeftLinePoint[Temp][1];

#if CompressImage == 0
            drawX >>= 1;
            drawY >>= 1;
#endif

            if ((drawX < tft180_x_max) && (drawY < tft180_y_max))
            {
                tft180_draw_point(drawX, drawY, RGB565_RED);
            }
        }
        for (Temp = GrayImage_Height - LeftLineCount; Temp < GrayImage_Height; Temp++)
        {
            drawX = LeftLinePoint[Temp][0] - 1;
            drawY = LeftLinePoint[Temp][1];

#if CompressImage == 0
            drawX >>= 1;
            drawY >>= 1;
#endif

            if ((drawX < tft180_x_max) && (drawY < tft180_y_max))
            {
                tft180_draw_point(drawX, drawY, RGB565_RED);
            }
        }
    }
}
