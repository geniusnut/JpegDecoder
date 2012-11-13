//
//  SupportFunc.h
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/13.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#ifndef JpegDecorder_SupportFunc_h
#define JpegDecorder_SupportFunc_h

namespace SupportFunc
{
    int CarrySurplus(int number, int divide)
    {
        return number / divide + ((number % divide) ? 1 : 0);
    }


    template <typename T> void SafeDeleteArray(T *array)
    {
        if (array) {
            delete[] array;
            array = 0;
        }
    }

    unsigned char Revise0to255(double num)
    {
        if (num > 255.0) {
            return 255;
        }
        if (num < 0.0) {
            return 0;
        }
        return (unsigned char)num;
    }

    void ToRGB(unsigned char Y, unsigned char Cb, unsigned char Cr,
                unsigned char *R, unsigned char *G, unsigned char *B)
    {
        double v1 = Y + (Cr - 0x80) * 1.4020;
        *R = Revise0to255(v1);
        double v2 = Y - (Cb - 0x80) * 0.34414 - (Cr - 0x80) * 0.71414;
        *G = Revise0to255(v2);
        double v3 = Y + (Cb - 0x80) * 1.77200;
        *B = Revise0to255(v3);
    }
};

#endif
