#include <nds.h>
#include <stdio.h>
#include <NEMain.h>
#include <math.h>
#include <filesystem.h>
#include <nf_lib.h>

// Models and graphics
#include "clippy_bin.h"
#include "cloud_bin.h"
#include "paper_bin.h"
#include "texture2.h"
#include "rainbow.h"
#include "texture3.h"
#include "paper.h"


// The time
double t;
double glitch_t;

typedef struct {
    bool wasFlipped;
    int x;
    int y;
    bool h;
    bool v;
} FlippedTile;


typedef struct {
    NE_Camera *camera;
    NE_Model *angel;
    NE_Model *paper;
    NE_Model *cloud;

    int polyId;
    int cloudPolyId;
} SceneData;



void Draw3DScene(void *arg)
{
    SceneData *scene = arg;

    NE_CameraUse(scene->camera);

    NE_PolyFormat(31, scene->polyId, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);

    // Draw paper ------
    float y = sinf((t + 10) / 30) * .05;
    float ry = sinf((t - 60) / 120) * 10;
    NE_ModelSetCoord(scene->paper, -3, y,0);
    NE_ModelSetRot(scene->paper, 0, 100 + ry, 0);
    NE_ModelDraw(scene->paper);


    // Draw clouds ------
    // cloud left
    NE_PolyFormat(22, scene->cloudPolyId, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);
    y = sinf((t + 360) / 30) * .2;
    ry = sinf(t / 120) * 20;
    NE_ModelScale(scene->cloud, 3,3,3);
    NE_ModelSetCoord(scene->cloud, 5, -3 + y, -10);
    NE_ModelSetRot(scene->cloud, 0, 120, 0);
    NE_ModelDraw(scene->cloud);
    

    // cloud top right
    y = sinf((t + 420) / 30) * .2;
    NE_PolyFormat(24, scene->cloudPolyId, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);
    NE_ModelScale(scene->cloud, 2,2,2);
    NE_ModelSetCoord(scene->cloud, 20, 4 + y, 14);
    NE_ModelSetRot(scene->cloud, 0, 80, 0);
    NE_ModelDraw(scene->cloud);

    // cloud front
    NE_PolyFormat(20, scene->cloudPolyId, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);
    y = sinf((t - 360) / 30) * .2;
    NE_ModelScale(scene->cloud, 1,1,1);
    NE_ModelSetCoord(scene->cloud, -5.5, -1 + y, 3);
    NE_ModelSetRot(scene->cloud, 0, 120, 0);
    NE_ModelDraw(scene->cloud);

    // Draw angel ------
     y = sinf(t / 30) * .2;
     ry = sinf(t / 120) * 40;
    NE_ModelSetCoord(scene->angel, -3, y, 0);
    NE_ModelSetRot(scene->angel, 0, 130 + ry, 0);
    NE_PolyFormat(31, scene->polyId, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);
    NE_ModelDraw(scene->angel);
}

void createAngel(SceneData *scene) {
    // Allocate space for the objects we'll use
    scene->angel = NE_ModelCreate(NE_Static);
    scene->camera = NE_CameraCreate();
    NE_Material *material = NE_MaterialCreate();
    
  // Load mesh from RAM and assign it to the object "Model".
    NE_ModelLoadStaticMesh(scene->angel, clippy_bin);
    // Load a RGB texture from RAM and assign it to "Material".
    NE_MaterialTexLoad(material, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, texture3Bitmap);

    // Assign texture to model...
    NE_ModelSetMaterial(scene->angel, material);

        // Set some properties to the material
    NE_MaterialSetProperties(material,
                  RGB15(31, 31, 31), // Diffuse
                  RGB15(16, 16, 16), // Ambient
                  RGB15(31, 31, 31),    // Specular
                  RGB15(0, 0, 0),    // Emission
                  false, true);     // Vertex color, use shininess table
}

void createPaper(SceneData *scene) {
    // Allocate space for the objects we'll use
    scene->paper = NE_ModelCreate(NE_Static);
    NE_Material *material = NE_MaterialCreate();
    
  // Load mesh from RAM and assign it to the object "Model".
    NE_ModelLoadStaticMesh(scene->paper, paper_bin);
    // Load a RGB texture from RAM and assign it to "Material".
    NE_MaterialTexLoad(material, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, paperBitmap);

    // Assign texture to model...
    NE_ModelSetMaterial(scene->paper, material);

        // Set some properties to the material
    NE_MaterialSetProperties(material,
                  RGB15(31, 31, 31), // Diffuse
                  RGB15(16, 16, 16), // Ambient
                  RGB15(31, 31, 31),    // Specular
                  RGB15(0, 0, 0),    // Emission
                  false, true);     // Vertex color, use shininess table
}


