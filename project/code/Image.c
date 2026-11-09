#include "zf_common_headfile.h"
#include "Binaryzation.h"
#include "Image.h"

#define CompressImage       1           //1为压缩图像，0为不压缩图像，默认不压缩

uint16 PixelNum;
uint16 PixelValueNum[256];

uint8 mt9v03x_image_Compress[MT9V03X_H / 3][MT9V03X_W / 3 + 1];

void Image_Init()
{
    PixelNum = Get_PixelNum(MT9V03X_W, MT9V03X_H);  // 分辨率固定，只算一次

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


void Image_Process(void)
{
    uint8 ImageThreshold;

    #if CompressImage       

        //压缩图像
        GrayImage_Compress(mt9v03x_image[0],mt9v03x_image_Compress[0]);         //压缩图像
        DrawPixelChart(mt9v03x_image_Compress[0], 63 * 40, PixelValueNum);      //转换成灰度直方图
        ImageThreshold = Otsu_GetThreshold(PixelValueNum, 63 * 40);             //获取大津法阈值
        tft180_show_gray_image(0, 0, mt9v03x_image_Compress[0], 63, 40, 63, 40, ImageThreshold);        //显示图像在tft180

    #else

        //没压缩图像
        DrawPixelChart(mt9v03x_image[0], PixelNum, PixelValueNum);
        ImageThreshold = Otsu_GetThreshold(PixelValueNum, PixelNum);
        tft180_show_gray_image(0, 0, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W / 2, MT9V03X_H / 2, ImageThreshold);

    #endif

    tft180_show_uint8(80, 70, ImageThreshold);
}