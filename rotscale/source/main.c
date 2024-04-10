// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of rotating and scaling 3D sprites.
// http://www.nightfoxandco.com

#include <stdio.h>
#include <time.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

#define MAXSPRITES 1


bool ppVisible = true;



// lerp function
int lerp(int a, int b, int t) {
    return a + ((b - a) * t >> 8);
}


int main(int argc, char **argv)
{
    // Set random seed based on the current time
    srand(time(NULL));

    // Initialize NitroFS and set it as the root folder of the filesystem
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");

    // Initialize 3D engine in the top screen in mode 0
    NF_Set3D(0, 0);

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);       // Top screen

    // Initialize 3D sprite system
    NF_InitSpriteBuffers();     // Initialize storage buffers
    NF_Init3dSpriteSys();

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/bg3", "bg3", 256, 256);

    NF_LoadSpriteGfx("sprite/pp", 2, 256, 128);
    NF_LoadSpritePal("sprite/pp", 2);

    // Transfer the required sprites to VRAM
    NF_Vram3dSpriteGfx(2, 2, true);
    NF_Vram3dSpritePal(2, 2);

    // Variables
    s16 x[MAXSPRITES];
    s16 y[MAXSPRITES];
    s16 ix[MAXSPRITES];
    s16 iy[MAXSPRITES];
    s16 rx[MAXSPRITES];
    s16 ry[MAXSPRITES];
    s16 rz[MAXSPRITES];
    s16 scale[MAXSPRITES];

    // Initialize sprite variables and create the sprites
    for (int n = 0; n < MAXSPRITES; n++)
    {
        int r;
        rx[n] = 0;
        ry[n] = 0;
        rz[n] = 0;
        scale[n] = 64;

        NF_Create3dSprite(n, 2, 2, 0, 32);
    }

    // Sort their priorites based on their IDs (lower IDs have higher priority)
    NF_Sort3dSprites();

    // Variable that contains the ID of the sprite selected by the user
    s16 id = 0;

    int t = 0;

    while (1)
    {
        t++;

        // Read keys
        scanKeys();
        u16 press = keysDown();

        if (press & KEY_A) {
            ppVisible = !ppVisible;
        }
        scale[id] = lerp(scale[id], ppVisible ? 64 : 0, 16);

        ry[id] += 3;
        if (ry[id] > 512)
            ry[id] -= 512;

        // Apply rotation and scale
        NF_Rotate3dSprite(id, rx[id], ry[id], rz[id]);
        NF_Scale3dSprite(id, scale[id], scale[id]);

        // Draw all 3D sprites
        NF_Draw3dSprites();

        // Tell the GPU to draw the scene and wait until it's done
        glFlush(0);

        // Print debug information
        consoleClear();
        printf("A / B - Sprite select %d\n", id);
        printf("Rotate Z (LEFT/RIGHT) %d\n", rz[id]);
        printf("Rotate Y (UP/DOWN) %d\n", ry[id]);
        printf("Rotate X (X/Y) %d\n", rx[id]);
        printf("Scale (R/L) %d\n", scale[id]);

        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
