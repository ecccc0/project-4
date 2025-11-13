#include "LevelTwo.h"

LevelTwo::LevelTwo()                                      : Scene { {0.0f}, nullptr   } {}
LevelTwo::LevelTwo(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelTwo::~LevelTwo() { shutdown(); }

void LevelTwo::initialise()
{
   mGameState.nextSceneID = 0;

   mGameState.bgm = LoadMusicStream("");
   SetMusicVolume(mGameState.bgm, 0.33f);
   // PlayMusicStream(gState.bgm);

   mGameState.jumpSound = LoadSound("");

   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      ,   // texture filepath
      TILE_DIMENSION,              // tile size
                              // texture cols & rows
      mOrigin                      // in-game origin
   );

   /*
      ----------- PROTAGONIST -----------
   */
   
   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelTwo::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   mGameState.->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      nullptr,        // collidable entities
      0               // col. entity count
   );

   Vector2 currentPlayerPosition = { mGameState.->getPosition().x, mOrigin.y };

   if (mGameState.->getPosition().y > 800.0f) mGameState.nextSceneID = 1;

   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelTwo::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.->render();
   mGameState.map->render();
}

void LevelTwo::shutdown()
{
   delete mGameState.;
   delete mGameState.map;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
}