#include "LevelThree.h"

#ifndef LEVELFOUR_H
#define LEVELFOUR_H

constexpr int LEVEL_WIDTH = ,
              LEVEL_HEIGHT = ;

class LevelFour : public Scene {
private:
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        
    };

public:
    static constexpr float TILE_DIMENSION       = ,
                        ACCELERATION_OF_GRAVITY = 981.0f,
                        END_GAME_THRESHOLD      = 800.0f;

    LevelFour();
    LevelFour(Vector2 origin, const char *bgHexCode);
    ~LevelFour();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVELFOUR_H