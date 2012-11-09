//
//  FileStream.cpp
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#include "FileStream.h"
using namespace std;

FileStream::FileStream(const char *filename)
:
mFile(filename, ios::in | ios::binary),
mBitPosition(0),
mCount(0),
mSkipFlag(false)
{
}

FileStream::~FileStream()
{
    mFile.close();
}

unsigned char FileStream::ReadByte()
{
    mBitPosition = 0;
    
    unsigned char res = 0;
    mFile.read((char*)&res, sizeof(unsigned char));
    ++mCount;
    return res;
}

unsigned short FileStream::Read2Byte()
{
    mBitPosition = 0;
    
    unsigned short res = 0;
    unsigned char upper = 0;
    unsigned char lower = 0;
    
    mFile.read((char*)&upper, sizeof(unsigned char));
    mFile.read((char*)&lower, sizeof(unsigned char));
    
    res |= upper;
    res <<= 8;
    res |= lower;
    
    mCount += 2;
    return res;
}

void FileStream::ReadNBytes(int n, unsigned char *res)
{
    for (int i=0; i<n; ++i) {
        res[i] = ReadByte();
    }
}

unsigned char FileStream::ReadNextByte()
{
    unsigned char res;
    mFile.read((char*)&res, sizeof(unsigned char));
    long pos = mFile.tellg();
    mFile.seekg(pos - 1); // back 1 byte
    return res;
}

unsigned char FileStream::ReadBits(int count)
{
    unsigned char res = 0;
    while (count != 0) {
        res <<= 1;
        if (mBitPosition == 0) {
            if (mSkipFlag) {
                ReadByte(); // skip 0x00 of 0xff00
                mSkipFlag = false;
            }
            mBitBuffer = ReadByte();
            if (mBitBuffer == 0xff) {
                if (ReadNextByte() == 0x00) {
                    mSkipFlag = true;
                }
            }
            mBitPosition = 8;
        }
        res |= ((mBitBuffer & 0x80) >> 7);
        mBitBuffer <<= 1;
        mBitPosition--;
        count--;
    }
    return res;
}

bool FileStream::IsEOF() const
{
    return mFile.eof();
}