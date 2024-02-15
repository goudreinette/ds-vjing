#include <stdio.h>
#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>


void setupGraphicsSystems() {
    // Initialize 2D engine in both screens and use mode 0
    NF_Set2D(0, 5);
    NF_Set2D(1, 5);

    // Initialize NitroFS and set it as the root folder of the filesystem
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");

    // Initialize bitmap backgrounds system
    NF_InitBitmapBgSys(0, 1);

    // Initialize storage buffers
    NF_Init16bitsBgBuffers();

    // Initialize storage buffers
    NF_Init16bitsBgBuffers();

    // Init + enable backbuffers
    NF_Enable16bitsBackBuffer(0);
    NF_Init16bitsBackBuffer(0);
}


void loadGraphics() {
    NF_Load16bitsBg("test-bg", 0);
}



int main() {
    setupGraphicsSystems();
    loadGraphics();

    // Load image to VRAM of both screens
    NF_Copy16bitsBuffer(0, 0, 0);

    consoleDemoInit();
    printf("\n Touch to play sound \n\n");
   

    while (1) {
        // Get keys
        scanKeys(); // Read keypad
        u16 newpress = keysDown(); // Keys just pressed

        // Get touch
        touchPosition touch;
        touchRead(&touch);

        // Get mic input
        if (newpress & KEY_TOUCH) {
            
        }

        // Wait for the screen refresh
        swiWaitForVBlank();
    }
}