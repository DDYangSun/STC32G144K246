#ifndef __BINARYZATION_H__
#define __BINARYZATION_H__

void DrawPixelChart(uint8 *GrayImage, uint16 Length, uint16 *PixelValueNum);
uint16 Get_PixelNum(uint16 Wide, uint16 Height);
uint8 Otsu_GetThreshold(uint16 *Histogram, uint16 PixelNum);

#endif
