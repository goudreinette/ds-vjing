#include <nds.h>
#include "screen_utils.h"
#include "scene.h"
#include "../../effolkronium/random.hpp"


class PicoPalette : public Scene
{
    int t = 0;

    int lastX = 0, lastY = 0;

    using Random = effolkronium::random_static;

    // const int palette[9] = {
    //     COLOR(0,0,0), 
    //     COLOR(31,31,31), 
    //     COLOR(31,0,0), 
    //     COLOR(0,31,0), 
    //     COLOR(0,0,31),
    //     COLOR(31,31,0), 
    //     COLOR(31,0,31), 
    //     COLOR(0,31,31), 
    //     COLOR(31,31,31)
    // };
    const int palette[16] = {
        RGB(0,0,0), // black 
        RGB(29,43,83), // dark blue
        RGB(126,37,83), // purple
        RGB(3,135,81), // dark green
        RGB(171,82,54), // brown
        RGB(95,87,79), // dark grey
        RGB(194,195,199), // light grey
        RGB(255,240,232), // white
        RGB(255,0,77), // red
        RGB(255,163,1), // orange
        RGB(255,236,38), // yellow
        RGB(1,228,55), // green
        RGB(41,173,255), // light blue
        RGB(131,118,156), // indigo
        RGB(255,119,168), // pink
        RGB(255,204,170) // peach
    };

public:
    void setup() {
        // Set the main diplay to frame buffer mode 0 (FB0).
        // In FB0 VRAM A is drawn to the screen.
        REG_DISPCNT_MAIN = MODE_FB0;
        // Enable VRAM A
        VRAM_A_CR = VRAM_ENABLE;
    }


    void update() {
        t++;
    }

    void drawCircleSine() {
        int x = SCREENWIDTH/2 + 50 * sin(t/10.0);
        int y = SCREENHEIGHT/2 + 50 * cos(t/10.0);

        drawCircleFilled(lastX, lastY, 10, palette[0]);
        drawCircleFilled(x, y, 10, palette[8]);

        lastX = x;
        lastY = y;
    }

    void draw() {
        // Visual noise
        // for (int i=0; i<100; i++) {
        //     int randomPixel = Random::get(0, SCREENHEIGHT*SCREENWIDTH);
        //     int randomColor = Random::get(0, 9);
        //     VRAM_A[randomPixel] = palette[randomColor];
        // }

        // Draw a small rectangle for every color
        int color = 0;
        for (int x=0; x<4; x++) {
            for (int y=0; y<4; y++) {
                rect(x*16, y*16, 16, 16, palette[color]);
                color++;
            }
        }

        drawCircleSine();

        swiWaitForVBlank();
    }
};
