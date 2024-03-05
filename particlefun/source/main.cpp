#include <stdio.h>
#include <nds.h>
#include <filesystem.h>
#include <nf_lib.h>
#include <vector>
#include <algorithm>

// Colors
enum Color {
    PURPLE = ARGB16(1, 5, 0, 10)
};

// Time
int t = 0;

touchPosition touch;
u16 keys;

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

int maxParticles = 100;
std::vector<Particle> particles;

enum SpriteIds {
    SPR_PARTICLE = 0,
    SPR_PLAYER = 3
};

struct Player {
    float x;
    float y;
    float targetX = 0.0;
    float targetY = 0.0;
    int spriteId;
} player;




/*
|--------------------------------------------------------------------------
| Utils
|--------------------------------------------------------------------------
*/
float lerp(float a, float b, float t) {
    return a + t * (b - a);
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
    NF_VramSpriteGfx(1, SPR_PARTICLE, SPR_PARTICLE, true);
    NF_VramSpritePal(0, SPR_PARTICLE, SPR_PARTICLE);
    NF_VramSpritePal(1, SPR_PARTICLE, SPR_PARTICLE);
}


void setupPlayerSprite() {
    NF_LoadSpriteGfx("sprite/ship", SPR_PLAYER, 32, 32);
    NF_LoadSpritePal("sprite/ship", SPR_PLAYER);
    NF_VramSpriteGfx(1, SPR_PLAYER, SPR_PLAYER, true);
    NF_VramSpritePal(1, SPR_PLAYER, SPR_PLAYER);
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
    setupPlayerSprite();
}

void addParticle(float dx, float dy) {
    Particle p;
    p.spriteId = spriteId++;
    p.x = SCREEN_WIDTH / 2;
    p.y = SCREEN_HEIGHT / 2;
    p.dx = dx;
    p.dy = dy;
    p.life = 0;
    p.maxLife = 600000;

    NF_CreateSprite(0, p.spriteId, SPR_PARTICLE, SPR_PARTICLE, p.x, p.y);
    NF_CreateSprite(1, p.spriteId, SPR_PARTICLE, SPR_PARTICLE, p.x, p.y - 192);

    particles.push_back(p);
}



void resetParticle(Particle &p) {
    p.x = SCREEN_WIDTH / 2;
    p.y = SCREEN_HEIGHT / 2;
    p.dx = ((rand() % 100) / 100.0 - 0.5) / 4.0;
    p.dy = ((rand() % 100) / 100.0 - 0.5) / 4.0;
    p.life = 0;
    p.maxLife = 60000;


    NF_ShowSprite(0, p.spriteId, true);
    NF_ShowSprite(1, p.spriteId, false);

    NF_MoveSprite(0, p.spriteId, p.x, p.y);
    NF_MoveSprite(1, p.spriteId, p.x, p.y - 192);
}

void updateParticles() {
    for (int i = 0; i < particles.size(); i++) {
        Particle &p = particles[i];
        p.x += p.dx;
        p.y += p.dy;
        p.life++;
        
        NF_SpriteFrame(0, p.spriteId, t / 100 % 2);
        NF_SpriteFrame(1, p.spriteId, t / 100 % 2);


        if (p.life > p.maxLife || p.x < -10 || p.x > SCREEN_WIDTH || p.y < -10 || p.y > SCREEN_HEIGHT * 2) {
            resetParticle(p);
            // NF_DeleteSprite(0, p.spriteId);
            // particles.erase(particles.begin() + i);
        } else {
            NF_MoveSprite(0, p.spriteId, p.x, p.y);
        }

        if (p.y > SCREEN_HEIGHT -10) {
            NF_ShowSprite(1, p.spriteId, true);
            NF_MoveSprite(1, p.spriteId, p.x, p.y - 192);
        } else {
            NF_ShowSprite(1, p.spriteId, false);
        }

        if (p.y > SCREEN_HEIGHT) {
            NF_ShowSprite(0, p.spriteId, false);
        }
    }
}

void createPlayer () {
    player.x = SCREEN_WIDTH / 2;
    player.y = SCREEN_HEIGHT / 2;
    player.spriteId = spriteId++;

    NF_CreateSprite(1, player.spriteId, SPR_PLAYER, SPR_PLAYER, player.x, player.y);
}

void updatePlayer() {
    if (keys & KEY_TOUCH) {
        player.targetX = touch.px - 16;
        player.targetY = touch.py - 16;
    }


    player.x = lerp(player.targetX, player.x, 0.99);
    player.y = lerp(player.targetY, player.y, 0.99);

    NF_SpriteFrame(1, player.spriteId, t / 20 % 3);
    NF_MoveSprite(1, player.spriteId, player.x, player.y);
}


/*
|--------------------------------------------------------------------------
| Main
|--------------------------------------------------------------------------
*/
int main(int argc, char **argv)
{
    setupGraphics();
    // consoleDemoInit();
    createPlayer();

    for (int i = 0; i < maxParticles; i++) {
        float dx = ((rand() % 100) / 100.0 - 0.5) / 4.0;
        float dy = ((rand() % 100) / 100.0 - 0.5) / 4.0;
        addParticle(dx, dy);
    }

    while (true) {
        t++;

        // Read keys, touch and piano
        scanKeys();
        keys = keysHeld();
        u16 down = keysDown();
        
        touchRead(&touch);


        updateParticles();
        updatePlayer();
        printf("\x1b[0;0HParticles: %d", particles.size());


        NF_SpriteOamSet(0);
        NF_SpriteOamSet(1);
        oamUpdate(&oamMain);
        oamUpdate(&oamSub);
    }
}
