/*
 * C128 CP/M 8564/8566 VIC-IIe screen scroll functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <hitech.h>
#include <vic.h>

/*
 * Scroll screen memory up 1 line starting at x for len words.
 */
void scrollVicUpX(uchar *scr, uchar x, uchar y, uchar len, uchar lines) {
    register uchar w;
    uchar i;
    ushort *scr16 = (ushort *) scr;
    ushort dest = (y * 20) + x;
    ushort sourceLine, destLine;
    for (i = 0; i < lines; i++) {
        destLine = dest + (i * 20);
        sourceLine = destLine + 20;
        for (w = 0; w < len; w++) {
            scr16[destLine + w] = scr16[sourceLine + w];
        }
    }
}

/*
 * Scroll screen memory up 1 line by number of lines.
 */
void scrollVicUp(uchar *scr, uchar y, uchar lines) {
    scrollVicUpX(scr, 0, y, 20, lines);
}

