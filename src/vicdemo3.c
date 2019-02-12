/*
 * C128 CP/M C Library C3L
 *
 * 8564/8566 VIC-IIe bitmap demo.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <sys.h>
#include <hitech.h>
#include <vic.h>
#include <vdc.h>
#include <rtc.h>

/*
 * Clear bitmap.
 */
void clearBitmap(uchar *bmp, uchar *scr) {
    /* Set to black */
    clearVicBmpCol(scr, 0x00);
    /* Clear bitmap */
    clearVicBmp(bmp, 0);
    /* White foreground and black background */
    clearVicBmpCol(scr, 0x10);
}

/*
 * Copy VDC char set to memory, set screen color, MMU bank, VIC bank, screen
 * memory and bitmap memory. Clear bitmap memory, color memory then enable screen.
 */
void init(uchar *bmp, uchar *scr, uchar *chr) {
    uchar vicBank = (ushort) bmp / 16384;
    /* Set screen and border color */
    outp(vicBorderCol, 14);
    outp(vicBgCol0, 0);
    /* Clear color to black */
    clearVicCol(0);
    clearBitmap(bmp, scr);
    /* Copy VDC alt char set to VIC mem */
    copyVdcChars(chr, 0x3000, 256);
    /* Set standard bitmap mode using MMU bank 1 */
    setVicBmpMode(1, vicBank, ((ushort) scr - (vicBank * 16384)) / 1024,
            ((ushort) bmp - (vicBank * 16384)) / 8192);
    /* Enable screen */
    outp(vicCtrlReg1, (inp(vicCtrlReg1) | 0x10));
}

/*
 * Restore screen color, set MMU bank, VIC bank, screen
 * memory and char set memory location for CP/M return.
 */
void done(uchar bgCol, uchar fgCol) {
    outp(vicBorderCol, bgCol);
    outp(vicBgCol0, fgCol);
    /* Clear color to black */
    clearVicCol(0);
    /* CPM default */
    setVicChrMode(0, 0, 11, 3);
}

/*
 * Wait for key press.
 */
void waitKey(uchar *bmp, uchar *scr, uchar *chr) {
    printVicBmp(bmp, scr, chr, 0, 24, 0x36, "Press Return");
    while (getch() == 0)
        ;
}
/*
 * Print centered text on top line in bitmap.
 */
void bannerBmp(uchar *bmp, uchar *scr, uchar *chr, char *str) {
    printVicBmp(bmp, scr, chr, ((40 - strlen(str)) >> 1), 0, 0x36, str);
}

/*
 * Draw lines.
 */
void lines(uchar *bmp, uchar *scr, uchar *chr) {
    uchar i;
    bannerBmp(bmp, scr, chr, " Bresenham lines ");
    for (i = 0; i < 16; i++) {
        drawVicLine(bmp, 0, 0, i * 20, 199, 1);
        drawVicLine(bmp, 319, 0, 319 - (i * 20), 199, 1);
    }
    waitKey(bmp, scr, chr);
    for (i = 0; i < 16; i++) {
        drawVicLine(bmp, 0, 0, i * 20, 199, 0);
        drawVicLine(bmp, 319, 0, 319 - (i * 20), 199, 0);
    }
}

/*
 * Draw horizontal lines.
 */
void linesH(uchar *bmp, uchar *scr, uchar *chr) {
    uchar i;
    bannerBmp(bmp, scr, chr, " Optimized horizontal lines ");
    for (i = 0; i < 159; i++) {
        drawVicLine(bmp, i, i + 20, 319 - i, i + 20, 1);
    }
    waitKey(bmp, scr, chr);
    for (i = 0; i < 159; i++) {
        drawVicLine(bmp, i, i + 20, 319 - i, i + 20, 0);
    }
}

/*
 * Draw vertical lines.
 */
void linesV(uchar *bmp, uchar *scr, uchar *chr) {
    uchar i;
    bannerBmp(bmp, scr, chr, " Optimized vertical lines ");
    for (i = 10; i < 199; i++) {
        drawVicLine(bmp, i + 57, 10, i + 57, i + 1, 1);
    }
    waitKey(bmp, scr, chr);
    for (i = 10; i < 199; i++) {
        drawVicLine(bmp, i + 57, 10, i + 57, i + 1, 0);
    }
}

/*
 * Draw Bezier curves.
 */
