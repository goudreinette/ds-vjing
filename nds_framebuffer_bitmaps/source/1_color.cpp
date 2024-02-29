#include <nds.h>
#include "screen_utils.h"
#include "scene.h"
#include "math.h"
#include "../../effolkronium/random.hpp"



class FirstSketch : public Scene
{
    int t = 0;

    using Random = effolkronium::random_static;


    const int palette[9] = {
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




public:
    void setup() {
        // Set the main diplay to frame buffer mode 0 (FB0).
        // In FB0 VRAM A is drawn to the screen.
        REG_DISPCNT_MAIN = MODE_FB0;
        // Enable VRAM A
        VRAM_A_CR = VRAM_ENABLE;

        initialize_ball();
    }


    void update() {
        t++;

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
        int countOfPalette = std::size(palette);

        for (int i=0; i<100; i++) {
            int randomPixel = Random::get(0, SCREENHEIGHT*SCREENWIDTH);
            int randomColor = Random::get(0, countOfPalette);
            VRAM_A[randomPixel] = palette[randomColor];
            // VRAM_A[randomPixel] = COLOR(0,0,0);
            // VRAM_A[randomPixel] = i % 2 == 0 ? COLOR(0,0,0) : COLOR(31,31,31)
        }

        // erase the ball
        // rect(old_ball.row, old_ball.col, old_ball.size, old_ball.size, 0);

        // draw it in its new position
        int randomRectColor = Random::get(0, 9);
        rect(ball.row, ball.col, ball.size, ball.size, palette[randomRectColor]);
        rect(ball.row, ball.col, ball.size, ball.size, COLOR(0,0,0));

        int randomColor = Random::get(0, 9);
        int circleSize = sin(t / 100.0) * 50.0;
        drawCircleFilled(SCREENWIDTH/2, SCREENHEIGHT/2, 20, palette[randomColor]);

        for (int i=1; i<9; i++) {
            drawCircleFilled(SCREENWIDTH/2, SCREENHEIGHT/2, 20, palette[randomColor]);
            // drawCircleFilled(SCREENWIDTH/2, SCREENHEIGHT/2, circleSize - i *10, i %2 == 0 ? COLOR(0,0,0) : COLOR(31,31,31));
        }

        // random lines  
        int start = Random::get(0, SCREENHEIGHT*SCREENWIDTH);
        
        // for (int y =0; y<10; y++) {
        //     dmaFillWords(COLOR(0,0,0), VRAM_A + start * SCREENHEIGHT, 20);
        // }
    }
};
