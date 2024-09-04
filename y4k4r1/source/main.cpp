#include <nds.h>
#include <stdio.h>
#include <NEMain.h>
#include <math.h>
#include <filesystem.h>
#include <nf_lib.h>

// Models and graphics
#include "heart_bin.h"
#include "texture2.h"
#include "heart.h"
#include "rainbow.h"
#include "texture3.h"
#include "paper.h"

// Includes
#include "scene_data.hpp"
#include "hearts_and_comments.hpp"
#include "wout3d.hpp"
#include "mic.hpp"


void draw_3d_scene(void *arg)
{
    scene_data *scene = (scene_data*) arg;

    NE_CameraUse(scene->camera);

    scanKeys();
    uint32_t keys_held = keysHeld();
    uint32_t keys_down = keysDown();


    // different scenes and features
    hearts_and_comments::update_draw_hearts(scene, keys_held);
    wout_bust::update_draw(scene, keys_held, keys_down);
}




int main(int argc, char *argv[])
{
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");


    mic::setup();

    // setup scene
    scene_data scene = {
            .camera = NE_CameraCreate()
    };


    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init Nitro Engine in normal 3D mode
    NE_Init3D();

    // libnds uses VRAM_C for the text console, reserve A and B only
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // Init console in non-3D screen

    // Setup models and scenes
    hearts_and_comments::load_assets(&scene);
    wout_bust::load_assets(&scene);


    // Set coordinates for the camera
    NE_CameraSet(scene.camera,
                 -10, 0, 0,  // Position
                  0, 0, 0,  // Look at
                  0, 1, 0); // Up direction

  

    // We set up a light and its color
    NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);
    NE_LightSet(2, NE_White, -1, -3, -0.5);

    // NE_ClearColorSet(RGB8(69,40,60), 31, 63);
//    NE_ClearColorSet(RGB8(95,205,228), 31, 63);
    NE_ClearColorSet(RGB8(0,0,0), 31, 63);

    // NE_ClearColorSet(NE_White, 31, 63);
    // setBackdropColorSub(RGB8(34,32,52));
    setBackdropColorSub(RGB8(95,205,228));

    consoleDemoInit();
    // Wrap values of parameters

    // Enable/update fog
//    NE_FogEnable(2, NE_White, 31, 2, 0x7C00);

    NE_SpecialEffectNoiseConfig(31);
    // NE_SpecialEffectSet(NE_NOISE);

    while (true)
    {
        t++;

        // Wait for next frame
        NE_WaitForVBL(NE_UpdateFlags::NE_UPDATE_PHYSICS);

        // Get keys information
//        scanKeys();
//        uint32_t keys = keysHeld();
//        uint32_t down = keysDown();


//        printf("\n \n %i",  mic::peak);

        // Special effects
//        if (down) {
//            glitch_t = 5;
//        }
//
//        if (glitch_t > 0) {
//            glitch_t--;
//            NE_SpecialEffectSet(NE_NOISE);
//        } else {
//            int r = rand() % 300;
//            if (r == 0) {
//                NE_SpecialEffectSet(NE_NOISE);
//            } else if (r == 1) {
//                NE_SpecialEffectSet(NE_SINE);
//            } else {
//                NE_SpecialEffectSet(NE_NONE);
//            }
//        }

            
        // Draw scene
        NE_ProcessArg(draw_3d_scene, &scene);
    }

    return 0;
}


