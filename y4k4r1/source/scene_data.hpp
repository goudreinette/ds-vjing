#pragma once

#include <NEMain.h>


// Global material
NE_Material *material_mp3;


// The time
double t;


struct scene_data {
    NE_Camera *camera;

    NE_Model *heart;
    int heart_poly_id;
    NE_Model *mp3;
    NE_Model *swf;
    NE_Model *trein;
    int mp3_poly_id;
    int trein_poly_id;

    NE_Model *track;
    int track_poly_id;

    NE_Model *ground;
    int ground_poly_id;


    NE_Model *wout_head;
    int wout_head_poly_id;
};