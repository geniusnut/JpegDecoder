//
//  FileStream.h
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#ifndef __JpegDecorder__FileStream__
#define __JpegDecorder__FileStream__

#include <fstream>

class FileStream
{
public:
    FileStream(const char *filename);
    ~FileStream();
    
    unsigned char  ReadByte();
    unsigned short Read2Byte();
    void           ReadNBytes(int n, unsigned char *res);
    unsigned char  ReadNextByte();
    unsigned char  ReadBits(int count);
    bool           IsEOF() const;
    
    int mCount;
private:
    
    std::ifstream mFile;
    unsigned char mBitPosition; // for ReadBits
    unsigned char mBitBuffer; // for ReadBits
    bool mSkipFlag;
};

#endif /* defined(__JpegDecorder__FileStream__) */
