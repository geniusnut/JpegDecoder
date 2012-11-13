//
//  JpegDecoder.cpp
//  JpegDecoder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#include "JpegDecoder.h"
#include "ByteStream.h"
#include "Config.h"
#include <cmath>
#include <fstream>
using namespace std;

// --- support functions declare ----
int sCarrySurplus(int number, int divide);

template <typename T> void sSafeDeleteArray(T *array);

unsigned char sRevise0to255(double num);
// --- END: support functions declare ----

JpegDecoder::JpegDecoder(const char *filename)
:
mFileStream(filename),
mRestartInterval(0)
{
    // create a cos table for iDCT
    for (int u=0; u<8; ++u) {
        for (int x=0; x<8; ++x) {
            mCosTable[u][x] = std::cos((2*x + 1) * u * 3.1415926535 / 16.0);
        }
    }
    
    for (int i=0; i<3; ++i) {
        mYCbCr[i] = 0;
        mRGB[i]   = 0;
        mPreDC[i] = 0;
    }
}

JpegDecoder::~JpegDecoder()
{
    for (int i=0; i<3; ++i) {
        sSafeDeleteArray<unsigned char>(mYCbCr[i]);
        sSafeDeleteArray<unsigned char>(mRGB[i]);
    }
}

void JpegDecoder::Decode()
{
    bool flag = false;
    while (!mFileStream.IsEOF()) {
        unsigned char mark = mFileStream.ReadByte();
        if (mark != 0xff) {
            continue;
        }
        
        mark = mFileStream.ReadByte();
        switch(mark) {
            case markSOI:
                if (flag) {
                    skipThumbnail();
                } else {
                    flag = true;
                }
                break;
            case markEOI:
            case 0:
                break;
            case markDRI:
                mFileStream.Read2Byte(); // length
                mRestartInterval = mFileStream.Read2Byte();
                break;
            case markDQT:
                cout << "DQT" << mFileStream.mCount << endl;
                analyzeSegment(&mDQT);
                mDQT.ShowConsole();
                break;
            case markDHT:
                cout << "DHT" << mFileStream.mCount << endl;
                analyzeSegment(&mDHT);
                break;
            case markSOF0:
                cout << "SOF" << mFileStream.mCount << endl;
                analyzeSegment(&mSOF);
                break;
            case markSOS:
                cout << "SOS" << mFileStream.mCount << endl;
                analyzeSegment(&mSOS);
                decodeData();
                cout << "END" << mFileStream.mCount << endl;
                break;
        } // switch(mark)
    } // while(!mFileStream.IsEOF)
}

void JpegDecoder::GetRGB(unsigned char *byte) const
{
    for (int i=0; i<mSOF.height * mSOF.width; ++i) {
        byte[3*i + 0] = mRGB[0][i];
        byte[3*i + 1] = mRGB[1][i];
        byte[3*i + 2] = mRGB[2][i];
    }
}

int JpegDecoder::GetWitdh() const
{
    return mSOF.width;
}

int JpegDecoder::GetHeight() const
{
    return mSOF.height;
}

void JpegDecoder::analyzeSegment(Segment *seg)
{
    unsigned short length = mFileStream.Read2Byte() - 2;
    unsigned char *bytes = new unsigned char[length];
    mFileStream.ReadNBytes(length, bytes);
    ByteStream bs(bytes, length);
    seg->Analyze(&bs);
    delete[] bytes;
}

void JpegDecoder::skipThumbnail()
{
    unsigned char forward = 0;
    unsigned mark = 0;
    while (mark != markEOI) {
        forward = mFileStream.ReadByte();
        if (forward == 0xff) {
            mark = mFileStream.ReadByte();
        }
    }
}

void JpegDecoder::decodeData()
{
    int blockNumV = sCarrySurplus(mSOF.height, 8);
    int blockNumH = sCarrySurplus(mSOF.width , 8);
    mUnitSize = mSOF.maxH * mSOF.maxV * 64;
    for (int i=0; i<3; ++i) {
        mRGB[i] = new unsigned char[mSOF.height * mSOF.width];
        mYCbCr[i] = new unsigned char[mUnitSize];
    }
    
    int MCUNumV = sCarrySurplus(blockNumV, mSOF.maxV);
    int MCUNumH = sCarrySurplus(blockNumH, mSOF.maxH);
    int MCUCount = 0;
    for (int y=0; y<MCUNumV; ++y) {
        for (int x=0; x<MCUNumH; ++x) {
            decodeMCU();
            ++MCUCount;
            YCbCr2RGB(x, y);
            
            if (!mRestartInterval) {
                continue;
            }
            if (mRestartInterval == MCUCount) {
				MCUCount = 0;
				if (mFileStream.ReadByte() == 0xff) {
					int rst = mFileStream.ReadNextByte();
					if (rst >= 0xd0 && rst <= 0xd7) {
						mFileStream.ReadByte();
					}
				}
            }
        }
    }
    for (int i=0; i<3; ++i) {
        sSafeDeleteArray(mYCbCr[i]);
    }
}

void JpegDecoder::decodeMCU()
{
    for (int i=0; i<3; ++i) {
        if (i != 0) {
            memset(mYCbCr[i], 0x80, sizeof(unsigned char) * mUnitSize);
        } else {
            memset(mYCbCr[i], 0x00, sizeof(unsigned char) * mUnitSize);
        }
        int cntY = mSOF.maxV / mSOF.sampV[i];
        int cntX = mSOF.maxH / mSOF.sampH[i];
        int stepV = mSOF.maxH * 8;
        
        for (int y=0; y<mSOF.sampV[i]; ++y) {
            for (int x=0; x<mSOF.sampH[i]; ++x) {
                decodeBlock(i);

                // Too chaotic. Refactor this.
                unsigned char *tmp = mYCbCr[i] + y * stepV * 8 + x * 8;
                for (int MCUy=0; MCUy<(8*cntY); ++MCUy) {
                    for (int MCUx=0; MCUx<(8*cntX); ++MCUx) {
                        tmp[MCUy*stepV+MCUx] = mWorkingBlock[(MCUy/cntY)*8+(MCUx/cntX)];
                    }
                }
            }
        }
    }
}

