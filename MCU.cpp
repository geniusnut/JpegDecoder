//
//  MCU.cpp
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/13.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#include "MCU.h"
#include "SOF.h"
#include "SupportFunc.h"

MCU::MCU(const SOF &sof)
:
width(sof.maxH * 8),
height(sof.maxV * 8),
blockNumH(SupportFunc::CarrySurplus(sof.width, 8)),
blockNumV(SupportFunc::CarrySurplus(sof.height, 8)),
dataSize(sof.maxH * sof.maxV * 64)
{
    for (int i=0; i<3; ++i) {
        YCbCr[i] = new unsigned char[dataSize];
        thinOutX[i] = sof.maxH / sof.sampH[i];
        thinOutY[i] = sof.maxV / sof.sampV[i];
    }
}

MCU::~MCU()
{
    for (int i=0; i<3; ++i) {
        SupportFunc::SafeDeleteArray(YCbCr[i]);
    }
}

void MCU::InitializeComponent()
{
    memset(YCbCr[0], 0,    sizeof(unsigned char) * dataSize);
    memset(YCbCr[1], 0x80, sizeof(unsigned char) * dataSize);
    memset(YCbCr[2], 0x00, sizeof(unsigned char) * dataSize);
}

void MCU::SetBlock(int compID, int bx, int by, int *block)
{
    unsigned char *tp = YCbCr[compID] + by * width * 8 + bx * 8;
    for (int MCUy=0; MCUy<(8*thinOutY[compID]); ++MCUy) {
        for (int MCUx=0; MCUx<(8*thinOutX[compID]); ++MCUx) {
            tp[MCUy * width + MCUx] = block[(MCUy/thinOutY[compID])*8+(MCUx/thinOutX[compID])];
        }
    }
}
