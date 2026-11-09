#include "zf_common_headfile.h"
#include "Binaryzation.h"
#include "Image.h"

uint16 PixelNum;
uint16 PixelValueNum[256];                                                 //灰度直方图

uint8 GrayImage_Bina[GrayImage_Height][GrayImage_Wide];                     //二值化后像素数组
uint8 mt9v03x_image_Compress[GrayImage_Height][GrayImage_Wide];             //压缩后像素数组

void Image_Init()
{
    PixelNum = Get_PixelNum(MT9V03X_W, MT9V03X_H);                         // 分辨率固定，只算一次

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     将摄像头传输图像压缩至更低分辨率
// 参数说明     GrayImage               灰度图像首地址
// 参数说明     GrayImage_Compress      压缩后灰度图像首地址
// 返回参数     void
// 备注信息     每帧统计前必须清零；188 * 120 变成 63 * 40
//-------------------------------------------------------------------------------------------------------------------
void GrayImage_Compress(uint8 GrayImage[MT9V03X_H][MT9V03X_W], uint8 GrayImage_Compress[MT9V03X_H / 3][MT9V03X_W / 3 + 1])
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


// 邻域白点数量阈值，0~8
#define THR_MAX 6    // 邻域白点≥6，中心黑点翻转成白点
#define THR_MIN 2    // 邻域白点≤2，中心白点翻转成黑点

// 全局临时缓存，不要放栈里，省时间、防栈溢出
uint8 FilterTemp[GrayImage_Height][GrayImage_Wide];

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

void Image_Process(void)
{
    uint8 ImageThreshold;

    //压缩图像(后续可以在后面写else if更新压缩分辨率)
    #if CompressImage    
    
        //压缩图像至63 * 40
        GrayImage_Compress(mt9v03x_image[0],GrayImageArray);         
        
    #endif

    DrawPixelChart(GrayImageArray, GrayImage_Wide * GrayImage_Height, PixelValueNum);                        //转换成灰度直方图
    ImageThreshold = Otsu_GetThreshold(PixelValueNum, GrayImage_Wide * GrayImage_Height);                    //获取大津法阈值
    ImageBinaryzation(GrayImageArray,ImageThreshold,GrayImage_Bina[0]);                                      //二值化
    ErosionFilter(GrayImage_Bina[0]);                                                                        //八邻域腐蚀滤波


    //显示二值化图像，这个不用动了，是为了跟最大的分辨率区分，除非也有溢出tft的显示区域再加
    #if CompressImage    
    
        tft180_show_gray_image(0, 0, GrayImage_Bina[0], GrayImage_Wide, GrayImage_Height, GrayImage_Wide, GrayImage_Height, 0);
        
    #else

        tft180_show_gray_image(0, 0, GrayImage_Bina[0], GrayImage_Wide, GrayImage_Height, GrayImage_Wide / 2, GrayImage_Height / 2, 0);

    #endif        

    tft180_show_uint8(80, 70, ImageThreshold);
}