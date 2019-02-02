/*
 * C128 CP/M 8564/8566 VIC-IIe screen functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <sys.h>
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
    outp(0xd018, (inp(0xd018) & 0x0f) | (scrMem << 4));
}

/*
 * Set character set 0-7 memory location (2K per character set).
 */
void setVicChrMem(uchar chrMem) {
    outp(0xd018, (inp(0xd018) & 0xf0) | (chrMem << 1));
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
    ushort c16 = (c << 8) + c;
    fillVicScr(scr, 0, vicScrSizeW, c16);
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