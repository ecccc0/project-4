#include "LevelFour.h"
#include "MenuScene.h"
#include "WinScene.h"
#include "LoseScene.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_SCENES = 7; // Menu + 4 Levels + Win + Lose

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

// Scene Pointers
MenuScene *gMenuScene = nullptr;
LevelOne *gLevelOne = nullptr;
LevelTwo *gLevelTwo = nullptr;
LevelThree *gLevelThree = nullptr;
LevelFour *gLevelFour = nullptr;
WinScene *gWinScene = nullptr;
LoseScene *gLoseScene = nullptr;

// Game State Globals
int gLives = 3;
int gCurrentLevelIndex = 0;

// Function Declarations
void switchToScene(int sceneIndex);
void handlePlayerDeath(); // Added this declaration
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(int sceneIndex)
{   
    if (sceneIndex < 0 || sceneIndex >= gLevels.size()) return; // Safety check

    gCurrentScene = gLevels[sceneIndex];
    gCurrentScene->initialise();
    gCurrentLevelIndex = sceneIndex;
}

/**
 * @brief Handles the logic for when the player dies (e.g., falling or AI collision).
 * Decrements lives and switches to the Lose Scene or restarts the current level.
 */
void handlePlayerDeath()
{   
    PlaySound(gCurrentScene->getState().deathSound);
    gLives--;
    
    if (gLives <= 0)
    {
        // No lives left, go to Lose Scene
        gCurrentScene->getState().nextSceneID = 6; // Index 6 is gLoseScene
    }
    else
    {
        // Lives remaining, restart the current level
        gCurrentScene->getState().nextSceneID = gCurrentLevelIndex;
    }
}


void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();

    // Create all scenes
    gMenuScene = new MenuScene(ORIGIN, "#4A4A4A"); // Menu
    gLevelOne = new LevelOne(ORIGIN, "#C0897E");
    gLevelTwo = new LevelTwo(ORIGIN, "#011627");
    gLevelThree = new LevelThree(ORIGIN, "#FF6347");
    gLevelFour = new LevelFour(ORIGIN, "#4682B4");
    gWinScene = new WinScene(ORIGIN, "#37833D"); // Win
    gLoseScene = new LoseScene(ORIGIN, "#8B0000"); // Lose

    // Add them to the vector IN ORDER
    gLevels.push_back(gMenuScene);  // Index 0
    gLevels.push_back(gLevelOne);   // Index 1
    gLevels.push_back(gLevelTwo);   // Index 2
    gLevels.push_back(gLevelThree); // Index 3
    gLevels.push_back(gLevelFour);  // Index 4
    gLevels.push_back(gWinScene);   // Index 5
    gLevels.push_back(gLoseScene);  // Index 6

    // Start at the menu scene
    switchToScene(0);

    SetTargetFPS(FPS);
}

void processInput() 
{
    // Player controls are only processed if we are in a level scene
    if (gCurrentLevelIndex >= 1 && gCurrentLevelIndex <= 4)
    {
        // Safety check in case player is null (e.g., during scene transition)
        if (gCurrentScene->getState().player == nullptr) return;
        
        gCurrentScene->getState().player->resetMovement();

        if      (IsKeyDown(KEY_A)) gCurrentScene->getState().player->moveLeft();
        else if (IsKeyDown(KEY_D)) gCurrentScene->getState().player->moveRight();

        if (IsKeyPressed(KEY_W) && 
            gCurrentScene->getState().player->isCollidingBottom())
        {
            gCurrentScene->getState().player->jump();
            PlaySound(gCurrentScene->getState().jumpSound);
        }

        if (GetLength(gCurrentScene->getState().player->getMovement()) > 1.0f) 
            gCurrentScene->getState().player->normaliseMovement();
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    
    // Use the scene's camera only if it's a level
    if (gCurrentLevelIndex >= 1 && gCurrentLevelIndex <= 4)
    {
        BeginMode2D(gCurrentScene->getState().camera);
        gCurrentScene->render();
        EndMode2D();
    }
    else
    {
        // For Menu/Win/Lose, just render without a camera
        gCurrentScene->render();
    }

    EndDrawing();
}

void shutdown() 
{
    delete gMenuScene;
    delete gLevelOne;
    delete gLevelTwo;
    delete gLevelThree;
    delete gLevelFour;
    delete gWinScene;
    delete gLoseScene;

    // gLevels vector just holds pointers, no need to delete them again
    gLevels.clear();

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        // Check if the current scene wants to switch
        if (gCurrentScene->getState().nextSceneID >= 0)
        {
            switchToScene(gCurrentScene->getState().nextSceneID);
        }

        render();
    }

    shutdown();

    return 0;
}