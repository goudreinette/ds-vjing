#include <nds.h>
#include <nf_lib.h>
#include <filesystem.h>
#include "screen_utils.h"
#include "scene.h"
#include "../../effolkronium/random.hpp"
#include "../perlin/perlin.cpp"





class PerlinScene : public Scene
{
    int t = 0;

    using Random = effolkronium::random_static;


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
        t++;

        for (int i=0; i<100; i++) {
            int x = Random::get(0, SCREENWIDTH);
            int y = Random::get(0, SCREENHEIGHT);

            int randomPixel = Random::get(0, SCREENHEIGHT*SCREENWIDTH);
            int color = noise3d(x, y, t, 1, 1) * 31;
            // VRAM_A[randomPixel] = RGB15(50, 50, 50);            
            VRAM_A[randomPixel] = noise3d(x, y, t, 1, 1) * 31;
        }
    }
};
