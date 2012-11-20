//
//  DHT.cpp
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#include "DHT.h"
#include "ByteStream.h"
#include "Config.h"
#include <iostream>
#include <algorithm>
#include <bitset>

using namespace std;

/*
 * Get decoded data by a code and that length
 */
int HuffmanTable::GetData(unsigned char length, unsigned short code) const
{
    int i;
    for (i=0; i<codeArray.size(); ++i) {
        if (codeArray[i] == code) {
            break;
        }
    }
    // nothing
    if (i == codeArray.size()) {
        return -1;
    }
    // check code length
    if (length == lengthArray[i]) {
        return dataArray[i];
    } else {
        return -1;
    }
}

// for Debug
void HuffmanTable::ShowConsole()
{
    for (int i=0; i<codeArray.size(); ++i) {
        cout << "len:" << (int)lengthArray[i] << " ";
        cout << "cod:" << (int)codeArray[i]   << " ";
        cout << "dat:" << (int)dataArray[i]   << endl;
    }
}


/*
 * Analyze segment bit-sequence
 *
 * DHT data format:
 * [ length(2) | DCAC(0.5) | ID(0.5) | alloc num(1x16) | data(alloc num[1 to 16]) ]
 */
void DHT::Analyze(ByteStream *bytes)
{
    while (!bytes->IsEnd()) {
        // determine table
        unsigned char th = bytes->GetByte();
        int tableIndex = (th >> 4) * 4 + (th & 0x0f);

        // allocated byte num
        unsigned char allocated[16];
        for (int i=0; i<16; ++i) {
            allocated[i] = bytes->GetByte();
        }

        // code define
        unsigned short code = 0;
        unsigned char bitnum = 0;
        for (int i=1; i<=16; ++i) {
            for (int j=0; j<allocated[i-1]; ++j) {
                bitnum = bytes->GetByte();
                tables[tableIndex].lengthArray.push_back(i);
                tables[tableIndex].codeArray.push_back(code);
                tables[tableIndex].dataArray.push_back(bitnum);
                ++code;
            }
            code <<= 1;
        }
    }
}

/*
 * Getter of the huffman table which has DCAC and ID
 *
 * tables array:
 * [ (DC, 0),..., (DC, 3), (AC, 0),..., (AC, 3) ]
 */
const HuffmanTable& DHT::operator() (int DCAC, int ID) const
{
    return tables[DCAC * 4 + ID];
}

// for Debug
void DHT::ShowConsole()
{
    for (int DCAC=0; DCAC<2; DCAC++) {
        for (int ID=0; ID<4; ID++) {
            if ((*this)(DCAC, ID).lengthArray.size() == 0) {
                continue;
            }
            cout << "class:" << DCAC << endl;
            cout << "id:" << ID << endl;
            for (int i=0; i<(*this)(DCAC, ID).lengthArray.size(); ++i) {
                bitset<16> bs((*this)(DCAC, ID).codeArray[i]);
                cout << bs << ": ";
                if (DCAC == DC) {
                    cout << (int)(*this)(DCAC, ID).dataArray[i] << endl;
                } else {
                    cout << "ran:" << (int)((*this)(DCAC, ID).dataArray[i] >> 4) << ", ";
                    cout << "num:" << (int)((*this)(DCAC, ID).dataArray[i] & 0x0f) << endl;
                }
            }
        }
    }
}
