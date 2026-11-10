#include "zf_common_headfile.h"
#include "Binaryzation.h"
#include "Image.h"

uint16 PixelNum;                                                            //像素总数
uint16 PixelValueNum[256];                                                  //灰度直方图

uint8 mt9v03x_image_Compress[GrayImage_Height][GrayImage_Wide];             //压缩后像素数组

uint8 LeftLinePos = 0,RightLinePos = 255;                                     //左右边界点


void Image_Init()
{
    PixelNum = Get_PixelNum(MT9V03X_W, MT9V03X_H);                         // 分辨率固定，只算一次

}

// 函数简介     将摄像头传输图像压缩至63 * 40分辨率
// 参数说明     GrayImage               灰度图像首地址
// 参数说明     GrayImage_Compress      压缩后灰度图像首地址
// 返回参数     void
// 备注信息     每帧统计前必须清零；188 * 120 变成 63 * 40
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


// 函数简介     将摄像头传输图像压缩至94 * 60分辨率
// 参数说明     GrayImage               灰度图像首地址
// 参数说明     GrayImage_Compress      压缩后灰度图像首地址
// 返回参数     void
// 备注信息     每帧统计前必须清零；188 * 120 变成 94 * 60
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


// 邻域白点数量阈值，0~8
#define THR_MAX 6    // 邻域白点≥6，中心黑点翻转成白点
#define THR_MIN 2    // 邻域白点≤2，中心白点翻转成黑点

/*
// 全局临时缓存，不要放栈里，省时间、防栈溢出
xdata uint8 FilterTemp[GrayImage_Height][GrayImage_Wide];

void ErosionFilter(uint8 img[GrayImage_Height][GrayImage_Wide])
{
    uint16 i,j;
    uint8 cnt;
    for (i = 1; i < GrayImage_Height - 1; i++)
    {
        for (j = 1; j < GrayImage_Wide - 1; j++)
        {
            // 统计八邻域里面255的个数，代替加法
            cnt = 0;
            if(img[i-1][j-1]) cnt++;
            if(img[i-1][j])   cnt++;
            if(img[i-1][j+1]) cnt++;
            if(img[i][j-1])   cnt++;
            if(img[i][j+1])   cnt++;
            if(img[i+1][j-1]) cnt++;
            if(img[i+1][j])   cnt++;
            if(img[i+1][j+1]) cnt++;

            FilterTemp[i][j] = img[i][j]; // 默认不变

            if(cnt >= THR_MAX && img[i][j] == 0)
            {
                FilterTemp[i][j] = 255;
            }
            else if(cnt <= THR_MIN && img[i][j] == 255)
            {
                FilterTemp[i][j] = 0;
            }
        }
    }
    // 回写
    for (i = 1; i < GrayImage_Height - 1; i++)
    {
        for (j = 1; j < GrayImage_Wide - 1; j++)
        {
            img[i][j] = FilterTemp[i][j];
        }
    }
}
*/
/*
//八邻域腐蚀滤波
void ErosionFilter(uint8 GrayImage[GrayImage_Height][GrayImage_Wide])
{
    uint16 i,j;

    for (i = 1;i < GrayImage_Height - 1;i++)
    {
        for (j = 1;j < GrayImage_Wide - 1;j++)
        {
            if (GrayImage[i - 1][j - 1] + GrayImage[i - 1][j] + GrayImage[i - 1][j + 1] + GrayImage[i][j - 1]
                + GrayImage[i][j + 1] + GrayImage[i + 1][j - 1] + GrayImage[i + 1][j] + GrayImage[i + 1][j + 1] > FilterThreshold_Max 
                && GrayImage[i][j] == 0)
            {
                GrayImage[i][j] = 255;
            }
            else if (GrayImage[i - 1][j - 1] + GrayImage[i - 1][j] + GrayImage[i - 1][j + 1] + GrayImage[i][j - 1]
            + GrayImage[i][j + 1] + GrayImage[i + 1][j - 1] + GrayImage[i + 1][j] + GrayImage[i + 1][j + 1] < FilterThreshold_Min 
            && GrayImage[i][j] == 255)
            {
                GrayImage[i][j] = 0;
            }
        }

    }

}
*/

