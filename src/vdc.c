/*
 * C128 8563 VDC functions.
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
 * Sets which disp and attr page is showing.
 */
void setVdcDspPage(ushort dispPage, ushort attrPage) {
    outVdc(vdcDspStAddrHi, (uchar) (dispPage >> 8));
    outVdc(vdcDspStAddrLo, (uchar) dispPage);
    outVdc(vdcAttrStAddrHi, (uchar) (attrPage >> 8));
    outVdc(vdcAttrStAddrLo, (uchar) attrPage);
}

/*
 * Set foreground and background color.
 */
void setVdcFgBg(uchar f, uchar b)
{
  outVdc(vdcFgBgColor,(f << 4) | b);
}

/*
 * Turn attributes on.
 */
void setVdcAttrsOn(void) {
    outVdc(vdcHzSmScroll, inVdc(vdcHzSmScroll) | 0x40);
}

/*
 * Turn attributes off.
 */
void setVdcAttrsOff(void) {
    outVdc(vdcHzSmScroll, inVdc(vdcHzSmScroll) & 0xbf);
}

/*
 * Set cursor's top and bottom scan lines and mode.
 */

void setVdcCursor(uchar top, uchar bottom, uchar mode) {
    outVdc(vdcCurStScanLine, (top | (mode << 5)));
    outVdc(vdcCurEndScanLine, bottom);
}

/*
 * Fast fill using block writes.
 */
void fillVdcMem(ushort vdcMem, ushort len, uchar value) {
    uchar blocks, remain;
    register uchar i;
    outVdc(vdcUpdAddrHi, (uchar) (vdcMem >> 8));
    outVdc(vdcUpdAddrLo, (uchar) vdcMem);
    outVdc(vdcVtSmScroll, (inVdc(vdcVtSmScroll) & 0x7f));
    outVdc(vdcCPUData, value);
    if (len > vdcMaxBlock) {
        blocks = len / vdcMaxBlock;
        remain = len % vdcMaxBlock;
        for (i = 1; i <= blocks; i++)
            outVdc(vdcWordCnt, vdcMaxBlock);
        if (remain > 1)
            outVdc(vdcWordCnt, --remain);
    } else if (len > 1)
        outVdc(vdcWordCnt, --len);
}

/*
 * Copy VDC character set to memory.
 */
void copyVdcChars(uchar *mem, ushort vdcMem, ushort chars) {
    register uchar c;
    ushort i;
    outVdc(vdcUpdAddrHi, (uchar) (vdcMem >> 8));
    outVdc(vdcUpdAddrLo, (uchar) vdcMem);
    for (i = 0; i < chars; i++) {
        for (c = 0; c < 8; c++) {
            mem[(i * 8) + c] = inVdc(vdcCPUData);
        }
        /* Skip bottom 8 bytes of VDC data */
        for (c = 0; c < 8; c++) {
            inVdc(vdcCPUData);
        }
    }
}
