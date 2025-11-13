#include "LevelTwo.h"

#ifndef LEVELTHREE_H
#define LEVELTHREE_H

constexpr int LEVEL_WIDTH = ,
              LEVEL_HEIGHT = ;

class LevelThree : public Scene {
private:
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        
    };

public:
    static constexpr float TILE_DIMENSION       = ,
                        ACCELERATION_OF_GRAVITY = 981.0f,
                        END_GAME_THRESHOLD      = 800.0f;

    LevelThree();
    LevelThree(Vector2 origin, const char *bgHexCode);
    ~LevelThree();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVELTHREE_H