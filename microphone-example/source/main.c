// mic input amplitude

#include <nds.h>
#include <stdio.h>
#include <nds/registers_alt.h>

#define __TEST_RMS
#define __TEST_PEAK

//the record sample rate
#define sample_rate  8000

//buffer which is written to by the arm7
u16* mic_buffer = 0;

//the mic buffer sent to the arm7 is a double buffer
//every time it is half full the arm7 signals us so we can read the
//data.  I want the buffer to swap about once per frame so i chose a
//buffer size large enough to hold two frames of 16bit mic data
u32 mic_buffer_size = sample_rate * 2 / 30;

//mic stream handler
void micHandler(void* data, int length)
{
    DC_InvalidateRange(data, length);
    
    s16 *ptr;
    int i;
    
    #ifdef __TEST_RMS
    
    // get rms (root mean square) of data
    u32 sum = 0;
    ptr = data;
    for( i = 0; i < length/2; i++ ) {
        sum += ((*ptr) * (*ptr)) >> 16;
        ptr++;
    }
    sum /= length;
    
    u32 rms = sqrt32(sum) << 8;
    
    //---------------------------------------
    // rms RESULT: 0..32768
    //---------------------------------------
    
    REG_WIN0H_SUB = (rms * 255) / 32768; // <----------------------
    
    #endif
    
    #ifdef __TEST_PEAK
    
    // get peak of data
    u32 peak = 0;
    ptr = data;
    for( i = 0; i < length/2; i++ ) {
        u32 test = (*ptr) * (*ptr);
        if( test > peak ) peak = test;
        ptr++;
    }
    
    peak = sqrt32(peak);
    REG_WIN1H_SUB = (peak * 255) / 32768;
    
    #endif
}

int main(void) 
{

    consoleDemoInit();

    iprintf("recording.....\nRMS\n\nPEAK");
    
    BG_PALETTE_SUB[0] = 0;
    BG_PALETTE_SUB[1]= RGB8(255,128,0);
    int i;
    for( i = 0; i < 16; i++ ) {
        BG_GFX_SUB[i] = 0x1111;
    }
    
    REG_BG1CNT_SUB = BG_MAP_BASE(1);
    
    videoSetModeSub( 
        MODE_0_2D | 
        DISPLAY_BG0_ACTIVE | 
        DISPLAY_BG1_ACTIVE | 
        DISPLAY_WIN0_ON | 
        DISPLAY_WIN1_ON );
    
    REG_WIN0V_SUB = 0x1017;
    REG_WIN1V_SUB = 0x2027;
    REG_WININ_SUB = 0x202; // bg1 inside window
    REG_WINOUT_SUB = 0x3D; // other stuff outside
    REG_WIN0H_SUB = 0;
    REG_WIN1H_SUB = 0;
    
    mic_buffer = (u16*)malloc(mic_buffer_size);
    soundMicRecord(mic_buffer, mic_buffer_size, MicFormat_12Bit, sample_rate, micHandler);


    swiIntrWait( 0, 0 );
    return 0;
}
