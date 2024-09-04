#pragma once

#include <nds.h>
#include <NEMain.h>

#include "scene_data.hpp"

// assets
#include "wout_head_low_bin.h"
#include "wout_bust.h"



namespace wout_bust {

    int ry = -90;
    void load_assets(scene_data *scene) {
        scene->wout_head = NE_ModelCreate(NE_Static);

        NE_Material *material = NE_MaterialCreate();
        // Load mesh from RAM and assign it to the object "Model".
        NE_ModelLoadStaticMesh(scene->wout_head, wout_head_low_bin);
        // Load a RGB texture from RAM and assign it to "Material".
        NE_MaterialTexLoad(material, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, wout_bustBitmap);

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


    void update_draw(scene_data *scene, uint32_t keys_held) {



        if (keys_held & KEY_LEFT) {
            ry--;
        }
        if (keys_held & KEY_RIGHT) {
            ry++;
        }

        NE_ModelScale(scene->wout_head, 20, 20, 20);
        NE_ModelSetCoord(scene->wout_head, 2,-3,0);
        NE_ModelSetRot(scene->wout_head, 0, ry, 0);
        NE_PolyFormat(31, scene->wout_head_poly_id, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);
        NE_ModelDraw(scene->wout_head);
    }
}