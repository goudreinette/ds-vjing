// mic input amplitude

#include <nds.h>
#include <stdio.h>
#include <nds/registers_alt.h>
#include <nf_lib.h>
#include <math.h>
#include <filesystem.h>

//the record sample rate
#define sample_rate 8000

//buffer which is written to by the arm7
u16* mic_buffer = 0;

//the mic buffer sent to the arm7 is a double buffer
//every time it is half full the arm7 signals us so we can read the
//data.  I want the buffer to swap about once per frame so i chose a
//buffer size large enough to hold two frames of 16bit mic data
u32 mic_buffer_size = sample_rate * 2 / 30;

// the mic peak level
u32 peak = 0;
u32 rms = 0;

// scale smoothed a bit
int scale_smoothed = 0;

// enums
enum spriteIndexes {
    eggcell = 1,
};

enum bgLayers {
    tilesTop = 1,
};

int t = 0;


//mic stream handler
void micHandler(void* data, int length)
{
    DC_InvalidateRange(data, length);   
    
    s16 *ptr;
    int i;
    
    // get peak of data
    peak = 0;
    ptr = data;
    for( i = 0; i < length/2; i++ ) {
        u32 test = (*ptr) * (*ptr);
        if( test > peak ) peak = test;
        ptr++;
    }
    
    peak = sqrt32(peak);


    // get rms (root mean square) of data
    u32 sum = 0;
    ptr = data;
    for( i = 0; i < length/2; i++ ) {
        sum += ((*ptr) * (*ptr)) >> 16;
        ptr++;
    }
    sum /= length;
    rms = sqrt32(sum) << 8;
}

long lerp(long a, long  b, long t) {
    return a + ((b - a) * t >> 8);
}

// map function cpp
int map(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}



void setupBottomScreenVolumeBar() {
    // Bottom screen shizzle
    BG_PALETTE_SUB[0] = 0;
    BG_PALETTE_SUB[1]= RGB8(255,0,128);
    int i;
    for( i = 0; i < 16; i++ ) {
        BG_GFX_SUB[i] = 0x1111;
    }
    
    REG_BG1CNT_SUB = BG_MAP_BASE(1);
    
    videoSetModeSub( 
        MODE_0_2D | 
        DISPLAY_BG0_ACTIVE | 
        DISPLAY_BG1_ACTIVE | 
        DISPLAY_WIN0_ON | 
        DISPLAY_WIN1_ON );
    
    REG_WIN0V_SUB = 0x1017;
    REG_WIN1V_SUB = 0x2027;
    REG_WININ_SUB = 0x202; // bg1 inside window
    REG_WINOUT_SUB = 0x3D; // other stuff outside
    REG_WIN0H_SUB = 0;
    REG_WIN1H_SUB = 0;
}





void setupTilesBg() {
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);       // Top screen

    // Load background files from NitroFS
    NF_LoadTiledBg("tiles", "tilesTop", 512, 512);

    // Create top screen backgrounds
    NF_CreateTiledBg(0, tilesTop, "tilesTop");
}

void setupEggCell() {
    NF_LoadSpriteGfx("eggcell", eggcell, 128, 128);
    NF_LoadSpritePal("eggcell", eggcell);

    NF_Vram3dSpriteGfx(eggcell, eggcell, false);
    NF_Vram3dSpritePal(eggcell, eggcell);

    NF_Create3dSprite(eggcell, eggcell, eggcell, 64, 25);
    NF_Scale3dSprite(eggcell, 32, 32);


    NF_Sort3dSprites();
}


void eggCellPulseTiles() {
    // Radiate in a circle from the center outwards
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 24; j++) {
            if (rand() % 100 > 70) {
                int distance = sqrt32(pow(i - 16, 2) + pow(j -10, 2));
                int tile = ((distance - t / 5) % 6) + 51 - peak / 1000;
                NF_SetTileOfMap(0, tilesTop, i, j, (int) tile);
            }
        }
    }
}



int main(void) 
{

    consoleDemoInit();

    printf("recording.....\nRMS\n\nPEAK");
    
    mic_buffer = (u16*)malloc(mic_buffer_size);
    soundMicRecord(mic_buffer, mic_buffer_size, MicFormat_12Bit, sample_rate, micHandler);

    // Init NFLib 3D in top screen
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");
    NF_Set2D(0, 0);
    NF_Set3D(0, 0);
    setupTilesBg();


    NF_Init3dSpriteSys();
    NF_InitSpriteBuffers();     // Initialize storage buffers
    
    
    setupEggCell();
    setupBottomScreenVolumeBar();


    while(1) {
        swiWaitForVBlank();
        

        // Scale the egg cell sprite based on the peak value
        int scale = map(peak, 0, 32768, 16, 128);
        // Smooth it a bit
        scale_smoothed = lerp(scale, scale_smoothed, 128);
        NF_Scale3dSprite(eggcell, scale_smoothed, scale_smoothed);

        // Progress bar
        REG_WIN1H_SUB = scale_smoothed * 2;


        // Make the time go faster on high volume
        t += scale_smoothed / 16;

        printf("\x1b[2;0H%li", peak);
        printf("\x1b[5;0H%i", scale_smoothed);

        // Pulse the tiles
        eggCellPulseTiles();

        // Update the tile map
        NF_UpdateVramMap(0, tilesTop);

        // Draw all 3D sprites
        NF_Draw3dSprites();
        // Tell the GPU to draw the scene and wait until it's done
        glFlush(0);
        // Wait for the screen refresh
        swiWaitForVBlank();
    }
}
