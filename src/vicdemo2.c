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
void init(uchar *scr, uchar *chr, uchar *bmp) {
    /* Black screen and border */
    outp(vicBorderCol, 0);
    outp(vicBgCol0, 0);
    /* Copy VDC alt char set to VIC mem */
    copyVdcChars(chr, 0x3000, 256);
    /* Set standard bitmap mode using MMU bank 1 and VIC bank 1 */
    setVicBmpMode(1, 1, ((ushort) scr - 16384) / 1024, ((ushort) bmp - 16384) / 8192);
    /* Clear bitmap */
    clearVicBmp(bmp, 0);
    /* Clear color to white */
    clearVicBmpCol(scr, 0x10);
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
    drawVicLine(bmp, 0, 0, 319, 199);
    waitKey();
}

main() {
    /* We need to use bank 1 since bank 0 doesn't have enough room left */
    uchar *vicMem = allocVicMem(1);
    /* Use beginning of bank 0 for RAM character set */
    uchar *chr = (uchar *) 0x4000;
    /* Use ram after character set for screen */
    uchar *scr = (uchar *) 0x4800;
    /* Use bottom of bank 1 for bitmap */
    uchar *bmp = (uchar *) 0x6000;
    /* Save screen/border color */
    uchar border = inp(vicBorderCol);
    uchar background = inp(vicBgCol0);
    init(scr, chr, bmp);
    run(bmp);
    free(vicMem);
    done(border, background);
}
