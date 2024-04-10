#include <nds.h>
#include <stdio.h>
#include "./screen_utils.h"
#include "scene.h"
#include "../../effolkronium/random.hpp"


class TilesScene : public Scene
{
    int t = 0;

	using Random = effolkronium::random_static;

public:
    void setup() {
		
    }

    void update() {
        t++;
    }

    void draw() {
        
    }
};
