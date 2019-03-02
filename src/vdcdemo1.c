/*
 * C128 CP/M C Library C3L
 *
 * 8563 VDC bitmap demo.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys.h>
#include <hitech.h>
#include <cia.h>
#include <vdc.h>
#include <graphics.h>

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
void waitKey(uchar *bmp, uchar *chr) {
    printVdcBmp(bmp, chr, 0, 24, " Press Return ");
    /* Debounce */
    while (getKey(0) == 0xfd)
        ;
    while (getKey(0) != 0xfd)
        ;
    /* Debounce */
    while (getKey(0) == 0xfd)
        ;
    printVdcBmp(bmp, chr, 0, 24, " Erasing pixels ");
}

/*
 * Print centered text on top line in bitmap.
 */
void bannerBmp(uchar *bmp, uchar *chr, char *str) {
    printVdcBmp(bmp, chr, ((80 - strlen(str)) >> 1), 0, str);
}

/*
 * Draw lines.
 */
void lines(uchar *bmp, uchar *chr) {
    uchar i;
    bannerBmp(bmp, chr, " Bresenham lines ");
    for (i = 0; i < 32; i++) {
        drawLine(bmp, 0, 0, i * 20, 199, 1);
        drawLine(bmp, 639, 0, 639 - (i * 20), 199, 1);
    }
    waitKey(bmp, chr);
    for (i = 0; i < 16; i++) {
        drawLine(bmp, 0, 0, i * 20, 199, 0);
        drawLine(bmp, 639, 0, 639 - (i * 20), 199, 0);
    }
}

/*
 * Run demo.
 */
void run(uchar *bmp, uchar *chr) {
    /* Use VIC pixel functions */
    setPixel = setVdcPix;
    clearPixel = clearVdcPix;
    lines(bmp, chr);
    waitKey(bmp, chr);
}

main() {
    /* We use pointer to be compatible with VIC */
    uchar *bmp = (uchar *) 0x0000;
    /* Save both VDC char sets */
    uchar *chr = (uchar *) malloc(4096);
    /* Use alternate character set */
    uchar *altChr = (uchar *) ((ushort) chr) + 0x0800;
    init(0, 0, chr);
    run(bmp, altChr);
    done(chr);
}
