/*
 * C128 CP/M 8564/8566 VIC-IIe memory management functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdlib.h>
#include <sys.h>
#include <hitech.h>
#include <vic.h>
#include <mmu.h>
#include <cia.h>

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
