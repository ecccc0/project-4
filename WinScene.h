#include "Scene.h"

#ifndef WINSCENE_H
#define WINSCENE_H

class WinScene : public Scene {
public:
    WinScene(Vector2 origin, const char* bgHexCode);
    ~WinScene();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // WINSCENE_H