void JpegDecoder::decodeBlock(int compID)
{
    memset(mWorkingBlock, 0, sizeof(int) * 64);
    
    // DC and AC
    decodeDC(compID);
    decodeAC(compID);
    
    // iDQT
    for (int i=0; i<64; ++i) {
        mWorkingBlock[i] *= mDQT.tables[mSOF.quantID[compID]][i];
    }
    
    // iDCT
    int iDCT[64];
    for (int y=0; y<8; ++y) {
        for (int x=0; x<8; ++x) {
            
            double sum = 0.0;
            for (int v=0; v<8; ++v) {
                double cv = ((v != 0) ? 1.0 : invSqrt2);
                for (int u=0; u<8; ++u) {
                    double cu = ((u != 0) ? 1.0 : invSqrt2);
                    sum += cu * cv * mWorkingBlock[v*8 + u] * mCosTable[u][x] * mCosTable[v][y];
                } //vu
            } // v
            iDCT[y*8 + x] = (int)(sum / 4.0 + 128);
            
        } // x
    } // y
	for (int i=0; i<64; ++i)  {
		mWorkingBlock[i] = iDCT[i];
	}
}

void JpegDecoder::decodeDC(int compID)
{
    const HuffmanTable &huff = mDHT(DC, mSOS(DC, compID));
    
    int bitnum = decodeHuffCode(huff);
    
    int diff = mFileStream.ReadBits(bitnum);
    // if the top bit of diff is 0, it's negative
    if ((diff & (1 << (bitnum - 1))) == 0) {
        // reverse sign
        diff -= (1 << bitnum) - 1;
    }
    mPreDC[compID] += diff;
    mWorkingBlock[0] = mPreDC[compID];
}

void JpegDecoder::decodeAC(int compID)
{
    const HuffmanTable &huff = mDHT(AC, mSOS(AC, compID));
    
    for (int i=1; i<64; ++i) {

        int run_bit = decodeHuffCode(huff);
        
        if (run_bit == 0) { // EOB
            while (i < 64) {
                mWorkingBlock[JpgZigzag[i]] = 0;
                ++i;
            }
            return;
        }
        
        int run = run_bit >> 4;
        int bit = run_bit & 0x0f;

        if (run + i > 63) {
            cout << mFileStream.mCount << endl;
        }
        while (run > 0) { // run length
            mWorkingBlock[JpgZigzag[i]] = 0;
            ++i;
            --run;
        }
		int a = mFileStream.ReadBits(bit);
        if ((a & (1 << (bit - 1))) == 0) {
            a -= (1 << bit) - 1;
        }
		mWorkingBlock[JpgZigzag[i]] = a;
    }
}

int JpegDecoder::decodeHuffCode(const HuffmanTable &huffTable)
{
    int bitnum = -1;
    unsigned char length = 0;
    unsigned short code = 0;
    while (bitnum < 0) {
        code <<= 1;
        unsigned char tmp = mFileStream.ReadBits(1);
        code |= tmp;
        ++length;
        bitnum = huffTable.GetData(length, code);
    }
    return bitnum;
}

void JpegDecoder::YCbCr2RGB(int x, int y)
{
    
    int lineNum = y * mSOF.maxV * 8;
    int offsetV = lineNum * mSOF.width;
    int offsetH = x * 8 * mSOF.maxH;
    int offset = offsetV + offsetH;
    int endX = mSOF.maxH * 8;
    int endY = mSOF.maxV * 8;
    
    unsigned char *pY  = mYCbCr[0];
    unsigned char *pCb = mYCbCr[1];
    unsigned char *pCr = mYCbCr[2];
    
    unsigned char *pR = mRGB[0] + offset;
    unsigned char *pG = mRGB[1] + offset;
    unsigned char *pB = mRGB[2] + offset;
    
    for (int picY=0; picY<endY; ++picY) {
        for (int picX=0; picX<endX; ++picX) {
            if (picX + offsetH >= mSOF.width) {
                pY  += endX - picX;
                pCb += endX - picX;
                pCr += endX - picX;
                break;
            }
            int index = picY * mSOF.width + picX;
 
            double v1 = *pY + (*pCr - 0x80) * 1.4020;
            pR[index] = sRevise0to255(v1);
            double v2 = *pY - (*pCb - 0x80) * 0.34414 - (*pCr - 0x80) * 0.71414;
            pG[index] = sRevise0to255(v2);
            double v3 = *pY + (*pCb - 0x80) * 1.77200;
            pB[index] = sRevise0to255(v3);
            pY++;
            pCb++;
            pCr++;
        }
    }
}


// --- support functions definition ---
int sCarrySurplus(int number, int divide)
{
    return number / divide + ((number % divide) ? 1 : 0);
}


template <typename T> void sSafeDeleteArray(T *array)
{
    if (array) {
        delete[] array;
        array = 0;
    }
}

unsigned char sRevise0to255(double num)
{
    if (num > 255.0) {
        return 255;
    }
    if (num < 0.0) {
        return 0;
    }
    return (unsigned char)num;
}
// --- END: support functions definition ---
