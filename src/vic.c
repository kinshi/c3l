/*
 * C128 CP/M 8564/8566 VIC-IIe general functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdlib.h>
#include <sys.h>
#include <hitech.h>
#include <mmu.h>
#include <cia.h>
#include <vic.h>

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
void setVicScrMem(uchar scrLoc) {
    outp(vicMemCtrl, (inp(vicMemCtrl) & 0x0f) | (scrLoc << 4));
}

/*
 * Set character set 0-7 memory location (2K per character set).
 */
void setVicChrMem(uchar chrLoc) {
    outp(vicMemCtrl, (inp(vicMemCtrl) & 0xf0) | (chrLoc << 1));
}

/*
 * Set standard character mode (no MCM or ECM).
 */
void setVicChrMode(uchar mmuRcr, uchar vicBank, uchar scrLoc, uchar chrLoc) {
    setVicMmuBank(mmuRcr);
    setVicBank(vicBank);
    setVicMode(0, 0, 0);
    setVicScrMem(scrLoc);
    setVicChrMem(chrLoc);
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
void setVicBmpMode(uchar mmuRcr, uchar vicBank, uchar scrLoc, uchar bmpMem) {
    setVicMmuBank(mmuRcr);
    setVicBank(vicBank);
    setVicMode(0, 1, 0);
    setVicScrMem(scrLoc);
    setVicBmpMem(bmpMem);
}
