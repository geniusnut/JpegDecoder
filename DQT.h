//
//  DQT.h
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#ifndef JpegDecorder_DQT_h
#define JpegDecorder_DQT_h

#include "Segment.h"

class ByteStream;

typedef struct S_DQTArray : public Segment
{
    void Analyze(ByteStream *byteStream);
    void ShowConsole() const;
    
    unsigned char tables[4][64];
} DQT;

#endif
