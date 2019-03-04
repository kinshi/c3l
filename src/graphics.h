/*
 * Generic bitmap graphics functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

extern void (*setPixel)(uchar *, ushort, ushort);
extern void (*clearPixel)(uchar *, ushort, ushort);
extern void (*drawLineH)(uchar *, ushort, ushort, ushort, uchar);
extern void (*drawLineV)(uchar *, ushort, ushort, ushort, uchar);
extern ushort bitmapSize;
extern uchar aspectRatio;
extern void drawLine(uchar *bmp, int x0, int y0, int x1, int y1, uchar setPix);
extern void drawBezier(uchar *bmp, int x0, int y0, int x1, int y1, int x2,
        int y2, uchar setPix);
extern drawOctant(uchar *bmp, int xc, int yc, int x, int y, uchar setPix);
extern drawEllipse(uchar *bmp, int xc, int yc, int a, int b, uchar setPix);
extern drawCircle(uchar *bmp, int xc, int yc, int a, uchar setPix);
void drawRect(uchar *bmp, int x0, int y0, int x1, int y1, uchar setPix);
void drawSquare(uchar *bmp, int x, int y, int len, uchar setPix);
