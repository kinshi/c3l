/*
 * C128 CP/M C Library C3L
 *
 * 8563 VDC text demo.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <hitech.h>
#include <cia.h>
#include <vdc.h>
#include <screen.h>

/*
 * Set screen color, MMU bank, VIC bank, screen memory and char set memory.
 * Clear screen and color memory then enable screen.
 */
void init() {
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
    /* Set default sizes and locations */
    scrSize = vdcScrSize;
    scrMem = (uchar *) vdcScrMem;
    scrColMem = (uchar *) vdcColMem;
    chrMem = (uchar *) vdcChrMem;
    /* Assign functions */
    clearScr = clearVdcScr;
    clearCol = clearVdcCol;
    print = printVdc;
    printCol = printVdcCol;
    saveVdc();
    setVdcCursor(0, 0, vdcCurNone);
    clearScr(32);
    clearCol(vdcAltChrSet | vdcWhite);
}

/*
 * Restore VDC and set screen for CP/M return.
 */
void done() {
    restoreVdc();
    /* Enable CIA 1 IRQ */
    outp(cia1Icr, 0x82);
    /* ADM-3A clear-home cursor */
    putchar(0x1a);
}

/*
 * Wait for Return.
 */
void waitKey() {
    printCol(0, 24, vdcAltChrSet | vdcLightYellow, " Press Return ");
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
    print(0, 0, "VDC using screen abstraction.");
    waitKey();
}

main() {
    init();
    run();
    done();
}
