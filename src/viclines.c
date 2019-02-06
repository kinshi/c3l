/*
 * C128 CP/M 8564/8566 VIC-IIe bitmap line functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdlib.h>
#include <assert.h>
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
    ushort lastByte = 40 * (y & 0xf8) + ((x + len -1) & 0x1f8) + (y & 0x07);
    uchar firstBits = x % 8;
    uchar lastBits = (x + len - 1) % 8;
    ushort fillBytes = len >> 3;
    ushort i;
    if (firstBits > 0) {
        /* Handle left over bits on first byte */
        bmp[firstByte] = bmp[firstByte] | fillTable[firstBits - 1];
        firstByte += 8;
        if (fillBytes > 0) {
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
        bmp[lastByte] = bmp[lastByte] | ~fillTable[lastBits - 1];
    }
}

/*
 * Bézier curve.
 */
void drawVicBezier(uchar *bmp, int x0, int y0, int x1, int y1, int x2, int y2) {
    int sx = x0 < x2 ? 1 : -1;
    int sy = y0 < y2 ? 1 : -1; /* step direction */
    int cur = sx * sy * ((x0 - x1) * (y2 - y1) - (x2 - x1) * (y0 - y1)); /* curvature */
    int x = x0 - 2 * x1 + x2, y = y0 - 2 * y1 + y2, xy = 2 * x * y * sx * sy;
    /* compute error increments of P0 */
    long dx = (1 - 2 * abs(x0 - x1)) * y * y + abs(y0 - y1) * xy
            - 2 * cur * abs(y0 - y2);
    long dy = (1 - 2 * abs(y0 - y1)) * x * x + abs(x0 - x1) * xy
            + 2 * cur * abs(x0 - x2);
    /* compute error increments of P2 */
    long ex = (1 - 2 * abs(x2 - x1)) * y * y + abs(y2 - y1) * xy
            + 2 * cur * abs(y0 - y2);
    long ey = (1 - 2 * abs(y2 - y1)) * x * x + abs(x2 - x1) * xy
            - 2 * cur * abs(x0 - x2);
    /* sign of gradient must not change */
    assert((x0 - x1) * (x2 - x1) <= 0 && (y0 - y1) * (y2 - y1) <= 0);
    if (cur == 0) { /* straight line */
        drawVicLine(bmp, x0, y0, x2, y2);
        return;
    }
    x *= 2 * x;
    y *= 2 * y;
    if (cur < 0) { /* negated curvature */
        x = -x;
        dx = -dx;
        ex = -ex;
        xy = -xy;
        y = -y;
        dy = -dy;
        ey = -ey;
    }
    /* algorithm fails for almost straight line, check error values */
    if (dx >= -y || dy <= -x || ex <= -y || ey >= -x) {
        drawVicLine(bmp, x0, y0, x1, y1); /* simple approximation */
        drawVicLine(bmp, x1, y1, x2, y2);
        return;
    }
    dx -= xy;
    ex = dx + dy;
    dy -= xy; /* error of 1.step */
    for (;;) { /* plot curve */
        setVicPix(bmp, x0, y0);
        ey = 2 * ex - dy; /* save value for test of y step */
        if (2 * ex >= dx) { /* x step */
            if (x0 == x2)
                break;
            x0 += sx;
            dy -= xy;
            ex += dx += y;
        }
        if (ey <= 0) { /* y step */
            if (y0 == y2)
                break;
            y0 += sy;
            dx -= xy;
            ex += dy += x;
        }
    }
}
