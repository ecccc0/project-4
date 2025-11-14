#include "CS3113/LevelFour.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 4;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

LevelOne *gLevelOne = nullptr;
LevelTwo *gLevelTwo = nullptr;
LevelThree *gLevelThree = nullptr;
LevelFour *gLevelFour = nullptr;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{   
    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();

    gLevelOne = new LevelOne(ORIGIN, "#C0897E");
    gLevelTwo = new LevelTwo(ORIGIN, "#011627");
    gLevelThree = new LevelThree(ORIGIN, "#FF6347");
    gLevelFour = new LevelFour(ORIGIN, "#4682B4");

    gLevels.push_back(gLevelOne);
    gLevels.push_back(gLevelTwo);
    gLevels.push_back(gLevelThree);
    gLevels.push_back(gLevelFour);

    switchToScene(gLevels[0]);

    SetTargetFPS(FPS);
}

void processInput() 
{
    gCurrentScene->getState().->resetMovement();

    if      (IsKeyDown(KEY_A)) gCurrentScene->getState().->moveLeft();
    else if (IsKeyDown(KEY_D)) gCurrentScene->getState().->moveRight();

    if (IsKeyPressed(KEY_W) && 
        gCurrentScene->getState().->isCollidingBottom())
    {
        gCurrentScene->getState().->jump();
        PlaySound(gCurrentScene->getState().jumpSound);
    }

    if (GetLength(gCurrentScene->getState().->getMovement()) > 1.0f) 
        gCurrentScene->getState().->normaliseMovement();

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
    BeginMode2D(gCurrentScene->getState().camera);

    gCurrentScene->render();

    EndMode2D();
    EndDrawing();
}

void shutdown() 
{
    delete gLevelOne;
    delete gLevelTwo;
    delete gLevelThree;
    delete gLevelFour;

    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

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

        if (gCurrentScene->getState().nextSceneID > 0)
        {
            int id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}