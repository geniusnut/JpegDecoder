//
//  SupportFunc.cpp
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/19.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#include "SupportFunc.h"

int SupportFunc::CarrySurplus(int number, int divide)
{
    return number / divide + ((number % divide) ? 1 : 0);
}


// convert the number to [0, 255] (unsigned char)
unsigned char SupportFunc::Revise0to255(double num)
{
    if (num > 255.0) {
        return 255;
    }
    if (num < 0.0) {
        return 0;
    }
    return (unsigned char)num;
}

void SupportFunc::ToRGB(unsigned char Y, unsigned char Cb, unsigned char Cr,
                  unsigned char *R, unsigned char *G, unsigned char *B)
{
    double v1 = Y + (Cr - 0x80) * 1.4020;
    *R = Revise0to255(v1);
    double v2 = Y - (Cb - 0x80) * 0.34414 - (Cr - 0x80) * 0.71414;
    *G = Revise0to255(v2);
    double v3 = Y + (Cb - 0x80) * 1.77200;
    *B = Revise0to255(v3);
}

int SupportFunc::CheckNegative(int val, int bitnum)
{
    if ((val & (1 << (bitnum - 1))) == 0) {
        val -= (1 << bitnum) - 1;
    }
    return val;
}
