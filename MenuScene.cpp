#include "MenuScene.h"
#include "cs3113.h" // For DrawText and ColorFromHex

// Access the global lives variable from main.cpp
extern int gLives;

MenuScene::MenuScene(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

MenuScene::~MenuScene() { shutdown(); }

void MenuScene::initialise() 
{ 
    mGameState.nextSceneID = -1; // -1 means "do not switch"
}

void MenuScene::update(float deltaTime) 
{
    if (IsKeyPressed(KEY_ENTER))
    {
        gLives = 3; // Reset lives for a new game
        mGameState.nextSceneID = 1; // Switch to LevelOne
    }
}

void MenuScene::render() 
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    
    // Draw text in the center of the screen
    DrawText("PROJECT 4: THE PLATFORMER", 
             GetScreenWidth() / 2 - MeasureText("PROJECT 4: THE PLATFORMER", 40) / 2, 
             GetScreenHeight() / 2 - 40, 
             40, 
             WHITE);
             
    DrawText("Press ENTER to start", 
             GetScreenWidth() / 2 - MeasureText("Press ENTER to start", 20) / 2, 
             GetScreenHeight() / 2 + 10, 
             20, 
             WHITE);
}

void MenuScene::shutdown() {}