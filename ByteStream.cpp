//
//  ByteStream.cpp
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#include "ByteStream.h"
#include <string>

ByteStream::ByteStream(const unsigned char *bytes, int length)
:
mBytes(0),
mLength(length),
mPosition(0)
{
    mBytes = new unsigned char[length];
    memcpy(mBytes, bytes, sizeof(unsigned char) * length);
}

unsigned char ByteStream::GetByte()
{
    unsigned char res = mBytes[mPosition];
    mPosition++;
    return res;
}

int ByteStream::GetLength() const
{
    return mLength;
}

bool ByteStream::IsEnd() const
{
    return (mPosition == mLength);
}
