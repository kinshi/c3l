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
 * Fast lookup for vertical bit.
 */
uchar vertTable[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

/*
 * Optimized horizontal line algorithm up to 15x faster than Bresenham.
 */
void drawVicLineH(uchar *bmp, ushort x, uchar y, ushort len, uchar setPix) {
    ushort pixByte = 40 * (y & 0xf8) + (x & 0x1f8) + (y & 0x07);
    uchar firstBits = x % 8;
    uchar lastBits = (x + len) % 8;
    ushort fillBytes = len >> 3;
    ushort i;
    if (firstBits > 0) {
        /* Handle left over bits on first byte */
        if (setPix) {
            bmp[pixByte] = bmp[pixByte] | fillTable[firstBits - 1];
        } else {
            bmp[pixByte] = bmp[pixByte] & ~fillTable[firstBits - 1];
        }
        pixByte += 8;
        if (fillBytes > 0) {
            fillBytes -= 1;
        }
    }
    /* Handle error in len / 8 */
    if ((lastBits > 0) && (lastBits < 4) && (fillBytes > 0)) {
        fillBytes += 1;
    }
    /* Fill in bytes */
    for (i = 0; i < fillBytes; i++) {
        if (setPix) {
            bmp[pixByte] = 0xff;
        } else {
            bmp[pixByte] = 0x00;
        }
        pixByte += 8;
    }
    /* Handle left over bits on last byte */
    if (lastBits > 0) {
        if (setPix) {
            bmp[pixByte] = bmp[pixByte] | ~fillTable[lastBits - 1];
        } else {
            bmp[pixByte] = bmp[pixByte] & fillTable[lastBits - 1];
        }
    }
}

/*
 * Optimized vertical line algorithm uses less calculation than setVicPix.
 */
void drawVicLineV(uchar *bmp, ushort x, uchar y, ushort len, uchar setPix) {
    ushort pixByte = 40 * (y & 0xf8) + (x & 0x1f8) + (y & 0x07);
    uchar vBit = vertTable[x & 0x07];
    uchar i;
    /* Plot pixels */
    for (i = 0; i < len; i++) {
        if (setPix) {
            bmp[pixByte] = bmp[pixByte] | vBit;
        } else {
            bmp[pixByte] = bmp[pixByte] & ~vBit;
        }
        y += 1;
        /* Increment based on char boundary */
        if ((y & 7) > 0) {
            pixByte += 1;
        } else {
            pixByte += 313;
        }
    }
}

/*
 * Bresenham’s line algorithm optimized for horizontal and vertical lines.
 */
void drawVicLine(uchar *bmp, int x0, int y0, int x1, int y1, uchar setPix) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;
    /* Horizontal line */
    if (y0 == y1) {
        if (x0 < x1) {
            drawVicLineH(bmp, x0, y0, dx + 1, setPix);
        } else {
            drawVicLineH(bmp, x1, y1, dx + 1, setPix);
        }
        /* Vertical line */
    } else if (x0 == x1) {
        if (y0 < y1) {
            drawVicLineV(bmp, x0, y0, dy + 1, setPix);
        } else {
            drawVicLineV(bmp, x1, y1, dy + 1, setPix);
        }
    } else {
        /* Bresenham line */
        for (;;) {
            if (setPix) {
                setVicPix(bmp, x0, y0);
            } else {
                clearVicPix(bmp, x0, y0);
            }
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
}

/*
 * Bézier curve.
 */
void drawVicBezier(uchar *bmp, int x0, int y0, int x1, int y1, int x2, int y2,
uchar setPix) {
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
        drawVicLine(bmp, x0, y0, x2, y2, setPix);
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
        drawVicLine(bmp, x0, y0, x1, y1, setPix); /* simple approximation */
        drawVicLine(bmp, x1, y1, x2, y2, setPix);
        return;
    }
    dx -= xy;
    ex = dx + dy;
    dy -= xy; /* error of 1.step */
    for (;;) { /* plot curve */
        if (setPix) {
            setVicPix(bmp, x0, y0);
        } else {
            clearVicPix(bmp, x0, y0);
        }
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