//方向向量
int8 Dir[8][2] = {
{0,1},      //上
{1,1},      //右上
{1,0},      //右
{1,-1},     //右下
{0,-1},     //下
{-1,-1},    //左下
{-1,0},     //左
{-1,1}      //左上
};

//寻找最长白列
uint8 FindMaxWhiteLine(uint8 mt9v03x_image[GrayImage_Height][GrayImage_Wide])
{
    uint8 Count_Max = 0,Count = 0;
    uint8 CountMaxPos = 0;

    uint16 i = 0,j = 0;
    for (i = 0;i < GrayImage_Wide;i++)
    {
        j = 0;
        Count = 0;
        while (mt9v03x_image[GrayImage_Height - 1 - j][i] == 255 && j < GrayImage_Height - 1)
        {
            Count++;
            j++;
        }
        if (Count > Count_Max)
        {
            Count_Max = Count;
            CountMaxPos = i;
        }
    }

    return CountMaxPos;
}

//寻找左右边界点
void FindBoundaryPoint(uint8 mt9v03x_image[GrayImage_Height][GrayImage_Wide],uint8 StartPos)
{
    uint8 i = 0;

    //向左找边界点
    i = 0;
    while (mt9v03x_image[GrayImage_Height - 1][StartPos - i] == 255 && i < StartPos)
    {
        i++;
    }
    LeftLinePos = StartPos - i + 1;

    //向右找边界点
    i = 0;
    while (mt9v03x_image[GrayImage_Height - 1][StartPos + i] == 255 && i < GrayImage_Wide - StartPos - 1)
    {
        i++;
    }
    RightLinePos = StartPos + i - 1;
}

//爬线
uint8 LeftLinePoint[GrayImage_Height][2],RightLinePoint[GrayImage_Height][2];   //左右边线坐标值
void FindLeftBoundary_ClimbLine(uint8 mt9v03x_image[GrayImage_Height][GrayImage_Wide])
{
    int8 x0 = 0,y0 = 0;
    int8 x1 = 0,y1 = 0;    
    uint8 i = 0,j = 0;

    LeftLinePoint[0][0] = LeftLinePos;
    LeftLinePoint[0][1] = GrayImage_Height - 1;

    while (j < GrayImage_Height - 1)       //这里小于等于的是边线数组长度       j = 0时存的是1的值，也要考虑数组长度，那么最大值应该是GrayImage_Height - 2
    {
        for (i = 0;i < 8;i++)               //顺时针旋转
        {
            x0 = (int8)(LeftLinePoint[j][0]) + Dir[(6 + i) % 8][0];
            y0 = (int8)(LeftLinePoint[j][1]) + Dir[(6 + i) % 8][1];

            if (x0 < 0 || y0 < 0)
                continue;
            
            x1 = (int8)(LeftLinePoint[j][0]) + Dir[(6 + i + 1) % 8][0];
            y1 = (int8)(LeftLinePoint[j][1]) + Dir[(6 + i + 1) % 8][1];

            if (x1 < 0 || y1 < 0)
                continue;
            
            if (mt9v03x_image[y0][x0] == 0 && mt9v03x_image[y1][x1] == 255)
            {
                break;
            }
        }
        j++;
        if (LeftLinePoint[j][1] <= LeftLinePoint[j - 1][1])
        {
            LeftLinePoint[j][0] = (uint8)x1;
            LeftLinePoint[j][1] = (uint8)y1;
        }
    }
}

