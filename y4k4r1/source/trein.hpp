#pragma once

#include "nds.h"
#include "NEMain.h"

#include "scene_data.hpp"

// assets
#include "trein_bin.h"
#include "track_bin.h"
#include "ground_bin.h"
#include "mp3.h"
#include "heart.h"
#include "wout_cape.h"
#include "wout_bust.h"

namespace trein {
    NE_Material* material;

    int distance = 6;

    struct train {
        float z = 20;
        float scale = 2;

        float x;
    };

    train trains[] = {
        train { .z = 50 + rand() % 20, .x = -distance }, // left
        train { .z =  50 + rand() % 20,  .x = 0 }, // center
        train { .z =  50 + rand() % 20, .x = distance } // right
    };

    void load_assets(scene_data *scene) {
        scene->trein = NE_ModelCreate(NE_Static);
        scene->track = NE_ModelCreate(NE_Static);
        scene->ground = NE_ModelCreate(NE_Static);
        material = NE_MaterialCreate();

        NE_ModelLoadStaticMesh(scene->trein, trein_bin);
        NE_ModelLoadStaticMesh(scene->track, track_bin);
        NE_ModelLoadStaticMesh(scene->ground, ground_bin);

        // Load a RGB texture from RAM and assign it to "Material".
        NE_MaterialTexLoad(material, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, mp3Bitmap);

        // Assign texture to model...
        NE_ModelSetMaterial(scene->trein, material);
        NE_ModelSetMaterial(scene->track, material);
        NE_ModelSetMaterial(scene->ground, material);

        // Set some properties to the material
        NE_MaterialSetProperties(material,
                                 RGB15(31, 31, 31), // Diffuse
                                 RGB15(15, 15, 15), // Ambient
                                 RGB15(0, 0, 0),    // Specular
                                 RGB15(0, 0, 0),    // Emission
                                 false, false);     // Vertex color, use shininess table
    }



    float cam_y;
    float cam_z = 0;
    float cam_x = 0;

    int current_lane = 1;

    void update_draw(scene_data *scene, uint32_t keys_held, uint32_t keys_down) {
        // scrolllll
//        cam_z += 0.1;


        NE_ClearColorSet(RGB8(95,205,228), 31, 63);

        NE_ModelScale(scene->trein, 2, 2, 2);
        NE_ModelSetRot(scene->trein, 0, 130, 0);

        NE_ModelScale(scene->ground, 5 + cam_z, 5, 10);
        NE_ModelSetCoord(scene->ground, 5,-3.1,0);
        NE_ModelDraw(scene->ground);

        for (train &t: trains) {
            t.z += -0.5;

            if (t.z < -20) {
                t.z = 50 + rand() % 20;
            }

            NE_PolyFormat(31, scene->trein_poly_id, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);

            NE_ModelSetCoord(scene->trein, t.z, -3, t.x);
            NE_ModelDraw(scene->trein);

            NE_PolyFormat(31, scene->track_poly_id, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);

            int track_height = -3;
            for (int track_i = 0; track_i < 6; ++track_i) {
                NE_ModelSetCoord(scene->track, cam_z - 5 + track_i * 7, track_height, t.x- .125);
                NE_ModelDraw(scene->track);
            }
        }



        float target_cam_y;
        float target_cam_x;

        if (keys_down & KEY_LEFT && current_lane > -1) {
             current_lane--;
        }
        if (keys_down & KEY_RIGHT  && current_lane < 1) {
            current_lane++;
        }

        train center_train = trains[current_lane + 1];

        if (center_train.z < 10) {
            target_cam_y = 2.5;
        } else {
            target_cam_y = -2.5;
        }

        if (current_lane == -1) {
            target_cam_x = -distance;
        } else if (current_lane == 0) {
            target_cam_x = 0;
        } else if (current_lane == 1) {
            target_cam_x = distance;
        }

        printf("\n \n %f",  cam_y);
//        printf("\n \n %f",  center_train.z);
        printf("\n \n %f",  cam_z);
//        printf("\n \n %i",  current_lane);

        cam_y = lerp(cam_y, target_cam_y, 0.1);
        cam_x = lerp(cam_x, target_cam_x, 0.1);

        NE_CameraSet(scene->camera,
                     cam_z - 10, cam_y, cam_x,  // Position
                     cam_z, 0, cam_x,  // Look at
                     0, 1, 0); // Up direction
    }
}