/*
 * C128 CP/M 8564/8566 VIC-IIe bitmap functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <sys.h>
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
void setPix(uchar *bmp, ushort x, uchar y) {
    ushort pixByte = 40 * (y & 0xf8) + (x & 0x1f8) + (y & 0x07);
    bmp[pixByte] = bmp[pixByte] | (bitTable[x & 0x07]);
}