//八邻域算法寻找边线
void FindBoundary_EightNeighborhood(uint8 mt9v03x_image_Eight[GrayImage_Height][GrayImage_Wide])
{
    uint8 MaxWhiteLinePos = 0;

    //寻找左右边界点
    MaxWhiteLinePos = FindMaxWhiteLine(mt9v03x_image_Eight[0]);
    FindBoundaryPoint(mt9v03x_image_Eight[0],MaxWhiteLinePos);

    //爬线
    FindLeftBoundary_ClimbLine(mt9v03x_image_Eight[0]);
}
uint8 Temp;
void Image_Process(void)
{
    uint8 ImageThreshold;

    //压缩图像(后续可以在后面写else if更新压缩分辨率)
    #if CompressImage == 2
    
        //压缩图像至63 * 40
        GrayImage_Compress_63_40(mt9v03x_image[0],GrayImageArray);         
        
    #else if CompressImage == 1
 
        //压缩图像至94 * 60
        GrayImage_Compress_94_60(mt9v03x_image[0],GrayImageArray);         

    #endif

    DrawPixelChart(GrayImageArray, GrayImage_Wide * GrayImage_Height, PixelValueNum);                        //转换成灰度直方图
    ImageThreshold = Otsu_GetThreshold(PixelValueNum, GrayImage_Wide * GrayImage_Height);                    //获取大津法阈值
    ImageBinaryzation(GrayImageArray,ImageThreshold,mt9v03x_image_Compress[0]);                              //二值化（占用很多帧率，看看还能不能优化）
    //ErosionFilter(mt9v03x_image_Compress[0]);                                                              //八邻域腐蚀滤波

    FindBoundary_EightNeighborhood(mt9v03x_image_Compress[0]);                                                                 //寻找左右边界点

    //显示二值化图像，这个不用动了，是为了跟最大的分辨率区分，除非也有溢出tft的显示区域再加
    #if CompressImage    
    
        tft180_show_gray_image(0, 0, mt9v03x_image_Compress[0], GrayImage_Wide, GrayImage_Height, GrayImage_Wide, GrayImage_Height, 0);
        
    #elif

        tft180_show_gray_image(0, 0, mt9v03x_image_Compress[0], GrayImage_Wide, GrayImage_Height, GrayImage_Wide / 2, GrayImage_Height / 2, 0);

    #endif        

    /*调试大津法阈值
    tft180_show_uint8(80, 70, ImageThreshold);
    */
    
    /*调试最长白列
    tft180_show_uint8(0, 102, FindMaxWhiteLine(GrayImageArray));
    tft180_draw_point((uint16)FindMaxWhiteLine(GrayImageArray),59,RGB565_RED);
    tft180_draw_point((uint16)FindMaxWhiteLine(GrayImageArray),58,RGB565_RED);
    tft180_draw_point((uint16)FindMaxWhiteLine(GrayImageArray),57,RGB565_RED);
    */

    /*调试左右边界点
    tft180_draw_point((uint16)LeftLinePos,59,RGB565_RED);
    tft180_draw_point((uint16)LeftLinePos,58,RGB565_RED);
    tft180_draw_point((uint16)LeftLinePos,57,RGB565_RED);
    tft180_draw_point((uint16)RightLinePos,59,RGB565_RED);
    tft180_draw_point((uint16)RightLinePos,58,RGB565_RED);
    tft180_draw_point((uint16)RightLinePos,57,RGB565_RED);
    */
    tft180_show_uint8(0, 102, LeftLinePoint[50][0]);
    tft180_show_uint8(0, 118, LeftLinePoint[50][1]);
    
    for (Temp = 0;Temp < GrayImage_Height;Temp++)
    {
        tft180_draw_point((uint16)LeftLinePoint[Temp][0],(uint16)LeftLinePoint[Temp][1],RGB565_RED);
        //mt9v03x_image_Compress[LeftLinePoint[Temp][0]][LeftLinePoint[Temp][1]] = 128;
    }
}


