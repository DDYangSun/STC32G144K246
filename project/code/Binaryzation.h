#ifndef __BINARYZATION_H__
#define __BINARYZATION_H__

#include "Image.h"

void DrawPixelChart(uint8 *GrayImage, uint16 Length, uint16 *PixelValueNum);
uint16 Get_PixelNum(uint16 Wide, uint16 Height);
uint8 Otsu_GetThreshold(uint16 *PixelValueNum,uint16 PixelNum);
void ImageBinaryzation(uint8 GrayImage[GrayImage_Height][GrayImage_Wide],uint8 Threshold,uint8 GrayImage_Bina[GrayImage_Height][GrayImage_Wide]);

#endif
