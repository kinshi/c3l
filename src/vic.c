/*
 * C128 CP/M 8564/8566 VIC-IIe memory management functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdlib.h>
#include <sys.h>
#include <string.h>
#include <hitech.h>
#include <mmu.h>
#include <cia.h>
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
 * Sprite table.
 */
uchar sprTable[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

/*
 * Allocate memory in TPA for VIC to use. maxBank Should be 0 - 2. Bank 3
 * should be allocated manually since that's the top bank of memory and
 * not all 16K is available.
 */
uchar *allocVicMem(uchar maxBank) {
    uchar bank, *vicMem;
    ushort gap;
    vicMem = (uchar *) malloc(1);
    bank = (ushort) vicMem / 16384;
    gap = 16384 - ((ushort) vicMem - (bank * 16384));
    realloc(vicMem, ((maxBank - bank) * 16384) + gap);
    return vicMem;
}

/*
 * Set VIC to MMU bank 0 or 1.
 */
void setVicMmuBank(uchar mmuRcr) {
    /* Set MMU RCR bit 6 to point VIC to MMU bank */
    outp(mmuRamCfg, (inp(mmuRamCfg) & 0xbf) | (mmuRcr * 0x40));
}

/*
 * Set VIC bank to 0 - 3.
 */
void setVicBank(uchar vicBank) {
    uchar saveDdr = inp(cia2DdrA);
    /* Set DDR port A to write */
    outp(cia2DdrA, inp(cia2DdrA) | 0x03);
    /* Set VIC to bank 0-3 */
    outp(cia2DataA, (inp(cia2DataA) & 0xfc) | (3 - vicBank));
    outp(cia2DdrA, saveDdr);
}

/*
 * Fill memory with word value starting at start.
 */
void fillVicMem(uchar *mem, ushort start, ushort len, ushort value) {
    register ushort i;
    ushort *mem16 = (ushort *) mem;
    for (i = 0; i < len; i++) {
        mem16[start + i] = value;
    }
}

/*
 * Set ecm, bmm and mcm to 0 (off) or 1 (on).
 */
void setVicMode(uchar ecm, uchar bmm, uchar mcm) {
    /* Set enhanced color and char/bitmap mode */
    outp(vicCtrlReg1,
            (inp(vicCtrlReg1) & 0x9f) | ((ecm * 0x40) + (bmm * 0x20)));
    /* Set multicolor mode */
    outp(vicCtrlReg2, (inp(vicCtrlReg2) & 0xef) | (mcm * 0x10));
}

/*
 * Set screen 0-15 memory location (1K per screen).
 */
void setVicScrMem(uchar scrMem) {
    outp(vicMemCtrl, (inp(vicMemCtrl) & 0x0f) | (scrMem << 4));
}

/*
 * Set character set 0-7 memory location (2K per character set).
 */
void setVicChrMem(uchar chrMem) {
    outp(vicMemCtrl, (inp(vicMemCtrl) & 0xf0) | (chrMem << 1));
}

/*
 * Set standard character mode (no MCM or ECM).
 */
void setVicChrMode(uchar mmuRcr, uchar vicBank, uchar scrMem, uchar chrMem) {
    setVicMmuBank(mmuRcr);
    setVicBank(vicBank);
    setVicMode(0, 0, 0);
    setVicScrMem(scrMem);
    setVicChrMem(chrMem);
}

/*
 * Clear screen using 16 bit word.
 */
void clearVicScr(uchar *scr, uchar c) {
    fillVicMem(scr, 0, vicScrSizeW, (c << 8) + c);
}

/*
 * Clear color memory.
 */
void clearVicCol(uchar color) {
    register ushort i;
    for (i = 0; i < vicScrSize; i++) {
        outp(vicColMem + i, color);
    }
}

/*
 * Print without color.
 */
void printVic(uchar *scr, uchar x, uchar y, char *str) {
    ushort scrOfs = (y * 40) + x;
    ushort len = strlen(str);
    ushort i;
    for (i = 0; i < len; i++) {
        scr[scrOfs + i] = str[i];
    }
}

/*
 * Print with color.
 */
void printVicCol(uchar *scr, uchar x, uchar y, uchar color, char *str) {
    ushort scrOfs = (y * 40) + x;
    ushort colOfs = vicColMem + scrOfs;
    ushort len = strlen(str);
    ushort i;
    for (i = 0; i < len; i++) {
        scr[scrOfs + i] = str[i];
        outp(colOfs + i, color);
    }
}

/*
 * Convert string to from ASCII to PETSCII.
 */
char *asciiToPet(char *str) {
    ushort len = strlen(str);
    char *petStr = (char *) malloc(len + 1);
    ushort i;
    for (i = 0; i < len; i++) {
        if ((str[i] > 96) && (str[i] <= 127)) {
            petStr[i] = str[i] - 96;
        } else {
            petStr[i] = str[i];
        }
    }
    petStr[len] = 0;
    return petStr;
}

/*
 * Print PETSCII without color.
 */
void printVicPet(uchar *scr, uchar x, uchar y, char *str) {
    char *petStr = asciiToPet(str);
    printVic(scr, x, y, petStr);
    free(petStr);
}

/*
 * Print PETSCII with color.
 */
void printVicColPet(uchar *scr, uchar x, uchar y, uchar color, char *str) {
    char *petStr = asciiToPet(str);
    printVicCol(scr, x, y, color, petStr);
    free(petStr);
}

/*
 * Scroll screen memory up 1 line starting at x for len words.
 */
void scrollVicUpX(uchar *scr, uchar x, uchar y, uchar len, uchar lines) {
    register uchar w;
    uchar i;
    ushort *scr16 = (ushort *) scr;
    ushort destLine = (y * 20) + x;
    ushort sourceLine = destLine + 20;
    for (i = 0; i < lines; i++) {
        for (w = 0; w < len; w++) {
            scr16[destLine + w] = scr16[sourceLine + w];
        }
        destLine += 20;
        sourceLine = destLine + 20;
    }
}

/*
 * Scroll screen memory up 1 line by number of lines.
 */
void scrollVicUp(uchar *scr, uchar y, uchar lines) {
    scrollVicUpX(scr, 0, y, 20, lines);
}

/*
 * Use screen memory as simple input line. Only backspace supported, but insert
 * and delete could be added later.
 */
char *readVicLine(uchar *scr, uchar x, uchar y, uchar len) {
    uchar strLen;
    char *str;
    uchar keyVal, lastKeyVal, i;
    ushort scrOfs = (y * 40) + x;
    ushort scrMin = scrOfs;
    ushort scrMax = scrMin + len - 1;
    lastKeyVal = 0x00;
    /* Show cursor */
    scr[scrOfs] = '_';
    do {
        keyVal = decodeKey();
        /* Debounce if current key equals last key */
        if (keyVal == lastKeyVal) {
            i = 0;
            do {
                /* ~1/60th second delay */
                while (inp(vicRaster) != 0xff)
                    ;
                while (inp(vicRaster) == 0xff)
                    ;
                keyVal = decodeKey();
                i++;
            } while ((keyVal == lastKeyVal) && (i < 8));
        }
        lastKeyVal = keyVal;
        /* Decoded key? */
        if (keyVal != 0x00) {
            /* Backspace? */
            if (keyVal == 0x7f) {
                if (scrOfs > scrMin) {
                    scr[scrOfs] = ' ';
                    scrOfs--;
                }
            } else {
                if ((scrOfs <= scrMax) && (keyVal != 0x0d)) {
                    scr[scrOfs] = keyVal;
                    scrOfs++;
                }
            }
            /* Show cursor */
            scr[scrOfs] = '_';
        }
    } while (keyVal != 0x0d);
    /* Figure out string length based on current screen offset */
    strLen = scrOfs - scrMin;
    str = (char *) malloc(strLen + 1);
    /* Screen to string */
    for (i = 0; i < strLen; i++) {
        str[i] = scr[scrMin + i];
    }
    str[strLen] = 0;
    return str;
}

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

/*
 * Configure sprite.
 */
void configVicSpr(uchar *scr, uchar *spr, uchar sprNum, uchar sprCol) {
    uchar vicBank = (ushort) scr / 16384;
    /* Set sprite memory location */
    scr[vicSprMemOfs + sprNum] = ((ushort) spr - (vicBank * 16384)) / 64;
    /* Sprite color */
    outp(vicSpr0Col + sprNum, sprCol);
}

/*
 * Enable sprite.
 */
void enableVicSpr(uchar sprNum) {
    /* Sprite enable */
    outp(vicSprEnable, inp(vicSprEnable) | sprTable[sprNum]);
}

/*
 * Disable sprite.
 */
void disableVicSpr(uchar sprNum) {
    /* Sprite disable */
    outp(vicSprEnable, inp(vicSprEnable) & ~sprTable[sprNum]);
}

/*
 * Set sprite location.
 */
void setVicSprLoc(uchar sprNum, ushort x, uchar y) {
    /* Set sprite X */
    if (x > 255) {
        outp(vicSprXmsb, inp(vicSprXmsb) | sprTable[sprNum]);
        outp(vicSpr0X + (sprNum << 1), x - 256);
    } else {
        outp(vicSprXmsb, inp(vicSprXmsb) & ~sprTable[sprNum]);
        outp(vicSpr0X + (sprNum << 1), x);
    }
    /* Sprite Y */
    outp(vicSpr0y + (sprNum << 1), y);
}

/*
 * Make sprite appear in foreground.
 */
void setVicSprFg(uchar sprNum) {
    /* Sprite priority */
    outp(vicSprFg, inp(vicSprFg) & ~sprTable[sprNum]);
}

/*
 * Make sprite appear in background.
 */
void setVicSprBg(uchar sprNum) {
    /* Sprite priority */
    outp(vicSprFg, inp(vicSprFg) | sprTable[sprNum]);
}
