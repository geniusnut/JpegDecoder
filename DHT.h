//
//  DHT.h
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//
//  To analyze and store huffman tables of jpeg images
//

#ifndef __JpegDecorder__DHT__
#define __JpegDecorder__DHT__

#include <vector>
#include "Segment.h"

class ByteStream;

/*
 * Definition of one huffman table
 */
struct HuffmanTable
{
    // Get data by a code and that length
    int  GetData(unsigned char length, unsigned short code) const;
    // for Debug
    void ShowConsole();

    // these indices should synchronize
    std::vector<unsigned char> lengthArray;
    std::vector<unsigned short> codeArray;
    std::vector<unsigned char> dataArray;
};

/*
 * Object to analyze DHT segments in jpeg and to store all huffman tables
 */
class DHT : public Segment
{
public:
    // Analyze given segment bit-sequence
    void Analyze(ByteStream *bytes);
    // Operator to get the huffman table by DC/AC and a table id
    const HuffmanTable& operator() (int DCAC, int ID) const;
    // for Debug
    void ShowConsole();

private:
    /*
     * Huffman tables
     * DC/AC     : 2
     * ID        : 0 to 3
     * above Sum : 8
     */
    HuffmanTable tables[8];
};

#endif /* defined(__JpegDecorder__DHT__) */
