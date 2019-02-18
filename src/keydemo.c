/*
 * C128 CP/M C Library C3L
 *
 * C128 keyboard demo.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys.h>
#include <hitech.h>
#include <cia.h>
#include <vic.h>
#include <vdc.h>

/*
 * Copy VDC char set to memory, set screen color, MMU bank, VIC bank, screen
 * memory and char set memory. Clear screen and color memory then enable screen.
 */
void init(uchar *scr, uchar *chr) {
    /* Clear CIA 1 ICR status */
    inp(cia1Icr);
    /* Clear all CIA 1 IRQ enable bits */
    outp(cia1Icr, 0x7f);
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
    /* Enable CIA 1 IRQ */
    outp(cia1Icr, 0x82);
}

/*
 * Return single row of key scan.
 */
readKey(uchar index) {
    uchar *ciaKeyScan = keyScan();
    uchar key = ciaKeyScan[index];
    free(ciaKeyScan);
    return key;
}

/*
 * Wait for key press.
 */
void waitKey(uchar *scr) {
    printVicCol(scr, 0, 24, 1, "Press Return ");
    while (readKey(0) != 0xfd)
        ;
}

/*
 * Display low level key scan and decoded key.
 */
void keyboard(uchar *scr) {
    uchar *ciaKeyScan, exitKey, keyVal;
    char str[40];
    clearVicScr(scr, 32);
    clearVicCol(1);
    printVic(scr, 4, 0, "Standard and extended key scan");
    printVicCol(scr, 0, 2, 14, " 0  1  2  3  4  5  6  7  8  9 10");
    printVicCol(scr, 0, 6, 3, "Key pressed:");
    do {
        ciaKeyScan = keyScan();
        exitKey = ciaKeyScan[0];
        sprintf(str, "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                ciaKeyScan[0], ciaKeyScan[1], ciaKeyScan[2], ciaKeyScan[3],
                ciaKeyScan[4], ciaKeyScan[5], ciaKeyScan[6], ciaKeyScan[7],
                ciaKeyScan[8], ciaKeyScan[9], ciaKeyScan[10]);
        printVic(scr, 0, 4, str);
        keyVal = decodeKey(ciaKeyScan);
        scr[253] = keyVal;
        free(ciaKeyScan);
    } while (exitKey != 0xfd);
}

/*
 * Run demo.
 */
void run(uchar *scr, uchar *chr, uchar *vicMem) {
    char str[40];
    printVic(scr, 0, 0, "Low level key scan of standard and      "
                        "extended keyboard. You can also decode  "
                        "unshifted and shifted characters. CIA 1 "
                        "interrupts are disabled, so as not to   "
                        "disrupt the key scan.");
    sprintf(str, "vicMem: %04x", vicMem);
    printVic(scr, 0, 6, str);
    sprintf(str, "chr:    %04x", chr);
    printVic(scr, 0, 7, str);
    sprintf(str, "scr:    %04x", scr);
    printVic(scr, 0, 8, str);
    waitKey(scr);
    keyboard(scr);
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
