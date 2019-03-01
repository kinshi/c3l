/*
 * C128 CP/M 8563 VDC bitmap functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <sys.h>
#include <string.h>
#include <hitech.h>
#include <vdc.h>

/*
 * Set bitmap memory location, attribute memory location and bitmap mode.
 */
void setVdcBmpMode(ushort dispPage, ushort attrPage) {
    setdsppagevdc(dispPage, attrPage);
    outVdc(vdcHzSmScroll, inVdc(vdcHzSmScroll) | 0x80);
}

/*
 * Clear screen.
 */
void clearVdcBmp(ushort vdcMem, ushort len, uchar c) {
    fillVdcMem(vdcMem, len, c);
}

/*
 * Clear bitmap color memory.
 */
void clearVdcBmpCol(ushort attrMem, ushort len, uchar color) {
    fillVdcMem(attrMem, len, color);
}
