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
 * Lookup for fast pixel selection
 */
uchar bitTable[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

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
    fillVicScr(bmp, 0, vicBmpSizeW, (c << 8) + c);
}

/*
 * Clear bitmap color memory.
 */
void clearVicBmpCol(uchar *scr, uchar color) {
    fillVicScr(scr, 0, vicScrSizeW, (color << 8) + color);
}

/*
 * Set pixel.
 */
void setVicPix(uchar *bmp, ushort x, uchar y) {
    ushort pixByte = 40 * (y & 0xf8) + (x & 0x1f8) + (y & 0x07);
    bmp[pixByte] = bmp[pixByte] | (bitTable[x & 0x07]);
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
