#pragma once

#include <nds.h>
#include <NEMain.h>

#include "scene_data.hpp"
#include "mic.hpp"

// assets
#include "heart_bin.h"
#include "heart.h"
#include "mp3.h"
#include "mp3_bin.h"
#include "swf_bin.h"





namespace hearts_and_comments {
    struct heart {
        float x;
        float y;
        float z;
        int ry, rx, rz;

        float s = 0;
        float vx = 0;
        float vy = 0;

        NE_Model *model;
    };

    bool particles = true;

    float scale_smoothed = 0;
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
            heart { .x = 0, .y = 1, .z = 5},

//            heart { .x = -1, .y = 4.3, .z = 3},
//            heart { .x = -2, .y = 6.5, .z = -2},
//            heart { .x = -1.5, .y = 2.2, .z = -1},
//            heart { .x = -2.5, .y = 7.8, .z = -0},
//            heart { .x = -1, .y = 3.9, .z = -2},
//            heart { .x = 2, .y = 2.1, .z = -4},
//            heart { .x = 1, .y = 7.3, .z = 3},
//            heart { .x = 3, .y = 3.2, .z = 3},
//            heart { .x = 4, .y = 9.8, .z = 2},
//            heart { .x = 5, .y = .5, .z = 1},
//            heart { .x = 3, .y = .2, .z = 4}
    };

    void load_assets(scene_data *scene) {
        scene->heart = NE_ModelCreate(NE_Static);
        scene->mp3 = NE_ModelCreate(NE_Static);
        scene->swf = NE_ModelCreate(NE_Static);

        for (heart &h : hearts) {
            h.model = scene->heart;
        }

        NE_Material *material_heart = NE_MaterialCreate();
        NE_Material *material_mp3 = NE_MaterialCreate();
        // Load mesh from RAM and assign it to the object "Model".
        NE_ModelLoadStaticMesh(scene->heart, heart_bin);
        NE_ModelLoadStaticMesh(scene->mp3, mp3_bin);
        NE_ModelLoadStaticMesh(scene->swf, swf_bin);
        // Load a RGB texture from RAM and assign it to "Material".
//        NE_MaterialTexLoad(material_heart, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, heartBitmap);
        NE_MaterialTexLoad(material_mp3, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, mp3Bitmap);

        // Assign texture to model...
        NE_ModelSetMaterial(scene->heart, material_heart);
        NE_ModelSetMaterial(scene->mp3, material_mp3);
        NE_ModelSetMaterial(scene->swf, material_mp3);

        // Set some properties to the material_heart
        NE_MaterialSetProperties(material_heart,
                                 RGB15(31, 5, 5), // Diffuse
                                 RGB15(0, 0, 0), // Ambient
                                 RGB15(0, 0, 0),    // Specular
                                 RGB15(5, 5, 5),    // Emission
                                 false, true);     // Vertex color, use shininess table

//        NE_MaterialSetProperties(material_mp3,
//                                 RGB15(31, 31, 31), // Diffuse
//                                 RGB15(15, 15, 15), // Ambient
//                                 RGB15(31, 31, 31),    // Specular
//                                 RGB15(0, 0, 0),    // Emission
//                                 false, true);     // Vertex color, use shininess table
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

        int down = keysDown();
        if (down & KEY_A) {
            particles = !particles;
        }


//        float scale = map(mic::peak, 0, 32768, 0.1, 5.0);
//        scale_smoothed = lerp(scale, scale_smoothed, 0.5);
//
//        printf("\n \n %f",  scale_smoothed);


        for (heart &h : hearts) {
            h.ry += 10;
            h.rx += rand() % 10;
            h.rz += rand() % 10;


            h.vy -= 0.00125;
            h.vx -= 0.00125;

            if (h.s < 0.25) {
                h.s += 0.01;
            }

            if (particles) {

            }
            h.y += h.vy;
            h.x += h.vx;

            if (h.y < -7) {
                if (particles) {
                    h.s = 0;
                    h.y = 7;
                    h.x = 5;

                    h.vy = ((float) (rand() % 10) / 100) * -1.0;
                    h.vx = -0.1;
//
                    int nextMesh = rand() % 3;
                    if (nextMesh == 0) {
                        h.model = scene->heart;
                    }
                    if (nextMesh == 1) {
                        h.model = scene->mp3;
                    }
                    if (nextMesh == 2) {
                        h.model = scene->swf;
                    }

                    if (keys_held & KEY_TOUCH) {
                        h.z = spawnZ * 10.0;
                    }
                }
            }

//            if (particles) {
                NE_ModelScale(h.model, h.s + scale_smoothed, h.s + scale_smoothed, h.s + scale_smoothed);
                NE_ModelSetCoord(h.model, h.x, h.y, h.z);
                NE_ModelSetRot(h.model, h.rx, h.ry, h.rz);
//                NE_PolyFormat(0, scene->heart_poly_id, NE_LIGHT_ALL, NE_CULL_NONE, NE_MODULATION);
                NE_PolyFormat(31, scene->heart_poly_id, NE_LIGHT_0, NE_CULL_BACK, NE_MODULATION);
                NE_ModelDraw(h.model);
//            }


        }
    }
}