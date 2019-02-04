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
#include <sys.h>
#include <hitech.h>
#include <vic.h>
#include <vdc.h>
#include <rtc.h>

/*
 * Copy VDC char set to memory, set screen color, MMU bank, VIC bank, screen
 * memory and char set memory. Clear screen and color memory then enable screen.
 */
void init(uchar *scr, uchar *chr, uchar *bmp, uchar vicBank) {
    /* Black screen and border */
    outp(vicBorderCol, 0);
    outp(vicBgCol0, 0);
    /* Clear color to black */
    clearVicCol(0);
    /* Clear bitmap */
    clearVicBmp(bmp, 0);
    /* Clear bitmap color to white foreground and black background */
    clearVicBmpCol(scr, 0x10);
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
void waitKey() {
    while (getch() == 0)
        ;
}

/*
 * Run demo.
 */
void run(uchar *bmp) {
    uchar i;
    for (i = 0; i < 16; i++) {
        drawVicLine(bmp, 0, 0, i * 20, 199);
        drawVicLine(bmp, 319, 0, 319 - (i * 20), 199);
    }
    waitKey();
}

main() {
    /* We need to use bank 1 since bank 0 doesn't have enough room left */
    uchar vicBank = 1;
    uchar *vicMem = allocVicMem(vicBank);
    /* Use beginning of bank 0 for RAM character set */
    uchar *chr = (uchar *) 0x4000;
    /* Use ram after character set for screen */
    uchar *scr1 = (uchar *) 0x4800;
    uchar *scr2 = (uchar *) 0x4c00;
    /* Use bottom of bank 1 for bitmap */
    uchar *bmp = (uchar *) 0x6000;
    /* Save screen/border color */
    uchar border = inp(vicBorderCol);
    uchar background = inp(vicBgCol0);
    init(scr2, chr, bmp, vicBank);
    run(bmp);
    free(vicMem);
    done(border, background);
}
