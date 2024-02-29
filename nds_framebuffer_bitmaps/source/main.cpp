#include "1_color.cpp"
#include "2_bw.cpp"
#include "3_with_sprites.cpp"
#include "4_pico_palette.cpp"
#include "5_perlin.cpp"
#include "6_chars.cpp"

FirstSketch first_sketch;
SecondSketch second_sketch;
WithSprites with_sprites;
PicoPalette pico_palette;
PerlinScene perlin_scene;
CharsScene chars_scene;

Scene* scenes[] = {
    &first_sketch, 
    &second_sketch, 
    &with_sprites, 
    &pico_palette,
    &perlin_scene,
    &chars_scene
};

int currentSceneIndex = 5;
int numScenes = 6;


void setupBottomScreen() {
    
}


int main(void) {
    // Top screen
    Scene* currentScene = scenes[currentSceneIndex];
    currentScene->setup();

    // Bottom screen
    setupBottomScreen();

    // main loop
    while(1) {
        scanKeys();
        int keys = keysDown();
        if (keys & KEY_UP) {
            currentSceneIndex = (currentSceneIndex + 1) % numScenes;
            currentScene = scenes[currentSceneIndex];
            currentScene->setup();
        } else if (keys & KEY_DOWN) {
            currentSceneIndex = (currentSceneIndex - 1 + numScenes) % numScenes;
            currentScene = scenes[currentSceneIndex];
            currentScene->setup();
        }

        currentScene->update();
        currentScene->draw();
    }

    return 0;
}
