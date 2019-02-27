/*
 * C128 CP/M C Library C3L
 *
 * 8564/8566 VIC-IIe sprite demo.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys.h>
#include <hitech.h>
#include <cia.h>
#include <vic.h>

/*
 * Sprite data.
 */
uchar sprData[] = { 0x00, 0x7e, 0x00, 0x03, 0xff, 0xc0, 0x07, 0xff, 0xe0, 0x1f,
        0xff, 0xf8, 0x1f, 0xff, 0xf8, 0x3f, 0xff, 0xfc, 0x7f, 0xff, 0xfe, 0x7f,
        0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xfe, 0x7f, 0xff, 0xfe, 0x3f,
        0xff, 0xfc, 0x1f, 0xff, 0xf8, 0x1f, 0xff, 0xf8, 0x07, 0xff, 0xe0, 0x03,
        0xff, 0xc0, 0x00, 0x7e, 0x00 };

/*
 * Set screen color, MMU bank, VIC bank, screen memory and char set memory.
 * Clear screen and color memory then enable screen.
 */
void init(uchar *scr, uchar *chr) {
    uchar vicBank = (ushort) scr / 16384;
    /* Clear CIA 1 ICR status */
    inp(cia1Icr);
    /* Clear all CIA 1 IRQ enable bits */
    outp(cia1Icr, 0x7f);
    /* Disable all VIC interrupts */
    outp(vicIntMask, 0x00);
#asm
    di
#endasm
    /* Set CIA 1 DDRs for keyboard scan */
    outp(cia1DdrA, 0xff);
    outp(cia1DdrB, 0x00);
    /* Set screen and border color */
    outp(vicBorderCol, 13);
    outp(vicBgCol0, 0);
    /* Clear color to black */
    clearVicCol(0);
    /* Clear screen */
    clearVicScr(scr, 32);
    /* Set standard character mode using MMU bank 1 and set VIC based on scr location */
    setVicChrMode(1, vicBank, ((ushort) scr - (vicBank * 16384)) / 1024,
            ((ushort) chr - (vicBank * 16384)) / 2048);
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
#asm
    ei
#endasm
}

/*
 * Wait for Return.
 */
void waitKey(uchar *scr) {
    printVicColPet(scr, 0, 24, 7, "Press Return");
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
 * Bounce sprite around screen.
 */
void bounceSpr(uchar *scr) {
    uchar y = 50, i;
    ushort x = 24;
    int xDir = 1, yDir = 1;
    uchar *spr = (uchar *) ((ushort) scr) - 64;
    /* Store sprite data in VIC memory above screen */
    for (i = 0; i < 63; i++) {
        spr[i] = sprData[i];
    }
    configVicSpr(scr, spr, 0, 6);
    setVicSprLoc(0, x, y);
    enableVicSpr(0);
    printVicColPet(scr, 0, 24, 7, "Press Return");
    /* Bounce sprite until return pressed */
    while (getKey(0) != 0xfd) {
        x += xDir;
        y += yDir;
        if (x > 319) {
            x = 319;
            xDir = -1;
        } else if (x < 24) {
            x = 24;
            xDir = 1;
        } else if (y > 228) {
            y = 228;
            yDir = -1;
        } else if (y < 50) {
            y = 50;
            yDir = 1;
        }
        /* Raster off screen? */
        while (inp(vicRaster) != 0xff)
            ;
        setVicSprLoc(0, x, y);
    }
    disableVicSpr(0);
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
            "of VIC bank 0. Sprite is located above  "
            "screen at 0x3bc0.");
    for (i = 0; i < 255; i++) {
        scr[i + 280] = i;
    }
    sprintf(str, "vicMem: %04x", vicMem);
    printVicColPet(scr, 0, 15, 14, str);
    sprintf(str, "chr:    %04x", chr);
    printVicColPet(scr, 0, 16, 14, str);
    sprintf(str, "scr:    %04x", scr);
    printVicColPet(scr, 0, 17, 14, str);
    /* Use VIC raster to seed random numbers */
    srand(inp(vicRaster));
    bounceSpr(scr);
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
