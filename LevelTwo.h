#include "LevelOne.h"

#ifndef LEVELTWO_H
#define LEVELTWO_H

constexpr int LEVEL_WIDTH = ,
              LEVEL_HEIGHT = ;

class LevelTwo : public Scene {
private:
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        
    };

public:
    static constexpr float TILE_DIMENSION       = ,
                        ACCELERATION_OF_GRAVITY = 981.0f,
                        END_GAME_THRESHOLD      = 800.0f;

    LevelTwo();
    LevelTwo(Vector2 origin, const char *bgHexCode);
    ~LevelTwo();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVELTWO_H