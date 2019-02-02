/*
 * C128 CP/M 8564/8566 VIC-IIe screen print functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <sys.h>
#include <string.h>
#include <hitech.h>
#include <vic.h>

/*
 * Print without color.
 */
void printVic(uchar *scr, uchar x, uchar y, char *str) {
    ushort scrOfs = (y * 40) + x;
    ushort len = strlen(str);
    ushort i;
    for (i = 0; i < len; i++) {
        scr[scrOfs + i] = str[i];
    }
}

/*
 * Print color to text screen.
 */
void printVicCol(uchar *scr, uchar x, uchar y, uchar color, char *str) {
    ushort scrOfs = (y * 40) + x;
    ushort colOfs = vicColMem + scrOfs;
    ushort len = strlen(str);
    ushort i;
    for (i = 0; i < len; i++) {
        scr[scrOfs + i] = str[i];
        outp(colOfs + i, color);
    }
}
