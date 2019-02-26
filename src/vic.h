/*
 * C128 8564/8566 VIC-IIe.
 *
 * Copyright (c) Steven P. Goldsmith. All rights reserved.
 */

#define vicSpr0X       0xd000 /* Sprite 0 horizontal position register */
#define vicSpr0y       0xd001 /* Sprite 0 vertical position register */
#define vicSpr1X       0xd002 /* Sprite 1 horizontal position register */
#define vicSpr1y       0xd003 /* Sprite 1 vertical position register */
#define vicSpr2X       0xd004 /* Sprite 2 horizontal position register */
#define vicSpr2y       0xd005 /* Sprite 2 vertical position register */
#define vicSpr3X       0xd006 /* Sprite 3 horizontal position register */
#define vicSpr3y       0xd007 /* Sprite 3 vertical position register */
#define vicSpr4X       0xd008 /* Sprite 4 horizontal position register */
#define vicSpr4y       0xd009 /* Sprite 4 vertical position register */
#define vicSpr5X       0xd00a /* Sprite 5 horizontal position register */
#define vicSpr5y       0xd00b /* Sprite 5 vertical position register */
#define vicSpr6X       0xd00c /* Sprite 6 horizontal position register */
#define vicSpr6y       0xd00d /* Sprite 6 vertical position register */
#define vicSpr7X       0xd00e /* Sprite 7 horizontal position register */
#define vicSpr7y       0xd00f /* Sprite 7 vertical position register */
#define vicSprXmsb     0xd010 /* Sprites 0-7 horizontal position (most significant bits) */
#define vicCtrlReg1    0xd011 /* Control/vertical fine scrolling register */
#define vicRaster      0xd012 /* Raster scan-line register */
#define vicLightPenX   0xd013 /* Light pen horizontal position */
#define vicLightPenY   0xd014 /* Light pen vertical position */
#define vicSprEnable   0xd015 /* Sprite enable register */
#define vicCtrlReg2    0xd016 /* Control/horizontal fine scrolling register */
#define vicSprYexp     0xd017 /* Sprite vertical expansion register */
#define vicMemCtrl     0xd018 /* Memory control register */
#define vicIntFlag     0xd019 /* Interrupt mask register */
#define vicIntMask     0xd01a /* Interrupt flag register */
#define vicSprFg       0xd01b /* Sprite-to-foreground priority register */
#define vicSprMcm      0xd01c /* Sprite multicolor mode register */
#define vicSprXexp     0xd01d /* Sprite horizontal expansion register */
#define vicSprColl     0xd01e /* Sprite-sprite collision register */
#define vicSprFgColl   0xd01f /* Sprite-foreground collision register */
#define vicBorderCol   0xd020 /* Border color register */
#define vicBgCol0      0xd021 /* Background color (source 0) register */
#define vicBgCol1      0xd022 /* Background color (source 1) register */
#define vicBgCol2      0xd023 /* Background color (source 2) register */
#define vicBgCol3      0xd024 /* Background color (source 3) register */
#define vicSprMcmCol0  0xd025 /* Sprite multicolor (source 0) register */
#define vicSprMcmCol1  0xd026 /* Sprite multicolor (source 1) register */
#define vicSpr0Col     0xd027 /* Sprite 0 color register */
#define vicSpr1Col     0xd028 /* Sprite 1 color register */
#define vicSpr2Col     0xd029 /* Sprite 2 color register */
#define vicSpr3Col     0xd02a /* Sprite 3 color register */
#define vicSpr4Col     0xd02b /* Sprite 4 color register */
#define vicSpr5Col     0xd02c /* Sprite 5 color register */
#define vicSpr6Col     0xd02d /* Sprite 6 color register */
#define vicSpr7Col     0xd02e /* Sprite 7 color register */
#define vicExtKey      0xd02f /* Extended keyboard scan register */
#define vicPcrcr       0xd030 /* Processor clock rate control register */

#define vicColMem 0xd800 /* VIC I/O mapped color memory */
#define vicScrSize 1000  /* VIC screen size in bytes */
#define vicScrSizeW 500  /* VIC screen size in words */
#define vicBmpSizeW 4000 /* VIC bitmap size in words */

extern uchar *allocVicMem(uchar maxBank);
extern void setVicMmuBank(uchar mmuRcr);
extern void setVicBank(uchar vicBank);
extern void setVicMode(uchar ecm, uchar bmm, uchar mcm);
extern void setVicScrMem(uchar scrMem);
extern void setVicChrMem(uchar chrMem);
extern void setVicChrMode(uchar mmuRcr, uchar vicBank, uchar scrMem, uchar chrMem);
extern void fillVicScr(uchar *scr, ushort start, ushort len, ushort value);
extern void clearVicScr(uchar *scr, uchar c);
extern void clearVicCol(uchar color);
extern void printVic(uchar *scr, uchar x, uchar y, char *str);
extern void printVicCol(uchar *scr, uchar x, uchar y, uchar color, char *str);
extern char *asciiToPet(char *str);
extern void printVicPet(uchar *scr, uchar x, uchar y, char *str);
extern void printVicColPet(uchar *scr, uchar x, uchar y, uchar color, char *str);
extern void scrollVicUpX(uchar *scr, uchar x, uchar y, uchar len, uchar lines);
extern void scrollVicUp(uchar *scr, uchar y, uchar lines);
extern char *readVicLine(uchar *scr, uchar x, uchar y, uchar len);
extern void setVicBmpMem(uchar bmpMem);
extern void setVicBmpMode(uchar mmuRcr, uchar vicBank, uchar scrMem, uchar bmpMem);
extern void clearVicBmp(uchar *bmp, uchar c);
extern void clearVicBmpCol(uchar *scr, uchar color);
extern void setVicPix(uchar *bmp, ushort x, uchar y);
extern void clearVicPix(uchar *bmp, ushort x, uchar y);
extern void printVicBmp(uchar *bmp, uchar *scr, uchar *chr, uchar x, uchar y, uchar color, char *str);
extern void drawVicLineH(uchar *bmp, ushort x, uchar y, ushort len, uchar setPix);
extern void drawVicLineV(uchar *bmp, ushort x, uchar y, ushort len, uchar setPix);
extern void drawVicLine(uchar *bmp, int x1, int y1, int x2, int y2, uchar setPix);
extern void drawVicBezier(uchar *bmp, int x0, int y0, int x1, int y1, int x2, int y2, uchar setPix);
extern void drawVicRect(uchar *bmp, int x0, int y0, int x1, int y1, uchar setPix);
extern void drawVicSquare(uchar *bmp, int x, int y, int len, uchar setPix);
extern void drawVicOctant(uchar *bmp, int xc, int yc, int x, int y, uchar setPix);
extern void drawVicEllipse(uchar *bmp, int x0, int y0, int x1, int y1, uchar setPix);
extern void drawVicCircle(uchar *bmp, int xc, int yc, int a, uchar setPix);
