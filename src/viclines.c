/*
 * C128 CP/M 8564/8566 VIC-IIe bitmap line functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <hitech.h>
#include <vic.h>

/*
 * Bresenham’s Line Generation Algorithm.
 *
 * Line is drawn from left to right, x1 < x2 and y1 < y2, slope of the line is
 * between 0 and 1 and line drawn from lower left to upper right.
 */
void drawVicLine(uchar *bmp, int x1, int y1, int x2, int y2) {
    int m = 2 * (y2 - y1);
    int slopeErr = m - (x2 - x1);
    int x, y;
    for (x = x1, y = y1; x <= x2; x++) {
        /* Add slope to increment angle formed */
        slopeErr += m;
        /* Slope error reached limit, time to increment y and update slope error */
        if (slopeErr >= 0) {
            y++;
            slopeErr -= 2 * (x2 - x1);
        }
        setPix(bmp, x, y);
    }
}
