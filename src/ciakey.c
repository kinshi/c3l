/*
 * C128 6526 CIA keyboard functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <sys.h>
#include <stdlib.h>
#include <hitech.h>
#include <cia.h>
#include <vic.h>

/*
 * Key column.
 */
uchar keyCol[8] = { 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f };

/*
 * Key to ASCII code without Shift key.
 */
uchar stdKeys[11][8] = {
        { 0x7f, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
        { '3', 'w', 'a', '4', 'z', 's', 'e', 0x00 },
        { '5', 'r', 'd', '6', 'c', 'f', 't', 'x' },
        { '7', 'y', 'g', '8', 'b', 'h', 'u', 'v' },
        { '9', 'i', 'j', '0', 'm', 'k', 'o', 'n' },
        { '+', 'p', 'l', '-', '.', ':', '@', ',' },
        { '\\', '*', ';', 0x00, 0x00, '=', '^', '/' },
        { '1', 0x00, 0x00, '2', 0x20, 0x00, 'q', 0x00 },
        { 0x00, '8', '5', 0x09, '2', '4', '7', '1' },
        { 0x1b, '+', '-', 0x0a, 0x0d, '6', '9', '3' },
        { 0x00, '0', '.', 0x00, 0x00, 0x00, 0x00, 0x00 }
};

/*
 * Scan standard and extended keys.
 */
uchar *keyScan() {
    uchar keyMask, i;
    uchar *ciaKeyScan = (uchar *) malloc(11);
    outp(cia1DdrA, 0xff);
    outp(cia1DdrB, 0x00);
    outp(vicExtKey, 0xff);
    /* Scan standard keys */
    for (i = 0, keyMask = 1; i < 8; i++, keyMask <<= 1) {
        outp(cia1DataA, ~keyMask);
        ciaKeyScan[i] = inp(cia1DataB);
    }
    outp(cia1DataA, 0xff);
    /* Scan extended keys */
    for (keyMask = 1; i < 11; i++, keyMask <<= 1) {
        outp(vicExtKey, ~keyMask);
        ciaKeyScan[i] = inp(cia1DataB);
    }
    return ciaKeyScan;
}

/*
 * Get key column. If column not found then 8 is returned.
 */
uchar getKeyCol(uchar keyVal) {
    register uchar i = 0;
    while((i < 8) && (keyCol[i] != keyVal)) {
        i++;
    }
    return i;
}

/*
 * Simple key decode.
 */
uchar decodeKey(uchar *ciaKeyScan) {
    register uchar i = 0;
    uchar keyCode = 0;
    uchar col;
    /* Find first key row */
    while((i < 11) && (ciaKeyScan[i] == 0xff)) {
        i++;
    }
    if(i < 11){
        col = getKeyCol(ciaKeyScan[i]);
        if(col < 8){
           keyCode = stdKeys[i][col];
        }
    }
    return keyCode;
}


