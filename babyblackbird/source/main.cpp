#include <stdio.h>
#include <nds.h>
#include <filesystem.h>
#include <nf_lib.h>

#include "./simplex/SimplexNoise.cpp"
#include <cmath>


// Deps
SimplexNoise noise;


// Background layers
struct {
    int tilesTop = 1;
    int tilesBottom = 0;
} bgLayers;


// Sprites
struct {
    int cursor = 0;
    int bby = 1;
} spriteIndexes;

// Background scroll variables
s16 x = 128;
s16 y = 96;

// Colors 
const int darkBlue = ARGB16(1, 4, 5, 10);
const int picoWhite = ARGB16(1, 31, 30, 29);
const int black = ARGB16(1, 0, 0, 0);

// Time
int t = 0;


// Global state 
int currentTile = 0;
int tileOffset = 0;

float perlinScale = 1;

struct {
    int scale = 64;
    int rotation = 0;
} bbyTransform;



// Behaviours
struct {
    bool bbyVisible = true;
    bool bbySpinning = false;
    bool bbyScaleControlledByTouch = false;

    bool fillRateControlledByTouch = false;
    bool drawingWithTouch = false;
    bool movingMapWithTouch = false;

    bool simplexNoise = true;
    bool fillRandomTiles = false;
} behaviours;


// Tile indexes
struct {
    int dither[2] = {64, 80};
} tileIndexes;


touchPosition touch;


/*
|--------------------------------------------------------------------------
| Utils
|--------------------------------------------------------------------------
*/
int lerp(int a, int b, int t) {
    return a + ((b - a) * t >> 8);
}

/*
|--------------------------------------------------------------------------
| Setup
|--------------------------------------------------------------------------
*/
void setupText() {
    // Initialize text system
    NF_InitTextSys(0); // Top screen
    // Load text font files from NitroFS
    NF_LoadTextFont("fnt/default", "normal", 256, 256, 0);
    // Create a text layer
    NF_CreateTextLayer(0, 2, 0, "normal");
}


void setupbbySprite() {
    NF_LoadSpriteGfx("sprite/bby", spriteIndexes.bby, 256, 128);
    NF_LoadSpritePal("sprite/bby", spriteIndexes.bby);

    NF_Vram3dSpriteGfx(spriteIndexes.bby, spriteIndexes.bby, true);
    NF_Vram3dSpritePal(spriteIndexes.bby, spriteIndexes.bby);

    NF_Create3dSprite(spriteIndexes.bby, spriteIndexes.bby, spriteIndexes.bby, 0, 32);
    NF_Sort3dSprites();
}


void setupTilesBg() {
    // Load background files from NitroFS
    NF_LoadTiledBg("bg/tiles", "tilesTop", 512, 512);
    NF_LoadTiledBg("bg/tiles", "tilesBottom", 512, 512);

    // Create top screen background
    NF_CreateTiledBg(0, bgLayers.tilesTop, "tilesTop");

    // Create bottom screen backgrounds
    NF_CreateTiledBg(1, bgLayers.tilesBottom, "tilesBottom");
}



void setupGraphics() {
    // Initialize NitroFS and set it as the root folder of the filesystem
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");

    // Initialize 2D engine in both screens and use mode 0
    NF_Set3D(0, 0);
    NF_Set2D(1, 0);

    // BG colors
    setBackdropColor(darkBlue); // Set the backdrop color to pico white
    setBackdropColorSub(darkBlue); // Set the backdrop color to pico white

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);       // Top screen
    NF_InitTiledBgSys(1);       // Bottom screen
    NF_Init3dSpriteSys();

    // Initialize sprite system
    NF_InitSpriteBuffers();     // Initialize storage buffers

    setupbbySprite();
    // setupAlgoraveTextBg();
    setupTilesBg();

}

/*
|--------------------------------------------------------------------------
| Update
|--------------------------------------------------------------------------
*/
void fillRandomTiles() {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            NF_SetTileOfMap(0, bgLayers.tilesTop, i, j, rand() % 16);
            NF_SetTileOfMap(1, bgLayers.tilesBottom, i, j, rand() % 16 + 25); //25
        }
    }
}

void randomizeSomeTiles(int amount = 128) {
    for (int i = 0; i < amount; i++) {
        NF_SetTileOfMap(0, bgLayers.tilesTop, rand() % 32, rand() % 32, rand() % 16);
        NF_SetTileOfMap(1, bgLayers.tilesBottom, rand() % 32, rand() % 32, rand() % 24 + 32); //25
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

        NF_SetTileOfMap(1, bgLayers.tilesBottom, rand() % 32, rand() % 32, empty ? 0 : rand() % 16);
        NF_SetTileOfMap(0, bgLayers.tilesTop, rand() % 32, rand() % 32, empty ? 0 : rand() % 6 + 36); //25
    }
}


