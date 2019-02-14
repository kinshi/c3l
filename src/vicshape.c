/*
 * C128 CP/M 8564/8566 VIC-IIe bitmap shape functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdlib.h>
#include <hitech.h>
#include <vic.h>

/*
 * Draw rectangle using line drawing.
 */
void drawVicRect(uchar *bmp, int x0, int y0, int x1, int y1, uchar setPix) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    /* Top */
    drawVicLineH(bmp, x0, y0, dx + 2, setPix);
    /* Left */
    drawVicLineV(bmp, x0, y0 + 1, dy - 1, setPix);
    /* Right */
    drawVicLineV(bmp, x1, y0 + 1, dy - 1, setPix);
    /* Bottom */
    drawVicLineH(bmp, x0, y1, dx + 2, setPix);
}

/*
 * Square approximation based on 1:0.75 aspect ratio.
 */
void drawVicSquare(uchar *bmp, int x, int y, int len, uchar setPix) {
    int yLen = (len >> 1) + ((len >> 1) >> 1);
    drawVicRect(bmp, x, y, x + len - 1, y + yLen - 1, setPix);
}

/*
 * Draw octant used by drawVicEllipse.
 */
void drawVicOctant(uchar *bmp, int xc, int yc, int x, int y, uchar setPix) {
    if (setPix) {
        setVicPix(bmp, xc + x, yc + y);
        setVicPix(bmp, xc + x, yc - y);
        setVicPix(bmp, xc - x, yc + y);
        setVicPix(bmp, xc - x, yc - y);
    } else {
        clearVicPix(bmp, xc + x, yc + y);
        clearVicPix(bmp, xc + x, yc - y);
        clearVicPix(bmp, xc - x, yc + y);
        clearVicPix(bmp, xc - x, yc - y);
    }
}

/*
 * Draw ellipse using digital differential analyzer (DDA) method.
 */
void drawVicEllipse(uchar *bmp, int xc, int yc, int a, int b, uchar setPix) {
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
            drawVicOctant(bmp, xc, yc, x, y, setPix);
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
            drawVicOctant(bmp, xc, yc, x, y, setPix);
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
void drawVicCircle(uchar *bmp, int xc, int yc, int a, uchar setPix) {
    /* Circle approximation based on 1:0.75 aspect ratio */
    drawVicEllipse(bmp, xc, yc, a, (a >> 1) + ((a >> 1) >> 1), setPix);
}
