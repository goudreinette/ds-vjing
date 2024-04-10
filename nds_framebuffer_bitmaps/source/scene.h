#ifndef SCENE_H

#define SCENE_H


class Scene {
    public:
        virtual void setup();
        virtual void update();
        virtual void draw();
};

#endif