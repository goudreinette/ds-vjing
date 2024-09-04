#pragma once

#include <nds.h>
#include <NEMain.h>

#include "scene_data.hpp"

// assets
#include "heart_bin.h"
#include "heart.h"


struct heart {
    float x;
    float y;
    float z;
    int ry;

    float s = 0;
    float vx = 0;
    float vy = 0;
};


namespace hearts_and_comments {
    heart hearts[] = {
            heart { .x = 0, .y = 10, .z = -4},
            heart { .x = 0, .y = 5, .z = -2},
            heart { .x = 0, .y = 6, .z = -1},
            heart { .x = 0, .y = 7, .z = -3},
            heart { .x = 0, .y = 4, .z = -5},
            heart { .x = 0, .y = 10, .z = 4},
            heart { .x = 0, .y = 5, .z = 2},
            heart { .x = 0, .y = 6, .z = 1},
            heart { .x = 0, .y = 7, .z = 3},
            heart { .x = 0, .y = 4, .z = 5},
            heart { .x = 0, .y = 6, .z = -4},
            heart { .x = 0, .y = 2, .z = -2},
            heart { .x = 0, .y = 7, .z = -1},
            heart { .x = 0, .y = 3, .z = -3},
            heart { .x = 0, .y = 2, .z = -5},
            heart { .x = 0, .y = 7, .z = 4},
            heart { .x = 0, .y = 3, .z = 2},
            heart { .x = 0, .y = 9, .z = 1},
            heart { .x = 0, .y = 0, .z = 3},
            heart { .x = 0, .y = 1, .z = 5}
    };

    void load_assets(scene_data *scene) {
        scene->heart = NE_ModelCreate(NE_Static);

        NE_Material *material = NE_MaterialCreate();
        // Load mesh from RAM and assign it to the object "Model".
        NE_ModelLoadStaticMesh(scene->heart, heart_bin);
        // Load a RGB texture from RAM and assign it to "Material".
        NE_MaterialTexLoad(material, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, heartBitmap);

        // Assign texture to model...
        NE_ModelSetMaterial(scene->heart, material);

        // Set some properties to the material
        NE_MaterialSetProperties(material,
                                 RGB15(31, 31, 31), // Diffuse
                                 RGB15(15, 15, 15), // Ambient
                                 RGB15(31, 31, 31),    // Specular
                                 RGB15(0, 0, 0),    // Emission
                                 false, true);     // Vertex color, use shininess table
    }


    void update_draw_hearts(scene_data *scene, uint32_t keys_held) {
        // Draw heart ------
        int y = sinf(t / 30) * .2;
        int ry = sinf(t / 120) * 40;

        printf("\x1b[2;1Hy4k4r1");
        printf("\x1b[5;1H<3<3<3<3<3<3");

        touchPosition touch;
        touchRead(&touch);

        float spawnZ = touch.rawx / (float) SCREEN_WIDTH / 16.0 * 2 - 1;
        printf("\n \n %f",  spawnZ);

        for (heart &h : hearts) {
            h.ry += 10;
            h.vy -= 0.00125;
            h.vx -= 0.00125;

            if (h.s < 0.25) {
                h.s += 0.01;
            }

            h.y += h.vy;
            h.x += h.vx;

            if (h.y < -7) {
                h.s = 0;
                h.y = 7;
                h.x = 5;

                h.vy = -0.2;
                h.vx = -0.1;

                if (keys_held & KEY_TOUCH) {
                    h.z = spawnZ * 10.0;
                }
            }

            NE_ModelScale(scene->heart, h.s, h.s, h.s);
            NE_ModelSetCoord(scene->heart, h.x, h.y, h.z);
            NE_ModelSetRot(scene->heart, 0, 130 + h.ry, 0);
            NE_PolyFormat(31, scene->heart_poly_id, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);
            NE_ModelDraw(scene->heart);
        }
    }
}