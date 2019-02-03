/*
 * C128 8563 VDC I/O functions.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#include <sys.h>
#include <hitech.h>
#include <vdc.h>

/*
 * Read VDC register.
 */
uchar inVdc(uchar regNum) {
    outp(vdcStatusReg, regNum);
    while ((inp(vdcStatusReg) & 0x80) == 0x00)
        ;
    return (inp(vdcDataReg));
}

/*
 * Write VDC register.
 */
void outVdc(uchar regNum, uchar regVal) {
    outp(vdcStatusReg, regNum);
    while ((inp(vdcStatusReg) & 0x80) == 0x00)
        ;
    outp(vdcDataReg, regVal);
}

/*
 * Copy VDC character set to memory.
 */
void copyVdcChars(uchar *chr, ushort vdcMem, ushort chars) {
    register uchar c;
    ushort i;
    outVdc(vdcUpdAddrHi, (uchar) (vdcMem >> 8));
    outVdc(vdcUpdAddrLo, (uchar) vdcMem);
    for (i = 0; i < chars; i++) {
        for (c = 0; c < 8; c++) {
            chr[(i * 8) + c] = inVdc(vdcCPUData);
        }
        /* Skip bottom 8 bytes of VDC data */
        for (c = 0; c < 8; c++) {
            inVdc(vdcCPUData);
        }
    }
}
