#include <stdio.h>
#include <nds.h>
#include <filesystem.h>
#include <nf_lib.h>


// Background layers
const int tilesTopLayer = 3;
const int tilesBottomLayer = 0;

// Background scroll variables
s16 x = 128;
s16 y = 96;

// Colors 
const int picoWhite = ARGB16(1, 31, 30, 29);

// Time
int t = 0;

// Global state 
int currentTile = 0;


/*
|--------------------------------------------------------------------------
| Setup
|--------------------------------------------------------------------------
*/
void setupText() {
    // Initialize text system
    NF_InitTextSys(0);          // Top screen
    // Load text font files from NitroFS
    NF_LoadTextFont("fnt/default", "normal", 256, 256, 0);
    // Create a text layer
    NF_CreateTextLayer(0, 2, 0, "normal");
}


void setupPointerSprite() {
    // Load sprite files from NitroFS
    NF_LoadSpriteGfx("sprite/pointer", 0, 8, 8);
    NF_LoadSpritePal("sprite/pointer", 0);

    // Transfer the required sprites to VRAM
    NF_VramSpriteGfx(1, 0, 0, true);
    NF_VramSpritePal(1, 0, 0);

    // Create sprite and set its priority layer
    NF_CreateSprite(1, 0, 0, 0, 124, 92);
    NF_SpriteLayer(1, 0, 2);
}


void setupGraphics() {
    // Prepare a NitroFS initialization screen
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);
    consoleDemoInit();
    printf("\n NitroFS init. Please wait.\n\n");
    printf(" Iniciando NitroFS,\n por favor, espere.\n\n");
    swiWaitForVBlank();

    // Initialize NitroFS and set it as the root folder of the filesystem
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");

    // Initialize 2D engine in both screens and use mode 0
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);

    // BG colors
    setBackdropColor(picoWhite); // Set the backdrop color to pico white
    setBackdropColorSub(picoWhite); // Set the backdrop color to pico white

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);       // Top screen
    NF_InitTiledBgSys(1);       // Bottom screen

    // Initialize sprite system
    NF_InitSpriteBuffers();     // Initialize storage buffers
    NF_InitSpriteSys(0);        // Top screen
    NF_InitSpriteSys(1);        // Bottom screen

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/tiles", "tilesTop", 512, 512);
    NF_LoadTiledBg("bg/tiles", "tilesBottom", 512, 512);

    // Create top screen background
    NF_CreateTiledBg(0, tilesTopLayer, "tilesTop");

    // Create bottom screen backgrounds
    NF_CreateTiledBg(1, tilesBottomLayer, "tilesBottom");

    setupPointerSprite();
    setupText();
}

/*
|--------------------------------------------------------------------------
| Update
|--------------------------------------------------------------------------
*/
void updateMapScroll(int keys) {
    if (keys & KEY_UP) {
        if (t%10 == 0) {
            currentTile--;
        }
        y--;
    }
    if (keys & KEY_DOWN) {
        if (t%10 == 0) {
            currentTile++;    
        }
        y++;    
    }

    if (keys & KEY_LEFT)
        x--;
    if (keys & KEY_RIGHT)
        x++;

    // Movement limits
    if (x < 0)
        x = 0;
    if (x > 767)
        x = 767;

    if (y < 0)
        y = 0;
    if (y > 511)
        y = 511;

    // Calculate background scroll from the coordinates of the pointer
    int bg_x = x - 128;
    if (bg_x < 0)
        bg_x = 0;
    if (bg_x > 512)
        bg_x = 512;

    int bg_y = y - 96;
    if (bg_y < 0)
        bg_y = 0;
    if (bg_y > 320)
        bg_y = 320;

    // Pointer sprite position
    int spr_x = (x - bg_x) - 4;
    int spr_y = (y - bg_y) - 4;
    NF_MoveSprite(1, 0, spr_x, spr_y);

    // Update scroll
    NF_ScrollBg(1, 0, bg_x, bg_y);
}



void fillRandomTiles() {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            NF_SetTileOfMap(0, tilesTopLayer, i, j, rand() % 16);
            NF_SetTileOfMap(1, tilesBottomLayer, i, j, rand() % 16 + 25); //25
            // NF_SetTileOfMap(0, tilesTopLayer, i, j, rand() % 64);
            // NF_SetTileOfMap(1, tilesBottomLayer, i, j, rand() % 64);
        }
    }
}

void randomizeSomeTiles(int amount = 128) {
    for (int i = 0; i < amount; i++) {
        NF_SetTileOfMap(0, tilesTopLayer, rand() % 32, rand() % 32, rand() % 16);
        NF_SetTileOfMap(1, tilesBottomLayer, rand() % 32, rand() % 32, rand() % 24 + 32); //25
        // NF_SetTileOfMap(1, tilesBottomLayer, rand() % 64, rand() % 32, rand() % 64); //25
    }
}

void fillChanceEmpty(int chance = 50) {
    for (int i = 0; i < 128; i++) {
        bool empty;

        if (rand() % 100 > chance) {
            empty = true;
        } else {
            empty = false;
        }

        NF_SetTileOfMap(0, tilesTopLayer, rand() % 32, rand() % 32, empty ? 0 : rand() % 16);
        NF_SetTileOfMap(1, tilesBottomLayer, rand() % 32, rand() % 32, empty ? 0 : rand() % 24 + 32); //25
        // NF_SetTileOfMap(1, tilesBottomLayer, rand() % 64, rand() % 32, rand() % 64); //25
    }
}

void printCursorPositionAndTileUnderCursor() {
    // Print pointer position
    char mytext[60];
    sprintf(mytext,"x:%d  y:%d ", x, y);
    NF_WriteText(0, 2, 1, 1, mytext);

    // Print the color of the tile under the pointer
    int tilenum = NF_GetTileOfMap(1, 0, x / 8, y / 8);
    sprintf(mytext," %d   ", tilenum);
    NF_WriteText(0, 2, 1, 3, mytext);
}


void updateBothVramMaps() {
    NF_UpdateVramMap(0, tilesTopLayer);
    NF_UpdateVramMap(1, tilesBottomLayer);
}



/*
|--------------------------------------------------------------------------
| Main
|--------------------------------------------------------------------------
*/
int main(int argc, char **argv)
{
    setupGraphics();
    // fillRandomTiles();
    // randomizeSo/meTiles();
    
    // updateBothVramMaps();

    while (true) {
        t++;

        // Read keys
        scanKeys();
        u16 keys = keysHeld();

        // Read touch screen
        touchPosition touch;
        touchRead(&touch);

        int emptyChance = touch.py;
        fillChanceEmpty(emptyChance);


        // Change the tile under the pointer if the user presses a button
        // randomizeSomeTiles(25);
        updateBothVramMaps();



        printCursorPositionAndTileUnderCursor();

        // Update text layers
        NF_UpdateTextLayers();

        // // Update OAM array
        // NF_SpriteOamSet(1);

        // Wait for the screen refresh
        swiWaitForVBlank();

        // // Update OAM
        // oamUpdate(&oamSub);
    }
}
