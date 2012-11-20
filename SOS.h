//
//  SOS.h
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#ifndef __JpegDecorder__SOS__
#define __JpegDecorder__SOS__

#include "Segment.h"

class ByteStream;

struct SOS : public Segment
{
    void Analyze(ByteStream *bytes);
    unsigned char operator() (int DCAC, int compID);
    void ShowConsole();

    unsigned char huffmanID[6]; // DC or AC of 3 components
};

#endif /* defined(__JpegDecorder__SOS__) */
