//
//  Segment.h
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#ifndef JpegDecorder_Segment_h
#define JpegDecorder_Segment_h

class ByteStream;

class Segment
{
public:
    virtual void Analyze(ByteStream *bytes) = 0;
};

#endif
