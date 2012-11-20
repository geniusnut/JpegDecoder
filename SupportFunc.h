//
//  SupportFunc.h
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/13.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#ifndef JpegDecorder_SupportFunc_h
#define JpegDecorder_SupportFunc_h


class SupportFunc
{
public:
    static int CarrySurplus(int number, int divide);

    template <typename T>
    static void SafeDeleteArray(T *array)
    {
        if (array) {
            delete[] array;
            array = 0;
        }
    }

    // convert the number to [0, 255] (unsigned char)
    static unsigned char Revise0to255(double num);

    static void ToRGB(unsigned char Y, unsigned char Cb, unsigned char Cr,
                      unsigned char *R, unsigned char *G, unsigned char *B);

    static int CheckNegative(int val, int bitnum);
};

#endif
