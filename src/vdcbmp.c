/*
 * C128 CP/M 8563 VDC bitmap functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <sys.h>
#include <string.h>
#include <hitech.h>
#include <vdc.h>

uchar vdcBitTable[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

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
void clearVdcBmp(ushort vdcMem, ushort len, uchar c) {
    fillVdcMem(vdcMem, len, c);
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
 * Print without color.
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
