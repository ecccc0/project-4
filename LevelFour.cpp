#include "LevelFour.h"
#include "cs3113.h" // For LOG

// Function from main.cpp that we will call
extern void handlePlayerDeath();

LevelFour::LevelFour()                                      : Scene { {0.0f}, nullptr   } {}
LevelFour::LevelFour(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelFour::~LevelFour() { shutdown(); }

void LevelFour::initialise()
{
   mGameState.nextSceneID = -1; // Use -1 for "no change"
   mGameState.enemyCount = 0;
   mGameState.enemies = nullptr;

   mGameState.bgm = LoadMusicStream("assets/game/07 - Final Defeat.wav"); // Boss music
   SetMusicVolume(mGameState.bgm, 0.40f);
   PlayMusicStream(mGameState.bgm);

   mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");
   mGameState.deathSound = LoadSound("assets/game/player_death.wav"); 
   mGameState.winSound = LoadSound("assets/game/level_win.wav");

   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVELFOUR_WIDTH, LEVELFOUR_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/tileset.png",        // texture filepath
      TILE_DIMENSION,              // tile size
      9, 4,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   /*
      ----------- PROTAGONIST -----------
   */
   std::map<Direction, std::vector<int>> playerAnimations;
   playerAnimations[RIGHT] = { 0, 1, 2, 3 };
   playerAnimations[LEFT]  = { 4, 5, 6, 7 };
   playerAnimations[UP]    = { 0 }; 
   playerAnimations[DOWN]  = { 0 }; 
   
   mGameState.player = new Entity(
        { mOrigin.x - (LEVELONE_WIDTH * TILE_DIMENSION / 4.0f), mOrigin.y }, 
        { TILE_DIMENSION * 0.8f, TILE_DIMENSION * 0.8f }, 
        "assets/player.png",     
        PLAYER
    );
   
   mGameState.player->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
   mGameState.player->setJumpingPower(700.0f); 
   
   /*
      ----------- BOSS -----------
   */
   mGameState.enemyCount = 1;
   mGameState.enemies = new Entity[mGameState.enemyCount];
   
      // Spawn the Boss (a fast flyer) using default constructor then configure
      mGameState.enemies[0].setPosition({ mOrigin.x, mOrigin.y - TILE_DIMENSION * 2 });
      mGameState.enemies[0].setScale({ TILE_DIMENSION * 1.5f, TILE_DIMENSION * 1.5f });
      mGameState.enemies[0].setTexture("assets/enemy_fly.png");
      mGameState.enemies[0].setEntityType(NPC);
      mGameState.enemies[0].setAIType(FLYER);
      mGameState.enemies[0].setAIState(WALKING); // Will patrol
      mGameState.enemies[0].setSpeed(250); // Make it fast
   
   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.player->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelFour::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   mGameState.player->update(
      deltaTime,      
      mGameState.player, 
      mGameState.map, 
      mGameState.enemies,    
      mGameState.enemyCount  
   );

   for (int i = 0; i < mGameState.enemyCount; i++)
   {
       mGameState.enemies[i].update(
           deltaTime,
           mGameState.player,
           mGameState.map,
           nullptr, 
           0
       );
   }

   // Check for collision with boss
   for (int i = 0; i < mGameState.enemyCount; i++)
   {
       if (mGameState.enemies[i].isActive() && mGameState.player->isColliding(&mGameState.enemies[i]))
       {
           handlePlayerDeath(); 
           return; 
       }
   }

   // Check for falling
   if (mGameState.player->getPosition().y > END_GAME_THRESHOLD)
   {
       handlePlayerDeath();
       return; 
   }

   // Win Condition: Reach the right wall
   float levelEndX = mGameState.map->getRightBoundary() - TILE_DIMENSION * 2;
   if (mGameState.player->getPosition().x > levelEndX)
   {
       PlaySound(mGameState.winSound);
       mGameState.nextSceneID = 5; // Go to WinScene (index 5)
   }

   // Camera Panning
   Vector2 currentPlayerPosition = { mGameState.player->getPosition().x, mOrigin.y };
   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelFour::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   for (int i = 0; i < mGameState.enemyCount; i++)
   {
       mGameState.enemies[i].render();
   }
   
   mGameState.player->render();
   mGameState.map->render();
}

void LevelFour::shutdown()
{
   delete mGameState.player;
   delete[] mGameState.enemies; 
   delete mGameState.map;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
   UnloadSound(mGameState.deathSound);
   UnloadSound(mGameState.winSound);
}