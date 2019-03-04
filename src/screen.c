/*
 * C128 CP/M graphics abstraction.
 *
 * Screen abstraction uses function pointers to drive text output, thus the
 * same code will work on the VIC and VDC. Technically this code would work
 * with other graphic displays as well. You just need to implement the functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <stdlib.h>
#include <assert.h>
#include <hitech.h>

/*
 * Set function pointer in your code before calling any functions.
 */
void (*clearScr)(uchar *, uchar);

/*
 * Set function pointer in your code before calling any functions.
 */
void (*clearCol)(uchar *, uchar);

/*
 * Set function pointer in your code before calling any functions.
 */
void (*print)(uchar *, uchar, uchar, char *);

/*
 * Set function pointer in your code before calling any functions.
 */
void (*printCol)(uchar *, uchar, uchar, uchar, char *);

/*
 * Screen size in bytes.
 */
ushort screenSize;
