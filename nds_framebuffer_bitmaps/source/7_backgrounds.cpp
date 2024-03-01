#include <nds.h>
#include <stdio.h>
#include "./screen_utils.h"
#include "scene.h"
#include "../../effolkronium/random.hpp"



#define A


class BackgroundsScene : public Scene
{
    int t = 0;

	using Random = effolkronium::random_static;
	
	u16* videoMemoryMain;
	u16* videoMemorySub;

public:
    void setup() {
		int x, y;

		//set the mode to allow for an extended rotation background
		videoSetMode(MODE_5_2D);
		videoSetModeSub(MODE_5_2D);

		//allocate a vram bank for each display
		vramSetBankA(VRAM_A_MAIN_BG);
		vramSetBankC(VRAM_C_SUB_BG);

		//create a background on each display
		int bgMain = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
		int bgSub = bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);

		videoMemoryMain = bgGetGfxPtr(bgMain);
		videoMemorySub = bgGetGfxPtr(bgSub);

		// printf("videoMemoryMain: %p\n", videoMemoryMain);

		//initialize it with a color
		for(x = 0; x < 256; x++)
			for(y = 0; y < 256; y++)
			{
				setPixelGfx(videoMemoryMain, x, y, ARGB16(1, 31, 0, 0));
				setPixelGfx(videoMemorySub, x, y, ARGB16(1, 0, 0, 31));
			}
    }


    void update() {
        t++;
    }


    void draw() {
        for (int i = 0; i < 256; i++) {
			int x = Random::get(0, 256);
			int y = Random::get(0, 192);
			int color = Random::get(0, 31);
			setPixelGfx(videoMemoryMain, x, y, ARGB16(1, color, color, color));

			setPixelGfx(videoMemorySub, x, y, ARGB16(1, color, color, color));
		}
    }
};
