#include <stdio.h>
#include <nds.h>
#include <filesystem.h>
#include <nf_lib.h>

#include "./simplex/SimplexNoise.cpp"


// Deps
SimplexNoise noise;


// Background layers
const int tilesTopLayer = 1;
const int algoraveTextLayer = 3;
const int tilesBottomLayer = 0;

// Sprites
struct {
    int cursor = 0;
    int parallelProblems = 1;
} spriteIndexes;

// Background scroll variables
s16 x = 128;
s16 y = 96;

// Colors 
const int picoWhite = ARGB16(1, 31, 30, 29);

// Time
int t = 0;

// Global state 
int currentTile = 0;
int tileOffset = 0;

float perlinScale = 1;


// Behaviours
struct {
    bool parallelProblemsVisible = false;
    bool parallelProblemsSpinning = false;

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
    NF_VramSpriteGfx(1, spriteIndexes.cursor, spriteIndexes.cursor, true);
    NF_VramSpritePal(1, spriteIndexes.cursor, spriteIndexes.cursor);

    // Create sprite and set its priority layer
    NF_CreateSprite(1, spriteIndexes.cursor, spriteIndexes.cursor, spriteIndexes.cursor, 124, 92);
    NF_SpriteLayer(1, 0, 2);
}


void setupParallelProblemsSprite() {
    NF_LoadSpriteGfx("sprite/pp", 0, 256, 128);
    NF_LoadSpritePal("sprite/pp", 0);

    NF_Vram3dSpriteGfx(0, 0, true);
    NF_Vram3dSpritePal(0, 0);
}

void setupAlgoraveTextBg() {
    NF_LoadAffineBg("bg/pp", "algoraveText", 256, 256);
    NF_LoadAffineBg("bg/algorave-text-empty", "algoraveTextEmpty", 256, 256);

    NF_CreateAffineBg(0, algoraveTextLayer, "algoraveTextEmpty", 0);
}

void setupTilesBg() {
    // Load background files from NitroFS
    NF_LoadTiledBg("bg/tiles", "tilesTop", 512, 512);
    NF_LoadTiledBg("bg/tiles", "tilesBottom", 512, 512);

    // Create top screen background
    NF_CreateTiledBg(0, tilesTopLayer, "tilesTop");


    // Create bottom screen backgrounds
    NF_CreateTiledBg(1, tilesBottomLayer, "tilesBottom");
}


void setupGraphics() {
    // Prepare a NitroFS initialization screen
    NF_Set2D(0, 2);
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
    NF_InitAffineBgSys(0);       // Top screen
    // NF_InitTiledBgSys(0);       // Top screen
    NF_InitTiledBgSys(1);       // Bottom screen

    // Initialize sprite system
    NF_InitSpriteBuffers();     // Initialize storage buffers
    NF_Init3dSpriteSys();       // Initialize 3D sprite system
    NF_InitSpriteSys(0);        // Top screen
    NF_InitSpriteSys(1);        // Bottom screen

    


    setupAlgoraveTextBg();
    setupTilesBg();

    // setupPointerSprite();
    // setupText();
    
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

        // NF_SetTileOfMap(0, tilesTopLayer, rand() % 32, rand() % 32, empty ? 0 : rand() % 16);
        // NF_SetTileOfMap(1, tilesBottomLayer, rand() % 32, rand() % 32, empty ? 0 : rand() % 24 + 32); //25
        // NF_SetTileOfMap(1, tilesBottomLayer, rand() % 64, rand() % 32, rand() % 64); //25
        NF_SetTileOfMap(1, tilesBottomLayer, rand() % 32, rand() % 32, empty ? 0 : rand() % 16);
        NF_SetTileOfMap(0, tilesTopLayer, rand() % 32, rand() % 32, empty ? 0 : rand() % 6 + 36); //25
    }
}

void printCursorPositionAndTileUnderCursor() {
    // Print pointer position
    char mytext[60];
    sprintf(mytext,"x:%d  y:%d ", x, y);
    NF_WriteText(0, 2, 2, 2, mytext);

    // Print the color of the tile under the pointer
    int tilenum = NF_GetTileOfMap(1, 0, x / 8, y / 8);
    sprintf(mytext," %d   ", tilenum);
    NF_WriteText(0, 2, 1, 4, mytext);
}

void printParticipants() {
    char mytext[60];
    // sprintf(mytext,"TEST TEST");
    NF_WriteText(0, 2, 4, 4, mytext);
}


void updateBothVramMaps() {
    NF_UpdateVramMap(0, tilesTopLayer);
    NF_UpdateVramMap(1, tilesBottomLayer);
}

void scaleAlgorave() {
    NF_AffineBgTransform(0, algoraveTextLayer, 512, 512, 0, 0);
    NF_AffineBgMove(0, algoraveTextLayer, 100, 0, 0);
}

void drawSimplexNoise(float scale = 1) {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            if (rand() % 100 > 75) {
                float t_ = t * 0.01;
                float value0 = SimplexNoise::noise(0.01 * i * scale, 0.01 * j * scale, t_);
                float value1 = SimplexNoise::noise(0.01 * i * scale, 0.01 * (j + 24) * scale, t_);

                int tile0 = abs(value0) * 8 + tileOffset;
                int tile1 = abs(value1) * 8 + tileOffset;

                NF_SetTileOfMap(0, tilesTopLayer, i, j, tile0);
                NF_SetTileOfMap(1, tilesBottomLayer, i, j, tile1);
            }
        }
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
    // fillRandomTiles();
    // randomizeSo/meTiles();
    
    // updateBothVramMaps();

    while (true) {
        t++;

        // Read keys, touch and piano
        scanKeys();
        u16 keys = keysHeld();
        u16 down = keysDown();

        touchPosition touch;
        touchRead(&touch);

        pianoScanKeys();
        u16 piano = pianoKeysDown();




        /*
        |--------------------------------------------------------------------------
        | Toggle behaviours
        */
        if (down & KEY_DOWN || piano & PIANO_C) {
            behaviours.parallelProblemsVisible = !behaviours.parallelProblemsVisible;
            if (behaviours.parallelProblemsVisible) {
                NF_CreateTiledBg(0, algoraveTextLayer, "algoraveText");
            } else {
                NF_CreateTiledBg(0, algoraveTextLayer, "algoraveTextEmpty");
            }
        }

        if (down & KEY_A || piano & PIANO_D) {
            behaviours.simplexNoise = !behaviours.simplexNoise;
        }

        if (down & KEY_B || piano & PIANO_E) {
            behaviours.fillRandomTiles = !behaviours.fillRandomTiles;
        }

        



        /*
        |--------------------------------------------------------------------------
        | Excecute behaviours
        */
        
        if (behaviours.movingMapWithTouch) {
            updateMapScroll(keys);
        }

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




        

        // scaleAlgorave();
        // NF_AffineBgMove(0, algoraveTextLayer, 100, 0, 0);




        // Change the tile under the pointer if the user presses a button
        // randomizeSomeTiles(25);
        updateBothVramMaps();

        // Update text layers
        NF_UpdateTextLayers();

        // // Update OAM array
        NF_SpriteOamSet(1);

        // Wait for the screen refresh
        swiWaitForVBlank();

        // // Update OAM
        oamUpdate(&oamSub);
    }
}
