#include <nds.h>
#include <stdio.h>
#include <NEMain.h>
#include <math.h>

// First you have to put the .bin files in the data folder. This will generate
// (after doing "make") some files named "binfilename_bin.h". For example,
// "model.bin" will generate a file named "model_bin.h". You have to include
// this in "main.c".
//
// The name you will have to use is "binfilename_bin". For example, for loading
// "model.bin" you will have to use:
//
//     NE_ModelLoadStaticMesh(Model, binfilename_bin);
//


#include "clippy_bin.h"
#include "cloud_bin.h"
#include "paper_bin.h"
#include "texture2.h"
#include "rainbow.h"
#include "texture3.h"
#include "paper.h"

double t;


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
    // Set polygon format
    // NE_PolyFormat(31, scene->polyId, NE_LIGHT_0, NE_CULL_BACK, NE_TOON_HIGHLIGHT_SHADING);
    
    
    // Draw anger
    NE_ModelDraw(scene->angel);

    // Draw paper
    float y = sinf((t + 10) / 30) * .05;
    float ry = sinf((t - 60) / 120) * 10;
    NE_ModelSetCoord(scene->paper, -3, y,0);
    NE_ModelSetRot(scene->paper, 0, 100 + ry, 0);
    NE_ModelDraw(scene->paper);


    // Draw clouds
    
    // cloud left
    NE_PolyFormat(22, scene->cloudPolyId, NE_LIGHT_0, NE_CULL_BACK, 0);
    y = sinf((t + 360) / 30) * .2;
    ry = sinf(t / 120) * 20;
    NE_ModelScale(scene->cloud, 3,3,3);
    NE_ModelSetCoord(scene->cloud, 5, -3 + y, -10);
    NE_ModelSetRot(scene->cloud, 0, 120, 0);
    NE_ModelDraw(scene->cloud);

    // cloud top right
    y = sinf((t + 420) / 30) * .2;
    NE_PolyFormat(24, scene->cloudPolyId, NE_LIGHT_0, NE_CULL_BACK, 0);
    NE_ModelScale(scene->cloud, 2,2,2);
    NE_ModelSetCoord(scene->cloud, 20, 4 + y, 14);
    NE_ModelSetRot(scene->cloud, 0, 80, 0);
    NE_ModelDraw(scene->cloud);

    // cloud front
    NE_PolyFormat(20, scene->cloudPolyId, NE_LIGHT_0, NE_CULL_BACK, 0);
    y = sinf((t - 360) / 30) * .2;
    NE_ModelScale(scene->cloud, 1,1,1);
    NE_ModelSetCoord(scene->cloud, -5.5, -1 + y, 3);
    NE_ModelSetRot(scene->cloud, 0, 120, 0);
    NE_ModelDraw(scene->cloud);
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

    SceneData scene = { 0 };
    scene.polyId = 0;

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



    // // Load background files from NitroFS
    // NF_LoadTiledBg("bg", "bg", 512, 512);

    // // Create top screen backgrounds
    // NF_CreateTiledBg(0, 0, "bg");

    while (1)
    {
        t++;

        // Wait for next frame
        NE_WaitForVBL(0);

        // Get keys information
        scanKeys();
        uint32_t keys = keysHeld();

        printf("\x1b[2;1Hit looks like you're afraid...");

        printf("\x1b[5;1HBE NOT AFRAID");

        float y = sinf(t / 30) * .2;
        float ry = sinf(t / 120) * 20;

        NE_ModelSetCoord(scene.angel, -3, y, 0);
        NE_ModelSetRot(scene.angel, 0, 130 + ry, 0);
        
        // printf("\x1b[2;1HZ: %i", scene.angel->rz);
        
        // Draw scene
        NE_ProcessArg(Draw3DScene, &scene);
    }

    return 0;
}
