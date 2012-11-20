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
#include "SupportFunc.h"
#include <cmath>
#include <fstream>
using namespace std;

JpegDecoder::JpegDecoder(const char *filename)
:
mFileStream(filename),
mRestartInterval(0),
mMCU(0)
{
    // create a cos table for iDCT
    for (int u=0; u<8; ++u) {
        for (int x=0; x<8; ++x) {
            mCosTable[u][x] = cos((2*x + 1) * u * 3.1415926535 / 16.0);
        }
    }

    for (int i=0; i<3; ++i) {
        mRGB[i]   = 0;
        mPreDC[i] = 0;
    }
}

JpegDecoder::~JpegDecoder()
{
    for (int i=0; i<3; ++i) {
        SupportFunc::SafeDeleteArray<unsigned char>(mRGB[i]);
    }
}


/*
 * Decode Jpeg Format Image to RGB data.
 * What this function does is:
 * 1. checking markers appearing
 * 2. analyzing each segment when each marker appears.
 * 3. decoding image sequence with 2. data.
 */
void JpegDecoder::Decode()
{
    // for thumbnai
    bool skipflag = false;

    while (!mFileStream.IsEOF()) {
        unsigned char mark = mFileStream.ReadByte();
        if (mark != 0xff) {
            continue;
        }

        mark = mFileStream.ReadByte();
        // APP, and COM segments are skipped
        switch(mark) {
            case markSOI:              // SOI (Start Of Image)
                // TODO: skip checking with information of an APP segment
                if (skipflag) {
                    skipThumbnail();
                } else {
                    skipflag = true;
                }
                break;
            case markEOI:              // EOI (End Of Image)
            case 0:                    // escape for 0xff
                break;
            case markDRI:              // DRI (Define Restart Interval)
                mFileStream.Read2Byte(); // length
                mRestartInterval = mFileStream.Read2Byte();
                break;
            case markDQT:              // DQT (Define Quantization Table)
                analyzeSegment(&mDQT);
                break;
            case markDHT:              // DHT (Define Huffman Table)
                analyzeSegment(&mDHT);
                break;
            case markSOF0:             // SOF (Start Of Frame)
                analyzeSegment(&mSOF);
                break;
            case markSOS:              // SOS (Start Of Scan)
                analyzeSegment(&mSOS);
                // After sos segments, image data sequence starts
                decodeData();
                break;
        } // switch(mark)
    } // while(!mFileStream.IsEOF)
}

//--- Getter -----
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
//--- END: Getter -----

/*
 * This function is to analyze given segments and get information
 *
 * 1. read a segment length
 * 2. read data sequence of this segment
 * 3. create a ByteStream instance
 * 4. analyze the ByteStream instance
 */
void JpegDecoder::analyzeSegment(Segment *seg)
{
    unsigned short length = mFileStream.Read2Byte() - 2;
    unsigned char *bytes = new unsigned char[length];
    mFileStream.ReadNBytes(length, bytes);
    ByteStream bs(bytes, length);
    seg->Analyze(&bs);
    delete[] bytes;
}

// Decoder skips data until the EOI marker appears
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

/*
 * This function decodes image data bit sequence.
 * data
 *  |- MCU
 *      |- 8x8 block
 *      |- 8x8 block
 *        :
 *  |- MCU
 *        :
 *
 * If a restart intarval is defined, then it resets mPreDC.
 */
void JpegDecoder::decodeData()
{
    mMCU = new MCU(mSOF);
    int MCUNumV = SupportFunc::CarrySurplus(mMCU->blockNumV, mSOF.maxV);
    int MCUNumH = SupportFunc::CarrySurplus(mMCU->blockNumH, mSOF.maxH);

    for (int i=0; i<3; ++i) {
        mRGB[i] = new unsigned char[mMCU->blockNumH * mMCU->blockNumV * 64];
    }

    int MCUCount = 0;
    for (int y=0; y<MCUNumV; ++y) {
        for (int x=0; x<MCUNumH; ++x) {
            decodeMCU();
            ++MCUCount;
            MCU2RGB(x, y);

            if (!mRestartInterval) {
                continue;
            }
            if (mRestartInterval == MCUCount) {
				MCUCount = 0;
				if (mFileStream.ReadByte() == 0xff) {
					int rst = mFileStream.ReadNextByte();
					if (rst >= 0xd0 && rst <= 0xd7) {
						mFileStream.ReadByte();
                        mPreDC[0] = mPreDC[1] = mPreDC[2] = 0;
					}
				}
            }
        }
    }

    if (!mMCU) {
        delete mMCU;
        mMCU = 0;
    }
}

