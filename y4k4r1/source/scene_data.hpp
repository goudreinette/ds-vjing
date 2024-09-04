#pragma once

#include <NEMain.h>


// The time
double t;
double glitch_t;




struct scene_data {
    NE_Camera *camera;

    NE_Model *heart;
    int heart_poly_id;

    NE_Model *wout_head;
    int wout_head_poly_id;
};