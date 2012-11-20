//
//  MCU.h
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/13.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#ifndef __JpegDecorder__MCU__
#define __JpegDecorder__MCU__

#include <iostream>

struct SOF;

struct MCU
{
    MCU(const SOF &sof);
    ~MCU();
    void InitializeComponent();
    void SetBlock(int compID, int bx, int by, int *block);

    int width;
    int height;
    int blockNumH;
    int blockNumV;
    int dataSize;
    unsigned char *YCbCr[3];
    int thinOutX[3];
    int thinOutY[3];
};


#endif /* defined(__JpegDecorder__MCU__) */
