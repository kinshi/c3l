/*
 * Generic bitmap graphics functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

extern uchar *bmpMem;
extern ushort bmpSize;
extern uchar *bmpColMem;
extern ushort bmpColSize;
extern uchar *bmpChrMem;
extern uchar aspectRatio;
extern void (*setPixel)(ushort, ushort);
extern void (*clearPixel)(ushort, ushort);
extern void (*clearBmp)(uchar);
extern void (*clearBmpCol)(uchar);
extern void (*drawLineH)(ushort, ushort, ushort, uchar);
extern void (*drawLineV)(ushort, ushort, ushort, uchar);
extern void drawLine(int x0, int y0, int x1, int y1, uchar setPix);
extern void drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, uchar setPix);
extern void drawOctant(int xc, int yc, int x, int y, uchar setPix);
extern void drawEllipse(int xc, int yc, int a, int b, uchar setPix);
extern void drawCircle(int xc, int yc, int a, uchar setPix);
extern void drawRect(int x0, int y0, int x1, int y1, uchar setPix);
extern void drawSquare(int x, int y, int len, uchar setPix);
