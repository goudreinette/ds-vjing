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
    float cam_z = -10;

    void update_draw(scene_data *scene, uint32_t keys_held) {
        NE_ModelScale(scene->trein, 2, 2, 2);
        NE_ModelSetRot(scene->trein, 0, 130, 0);

        NE_ModelSetCoord(scene->ground, 5,-3.1,0);
        NE_ModelScale(scene->ground, 5, 5, 10);
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
//            NE_ModelSetCoord(scene->track, 45, track_height, t.x);
//            NE_ModelDraw(scene->track);
//            NE_ModelSetCoord(scene->track, 38, track_height, t.x);
//            NE_ModelDraw(scene->track);
//            NE_ModelSetCoord(scene->track, 31, track_height, t.x);
//            NE_ModelDraw(scene->track);
            NE_ModelSetCoord(scene->track, 24, track_height, t.x - .125);
            NE_ModelDraw(scene->track);
            NE_ModelSetCoord(scene->track, 17, track_height, t.x- .125);
            NE_ModelDraw(scene->track);
            NE_ModelSetCoord(scene->track, 10, track_height, t.x- .125);
            NE_ModelDraw(scene->track);
            NE_ModelSetCoord(scene->track, 3.2, track_height, t.x- .125);
            NE_ModelDraw(scene->track);
            NE_ModelSetCoord(scene->track, -4, track_height, t.x- .125);
            NE_ModelDraw(scene->track);
        }



        train center_train = trains[1];
        float target_cam_y;

        if (center_train.z < 10) {
            target_cam_y = 4;
        } else {
            target_cam_y = -2;
        }

        printf("\n \n %f",  cam_y);
        cam_y = lerp(cam_y, target_cam_y, 0.1);

        NE_CameraSet(scene->camera,
                     -10, cam_y, 0,  // Position
                     0, 0, 0,  // Look at
                     0, 1, 0); // Up direction
    }
}