#pragma once

#include <nds.h>
#include <NEMain.h>

#include "scene_data.hpp"

// assets
#include "wout_head_low_bin.h"
#include "wout_cape_bin.h"
#include "wout_cape.h"
#include "wout_bust.h"

#include "mp3_bin.h"
#include "mp3.h"



namespace wout_bust {

    int ry = -90;

    NE_Material *material;

    bool is_caped = false;

    void load_assets(scene_data *scene) {
        scene->wout_head = NE_ModelCreate(NE_Static);

        material = NE_MaterialCreate();
        // Load mesh from RAM and assign it to the object "Model".
        NE_ModelLoadStaticMesh(scene->wout_head, wout_cape_bin);
        // Load a RGB texture from RAM and assign it to "Material".
        NE_MaterialTexLoad(material, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, wout_capeBitmap);

        // Assign texture to model...
        NE_ModelSetMaterial(scene->wout_head, material);

        // Set some properties to the material
        NE_MaterialSetProperties(material,
                                 RGB15(31, 31, 31), // Diffuse
                                 RGB15(15, 15, 15), // Ambient
                                 RGB15(31, 31, 31),    // Specular
                                 RGB15(0, 0, 0),    // Emission
                                 false, true);     // Vertex color, use shininess table
    }


    void update_draw(scene_data *scene, uint32_t keys_held, uint32_t keys_down) {

        touchPosition touch;
        touchRead(&touch);

        float spawnZ = touch.rawx / ( (float) SCREEN_WIDTH / 32);

        printf("\n \n %f",  spawnZ);

        if (keys_held & KEY_TOUCH) {
            ry = spawnZ + 110;
        }

        if (keys_held & KEY_LEFT) {
            ry--;
        }
        if (keys_held & KEY_RIGHT) {
            ry++;
        }

        if (keys_down & KEY_UP) {
            is_caped = false;
            NE_ModelLoadStaticMesh(scene->wout_head, wout_head_low_bin);
            NE_MaterialTexLoad(material, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, wout_bustBitmap);
            NE_ModelSetMaterial(scene->wout_head, material);
        }
        if (keys_down & KEY_DOWN) {
            is_caped = true;
            NE_ModelLoadStaticMesh(scene->wout_head, wout_cape_bin);
            NE_MaterialTexLoad(material, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, wout_capeBitmap);
            NE_ModelSetMaterial(scene->wout_head, material);
        }



        ry+= 2;

//        NE_ModelScale(scene->wout_head, 20, 20, 20);
        NE_ModelScale(scene->wout_head, 10, 10, 10);
        if (is_caped) {
            NE_ModelSetCoord(scene->wout_head, -4,-12,0);
        } else {
            NE_ModelSetCoord(scene->wout_head, 2,-3,0);
        }

        NE_ModelSetRot(scene->wout_head, 0, ry, 0);
        NE_PolyFormat(31, scene->wout_head_poly_id, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);
        NE_ModelDraw(scene->wout_head);
    }
}