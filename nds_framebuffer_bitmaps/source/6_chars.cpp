#include <nds.h>
#include <stdio.h>

#include "scene.h"
#include "../../effolkronium/random.hpp"





class CharsScene : public Scene
{
    int t = 0;

    using Random = effolkronium::random_static;

	int w = 32;

	touchPosition touch;
	PrintConsole topScreen;
	PrintConsole bottomScreen;

	// 24 * 32
	const static int screenWidthChars = 32;
	const static int screenHeightChars = 24;
	const int numChars = screenWidthChars * screenHeightChars;
	char* text[screenWidthChars*screenHeightChars];	

public:
    void setup() {
		videoSetMode(MODE_0_2D);
		videoSetModeSub(MODE_0_2D);

		vramSetBankA(VRAM_A_MAIN_BG);
		vramSetBankC(VRAM_C_SUB_BG);

		consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
		consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
    }


    void update() {
        
    }


    void draw() {
        t++;


		// BOTTOM SCREEN 
		consoleSelect(&bottomScreen);

		for (int x = 0; x < screenWidthChars; x++) {
			for (int y = 0; y < screenHeightChars; y++) {
				int location = x*y;
				const char * character =  t % numChars == location ? "X" : "O";
				text[location] = (char*)character;
			}
		}

		consoleClear();
		printf(*text);

        

		swiWaitForVBlank();
    }
};
