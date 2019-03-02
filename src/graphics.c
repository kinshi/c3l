/*
 * C128 CP/M graphics abstraction.
 *
 * Function pointers are used to drive the graphics, thus the same code will
 * work on the VIC and VDC. Technically this code would work with other graphic
 * displays as well. You just need to implement the functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdlib.h>
#include <assert.h>
#include <hitech.h>

/*
 * Set function pointer in your code before calling any functions.
 */
void (*setPixel)(uchar *, ushort, ushort);

/*
 * Set function pointer in your code before calling any functions.
 */
void (*clearPixel)(uchar *, ushort, ushort);

/*
 * Set function pointer if implemented.
 */
void (*drawLineH)(uchar *, ushort, ushort, ushort, uchar);

/*
 * Set function pointer if implemented.
 */
void (*drawLineV)(uchar *, ushort, ushort, ushort, uchar);

/*
 * Bresenham’s line algorithm. setPix is 1 to set or 0 to clear pixel.
 */
void drawLine(uchar *bmp, int x0, int y0, int x1, int y1, uchar setPix) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;
    /* Horizontal line */
    if ((drawLineH != NULL) && (y0 == y1)) {
        if (x0 < x1) {
            (*drawLineH)(bmp, x0, y0, dx + 1, setPix);
        } else {
            (*drawLineH)(bmp, x1, y1, dx + 1, setPix);
        }
        /* Vertical line */
    } else if ((drawLineV != NULL) && (x0 == x1)) {
        if (y0 < y1) {
            (*drawLineV)(bmp, x0, y0, dy + 1, setPix);
        } else {
            (*drawLineV)(bmp, x1, y1, dy + 1, setPix);
        }
    } else {
        /* Bresenham line */
        for (;;) {
            if (setPix) {
                (*setPixel)(bmp, x0, y0);
            } else {
                (*clearPixel)(bmp, x0, y0);
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
void drawBezier(uchar *bmp, int x0, int y0, int x1, int y1, int x2, int y2,
uchar setPix) {
    int sx = x0 < x2 ? 1 : -1;
    int sy = y0 < y2 ? 1 : -1; /* Step direction */
    int cur = sx * sy * ((x0 - x1) * (y2 - y1) - (x2 - x1) * (y0 - y1)); /* Curvature */
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
        drawLine(bmp, x0, y0, x2, y2, setPix);
        return;
    }
    x *= 2 * x;
    y *= 2 * y;
    if (cur < 0) { /* Negated curvature */
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
        drawLine(bmp, x0, y0, x1, y1, setPix); /* simple approximation */
        drawLine(bmp, x1, y1, x2, y2, setPix);
        return;
    }
    dx -= xy;
    ex = dx + dy;
    dy -= xy; /* Error of 1.step */
    for (;;) { /* plot curve */
        if (setPix) {
            (*setPixel)(bmp, x0, y0);

        } else {
            (*clearPixel)(bmp, x0, y0);
        }
        ey = 2 * ex - dy; /* Save value for test of y step */
        if (2 * ex >= dx) { /* X step */
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

/*
 * Draw octant used by drawEllipse.
 */
void drawOctant(uchar *bmp, int xc, int yc, int x, int y, uchar setPix) {
    if (setPix) {
        (*setPixel)(bmp, xc + x, yc + y);
        (*setPixel)(bmp, xc + x, yc - y);
        (*setPixel)(bmp, xc - x, yc + y);
        (*setPixel)(bmp, xc - x, yc - y);
    } else {
        (*clearPixel)(bmp, xc + x, yc + y);
        (*clearPixel)(bmp, xc + x, yc - y);
        (*clearPixel)(bmp, xc - x, yc + y);
        (*clearPixel)(bmp, xc - x, yc - y);
    }
}

/*
 * Draw ellipse using digital differential analyzer (DDA) method.
 */
void drawEllipse(uchar *bmp, int xc, int yc, int a, int b, uchar setPix) {
    long aa = (long) a * a; /* a^2 */
    long bb = (long) b * b; /* b^2 */
    long aa2 = aa << 1; /* 2(a^2) */
    long bb2 = bb << 1; /* 2(b^2) */
    {
        long x = 0;
        long y = b;
        long xbb2 = 0;
        long yaa2 = y * aa2;
        long errVal = -y * aa; /* b^2 x^2 + a^2 y^2 - a^2 b^2 -b^2x */
        while (xbb2 <= yaa2) /* draw octant from top to top right */
        {
            drawOctant(bmp, xc, yc, x, y, setPix);
            x += 1;
            xbb2 += bb2;
            errVal += xbb2 - bb;
            if (errVal >= 0) {
                y -= 1;
                yaa2 -= aa2;
                errVal -= yaa2;
            }
        }
    }
    {
        long x = a;
        long y = 0;
        long xbb2 = x * bb2;
        long yaa2 = 0;
        long errVal = -x * bb;
        while (xbb2 > yaa2) /* draw octant from right to top right */
        {
            drawOctant(bmp, xc, yc, x, y, setPix);
            y += 1;
            yaa2 += aa2;
            errVal += yaa2 - aa;
            if (errVal >= 0) {
                x -= 1;
                xbb2 -= bb2;
                errVal -= xbb2;
            }
        }
    }
}

/*
 * Draw circle using ellipse with aspect ration adjustment.
 */
void drawCircle(uchar *bmp, int xc, int yc, int a, uchar setPix) {
    /* Circle approximation based on 1:0.75 aspect ratio */
    drawEllipse(bmp, xc, yc, a, (a >> 1) + ((a >> 1) >> 1), setPix);
}

/*
 * Draw rectangle using line drawing.
 */
void drawRect(uchar *bmp, int x0, int y0, int x1, int y1, uchar setPix) {
    /* Top */
    drawLine(bmp, x0, y0, x1, y0, setPix);
    /* Left */
    drawLine(bmp, x0, y0, x0, y1, setPix);
    /* Right */
    drawLine(bmp, x1, y0, x1, y1, setPix);
    /* Bottom */
    drawLine(bmp, x0, y1, x1, y1, setPix);
}

/*
 * Square approximation based on 1:0.75 aspect ratio.
 */
void drawSquare(uchar *bmp, int x, int y, int len, uchar setPix) {
    int yLen = (len >> 1) + ((len >> 1) >> 1);
    drawRect(bmp, x, y, x + len - 1, y + yLen - 1, setPix);
}
