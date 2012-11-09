//
//  DQT.cpp
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#include "DQT.h"
#include "Config.h"
#include "ByteStream.h"
#include <iostream>

using namespace std;

void DQT::Analyze(ByteStream *bytes)
{
    int tableNum = bytes->GetLength() / 64;
    for (int i=0; i<tableNum; ++i) {
        unsigned char *table = tables[bytes->GetByte() & 0x0f];
        for (int j=0; j<64; ++j) {
            table[JpgZigzag[j]] = bytes->GetByte();
        }
    }
}

void DQT::ShowConsole() const
{
    cout << "DQT" << endl;
    for (int i=0; i<2; ++i) {
        cout << "Table ID:" << i << endl;
        for (int j=0; j<8; ++j) {
            for (int k=0; k<8; ++k) {
                cout << hex << (int)tables[i][j*8 + k] << " ";
            }
            cout << endl;
        }
    }
    cout << endl;
}
