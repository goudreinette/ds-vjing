#include <nds.h>
#include <stdio.h>

#include "scene.h"
#include "../../effolkronium/random.hpp"





class CharsScene : public Scene
{
    int t = 0;

    using Random = effolkronium::random_static;


public:
    void setup() {
        consoleDemoInit();

        // PrintConsole* console = consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 8, 0, true, true);
        // consoleSelect(console);
        // consoleInit(NULL, 3, BgType_Text4bpp, BgSize_T_256x256, 8, 0, true, true);
    }


    void update() {
        
    }


    void draw() {
        t++;

        if (t % 10000 == 0) {
            printf("PARRALLEL PROBLEMSPARRALLEL PROB");
        }

        if (t % 300000 == 1000) {
            consoleClear();
        }
        
    }
};
