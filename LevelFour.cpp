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

   // BGM is handled globally in main.cpp now

   mGameState.jumpSound = LoadSound("assets/Dirt Jump.wav");
   mGameState.deathSound = LoadSound("assets/Death Sound.ogg"); 
   mGameState.winSound = LoadSound("assets/level_win.wav");

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

   
     mGameState.player = new Entity(
        { mOrigin.x - (LEVELONE_WIDTH * TILE_DIMENSION / 4.0f), mOrigin.y - 50.0f }, 
        { TILE_DIMENSION * 0.8f, TILE_DIMENSION * 0.8f }, 
        "assets/player.png",     
        PLAYER
    );

   
   mGameState.player->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
   mGameState.player->setJumpingPower(550.0f); 
   
   /*
      ----------- BOSS -----------
   */
   mGameState.enemyCount = 1;
   mGameState.enemies = new Entity[mGameState.enemyCount];
   
   // Configure the default-constructed Entity allocated in the array
   mGameState.enemies[0].setPosition({ mOrigin.x, mOrigin.y - TILE_DIMENSION * 2 }); // Start in middle
   mGameState.enemies[0].setScale({ TILE_DIMENSION * 1.5f, TILE_DIMENSION * 1.5f }); // Make it bigger
   mGameState.enemies[0].setColliderDimensions({ TILE_DIMENSION * 1.5f, TILE_DIMENSION * 1.5f });
   mGameState.enemies[0].setTexture("assets/enemy_fly.png"); // Use flyer texture (or a new boss texture)
   // Keep the boss as an NPC so collision/stomp logic works, but make it stationary
   mGameState.enemies[0].setEntityType(NPC);
   mGameState.enemies[0].setAIType(FLYER);
   mGameState.enemies[0].setAIState(IDLE);    // Don't patrol
   mGameState.enemies[0].setAcceleration({ 0.0f, 0.0f }); // No gravity
   mGameState.enemies[0].resetMovement();
   mGameState.enemies[0].setSpeed(0); // No movement
   
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

   // Player update runs first, which will set its collision flags
   mGameState.player->update(
      deltaTime,
      mGameState.player,
      mGameState.map,
      nullptr,   // Do not resolve physics against boss; handle manually below
      0
   );

   // Boss update
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
           // Player is touching the boss. Check HOW:
           // Stomp if the player is falling AND the player's bottom is above the boss's top (with small epsilon)
           bool isPlayerFalling = mGameState.player->getVelocity().y > 0;
           Vector2 playerPos = mGameState.player->getPosition();
           Vector2 enemyPos  = mGameState.enemies[i].getPosition();
           Vector2 playerDim = mGameState.player->getColliderDimensions();
           Vector2 enemyDim  = mGameState.enemies[i].getColliderDimensions();
           float playerBottom = playerPos.y + playerDim.y / 2.0f;
           float enemyTop     = enemyPos.y  - enemyDim.y  / 2.0f;
           const float epsilon = 2.0f;

           if (isPlayerFalling && playerBottom <= enemyTop + epsilon)
           {
               // --- WIN CONDITION: Player stomped the boss ---
               PlaySound(mGameState.winSound);
               mGameState.nextSceneID = 5; // Go to WinScene
               mGameState.enemies[i].deactivate(); // Stop the boss
               return; // Exit update
           }
           else
           {
               // --- LOSE CONDITION: Player hit side or bottom ---
               handlePlayerDeath(); 
               return; 
           }
       }
   }

   // Check for falling
   if (mGameState.player->getPosition().y > END_GAME_THRESHOLD)
   {
       handlePlayerDeath();
       return; 
   }

   // --- OLD WIN CONDITION (REMOVED) ---
   // float levelEndX = mGameState.map->getRightBoundary() - TILE_DIMENSION * 2;
   // if (mGameState.player->getPosition().x > levelEndX)
   // {
   //     PlaySound(mGameState.winSound);
   //     mGameState.nextSceneID = 5; // Go to WinScene (index 5)
   // }

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