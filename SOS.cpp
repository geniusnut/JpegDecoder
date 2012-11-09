//
//  SOS.cpp
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#include "SOS.h"
#include "ByteStream.h"
#include <iostream>
using namespace std;

void SOS::Analyze(ByteStream *bytes)
{
    if (bytes->GetByte() != 3) {
        cerr << "[SOFReader] this supports 3 components only." << endl;
        exit(1);
    }
    
    for (int i=0; i<3; ++i) {
        bytes->GetByte(); // component ID
        unsigned char DCAC = bytes->GetByte();
        huffmanID[i] = DCAC >> 4;
        huffmanID[3 + i] = DCAC & 0x0f;
    }
    
    // skip (support only Base line)
    while (!bytes->IsEnd()) {
        bytes->GetByte();
    }
}

unsigned char SOS::operator()(int DCAC, int compID)
{
    return huffmanID[DCAC * 3 + compID];
}

void SOS::ShowConsole()
{
    cout << "DC" << endl;
    cout << "Y:" << (int)huffmanID[0] << endl;
    cout << "Cb:" << (int)huffmanID[1] << endl;
    cout << "Cr:" << (int)huffmanID[2] << endl;
    cout << "AC" << endl;
    cout << "Y:" << (int)huffmanID[3] << endl;
    cout << "Cb:" << (int)huffmanID[4] << endl;
    cout << "Cr:" << (int)huffmanID[5] << endl;
}