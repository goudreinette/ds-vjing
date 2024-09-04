#pragma once

#include "nds.h"

namespace mic {
    //the record sample rate
    #define sample_rate 8000

    //buffer which is written to by the arm7
    u16* mic_buffer = 0;

    //the mic buffer sent to the arm7 is a double buffer
    //every time it is half full the arm7 signals us so we can read the
    //data.  I want the buffer to swap about once per frame so i chose a
    //buffer size large enough to hold two frames of 16bit mic data
    u32 mic_buffer_size = sample_rate * 2 / 30;

    // the mic peak level
    u32 peak = 0;
    u32 rms = 0;

    void micHandler(void* data, int length) {
        DC_InvalidateRange(data, length);

        s16 *ptr;
        int i;

        // get peak of data
        peak = 0;
        ptr = (s16*) data;
        for( i = 0; i < length/2; i++ ) {
            u32 test = (*ptr) * (*ptr);
            if( test > peak ) peak = test;
            ptr++;
        }

        peak = sqrt32(peak);


        // get rms (root mean square) of data
        u32 sum = 0;
        ptr = (s16*) data;
        for( i = 0; i < length/2; i++ ) {
            sum += ((*ptr) * (*ptr)) >> 16;
            ptr++;
        }
        sum /= length;
        rms = sqrt32(sum) << 8;

    }

    void setup() {
        mic_buffer = (u16*)malloc(mic_buffer_size);
        soundMicRecord(mic_buffer, mic_buffer_size, MicFormat_12Bit, sample_rate, micHandler);
    }

    //mic stream handler



}