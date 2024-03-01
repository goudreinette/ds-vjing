#include <math.h>

#ifndef SCREEN_UTILS_H

#define SCREEN_UTILS_H


// Screen stuff -------------------------------------------------
// typedef volatile unsigned int vu32;
typedef unsigned short u16;
typedef volatile unsigned short vu16;
typedef volatile unsigned char vu8;

#define REG_DISPCNT_MAIN  (*(vu32*)0x04000000)
#define REG_DISPCNT_SUB   (*(vu32*)0x04001000)
#define MODE_FB0          (0x00020000)
#define VRAM_A            ((u16*)0x6800000)
#define VRAM_A_CR         (*(vu8*)0x04000240)
#define VRAM_ENABLE       (1<<7)
#define SCANLINECOUNTER   *(vu16 *)0x4000006

// COLOR uses numbers from (0-31) for each of the RGB components
#define COLOR(r,g,b)  ((r) | (g)<<5 | (b)<<10)
#define RGB(r,g,b)  ((r / 8) | (g / 8)<<5 | (b / 8)<<10)
#define HSL(h,s,l)    (COLOR(0,0,0) | (h)<<10 | (s)<<5 | (l))
#define OFFSET(r,c,w) ((r)*(w)+(c))

#define SCREENWIDTH  (256)
#define SCREENHEIGHT (192)


// void waitForVblank() {
//     while (SCANLINECOUNTER > SCREENHEIGHT);
//     while (SCANLINECOUNTER < SCREENHEIGHT);
// }



// Drawing functions --------------------------------------------
static void setPixel(int row, int col, u16 color) {
    VRAM_A[OFFSET(row, col, SCREENWIDTH)] = color;
}

static void setPixelGfx(u16* videoMem, int row, int col, u16 color) {
    videoMem[OFFSET(col, row, SCREENWIDTH)] = color;
}



static void rect(int row, int col, int width, int height, u16 color) {
    int r, c;
    for (c=col; c<col+width; c++) {
        for (r=row; r<row+height; r++) {
            setPixel(r, c, color);
        }
    }
}

static void drawTriangleFilled(int x1, int y1, int x2, int y2, int x3, int y3, u16 color) {
    int min_x = std::min(x1, std::min(x2, x3));
    int max_x = std::max(x1, std::max(x2, x3));
    int min_y = std::min(y1, std::min(y2, y3));
    int max_y = std::max(y1, std::max(y2, y3));

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            if (x1 == x2 && x2 == x3) {
                if (x == x1) {
                    setPixel(x, y, color);
                }
            } else if (y1 == y2 && y2 == y3) {
                if (y == y1) {
                    setPixel(x, y, color);
                }
            } else {
                int x1_x2 = x1 - x2;
                int x2_x3 = x2 - x3;
                int x3_x1 = x3 - x1;
                int y1_y2 = y1 - y2;
                int y2_y3 = y2 - y3;
                int y3_y1 = y3 - y1;

                int x_x1 = x - x1;
                int x_x2 = x - x2;
                int x_x3 = x - x3;
                int y_y1 = y - y1;
                int y_y2 = y - y2;
                int y_y3 = y - y3;

                int a = x1_x2 * y_y1 - y1_y2 * x_x1;
                int b = x2_x3 * y_y2 - y2_y3 * x_x2;
                int c = x3_x1 * y_y3 - y3_y1 * x_x3;

                if ((a >= 0 && b >= 0 && c >= 0) || (a <= 0 && b <= 0 && c <= 0)) {
                    setPixel(x, y, color);
                }
            }
        }
    }
}

static void drawCircleFilled(int center_y, int center_x, int radius, u16 color) {
    int x = 0;
    int y = radius;
    int d = 1 - radius;

    while (x <= y) {
        for (int i = center_x - x; i <= center_x + x; i++) {
            setPixel(i, center_y + y, color); // Upper half
            setPixel(i, center_y - y, color); // Lower half
        }

        for (int i = center_x - y; i <= center_x + y; i++) {
            setPixel(i, center_y + x, color); // Right half
            setPixel(i, center_y - x, color); // Left half
        }

        x++;

        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
    }
}

#endif
