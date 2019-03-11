/*
 * C128 CP/M bitmap Bézier curve abstraction.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdlib.h>
#include <assert.h>
#include <hitech.h>
#include <graphics.h>

/*
 * Bézier curve.
 */
void drawBezier(int x0, int y0, int x1, int y1, int x2, int y2,
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
        drawLine(x0, y0, x2, y2, setPix);
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
        drawLine(x0, y0, x1, y1, setPix); /* simple approximation */
        drawLine(x1, y1, x2, y2, setPix);
        return;
    }
    dx -= xy;
    ex = dx + dy;
    dy -= xy; /* Error of 1.step */
    for (;;) { /* plot curve */
        if (setPix) {
            (*setPixel)(x0, y0);

        } else {
            (*clearPixel)(x0, y0);
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
