//
// Created by zhouxi on 7/10/2018.
//

#ifndef DJICAMERAVIDEOTOPICTUREPROJECT_CONVERTUTIL_H
#define DJICAMERAVIDEOTOPICTUREPROJECT_CONVERTUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  Bmp_FileHeader_Size    14
#define  Bmp_Info_Size    40

typedef unsigned char  uint8_t;

void convertNV12ToYV12(uint8_t * nv12bytes, uint8_t * i420bytes, int width, int height);

bool convertYV12ToBGR24_Table(uint8_t * pYUV, uint8_t * pBGR24,int width,int height);

void createBMPHeader(uint8_t * header, int width ,int height, int bgr_size);


#endif //DJICAMERAVIDEOTOPICTUREPROJECT_CONVERTUTIL_H
