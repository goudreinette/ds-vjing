#include <nds.h>
#include <nf_lib.h>
#include <filesystem.h>
#include "screen_utils.h"
#include "scene.h"
#include "../../effolkronium/random.hpp"


class WithSprites : public Scene
{
    int t = 0;

    using Random = effolkronium::random_static;

    int CIRCLE_IMG = 2;


    void initBGSys() {
        // Initialize bitmap backgrounds system
        NF_InitBitmapBgSys(0, 1);
        // Initialize storage buffers
        NF_Init16bitsBgBuffers();
        // Initialize backbuffers
        NF_Init16bitsBackBuffer(0);
        // Enable backbuffers
        NF_Enable16bitsBackBuffer(0);
    }

public:
    void setup() {

        // Set the main diplay to frame buffer mode 0 (FB0).
        // In FB0 VRAM A is drawn to the screen.
        // REG_DISPCNT_MAIN = MODE_FB0;
        // // Enable VRAM A
        // VRAM_A_CR = VRAM_ENABLE;
        
        // Initialize 2D engine in both screens and use mode 0
        // NF_Set2D(0, 5);
        // NF_Set2D(1, 5);

        // // Initialize NitroFS and set it as the root folder of the filesystem
        // nitroFSInit(NULL);
        // NF_SetRootFolder("NITROFS");
        
        // initBGSys();    

        // NF_Load16bitsBg("clear", 1);
        // NF_Load16bitsImage("circlesmall", CIRCLE_IMG, 16, 16);
    }


    void update() {
        
    }


    void draw() {
        for (int i=0; i<100; i++) {
            int randomPixel = Random::get(0, SCREENHEIGHT*SCREENWIDTH);
            VRAM_A[randomPixel] = i % 2 == 0 ? COLOR(0,0,0) : COLOR(31,31,31);
        }

        // on touch
        // touchPosition touch;
        // touchRead(&touch);
        
        // if (touch.px > 0) {
        //     NF_Copy16bitsBuffer(0, 1, 1);
        //     NF_Flip16bitsBackBuffer(0);
        //     NF_Draw16bitsImage(0, CIRCLE_IMG, 100, 100, true);
        // }
    }
};
