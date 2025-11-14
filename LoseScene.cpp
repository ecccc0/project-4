#include "LoseScene.h"
#include "cs3113.h" // For ColorFromHex

LoseScene::LoseScene(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LoseScene::~LoseScene() { shutdown(); }

void LoseScene::initialise() 
{ 
    mGameState.nextSceneID = -1; 
}

void LoseScene::update(float deltaTime) 
{
    if (IsKeyPressed(KEY_ENTER))
    {
        mGameState.nextSceneID = 0; // Switch to MenuScene
    }
}

void LoseScene::render() 
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    
    DrawText("You Lose!", 
             GetScreenWidth() / 2 - MeasureText("You Lose!", 40) / 2, 
             GetScreenHeight() / 2 - 20, 
             40, 
             WHITE);
             
    DrawText("Press ENTER to return to menu", 
             GetScreenWidth() / 2 - MeasureText("Press ENTER to return to menu", 20) / 2, 
             GetScreenHeight() / 2 + 30, 
             20, 
             WHITE);
}

void LoseScene::shutdown() {}