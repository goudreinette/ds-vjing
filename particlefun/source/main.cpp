#include <stdio.h>
#include <nds.h>
#include <filesystem.h>
#include <nf_lib.h>
#include <vector>

// Colors
enum Color {
    PURPLE = ARGB16(1, 5, 0, 10)
};

// Time
int t = 0;

touchPosition touch;

int spriteId = 0;

typedef struct Particle {
    int spriteId = 0;
    float x;
    float y;
    float dx;
    float dy;
    int life;
    int maxLife;
};

std::vector<Particle> particles;

enum SpriteIds {
    SPR_PARTICLE = 0
};




/*
|--------------------------------------------------------------------------
| Utils
|--------------------------------------------------------------------------
*/
int lerp(int a, int b, int t) {
    return a + ((b - a) * t >> 8);
}



/*
|--------------------------------------------------------------------------
| Setup
|--------------------------------------------------------------------------
*/
void setupParticleSprite() {
    NF_LoadSpriteGfx("sprite/particle", SPR_PARTICLE, 8, 8);
    NF_LoadSpritePal("sprite/particle", SPR_PARTICLE);
    NF_VramSpriteGfx(0, SPR_PARTICLE, SPR_PARTICLE, true);
    NF_VramSpritePal(0, SPR_PARTICLE, SPR_PARTICLE);
}

void setupGraphics() {
    // Initialize NitroFS and set it as the root folder of the filesystem
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");

    // Initialize 2D engine in both screens and use mode 0
    NF_Set3D(0, 0);
    NF_Set2D(1, 0);

    // BG colors
    setBackdropColor(PURPLE); // Set the backdrop color to pico white
    setBackdropColorSub(PURPLE); // Set the backdrop color to pico white

    // Setup particle sprite
    NF_InitSpriteBuffers();
    NF_InitSpriteSys(0);
    NF_InitSpriteSys(1);
    setupParticleSprite();
}

void addParticle(float dx, float dy) {
    Particle p;
    p.spriteId = spriteId++;
    p.x = SCREEN_WIDTH / 2;
    p.y = SCREEN_HEIGHT / 2;
    p.dx = dx;
    p.dy = dy;
    p.life = 0;
    p.maxLife = 60000;

    NF_CreateSprite(0, p.spriteId, SPR_PARTICLE, SPR_PARTICLE, p.x, p.y);

    particles.push_back(p);
}

void updateParticles() {
    for (int i = 0; i < particles.size(); i++) {
        Particle &p = particles[i];
        p.x += p.dx;
        p.y += p.dy;
        p.life++;


        if (p.life > p.maxLife || p.x < 0 || p.x > SCREEN_WIDTH || p.y < 0 || p.y > SCREEN_HEIGHT) {
            particles.erase(particles.begin() + i);
        } else {
            NF_MoveSprite(0, i, p.x, p.y);
        }
    }

}


/*
|--------------------------------------------------------------------------
| Main
|--------------------------------------------------------------------------
*/
int main(int argc, char **argv)
{

    setupGraphics();

    for (int i = 0; i < 10; i++) {
        float dx = ((rand() % 100) / 100.0 - 0.5) / 4.0;
        float dy = ((rand() % 100) / 100.0 - 0.5) / 4.0;
        addParticle(dx, dy);
    }

    while (true) {
        t++;

        // Read keys, touch and piano
        scanKeys();
        u16 keys = keysHeld();
        u16 down = keysDown();
        
        touchRead(&touch);

        updateParticles();


        NF_SpriteOamSet(0);
        oamUpdate(&oamMain);
        oamUpdate(&oamSub);
    }
}