void createClouds(SceneData *scene) {
    scene->cloudPolyId = 1;


    // Allocate space for the objects we'll use
    scene->cloud = NE_ModelCreate(NE_Static);
    NE_Material *material = NE_MaterialCreate();
    
  // Load mesh from RAM and assign it to the object "Model".
    NE_ModelLoadStaticMesh(scene->cloud, cloud_bin);
    
    // Assign texture to model...
    NE_ModelSetMaterial(scene->cloud, material);

        // Set some properties to the material
    NE_MaterialSetProperties(material,
                  RGB15(31, 31, 31), // Diffuse
                  RGB15(20,20,20), // Ambient
                  RGB15(0,0,0),    // Specular
                  RGB15(5, 5, 5),    // Emission
                  false, true);     // Vertex color, use shininess table
}


int main(int argc, char *argv[])
{
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");


    SceneData scene = { 0 };
    scene.polyId = 0;

    FlippedTile lastFlippedTile = { false, 0, 0, false, false };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init Nitro Engine in normal 3D mode
    NE_Init3D();

    // libnds uses VRAM_C for the text console, reserve A and B only
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // Init console in non-3D screen
    consoleDemoInit();


    createAngel(&scene);
    createClouds(&scene);
    createPaper(&scene);


    // Set coordinates for the camera
    NE_CameraSet(scene.camera,
                 -10, 3, 0,  // Position
                  0, 3, 0,  // Look at
                  0, 1, 0); // Up direction

  

    // We set up a light and its color
    NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);
    // NE_LightSet(2, NE_White, -1, -3, -0.5);

    // NE_ClearColorSet(RGB8(69,40,60), 31, 63);
    NE_ClearColorSet(RGB8(95,205,228), 31, 63);

    // NE_ClearColorSet(NE_White, 31, 63);
    // setBackdropColorSub(RGB8(34,32,52));
    setBackdropColorSub(RGB8(95,205,228));


    NF_Set2D(1, 0);
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(1);

    NF_LoadTiledBg("dialog", "bg", 512, 512);
    NF_CreateTiledBg(1, 0, "bg");


    // Wrap values of parameters
    u16 depth = 0x7C00;
    u8 shift = 2;
    u8 mass = 2;



    // Enable/update fog
    NE_FogEnable(shift, NE_White, 31, mass, depth);

    NE_SpecialEffectNoiseConfig(31);
    // NE_SpecialEffectSet(NE_NOISE);

    while (1)
    {
        t++;

        // Wait for next frame
        NE_WaitForVBL(0);

        // Get keys information
        scanKeys();
        uint32_t keys = keysHeld();
        uint32_t down = keysDown();


        // printf("\x1b[2;1Hit looks like you're afraid...");
        // printf("\x1b[5;1HBE NOT AFRAID");

        // Special effects
        if (down) {
            glitch_t = 5;
        }        

        if (glitch_t > 0) {
            glitch_t--;
            NE_SpecialEffectSet(NE_NOISE);
        } else {
            int r = rand() % 300;
            if (r == 0) {
                NE_SpecialEffectSet(NE_NOISE);
            } else if (r == 1) {
                NE_SpecialEffectSet(NE_SINE);
            } else {
                NE_SpecialEffectSet(NE_NONE);
            }
        }

        // Bottom screen special effects
        if (lastFlippedTile.wasFlipped == true) {
            if (lastFlippedTile.h) {
                NF_SetTileHflip(1, 0, lastFlippedTile.x, lastFlippedTile.y);
            } else {
                NF_SetTileVflip(1, 0, lastFlippedTile.x, lastFlippedTile.y);
            }
            NF_UpdateVramMap(1, 0);
            lastFlippedTile.wasFlipped = false;
        }

        if (rand() % 5 == 0 || glitch_t > 0) {
            lastFlippedTile.wasFlipped = true;
            lastFlippedTile.x = rand() % 32;
            lastFlippedTile.y = rand() % 20;
            if (rand() % 2 == 0) {
                NF_SetTileHflip(1, 0, lastFlippedTile.x, lastFlippedTile.y);
                lastFlippedTile.h = true;
                lastFlippedTile.v = false;
            } else {
                NF_SetTileVflip(1, 0, lastFlippedTile.x, lastFlippedTile.y);
                lastFlippedTile.h = false;
                lastFlippedTile.v = true;
            }
            
            NF_UpdateVramMap(1, 0);
        }
        
            
        // Draw scene
        NE_ProcessArg(Draw3DScene, &scene);
    }

    return 0;
}


