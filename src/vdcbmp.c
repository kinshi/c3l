/*
 * C128 CP/M 8563 VDC bitmap functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <sys.h>
#include <string.h>
#include <hitech.h>
#include <vdc.h>

/*
 * Lookup for fast pixel selection.
 */
uchar vdcBitTable[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

/*
 * Lookup for fast horizontal pixel fill.
 */
uchar vdcFillTable[7] = { 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };

/*
 * Set bitmap memory location, attribute memory location and bitmap mode.
 */
void setVdcBmpMode(ushort dispPage, ushort attrPage) {
    setVdcDspPage(dispPage, attrPage);
    outVdc(vdcHzSmScroll, inVdc(vdcHzSmScroll) | 0x80);
}

/*
 * Clear screen.
 */
void clearVdcBmp(uchar *bmp, ushort len, uchar c) {
    fillVdcMem((ushort) bmp, len, c);
}

/*
 * Clear bitmap color memory.
 */
void clearVdcBmpCol(ushort attrMem, ushort len, uchar color) {
    fillVdcMem(attrMem, len, color);
}

/*
 * Set pixel.
 */
void setVdcPix(uchar *bmp, ushort x, ushort y) {
    ushort vdcMem = (ushort) bmp;
    uchar saveByte;
    ushort pixByte;
    pixByte = vdcMem + (y << 6) + (y << 4) + (x >> 3);
    outVdc(vdcUpdAddrHi, (uchar) (pixByte >> 8));
    outVdc(vdcUpdAddrLo, (uchar) pixByte);
    saveByte = inVdc(vdcCPUData);
    outVdc(vdcUpdAddrHi, (uchar) (pixByte >> 8));
    outVdc(vdcUpdAddrLo, (uchar) pixByte);
    outVdc(vdcCPUData, saveByte | vdcBitTable[x & 0x07]);
}

/*
 * Clear pixel.
 */
void clearVdcPix(uchar *bmp, ushort x, ushort y) {
    ushort vdcMem = (ushort) bmp;
    uchar saveByte;
    ushort pixByte;
    pixByte = vdcMem + (y << 6) + (y << 4) + (x >> 3);
    outVdc(vdcUpdAddrHi, (uchar) (pixByte >> 8));
    outVdc(vdcUpdAddrLo, (uchar) pixByte);
    saveByte = inVdc(vdcCPUData);
    outVdc(vdcUpdAddrHi, (uchar) (pixByte >> 8));
    outVdc(vdcUpdAddrLo, (uchar) pixByte);
    outVdc(vdcCPUData, saveByte & ~vdcBitTable[x & 0x07]);
}

/*
 * Optimized horizontal line algorithm up to 40x faster than Bresenham.
 */
void drawVdcLineH(uchar *bmp, ushort x, ushort y, ushort len, uchar setPix) {
    ushort vdcMem = (ushort) bmp;
    ushort pixByte = vdcMem + (y << 6) + (y << 4) + (x >> 3);
    uchar firstBits = x % 8;
    uchar lastBits = (x + len - 1) % 8;
    ushort fillBytes = (len - lastBits - 1) >> 3;
    ushort i;
    static uchar fillTable[7] = { 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
    if (firstBits > 0) {
        /* Handle left over bits on first byte */
        if (setPix) {
            orVdcByte(pixByte, fillTable[firstBits - 1]);
        } else {
            andVdcByte(pixByte, ~fillTable[firstBits - 1]);
        }
        pixByte += 1;
    } else {
        outVdc(vdcUpdAddrHi, (uchar) (pixByte >> 8));
        outVdc(vdcUpdAddrLo, (uchar) pixByte);
    }
    /* Fill in bytes */
    for (i = 0; i < fillBytes; i++) {
        if (setPix) {
            outVdc(vdcCPUData, 0xff);
        } else {
            outVdc(vdcCPUData, 0x00);
        }
    }
    pixByte += fillBytes;
    /* Handle left over bits on last byte */
    if (lastBits > 0) {
        if (setPix) {
            orVdcByte(pixByte, ~fillTable[lastBits - 1]);
        } else {
            andVdcByte(pixByte, fillTable[lastBits - 1]);
        }
    }
}

/*
 * Optimized vertical line algorithm uses less calculation than setVdcPix.
 */
void drawVdcLineV(uchar *bmp, ushort x, ushort y, ushort len, uchar setPix) {
    ushort vdcMem = (ushort) bmp;
    ushort pixByte = vdcMem + (y << 6) + (y << 4) + (x >> 3);
    uchar vBit = vdcBitTable[x & 0x07];
    uchar saveByte, i;
    /* Plot pixels */
    for (i = 0; i < len; i++) {
        if (setPix) {
            outVdc(vdcUpdAddrHi, (uchar) (pixByte >> 8));
            outVdc(vdcUpdAddrLo, (uchar) pixByte);
            saveByte = inVdc(vdcCPUData);
            outVdc(vdcUpdAddrHi, (uchar) (pixByte >> 8));
            outVdc(vdcUpdAddrLo, (uchar) pixByte);
            outVdc(vdcCPUData, saveByte | vBit);
        } else {
            outVdc(vdcUpdAddrHi, (uchar) (pixByte >> 8));
            outVdc(vdcUpdAddrLo, (uchar) pixByte);
            saveByte = inVdc(vdcCPUData);
            outVdc(vdcUpdAddrHi, (uchar) (pixByte >> 8));
            outVdc(vdcUpdAddrLo, (uchar) pixByte);
            outVdc(vdcCPUData, saveByte & ~vBit);
        }
        pixByte += 80;
    }
}

/*
 * Print without color. Optimized by setting VDC address once for each scan line.
 */
void printVdcBmp(uchar *bmp, uchar *chr, uchar x, uchar y, char *str) {
    ushort vdcMem = (ushort) bmp;
    ushort dispOfs = ((y * 80) * 8) + vdcMem + x;
    ushort len = strlen(str);
    ushort i, chrOfs;
    uchar c;
    /* Draw 8 scan lines */
    for (c = 0; c < 8; c++) {
        outVdc(vdcUpdAddrHi, (uchar) (dispOfs >> 8));
        outVdc(vdcUpdAddrLo, (uchar) dispOfs);
        for (i = 0; i < len; i++) {
            chrOfs = (str[i] << 3) + c;
            outVdc(vdcCPUData, chr[chrOfs]);
        }
        /* Next scan line */
        dispOfs += 80;
    }
}