void updateBothVramMaps() {
    NF_UpdateVramMap(0, bgLayers.tilesTop);
    NF_UpdateVramMap(1, bgLayers.tilesBottom);
}

void drawSimplexNoise(float scale = 1) {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            if (rand() % 100 > 85) {
                float t_ = t * 0.01;
                float value0 = SimplexNoise::noise(0.01 * i * scale, 0.01 * j * scale, t_);
                float value1 = SimplexNoise::noise(0.01 * i * scale, 0.01 * (j + 24) * scale, t_);

                int tile0 = abs(value0) * 8 + tileOffset;
                int tile1 = abs(value1) * 8 + tileOffset;

                NF_SetTileOfMap(0, bgLayers.tilesTop, i, j, tile0);
                NF_SetTileOfMap(1, bgLayers.tilesBottom, i, j, tile1);
            }
        }
    }
}

void updateBby(bool controlledByTouch = false) {
    if (controlledByTouch && behaviours.bbyScaleControlledByTouch) {
        bbyTransform.scale = lerp(touch.py, bbyTransform.scale, 4);
    } else {
        bbyTransform.scale = lerp(bbyTransform.scale, behaviours.bbyVisible ? 64 : 0, 32);
    }

    if (behaviours.bbySpinning) {
        bbyTransform.rotation += 10;
        if (bbyTransform.rotation > 512)
            bbyTransform.rotation -= 512;
    } else {
        bbyTransform.rotation = lerp(0, bbyTransform.rotation, 128);
    }
    
    // Apply rotation and scale
    NF_Rotate3dSprite(spriteIndexes.bby, 0, bbyTransform.rotation, 0);
    NF_Scale3dSprite(spriteIndexes.bby, bbyTransform.scale, bbyTransform.scale);
}


/*
|--------------------------------------------------------------------------
| Main
|--------------------------------------------------------------------------
*/
int main(int argc, char **argv)
{
    setupGraphics();

    while (true) {
        t++;

        // Read keys, touch and piano
        scanKeys();
        u16 keys = keysHeld();
        u16 down = keysDown();

        
        touchRead(&touch);

        pianoScanKeys();
        u16 piano = pianoKeysDown();


        /*
        |--------------------------------------------------------------------------
        | Toggle behaviours
        */
        if (down & KEY_DOWN || piano & PIANO_C) {
            behaviours.bbyVisible = !behaviours.bbyVisible;
            if (behaviours.bbyVisible) {
                // NF_CreateTiledBg(0, bgLayers.algoraveText, "algoraveText");
            } else {
                // NF_CreateTiledBg(0, bgLayers.algoraveText, "algoraveTextEmpty");
            }
        }

        if (down & KEY_A || piano & PIANO_D) {
            behaviours.simplexNoise = !behaviours.simplexNoise;
        }

        if (down & KEY_B || piano & PIANO_E) {
            behaviours.fillRandomTiles = !behaviours.fillRandomTiles;
        }

        if (down & KEY_Y || piano & PIANO_F) {
            behaviours.bbyVisible = !behaviours.bbyVisible;
        }

        if (down & KEY_X || piano & PIANO_G) {
            behaviours.bbySpinning = !behaviours.bbySpinning;
        }

        if (down & KEY_L || piano & PIANO_A) {
            behaviours.bbyScaleControlledByTouch = !behaviours.bbyScaleControlledByTouch;
        }

        



        /*
        |--------------------------------------------------------------------------
        | Excecute behaviours
        */
        if (behaviours.simplexNoise) {
            // PERLIN OPTIONS
            if (keys & KEY_LEFT) {
                tileOffset = touch.px * 0.2;
                perlinScale = touch.py * 0.02;
            }
            drawSimplexNoise(perlinScale);
        }

        if (behaviours.fillRandomTiles) {
            int emptyChance = touch.py;
            fillChanceEmpty(emptyChance);
        }


        updateBby(keys & KEY_TOUCH);



        // Draw all 3D sprites
        NF_Draw3dSprites();

        // Tell the GPU to draw the scene and wait until it's done
        glFlush(0);

        // Change the tile under the pointer if the user presses a button
        // randomizeSomeTiles(25);
        updateBothVramMaps();

        // Update text layers
        NF_UpdateTextLayers();

        // Update OAM array
        NF_SpriteOamSet(1);

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Update OAM
        oamUpdate(&oamSub);
    }
}
