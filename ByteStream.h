//
//  ByteStream.h
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#ifndef __JpegDecorder__ByteStream__
#define __JpegDecorder__ByteStream__


class ByteStream
{
public:
    ByteStream(const unsigned char *bytes, int length);
    unsigned char GetByte();
    int GetLength() const;
    bool IsEnd() const;

private:
    unsigned char *mBytes;
    int mLength;
    int mPosition;
};

#endif /* defined(__JpegDecorder__ByteStream__) */
