#pragma once

#include "nds.h"
#include "NEMain.h"
#include <vector>
#include <algorithm>

#include "../../include/utils.hpp"
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

    // tracks
    float track_height = -3;
    const int num_track_parts = 7;
    int track_gap = 7;
    int track_start_z = 0;

    // ground
    int ground_start_z = 0;

    float speed = 0.2;

    // lanes
    int distance = 6;
    int current_lane = 1;


    struct train {
        float z = 20;
        float scale = 2;

        float x;
    };

    train trains[] = {
        train{.z = 50 + rand() % 20, .x = -distance}, // left
        train{.z =  50 + rand() % 20, .x = 0}, // center
        train{.z =  50 + rand() % 20, .x = distance} // right
    };

    void load_assets(scene_data *scene) {
        scene->trein = NE_ModelCreate(NE_Static);
        scene->track = NE_ModelCreate(NE_Static);
        scene->ground = NE_ModelCreate(NE_Static);

        NE_ModelLoadStaticMesh(scene->trein, trein_bin);
        NE_ModelLoadStaticMesh(scene->track, track_bin);
        NE_ModelLoadStaticMesh(scene->ground, ground_bin);

        // Assign texture to model...
        NE_ModelSetMaterial(scene->trein, material_mp3);
        NE_ModelSetMaterial(scene->track, material_mp3);
        NE_ModelSetMaterial(scene->ground, material_mp3);
    }

    // camera and lanes


    void update_draw(scene_data *scene, uint32_t keys_held, uint32_t keys_down) {
        cam_z += speed;

        NE_ModelScale(scene->trein, 2, 2, 2);
        NE_ModelSetRot(scene->trein, 0, 130, 0);

        NE_ModelScale(scene->ground, 5, 5, 10);
        NE_ModelSetCoord(scene->ground, ground_start_z, -3.1, 0);
        NE_ModelDraw(scene->ground);
        NE_ModelSetCoord(scene->ground, ground_start_z + 66, -3.1, 0);
        NE_ModelDraw(scene->ground);

        if (ground_start_z < cam_z - 66) {
            ground_start_z += 66;
        }

        // draw tracks
        NE_PolyFormat(31, scene->track_poly_id, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);

        int cam_z_int = (int) cam_z;

        for (int track_i = 0; track_i < num_track_parts; ++track_i) {
            for (int lane = -1; lane <= 1; ++lane) {
                NE_ModelSetCoord(scene->track, track_start_z + track_i * track_gap, track_height, lane * distance);
                NE_ModelDraw(scene->track);
            }
        }

        if (track_start_z < cam_z - 10) {
            track_start_z += 7;
        }


        // draw trains
        NE_PolyFormat(31, scene->trein_poly_id, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);

        for (train &t: trains) {
            t.z += -0.3 - speed / 4;

            // respawn in the distance
            if (t.z < cam_z - 10) {
                t.z = cam_z + 50 + rand() % 20;
            }

            NE_ModelSetCoord(scene->trein, t.z, -3, t.x);
            NE_ModelDraw(scene->trein);
        }

        // controls
        if (keys_down & KEY_LEFT && current_lane > -1) {
            current_lane--;
        }
        if (keys_down & KEY_RIGHT && current_lane < 1) {
            current_lane++;
        }

        if (keys_held & KEY_UP) {
            speed += 0.01;
        }
        if (keys_held & KEY_DOWN) {
            speed -= 0.01;
        }

        // auto switching lanes
        if (rand() % 150 == 1) {
            int ii = rand() % 2;
            if (ii == 0 && current_lane > -1) {
                current_lane--;
            }
            if (ii == 1 && current_lane < 1) {
                current_lane++;
            }
        }


        train train_in_lane = trains[current_lane + 1];
        if (train_in_lane.z < cam_z + 20) {
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

        float lerp_speed = map(speed, 0.0, 3.0, 0.1, 0.6);
        cam_y = lerp(cam_y, target_cam_y, lerp_speed);
        cam_x = lerp(cam_x, target_cam_x, lerp_speed);

        printf("\n \n %i", cam_z_int);
        printf("\n \n %i", track_start_z);
        printf("\n \n speed: %f", speed);
    }

    void exit() {
        trein::track_start_z = 0;
        trein::ground_start_z = 0;
        target_cam_z = -10.0;
        target_cam_x = 0.0;
        target_cam_y = 0.0;

        for (train &t : trains) {
            t.z = 50 + rand() % 20;
        }
    }

}