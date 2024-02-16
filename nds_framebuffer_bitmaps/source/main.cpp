#include "1_color.cpp"
#include "2_bw.cpp"
#include "3_with_sprites.cpp"
#include "4_pico_palette.cpp"


FirstSketch first_sketch;
SecondSketch second_sketch;
WithSprites with_sprites;
PicoPalette pico_palette;


Scene* scenes[] = {
    &first_sketch, 
    &second_sketch, 
    &with_sprites, 
    &pico_palette
};

int currentSceneIndex = 3;


int main(void) {
    Scene* currentScene = scenes[currentSceneIndex];
    currentScene->setup();

    // main loop
    while(1) {
        scanKeys();
        int keys = keysDown();
        if (keys & KEY_UP) {
            currentSceneIndex = (currentSceneIndex + 1) % 4;
            currentScene = scenes[currentSceneIndex];
            currentScene->setup();
        } else if (keys & KEY_DOWN) {
            currentSceneIndex = (currentSceneIndex - 1 + 4) % 4;
            currentScene = scenes[currentSceneIndex];
            currentScene->setup();
        }

        currentScene->update();
        currentScene->draw();
    }

    return 0;
}
