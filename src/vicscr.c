/*
 * C128 CP/M 8564/8566 VIC-IIe screen functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdlib.h>
#include <sys.h>
#include <string.h>
#include <hitech.h>
#include <vic.h>

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
 * Fill screen with word value starting at start.
 */
void fillVicScr(uchar *scr, ushort start, ushort len, ushort value) {
    register ushort i;
    ushort *scr16 = (ushort *) scr;
    for (i = 0; i < len; i++) {
        scr16[start + i] = value;
    }
}

/*
 * Clear screen using 16 bit word.
 */
void clearVicScr(uchar *scr, uchar c) {
    fillVicScr(scr, 0, vicScrSizeW, (c << 8) + c);
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
