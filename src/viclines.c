/*
 * C128 CP/M 8564/8566 VIC-IIe bitmap line functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdlib.h>
#include <hitech.h>
#include <vic.h>

/*
 * Lookup for fast horizontal pixel fill.
 */
uchar fillTable[7] = { 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };

/*
 * Bresenham’s line generation algorithm.
 */
void drawVicLine(uchar *bmp, int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;
    for (;;) {
        setVicPix(bmp, x0, y0);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

/*
 * Optimized horizontal line algorithm up to 15x faster than drawVicLine.
 */
void drawVicLineH(uchar *bmp, ushort x, uchar y, ushort len) {
    ushort firstByte = 40 * (y & 0xf8) + (x & 0x1f8) + (y & 0x07);
    uchar firstBits = x % 8;
    uchar lastBits = (x + len - 1) % 8;
    ushort fillBytes = len >> 3;
    ushort i;
    if (firstBits > 0) {
        /* Handle left over bits on first byte */
        bmp[firstByte] = bmp[firstByte] | fillTable[firstBits - 1];
        firstByte += 8;
        if(fillBytes > 0){
            fillBytes -= 1;
        }
    }
    /* Fill in bytes */
    for (i = 0; i < fillBytes; i++) {
        bmp[firstByte] = 0xff;
        firstByte += 8;
    }
    /* Handle left over bits on last byte */
    if (lastBits > 0) {
        bmp[firstByte] = bmp[firstByte] | ~fillTable[lastBits - 1];
    }
}
