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


// Stars
struct Star {
    int x;
    int y;
    int blinkEveryN;
};

Star stars[20];

// Clouds
struct Cloud {
    int x;
    int y;
    int scale;
};

Cloud clouds[10];

// Sprites
struct {
    int bby = 1;
    int sparkle1 = 2;
    int sparkle2 = 3;
    int clouds = 4; // 2-9
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

void setupStars() {
    for (int i = 0; i < 20; i++) {
        stars[i] = {rand() % 32, rand() % 24, rand() % 100 + 20};
    }
}

void setupSparkleSprites() {
    NF_LoadSpriteGfx("sprite/sparkle1", spriteIndexes.sparkle1, 16, 16);
    NF_LoadSpritePal("sprite/sparkle1", spriteIndexes.sparkle1);
    NF_LoadSpriteGfx("sprite/sparkle2", spriteIndexes.sparkle2, 16, 16);

    NF_Vram3dSpriteGfx(spriteIndexes.sparkle1, spriteIndexes.sparkle1, false);
    NF_Vram3dSpriteGfx(spriteIndexes.sparkle2, spriteIndexes.sparkle2, false);
    NF_Vram3dSpritePal(spriteIndexes.sparkle1, spriteIndexes.sparkle1);

    NF_Create3dSprite(spriteIndexes.sparkle1, spriteIndexes.sparkle1, spriteIndexes.sparkle1, 240, 90);
    NF_Create3dSprite(spriteIndexes.sparkle2, spriteIndexes.sparkle2, spriteIndexes.sparkle1, 0, 70);
}

void setupbbySprite() {
    NF_LoadSpriteGfx("sprite/bby", spriteIndexes.bby, 256, 128);
    NF_LoadSpritePal("sprite/bby", spriteIndexes.bby);

    NF_Vram3dSpriteGfx(spriteIndexes.bby, spriteIndexes.bby, true);
    NF_Vram3dSpritePal(spriteIndexes.bby, spriteIndexes.bby);

    NF_Create3dSprite(spriteIndexes.bby, spriteIndexes.bby, spriteIndexes.bby, 0, 16);
}


void setupCloudsSprites() {
    NF_LoadSpriteGfx("sprite/cloud", spriteIndexes.clouds, 64, 64);
    NF_LoadSpritePal("sprite/cloud", spriteIndexes.clouds);
    NF_Vram3dSpriteGfx(spriteIndexes.clouds, spriteIndexes.clouds, true);
    NF_Vram3dSpritePal(spriteIndexes.clouds, spriteIndexes.clouds);

    // Add clouds
    clouds[0] = {-40, 0, 1};
    clouds[1] = {-10, -32, 1};
    clouds[2] = {36, -48, 1};
    clouds[3] = {72, -48, 1};
    clouds[4] = {100, 180, 1};
    clouds[5] = {140, 170, 1};
    clouds[6] = {180, 160, 1};
    clouds[7] = {220, 140, 1};
    clouds[8] = {-32, 170, 1};
    clouds[9] = {220, -48, 1};

    for (int i = 0; i < 10; i++) {
        NF_Create3dSprite(spriteIndexes.clouds + i, spriteIndexes.clouds, spriteIndexes.clouds, clouds[i].x, clouds[i].y);
    }
}




void setupTilesBg() {
    // Load background files from NitroFS
    NF_LoadTiledBg("bg/tiles", "tilesTop", 512, 512);
    NF_LoadTiledBg("bg/tiles", "tilesBottom", 512, 512);

    // Create top screen backgrounds
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
    setupCloudsSprites();
    setupSparkleSprites();
    setupStars();
    setupTilesBg();

    NF_Sort3dSprites();
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


void sparkleStars() {
    for (int i = 0; i < 20; i++) {
        NF_SetTileOfMap(0, bgLayers.tilesTop, stars[i].x, stars[i].y, t % stars[i].blinkEveryN < 5 ? 51 : 50);
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

    if (behaviours.bbyVisible) {
        NF_Show3dSprite(spriteIndexes.sparkle1, true);
        NF_Show3dSprite(spriteIndexes.sparkle2, true);
    } else {
        NF_Show3dSprite(spriteIndexes.sparkle1, false);
        NF_Show3dSprite(spriteIndexes.sparkle2, false);
    }
    
    // Apply rotation and scale
    NF_Rotate3dSprite(spriteIndexes.bby, 0, bbyTransform.rotation, 0);
    NF_Scale3dSprite(spriteIndexes.bby, bbyTransform.scale, bbyTransform.scale);
}


void updateClouds() {
    for (int i = 0; i < 10; i++) {
        // do simplex random
        float x_offset = SimplexNoise::noise(0.01 * i * 100, 0.01, t / 100.0) * 2;
        float y_offset = SimplexNoise::noise(0.01 * i  * 100 + 1000, 0.01, t / 100.0) * 2;
        float scale_offset = SimplexNoise::noise(0.01 * i  * 100 + 1000, 0.01, t / 100.0);
        NF_Move3dSprite(spriteIndexes.clouds + i, clouds[i].x + x_offset, clouds[i].y + y_offset);
        // NF_Scale3dSprite(spriteIndexes.clouds + i, bbyTransform.scale + scale_offset, bbyTransform.scale + scale_offset);
    }
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
        } else {
            // sparkleStars();
        }

        sparkleStars();


        updateBby(keys & KEY_TOUCH);
        updateClouds();



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
