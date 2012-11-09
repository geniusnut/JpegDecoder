//
//  JpegDecorder.h
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#ifndef __JpegDecorder__JpegDecorder__
#define __JpegDecorder__JpegDecorder__

#include <iostream>
#include "DQT.h"
#include "DHT.h"
#include "SOF.h"
#include "SOS.h"
#include "FileStream.h"

class JpegDecorder
{
public:
    JpegDecorder(const char *filename);
    ~JpegDecorder();
    
    void Decode();
    void GetRGB(unsigned char *byte) const;
    int  GetWitdh() const;
    int  GetHeight() const;
    
private:
    void analyzeSegment(Segment *seg);
    void skipThumbnail();
    void decodeData();
    void decodeMCU();
    void decodeBlock(int compID);
    void decodeDC(int compID);
    void decodeAC(int compID);
    unsigned char decodeHuffCode(const HuffmanTable &huffTable);
    void YCbCr2RGB(int x, int y);
    
    DQT           mDQT;
    DHT           mDHT;
    SOF           mSOF;
    SOS           mSOS;
    FileStream    mFileStream;
    unsigned char mRestartInterval;
    double        mCosTable[8][8];

    unsigned char *mRGB[3];
    unsigned char *mYCbCr[3];
    unsigned char mWorkingBlock[64];
    
    unsigned char mPreDC[3];
};
#endif /* defined(__JpegDecorder__JpegDecorder__) */
