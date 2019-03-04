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
void init(uchar *bmp, ushort attrPage, uchar *chr) {
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
    setVdcBmpMode((ushort) bmp, attrPage);
    clearVdcBmp(bmp, vdcBmpSize, 0);
    /* Use VIC pixel functions */
    setPixel = setVdcPix;
    clearPixel = clearVdcPix;
    /* Use optimized horizontal and vertical lines on the VIC */
    drawLineH = drawVdcLineH;
    drawLineV = drawVdcLineV;
    /* VDC aspect ratio */
    aspectRatio = 3;
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
    for (i = 0; i < 32; i++) {
        drawLine(bmp, 0, 0, i * 20, 199, 0);
        drawLine(bmp, 639, 0, 639 - (i * 20), 199, 0);
    }
}

/*
 * Draw horizontal lines.
 */
void linesH(uchar *bmp, uchar *chr) {
    uchar i;
    bannerBmp(bmp, chr, " Optimized horizontal lines ");
    /* Use optimized horizontal lines */
    for (i = 0; i < 159; i++) {
        drawLine(bmp, i, i + 20, 639 - i, i + 20, 1);
    }
    waitKey(bmp, chr);
    for (i = 0; i < 159; i++) {
        drawLine(bmp, i, i + 20, 639 - i, i + 20, 0);
    }
}

/*
 * Draw vertical lines.
 */
void linesV(uchar *bmp, uchar *chr) {
    uchar i;
    bannerBmp(bmp, chr, " Optimized vertical lines ");
    for (i = 10; i < 199; i++) {
        drawLine(bmp, i + 114, 10, i + 114, i + 1, 1);
    }
    waitKey(bmp, chr);
    for (i = 10; i < 199; i++) {
        drawLine(bmp, i + 114, 10, i + 114, i + 1, 0);
    }
}

/*
 * Draw Bezier curves.
 */
void bezier(uchar *bmp, uchar *chr) {
    uchar i;
    bannerBmp(bmp, chr, " Bezier curves ");
    for (i = 0; i < 35; i++) {
        drawBezier(bmp, i * 5, 10, 639, 15 + i * 5, 639, 15 + i * 5, 1);
    }
    waitKey(bmp, chr);
    for (i = 0; i < 35; i++) {
        drawBezier(bmp, i * 5, 10, 639, 15 + i * 5, 639, 15 + i * 5, 0);
    }
}

/*
 * Draw rectangles.
 */
void rectangles(uchar *bmp, uchar *chr) {
    uchar i;
    bannerBmp(bmp, chr, " Rectangles ");
    for (i = 1; i < 30; i++) {
        drawRect(bmp, i * 2, i * 2, (i * 20) + 20, (i * 5) + 20, 1);
    }
    waitKey(bmp, chr);
    for (i = 1; i < 30; i++) {
        drawRect(bmp, i * 2, i * 2, (i * 20) + 20, (i * 5) + 20, 0);
    }
}

/*
 * Draw squares.
 */
void squares(uchar *bmp, uchar *chr) {
    uchar i;
    bannerBmp(bmp, chr, " Squares ");
    for (i = 0; i < 10; i++) {
        drawSquare(bmp, i * 8, i * 8, (i * 8) + 8, 1);
    }
    waitKey(bmp, chr);
    for (i = 0; i < 10; i++) {
        drawSquare(bmp, i * 8, i * 8, (i * 8) + 8, 0);
    }
}

/*
 * Draw ellipses.
 */
void ellipses(uchar *bmp, uchar *chr) {
    ushort i;
    bannerBmp(bmp, chr, " Ellipses ");
    for (i = 1; i < 9; i++) {
        drawEllipse(bmp, 319, 99, i * 39, i * 10, 1);
    }
    waitKey(bmp, chr);
    for (i = 1; i < 9; i++) {
        drawEllipse(bmp, 319, 99, i * 39, i * 10, 0);
    }
}

/*
 * Draw circles.
 */
void circles(uchar *bmp, uchar *chr) {
    ushort i;
    bannerBmp(bmp, chr, " Circles ");
    for (i = 1; i < 10; i++) {
        drawCircle(bmp, 319, 99, i * 20, 1);
    }
    waitKey(bmp, chr);
    for (i = 1; i < 10; i++) {
        drawCircle(bmp, 319, 99, i * 20, 0);
    }
}

/*
 * Run demo.
 */
void run(uchar *bmp, uchar *chr) {
    lines(bmp, chr);
    clearVdcBmp(bmp, vdcBmpSize, 0);
    linesH(bmp, chr);
    clearVdcBmp(bmp, vdcBmpSize, 0);
    linesV(bmp, chr);
    clearVdcBmp(bmp, vdcBmpSize, 0);
    bezier(bmp, chr);
    clearVdcBmp(bmp, vdcBmpSize, 0);
    rectangles(bmp, chr);
    clearVdcBmp(bmp, vdcBmpSize, 0);
    squares(bmp, chr);
    clearVdcBmp(bmp, vdcBmpSize, 0);
    ellipses(bmp, chr);
    clearVdcBmp(bmp, vdcBmpSize, 0);
    circles(bmp, chr);

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
