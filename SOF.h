//
//  SOF.h
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#ifndef __JpegDecorder__SOF__
#define __JpegDecorder__SOF__

#include "Segment.h"

class ByteStream;

struct SOF : public Segment
{
    void Analyze(ByteStream *bytes);
    void ShowConsole();

    int width;
    int height;
    unsigned char sampV[3];
    unsigned char sampH[3];
    unsigned char quantID[3];
    unsigned char maxV;
    unsigned char maxH;
};

#endif /* defined(__JpegDecorder__SOF__) */
