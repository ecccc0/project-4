#include "LevelThree.h" // Includes Scene.h

#ifndef LEVELFOUR_H
#define LEVELFOUR_H

// Boss Arena dimensions
constexpr int LEVELFOUR_WIDTH = 18,
              LEVELFOUR_HEIGHT = 10;

class LevelFour : public Scene {
private:
    // 18x10 Boss Arena Layout
    unsigned int mLevelData[LEVELFOUR_WIDTH * LEVELFOUR_HEIGHT] = {
        12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12,
        12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12,
        12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12,
        12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12,
        12, 0, 0, 0, 0, 3, 3, 3, 0, 0, 3, 3, 3, 0, 0, 0, 0, 12,
        12, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12,
        12, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12,
        12, 3, 12, 12, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 12,
        12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
        12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12
        // 3 = top tile, 12 = bottom/wall tile, 0 = empty
    };

public:
    static constexpr float TILE_DIMENSION       = 64.0f,
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