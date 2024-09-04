#pragma once

#include <NEMain.h>


// The time
double t;
double glitch_t;



float lerp(float a, float b, float f)
{
    return a * (1.0 - f) + (b * f);
}


// map function cpp
float map(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
int map(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

struct scene_data {
    NE_Camera *camera;

    NE_Model *heart;
    int heart_poly_id;
    NE_Model *mp3;
    NE_Model *swf;
    int mp3_poly_id;

    NE_Model *wout_head;
    int wout_head_poly_id;
};