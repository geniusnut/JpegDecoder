//
//  Config.h
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#ifndef JpegDecorder_Config_h
#define JpegDecorder_Config_h

static int const JpgZigzag[64] = {
    0,  1,  8,  16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

static const int DC = 0;
static const int AC = 1;

static const unsigned char markSOI  = 0xd8;
static const unsigned char markEOI  = 0xd9;
static const unsigned char markDQT  = 0xdb;
static const unsigned char markDHT  = 0xc4;
static const unsigned char markSOF0 = 0xc0;
static const unsigned char markSOS  = 0xda;
static const unsigned char markDRI  = 0xdd;
static const unsigned char markCOM  = 0xfe;
static const unsigned char markAPP0 = 0xe0;
static const unsigned char markAPP13= 0xed;
static const unsigned char markAPP15= 0xef;

static const double invSqrt2 = 0.70710678118; // 1 / sqrt(2)

#endif
