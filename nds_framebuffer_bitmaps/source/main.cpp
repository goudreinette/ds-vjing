#include <random>
#include <nds.h>
#include "../../effolkronium/random.hpp"
#include "screen_utils.h"


int palette[] = {
    COLOR(0,0,0), 
    COLOR(31,31,31), 
    COLOR(31,0,0), 
    COLOR(0,31,0), 
    COLOR(0,0,31),
    COLOR(31,31,0), 
    COLOR(31,0,31), 
    COLOR(0,31,31), 
    COLOR(31,31,31)
};

// Random --------------------------------------------------------
using Random = effolkronium::random_static;



// The ball ------------------------------------------------------
typedef struct BALL{
    int row;
    int col;
    int size;
    int rdel;
    int cdel;
    u16 color;
} BALL;

BALL ball;
BALL old_ball;

void initialize_ball() {
    ball.row = 10;
    ball.col= 10;
    ball.size = 5;
    ball.rdel= 1;
    ball.cdel = 2;
    ball.color = COLOR(0,0,0); //COLOR(31,31,31);
}





// Drawing functions --------------------------------------------
void setPixel(int row, int col, u16 color) {
    VRAM_A[OFFSET(row, col, SCREENWIDTH)] = color;
}

void drawRect(int row, int col, int width, int height, u16 color) {
    int r, c;
    for (c=col; c<col+width; c++) {
        for (r=row; r<row+height; r++) {
            setPixel(r, c, color);
        }
    }
}

void drawTriangleFilled(int x1, int y1, int x2, int y2, int x3, int y3, u16 color) {
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

void drawCircleFilled(int center_y, int center_x, int radius, u16 color) {
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




// Main routines ------------------------------------------------
void update() {
    old_ball = ball;

    // move ball
    ball.row += ball.rdel;
    ball.col+= ball.cdel;

    // check for collisions with the sides of the screen
    if (ball.row + ball.size > SCREENHEIGHT) {
        ball.row = SCREENHEIGHT - ball.size;
        ball.rdel *= -1;
    }
    if (ball.col + ball.size > SCREENWIDTH) {
        ball.col = SCREENWIDTH- ball.size;
        ball.cdel *= -1;
    }
    if (ball.row < 0) {
        ball.row = 0;
        ball.rdel *= -1;
    }
    if (ball.col< 0) {
        ball.col = 0;
        ball.cdel *= -1;
    }
}

void draw() {
    // slightly erase everything 
    for (int i=0; i<100; i++) {
        int randomPixel = Random::get(0, SCREENHEIGHT*SCREENWIDTH);
        int randomColor = Random::get(0, 9);
        VRAM_A[randomPixel] = palette[randomColor];
        // VRAM_A[randomPixel] = COLOR(0,0,0);
    }

    // erase the ball
    // drawRect(old_ball.row, old_ball.col, old_ball.size, old_ball.size, 0);

    // draw it in its new position
    int randomRectColor = Random::get(0, 9);
    drawRect(ball.row, ball.col, ball.size, ball.size, palette[randomRectColor]);
    drawRect(ball.row, ball.col, ball.size, ball.size, COLOR(0,0,0));

    int randomColor = Random::get(0, 9);
    // drawCircleFilled(SCREENWIDTH/2, SCREENHEIGHT/2, 20, palette[randomColor]);
    drawCircleFilled(SCREENWIDTH/2, SCREENHEIGHT/2, 20, COLOR(0,0,0));
}


int main(void) {
    // Set the main diplay to frame buffer mode 0 (FB0).
    // In FB0 VRAM A is drawn to the screen.
    REG_DISPCNT_MAIN = MODE_FB0;

    // Enable VRAM A
    VRAM_A_CR = VRAM_ENABLE;

    initialize_ball();

    // main loop
    while(1) {
        update();
        // waitForVblank();
        draw();
    }

    return 0;
}