void bezier(uchar *bmp, uchar *scr, uchar *chr) {
    uchar i;
    bannerBmp(bmp, scr, chr, " Bezier curves ");
    for (i = 0; i < 35; i++) {
        drawVicBezier(bmp, i * 5, 10, 319, 15 + i * 5, 319, 15 + i * 5, 1);
    }
    waitKey(bmp, scr, chr);
    for (i = 0; i < 35; i++) {
        drawVicBezier(bmp, i * 5, 10, 319, 15 + i * 5, 319, 15 + i * 5, 0);
    }
}

/*
 * Draw rectangles lines.
 */
void rectangles(uchar *bmp, uchar *scr, uchar *chr) {
    uchar i;
    bannerBmp(bmp, scr, chr, " Rectangles ");
    for (i = 1; i < 30; i++) {
        drawVicRect(bmp, i * 2, i * 2, (i * 10) + 20, (i * 5) + 20, 1);
    }
    waitKey(bmp, scr, chr);
    for (i = 1; i < 30; i++) {
        drawVicRect(bmp, i * 2, i * 2, (i * 10) + 20, (i * 5) + 20, 0);
    }
}

/*
 * Draw ellipses.
 */
void ellipses(uchar *bmp, uchar *scr, uchar *chr) {
    ushort i;
    bannerBmp(bmp, scr, chr, " Ellipses ");
    for (i = 1; i < 9; i++) {
        drawVicEllipse(bmp, 159, 99, i * 19, i * 10, 1);
    }
    waitKey(bmp, scr, chr);
    for (i = 1; i < 9; i++) {
        drawVicEllipse(bmp, 159, 99, i * 19, i * 10, 0);
    }
}

/*
 * Draw circles.
 */
void circles(uchar *bmp, uchar *scr, uchar *chr) {
    ushort i;
    bannerBmp(bmp, scr, chr, " Circles ");
    for (i = 1; i < 12; i++) {
        drawVicCircle(bmp, 159, 99, i * 10, 1);
    }
    waitKey(bmp, scr, chr);
    for (i = 1; i < 12; i++) {
        drawVicCircle(bmp, 159, 99, i * 10, 0);
    }
}

/*
 * Run demo.
 */
void run(uchar *bmp, uchar *scr, uchar *chr, uchar *vicMem) {
    char str[40];
    printVicBmp(bmp, scr, chr, 0, 0, 0x16,
            "This demo will show off bitmap graphics."
                    "No interrupts are disabled and getch is "
                    "used to read keyboard.                  ");
    sprintf(str, "mem: %04x", vicMem);
    printVicBmp(bmp, scr, chr, 0, 4, 0x12, str);
    sprintf(str, "chr: %04x", chr);
    printVicBmp(bmp, scr, chr, 0, 6, 0x12, str);
    sprintf(str, "scr: %04x", scr);
    printVicBmp(bmp, scr, chr, 0, 8, 0x12, str);
    sprintf(str, "bmp: %04x", bmp);
    printVicBmp(bmp, scr, chr, 0, 10, 0x12, str);
    waitKey(bmp, scr, chr);
    clearBitmap(bmp, scr);
    lines(bmp, scr, chr);
    clearBitmap(bmp, scr);
    linesH(bmp, scr, chr);
    clearBitmap(bmp, scr);
    linesV(bmp, scr, chr);
    clearBitmap(bmp, scr);
    bezier(bmp, scr, chr);
    clearBitmap(bmp, scr);
    rectangles(bmp, scr, chr);
    clearBitmap(bmp, scr);
    ellipses(bmp, scr, chr);
    clearBitmap(bmp, scr);
    circles(bmp, scr, chr);
}

main() {
    /* We need to use bank 2 since program is over 16K */
    uchar vicBank = 2;
    uchar *vicMem = allocVicMem(vicBank);
    /* Use beginning of bank 2 for RAM character set */
    uchar *chr = (uchar *) 0x8000;
    /* Use ram after character set for screen */
    uchar *scr = (uchar *) 0x8800;
    /* Use bottom of bank 2 for bitmap */
    uchar *bmp = (uchar *) 0xa000;
    /* Save border/background color */
    uchar border = inp(vicBorderCol);
    uchar background = inp(vicBgCol0);
    init(bmp, scr, chr);
    run(bmp, scr, chr, vicMem);
    done(border, background);
    free(vicMem);
}