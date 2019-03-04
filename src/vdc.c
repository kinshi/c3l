/*
 * C128 8563 VDC functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <sys.h>
#include <string.h>
#include <hitech.h>
#include <vdc.h>

/*
 * VDC registers to save and restore.
 */
uchar vdcSavedRegs[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 20, 21,
        22, 23, 24, 25, 26, 27, 28, 29, 34, 35, 36 };

/*
 * Lookup for fast pixel selection.
 */
uchar vdcBitTable[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

/*
 * Lookup for fast horizontal pixel fill.
 */
uchar vdcFillTable[7] = { 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };

/*
 * Saved registers.
 */
uchar vdcRegs[sizeof(vdcSavedRegs) - 1];

/*
 * Save key VDC registers.
 */
void saveVdc() {
    uchar i;
    for (i = 0; i < sizeof(vdcRegs); i++)
        vdcRegs[i] = inVdc(vdcSavedRegs[i]);
}

/*
 * Restore key VDC registers.
 */
void restoreVdc() {
    uchar i;
    for (i = 0; i < sizeof(vdcRegs); i++)
        outVdc(vdcSavedRegs[i], vdcRegs[i]);
}

/*
 * Read VDC register.
 */
uchar inVdc(uchar regNum) {
    outp(vdcStatusReg, regNum);
    while ((inp(vdcStatusReg) & 0x80) == 0x00)
        ;
    return (inp(vdcDataReg));
}

/*
 * Write VDC register.
 */
void outVdc(uchar regNum, uchar regVal) {
    outp(vdcStatusReg, regNum);
    while ((inp(vdcStatusReg) & 0x80) == 0x00)
        ;
    outp(vdcDataReg, regVal);
}

/*
 * Sets which disp and attr page is showing.
 */
void setVdcDspPage(ushort dispPage, ushort attrPage) {
    outVdc(vdcDspStAddrHi, (uchar) (dispPage >> 8));
    outVdc(vdcDspStAddrLo, (uchar) dispPage);
    outVdc(vdcAttrStAddrHi, (uchar) (attrPage >> 8));
    outVdc(vdcAttrStAddrLo, (uchar) attrPage);
}

/*
 * Set foreground and background color.
 */
void setVdcFgBg(uchar f, uchar b) {
    outVdc(vdcFgBgColor, (f << 4) | b);
}

/*
 * Turn attributes on.
 */
void setVdcAttrsOn() {
    outVdc(vdcHzSmScroll, inVdc(vdcHzSmScroll) | 0x40);
}

/*
 * Turn attributes off.
 */
void setVdcAttrsOff() {
    outVdc(vdcHzSmScroll, inVdc(vdcHzSmScroll) & 0xbf);
}

/*
 * Set cursor's top and bottom scan lines and mode.
 */

void setVdcCursor(uchar top, uchar bottom, uchar mode) {
    outVdc(vdcCurStScanLine, (top | (mode << 5)));
    outVdc(vdcCurEndScanLine, bottom);
}

/*
 * Or VDC byte with value and store it.
 */
void orVdcByte(ushort vdcMem, uchar value) {
    uchar saveByte;
    outVdc(vdcUpdAddrHi, (uchar) (vdcMem >> 8));
    outVdc(vdcUpdAddrLo, (uchar) vdcMem);
    saveByte = inVdc(vdcCPUData);
    outVdc(vdcUpdAddrHi, (uchar) (vdcMem >> 8));
    outVdc(vdcUpdAddrLo, (uchar) vdcMem);
    outVdc(vdcCPUData, saveByte | value);
}

/*
 * And VDC byte with value and store it.
 */
void andVdcByte(ushort vdcMem, uchar value) {
    uchar saveByte;
    outVdc(vdcUpdAddrHi, (uchar) (vdcMem >> 8));
    outVdc(vdcUpdAddrLo, (uchar) vdcMem);
    saveByte = inVdc(vdcCPUData);
    outVdc(vdcUpdAddrHi, (uchar) (vdcMem >> 8));
    outVdc(vdcUpdAddrLo, (uchar) vdcMem);
    outVdc(vdcCPUData, saveByte & value);
}

/*
 * Fast fill using block writes.
 */
void fillVdcMem(ushort vdcMem, ushort len, uchar value) {
    uchar blocks, remain;
    register uchar i;
    outVdc(vdcUpdAddrHi, (uchar) (vdcMem >> 8));
    outVdc(vdcUpdAddrLo, (uchar) vdcMem);
    outVdc(vdcVtSmScroll, (inVdc(vdcVtSmScroll) & 0x7f));
    outVdc(vdcCPUData, value);
    if (len > vdcMaxBlock) {
        blocks = len / vdcMaxBlock;
        remain = len % vdcMaxBlock;
        for (i = 1; i <= blocks; i++)
            outVdc(vdcWordCnt, vdcMaxBlock);
        if (remain > 1)
            outVdc(vdcWordCnt, --remain);
    } else if (len > 1)
        outVdc(vdcWordCnt, --len);
}

/*
 * Copy VDC character set to memory.
 */
void copyVdcChrMem(uchar *mem, ushort vdcMem, ushort chars) {
    register uchar c;
    ushort vdcOfs = vdcMem, memOfs = 0, i;
    for (i = 0; i < chars; i++) {
        outVdc(vdcUpdAddrHi, (uchar) (vdcOfs >> 8));
        outVdc(vdcUpdAddrLo, (uchar) vdcOfs);
        /* Only use 8 bytes of 16 byte character definition */
        for (c = 0; c < 8; c++) {
            mem[memOfs + c] = inVdc(vdcCPUData);
        }
        memOfs += 8;
        vdcOfs += 16;
    }
}

/*
 * Copy character set to VDC memory.
 */
void copyVdcMemChr(uchar *mem, ushort vdcMem, ushort chars) {
    register uchar c;
    ushort vdcOfs = vdcMem, memOfs = 0, i;
    for (i = 0; i < chars; i++) {
        outVdc(vdcUpdAddrHi, (uchar) (vdcOfs >> 8));
        outVdc(vdcUpdAddrLo, (uchar) vdcOfs);
        /* Only use 8 bytes of 16 byte character definition */
        for (c = 0; c < 8; c++) {
            outVdc(vdcCPUData, mem[memOfs + c]);
        }
        memOfs += 8;
        vdcOfs += 16;
    }
}

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
