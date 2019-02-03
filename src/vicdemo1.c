/*
 * C128 CP/M C Library C3L
 *
 * 8564/8566 VIC-IIe screen print demo.
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
void init(uchar *scr, uchar *chr) {
    /* Black screen and border */
    outp(vicBorderCol, 0);
    outp(vicBgCol0, 0);
    /* Copy VDC alt char set to VIC mem */
    copyVdcChars(chr, 0x3000, 256);
    /* Set standard character mode using MMU bank 1 and VIC bank 0 */
    setVicChrMode(1, 0, (ushort) scr / 1024, (ushort) chr / 2048);
    /* Clear screen */
    clearVicScr(scr, 32);
    /* Clear color to white */
    clearVicCol(1);
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
void waitKey(uchar *scr) {
    printVicCol(scr, 0, 24, 1, "Press Return ");
    while (getch() == 0)
        ;
}

/*
 * Text output without color.
 */
void fillScr(uchar *scr) {
    register uchar i;
    for (i = 0; i < 24; i++) {
        printVic(scr, 0, i, "|Watch how fast you can fill the screen|");
    }
    waitKey(scr);
}

/*
 * Color text output.
 */
void fillScrCol(uchar *scr) {
    register uchar i;
    clearVicScr(scr, 32);
    for (i = 0; i < 24; i++) {
        printVicCol(scr, 4, i, i / 2 + 1, "You can do color text as well");
    }
    waitKey(scr);
}

/*
 * Scroll screen.
 */
void scrollScrUp(uchar *scr) {
    register uchar i;
    scrollVicUp(scr, 0, 24);
    fillVicScr(scr, 480, 20, 0x2020);
    for (i = 0; i < 24; i++) {
        scrollVicUp(scr, 0, 24);
    }
    clearVicCol(1);
    for (i = 0; i < 24; i++) {
        printVic(scr, 0, i, "You can scroll any part of the screen!!!");
    }
    waitKey(scr);
    scrollVicUpX(scr, 0, 0, 10, 24);
    fillVicScr(scr, 480, 10, 0x2020);
    for (i = 0; i < 24; i++) {
        scrollVicUpX(scr, 0, 0, 10, 24);
    }
    waitKey(scr);
    scrollVicUpX(scr, 10, 0, 10, 24);
    fillVicScr(scr, 490, 10, 0x2020);
    for (i = 0; i < 24; i++) {
        scrollVicUpX(scr, 10, 0, 10, 24);
    }
}

/*
 * Run demo.
 */
void run(uchar *scr, uchar *chr, uchar *vicMem) {
    char str[40];
    char *dateStr, *timeStr;
    /* Binary, 24h, DST */
    setRtcMode(0x87);
    dateStr = getRtcDate();
    timeStr = getRtcTime();
    printVic(scr, 0, 0, "Simple character mode using the VDC     "
            "character set and one screen. No        "
            "interrupts are disabled and getch is    "
            "used to read keyboard. Since no color is"
            "updated text output is blazing fast!");
    sprintf(str, "Date:   %s", dateStr);
    printVic(scr, 0, 6, str);
    sprintf(str, "Time:   %s", timeStr);
    printVic(scr, 0, 7, str);
    sprintf(str, "vicMem: %04x", vicMem);
    printVic(scr, 0, 8, str);
    sprintf(str, "chr:    %04x", chr);
    printVic(scr, 0, 9, str);
    sprintf(str, "scr:    %04x", scr);
    printVic(scr, 0, 10, str);
    free(dateStr);
    free(timeStr);
    waitKey(scr);
    fillScr(scr);
    fillScrCol(scr);
    scrollScrUp(scr);
}

main() {
    /* Program is small enough to use left over bank 0 memory */
    uchar *vicMem = allocVicMem(0);
    /* Use space after ROM character set for RAM character set */
    uchar *chr = (uchar *) 0x3000;
    /* Use ram after character set for screen */
    uchar *scr = (uchar *) 0x3800;
    /* Save screen/border color */
    uchar border = inp(vicBorderCol);
    uchar background = inp(vicBgCol0);
    init(scr, chr);
    run(scr, chr, vicMem);
    free(vicMem);
    done(border, background);
}
