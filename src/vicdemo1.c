/*
 * C128 CP/M C Library C3L
 *
 * 8564/8566 VIC-IIe character ROM demo.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <sys.h>
#include <hitech.h>
#include <vic.h>

/*
 * Set screen color, MMU bank, VIC bank, screen memory and char set memory.
 * Clear screen and color memory then enable screen.
 */
void init(uchar *scr, uchar *chr) {
    uchar vicBank = (ushort) scr / 16384;
    /* Black screen and border */
    outp(vicBorderCol, 0);
    outp(vicBgCol0, 0);
    /* Set standard character mode using MMU bank 1 and set VIC based on scr location */
    setVicChrMode(1, vicBank, ((ushort) scr - (vicBank * 16384)) / 1024,
            ((ushort) chr - (vicBank * 16384)) / 2048);
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
    printVicColPet(scr, 0, 24, 7, "Press Return ");
    while (getch() == 0)
        ;
}

/*
 * Run demo.
 */
void run(uchar *scr, uchar *chr, uchar *vicMem) {
    uchar i;
    char str[40];
    /* Note the use of printVicPet that converts ASCII to PETSCII */
    printVicPet(scr, 0, 0, "Simple character mode using ROM for the "
            "character set and one screen at the end "
            "of VIC bank 0. This leaves about 15K for"
            "your program. Once your program grows   "
            "beyond 0x3c00 you have to move to VIC   "
            "bank 1.");
    for (i = 0; i < 255; i++) {
        scr[i + 280] = i;
    }
    sprintf(str, "vicMem: %04x", vicMem);
    printVicColPet(scr, 0, 15, 14, str);
    sprintf(str, "chr:    %04x", chr);
    printVicColPet(scr, 0, 16, 14, str);
    sprintf(str, "scr:    %04x", scr);
    printVicColPet(scr, 0, 17, 14, str);
    waitKey(scr);
}

main() {
    /* Program is small enough to use left over bank 0 memory */
    uchar *vicMem = allocVicMem(0);
    /* Use ROM character set */
    uchar *chr = (uchar *) 0x1800;
    /* Use ram at end of bank */
    uchar *scr = (uchar *) 0x3c00;
    /* Save screen/border color */
    uchar border = inp(vicBorderCol);
    uchar background = inp(vicBgCol0);
    init(scr, chr);
    run(scr, chr, vicMem);
    free(vicMem);
    done(border, background);
}
