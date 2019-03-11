/*
 * C128 CP/M bitmap ellipse abstraction.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <hitech.h>
#include <graphics.h>

/*
 * Draw octant used by drawEllipse.
 */
void drawOctant(int xc, int yc, int x, int y, uchar setPix) {
    if (setPix) {
        (*setPixel)(xc + x, yc + y);
        (*setPixel)(xc + x, yc - y);
        (*setPixel)(xc - x, yc + y);
        (*setPixel)(xc - x, yc - y);
    } else {
        (*clearPixel)(xc + x, yc + y);
        (*clearPixel)(xc + x, yc - y);
        (*clearPixel)(xc - x, yc + y);
        (*clearPixel)(xc - x, yc - y);
    }
}

/*
 * Draw ellipse using digital differential analyzer (DDA) method.
 */
void drawEllipse(int xc, int yc, int a, int b, uchar setPix) {
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
            drawOctant(xc, yc, x, y, setPix);
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
            drawOctant(xc, yc, x, y, setPix);
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
 * Draw circle using ellipse with aspect ratio adjustment.
 */
void drawCircle(int xc, int yc, int a, uchar setPix) {
    /* Circle approximation based on 1:0.75 aspect ratio */
    drawEllipse(xc, yc, a,
            (a / aspectRatio) + ((a / aspectRatio) / aspectRatio), setPix);
}
