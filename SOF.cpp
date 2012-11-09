//
//  SOF.cpp
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#include "SOF.h"
#include "ByteStream.h"
#include <algorithm>
#include <iostream>
using namespace std;

void SOF::Analyze(ByteStream *bytes)
{
    if (bytes->GetByte() != 8) {
        cerr << "[SOFReader] this only supports base line format now!" << endl;
        exit(1);
    }
    
    height = (bytes->GetByte() << 8) + bytes->GetByte();
    width = (bytes->GetByte() << 8) + bytes->GetByte();
    if (bytes->GetByte() != 3) {
        cerr << "[SOFReader] this only supports 3 of the components now!" << endl;
        exit(1);
    }
    maxV = 0;
    maxH = 0;
    for (int i=0; i<3; ++i) {
        bytes->GetByte();
        unsigned char samp = bytes->GetByte();
        sampH[i] = samp >> 4;
        sampV[i] = samp & 0x0f;
        maxH = max(sampH[i], maxH);
        maxV = max(sampV[i], maxV);
        quantID[i] = bytes->GetByte();
    }
    
    if (!bytes->IsEnd()) {
        cerr << "[SORReader] strange length of byte stream!" << endl;
        exit(1);
    }
}

void SOF::ShowConsole()
{
    cout << "width:" << width << endl;
    cout << "height:" << height << endl;
    cout << "maxV:" << (int)maxV << endl;
    cout << "maxH:" << (int)maxH << endl;
    for (int i=0; i<3; ++i) {
        cout << "sampV :" << (int)sampV[i] << endl;
        cout << "sampH :" << (int)sampH[i] << endl;
        cout << "quant :" << (int)quantID[i] << endl;
        cout << endl;
    }
}