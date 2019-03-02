/*
 * C128 CP/M 8564/8566 VIC-IIe bitmap functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <sys.h>
#include <string.h>
#include <hitech.h>
#include <vic.h>

/*
 * Lookup for fast pixel selection.
 */
uchar bitTable[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

/*
 * Lookup for fast horizontal pixel fill.
 */
uchar fillTable[7] = { 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };

/*
 * Set bitmap 0-1 memory location (8K per bitmap).
 */
void setVicBmpMem(uchar bmpMem) {
    outp(vicMemCtrl, (inp(vicMemCtrl) & 0xf0) | (bmpMem << 3));
}

/*
 * Set bitmap mode.
 */
void setVicBmpMode(uchar mmuRcr, uchar vicBank, uchar scrMem, uchar bmpMem) {
    setVicMmuBank(mmuRcr);
    setVicBank(vicBank);
    setVicMode(0, 1, 0);
    setVicScrMem(scrMem);
    setVicBmpMem(bmpMem);
}

/*
 * Clear screen using 16 bit word.
 */
void clearVicBmp(uchar *bmp, uchar c) {
    fillVicMem(bmp, 0, vicBmpSizeW, (c << 8) + c);
}

/*
 * Clear bitmap color memory.
 */
void clearVicBmpCol(uchar *scr, uchar color) {
    fillVicMem(scr, 0, vicScrSizeW, (color << 8) + color);
}

/*
 * Set pixel.
 */
void setVicPix(uchar *bmp, ushort x, ushort y) {
    ushort pixByte = 40 * (y & 0xf8) + (x & 0x1f8) + (y & 0x07);
    bmp[pixByte] = bmp[pixByte] | (bitTable[x & 0x07]);
}

/*
 * Clear pixel.
 */
void clearVicPix(uchar *bmp, ushort x, ushort y) {
    ushort pixByte = 40 * (y & 0xf8) + (x & 0x1f8) + (y & 0x07);
    bmp[pixByte] = bmp[pixByte] & ~(bitTable[x & 0x07]);
}

/*
 * Optimized horizontal line algorithm up to 15x faster than Bresenham.
 */
void drawVicLineH(uchar *bmp, ushort x, ushort y, ushort len, uchar setPix) {
    ushort pixByte = 40 * (y & 0xf8) + (x & 0x1f8) + (y & 0x07);
    uchar firstBits = x % 8;
    uchar lastBits = (x + len - 1) % 8;
    ushort fillBytes = (len - lastBits - 1) >> 3;
    ushort i;
    if (firstBits > 0) {
        /* Handle left over bits on first byte */
        if (setPix) {
            bmp[pixByte] = bmp[pixByte] | fillTable[firstBits - 1];
        } else {
            bmp[pixByte] = bmp[pixByte] & ~fillTable[firstBits - 1];
        }
        pixByte += 8;
    }
    /* Fill in bytes */
    for (i = 0; i < fillBytes; i++) {
        if (setPix) {
            bmp[pixByte] = 0xff;
        } else {
            bmp[pixByte] = 0x00;
        }
        pixByte += 8;
    }
    /* Handle left over bits on last byte */
    if (lastBits > 0) {
        if (setPix) {
            bmp[pixByte] = bmp[pixByte] | ~fillTable[lastBits - 1];
        } else {
            bmp[pixByte] = bmp[pixByte] & fillTable[lastBits - 1];
        }
    }
}

/*
 * Optimized vertical line algorithm uses less calculation than setVicPix.
 */
void drawVicLineV(uchar *bmp, ushort x, ushort y, ushort len, uchar setPix) {
    ushort pixByte = 40 * (y & 0xf8) + (x & 0x1f8) + (y & 0x07);
    uchar vBit = bitTable[x & 0x07];
    uchar i;
    /* Plot pixels */
    for (i = 0; i < len; i++) {
        if (setPix) {
            bmp[pixByte] = bmp[pixByte] | vBit;
        } else {
            bmp[pixByte] = bmp[pixByte] & ~vBit;
        }
        y += 1;
        /* Increment based on char boundary */
        if ((y & 7) > 0) {
            pixByte += 1;
        } else {
            pixByte += 313;
        }
    }
}

/*
 * Print with foreground/background color.
 */
void printVicBmp(uchar *bmp, uchar *scr, uchar *chr, uchar x, uchar y,
uchar color, char *str) {
    ushort *bmp16 = (ushort *) bmp;
    ushort *chr16 = (ushort *) chr;
    ushort bmpOfs = (y * 160) + (x * 4);
    ushort colOfs = (y * 40) + x;
    ushort len = strlen(str);
    ushort i, chrOfs, destOfs;
    uchar c;
    for (i = 0; i < len; i++) {
        chrOfs = str[i] << 2;
        destOfs = i << 2;
        scr[colOfs + i] = color;
        for (c = 0; c < 4; c++) {
            bmp16[bmpOfs + destOfs + c] = chr16[chrOfs + c];
        }
    }
}
