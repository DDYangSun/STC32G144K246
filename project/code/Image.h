#ifndef __IMAGE_H__
#define __IMAGE_H__

#define CompressImage       1           //1为压缩图像，0为不压缩图像，默认不压缩

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

void Image_Init(void);
void Image_Process(void);


#endif