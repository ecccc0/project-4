#include "WinScene.h"
#include "cs3113.h" // For ColorFromHex

WinScene::WinScene(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

WinScene::~WinScene() { shutdown(); }

void WinScene::initialise() 
{ 
    mGameState.nextSceneID = -1; 
}

void WinScene::update(float deltaTime) 
{
    if (IsKeyPressed(KEY_ENTER))
    {
        mGameState.nextSceneID = 0; // Switch to MenuScene
    }
}

void WinScene::render() 
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    
    DrawText("You Win!", 
             GetScreenWidth() / 2 - MeasureText("You Win!", 40) / 2, 
             GetScreenHeight() / 2 - 20, 
             40, 
             WHITE);
             
    DrawText("Press ENTER to return to menu", 
             GetScreenWidth() / 2 - MeasureText("Press ENTER to return to menu", 20) / 2, 
             GetScreenHeight() / 2 + 30, 
             20, 
             WHITE);
}

void WinScene::shutdown() {}