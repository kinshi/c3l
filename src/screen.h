/*
 * Generic text based screen functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

extern void (*clearScr)(uchar *, uchar);
extern void (*clearCol)(uchar *, uchar);
extern void (*print)(uchar *, uchar, uchar, char *);
extern void (*printCol)(uchar *, uchar, uchar, uchar, char *);
extern ushort screenSize;
