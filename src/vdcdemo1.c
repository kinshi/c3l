/*
 * C128 CP/M C Library C3L
 *
 * 8563 VDC bitmap demo.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys.h>
#include <hitech.h>
#include <cia.h>
#include <vdc.h>

/*
 * Set screen color, MMU bank, VIC bank, screen memory and char set memory.
 * Clear screen and color memory then enable screen.
 */
void init(ushort dispPage, ushort attrPage, uchar *chr) {
    /* Clear all CIA 1 IRQ enable bits */
    outp(cia1Icr, 0x7f);
    /* Clear CIA 1 ICR status */
    inp(cia1Icr);
    /* Clear all CIA 2 IRQ enable bits */
    outp(cia2Icr, 0x7f);
    /* Clear CIA 2 ICR status */
    inp(cia2Icr);
    /* Set CIA 1 DDRs for keyboard scan */
    outp(cia1DdrA, 0xff);
    outp(cia1DdrB, 0x00);
    saveVdc();
    setVdcCursor(0, 0, vdcCurNone);
    /* Copy VDC char sets to VIC mem */
    copyVdcChrMem(chr, 0x2000, 512);
    setVdcFgBg(15, 0);
    setVdcAttrsOff();
    setVdcBmpMode(dispPage, attrPage);
    clearVdcBmp(dispPage, 16000, 0);
}

/*
 * Restore screen color, set MMU bank, VIC bank, screen
 * memory and char set memory location for CP/M return.
 */
void done(uchar *chr) {
    restoreVdc();
    copyVdcMemChr(chr, 0x2000, 512);
    free(chr);
    /* Enable CIA 1 IRQ */
    outp(cia1Icr, 0x82);
    /* ADM-3A clear-home cursor */
    putchar(0x1a);
}

/*
 * Wait for Return.
 */
void waitKey() {
    /* Debounce */
    while (getKey(0) == 0xfd)
        ;
    while (getKey(0) != 0xfd)
        ;
    /* Debounce */
    while (getKey(0) == 0xfd)
        ;
}

/*
 * Run demo.
 */
void run() {
    waitKey();
}

main() {
    /* Save both VDC char sets */
    uchar *chr = (uchar *) malloc(4096);
    init(0, 0, chr);
    run();
    done(chr);
}
