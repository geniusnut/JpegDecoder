//
//  DHT.h
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#ifndef __JpegDecorder__DHT__
#define __JpegDecorder__DHT__

#include <vector>
#include "Segment.h"

class ByteStream;

typedef struct S_HuffmanTable
{
    int GetData(unsigned char length, unsigned short code) const;
    void ShowConsole();
	void WriteFile(const char *filename);
    
    // these indices should synchronize
    std::vector<unsigned char> lengthArray;
    std::vector<unsigned short> codeArray;
    std::vector<unsigned char> dataArray;
} HuffmanTable;

class DHT : public Segment
{
public:
    void Analyze(ByteStream *bytes);
    const HuffmanTable& operator() (int DCAC, int ID) const;
    void ShowConsole();
  
private:
    HuffmanTable tables[8];
};

#endif /* defined(__JpegDecorder__DHT__) */