void JpegDecoder::decodeMCU()
{
    mMCU->InitializeComponent();
    for (int i=0; i<3; ++i) {
        for (int y=0; y<mSOF.sampV[i]; ++y) {
            for (int x=0; x<mSOF.sampH[i]; ++x) {
                decodeBlock(i);
                mMCU->SetBlock(i, x, y, mWorkingBlock);
            }
        }
    }
}

/*
 * This function decodes one 8x8 block.
 * The decode flow is:
 * 1. DC huffman decoding of 1 component
 * 2. AC huffman decoding of 63 components
 * 3. inverse quantization
 * 4. inverse DCT conversion
 */
void JpegDecoder::decodeBlock(int compID)
{
    memset(mWorkingBlock, 0, sizeof(int) * 64);

    // DC and AC
    decodeDC(compID);
    decodeAC(compID);

    // inverse Quantization
    for (int i=0; i<64; ++i) {
        mWorkingBlock[i] *= mDQT.tables[mSOF.quantID[compID]][i];
    }

    /*
     * inverse DCT conversion
     *       1                 (2x+1)uπ            (2y+1)vπ
     * Syx = - ΣΣ (CuCv * cos( --------- ) * cos( ---------- )
     *       4                    16                  16
     *
     *            1      (u, v = 0)
     * Cu, Cv = [
     *            √2     (other)
     */
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

/*
 * DC and AC component and Huffman code:
 *
 * DC:
 * [ huffman code | read bit ]
 *
 * AC:
 * [ huffman code | [ run-length | read bit] ]
 *
 * Sequence:
 * DC, AC, AC,..., AC, DC, AC,...
 *
 * These are decoded with the huffman table corresponding with componentID and DC or AC
 */
void JpegDecoder::decodeDC(int compID)
{
    const HuffmanTable &huff = mDHT(DC, mSOS(DC, compID));

    int bitnum = decodeHuffCode(huff);

    int diff = mFileStream.ReadBits(bitnum);
    diff = SupportFunc::CheckNegative(diff, bitnum);

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
        a = SupportFunc::CheckNegative(a, bit);

		mWorkingBlock[JpgZigzag[i]] = a;
    }
}

// Check and decode huffman codes from the top of bits
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

        // GetData returns -1 if a given code is not defined in the table.
        bitnum = huffTable.GetData(length, code);
    }
    return bitnum;
}

/*
 * This converts and pastes MCU blocks (mMCU) to RGB image data (mRGB)
 */
void JpegDecoder::MCU2RGB(int x, int y)
{
    int offsetV = y * mMCU->height * mSOF.width;
    int offsetH = x * mMCU->width;
    int offset = offsetH + offsetV;

    unsigned char *pY  = mMCU->YCbCr[0];
    unsigned char *pCb = mMCU->YCbCr[1];
    unsigned char *pCr = mMCU->YCbCr[2];

    unsigned char *pR = mRGB[0] + offset;
    unsigned char *pG = mRGB[1] + offset;
    unsigned char *pB = mRGB[2] + offset;


    for (int picY = 0; picY < mMCU->height; ++picY) {
        for (int picX = 0; picX < mMCU->width; ++picX) {
            if (picX + offsetH >= mSOF.width) {
                // cut off the useless data (the right and bottom end of data)
                pY  += mMCU->width - picX;
                pCb += mMCU->width - picX;
                pCr += mMCU->width - picX;
                break;
            }
            int index = picY * mSOF.width + picX;

            SupportFunc::ToRGB(*pY, *pCb, *pCr, &pR[index], &pG[index], &pB[index]);
            pY++;
            pCb++;
            pCr++;
        }
    }
}
