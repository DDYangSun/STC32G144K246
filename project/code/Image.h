#ifndef __IMAGE_H__
#define __IMAGE_H__

#define CompressImage       1           //1为压缩图像至94*60，2为压缩至63*40，0为不压缩

#if CompressImage == 2                              //压缩分辨率为63 * 40

    #define GrayImageArray     mt9v03x_image_Compress[0]
    #define GrayImage_Wide      63
    #define GrayImage_Height    40

#elif CompressImage == 1

    #define GrayImageArray     mt9v03x_image_Compress[0]
    #define GrayImage_Wide      94
    #define GrayImage_Height    60

#elif CompressImage == 0

    #define GrayImageArray     mt9v03x_image[0]
    #define GrayImage_Wide      188                 //分辨率为188 * 120
    #define GrayImage_Height    120

#endif

#define FilterThreshold_Max     255 * 7             //周围7个点为白色
#define FilterThreshold_Min     255 * 1             //周围7个点为黑色

// 赛道线颜色：白线黑底=255；黑线白底=0
#define TRACK_LINE_COLOR        255
// 线宽范围：太窄当噪点，太宽当背景/整行白
#define TRACK_LINE_MIN_WIDTH    2
#define TRACK_LINE_MAX_WIDTH    (GrayImage_Wide / 2)
// 至少要有几行才认为线有效
#define TRACK_LINE_MIN_ROWS     4

// 左右边线结果：按行存放，y 从 GrayImage_Height-LeftLineCount 到 GrayImage_Height-1 有效
extern uint8 LeftLinePoint[GrayImage_Height][2];
extern uint8 RightLinePoint[GrayImage_Height][2];
extern uint16 LeftLineCount;
extern uint16 RightLineCount;
extern uint8 LineValid;

void Image_Init(void);
void Image_Process(void);

#endif
