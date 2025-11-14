#include "LevelTwo.h"
#include "cs3113.h" // For LOG

// Function from main.cpp that we will call
extern void handlePlayerDeath();

LevelTwo::LevelTwo()                                      : Scene { {0.0f}, nullptr   } {}
LevelTwo::LevelTwo(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelTwo::~LevelTwo() { shutdown(); }

void LevelTwo::initialise()
{
    mGameState.nextSceneID = -1;
    mGameState.enemyCount = 0;
    mGameState.enemies = nullptr;

    mGameState.bgm = LoadMusicStream("assets/game/07 - Final Defeat.wav"); // Final BGM
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");
    mGameState.deathSound = LoadSound("assets/game/Death Sound.wav");
    mGameState.winSound  = LoadSound("assets/game/level_win.wav");

   /*
      ----------- MAP -----------
   */
    mGameState.map = new Map(
        LEVELTWO_WIDTH, LEVELTWO_HEIGHT,   
        (unsigned int *) mLevelData, 
        "assets/tileset.png",        
        TILE_DIMENSION,              
        9, 4,                        
        mOrigin                      
    );

   /*
      ----------- PROTAGONIST -----------
   */
    mGameState.player = new Entity(
        { mOrigin.x - (LEVELONE_WIDTH * TILE_DIMENSION / 4.0f), mOrigin.y }, 
        { TILE_DIMENSION * 0.8f, TILE_DIMENSION * 0.8f }, 
        "assets/player.png",     
        PLAYER
    );
   
    mGameState.player->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    mGameState.player->setJumpingPower(700.0f); 
    
    /*
        ----------- ENEMIES -----------
    */
    mGameState.enemyCount = 3; // All three enemy types
    mGameState.enemies = new Entity[mGameState.enemyCount];
   
   // Spawn a WANDERER (default construct then configure)
    mGameState.enemies[0].setPosition({ mOrigin.x - TILE_DIMENSION * 2, mOrigin.y - TILE_DIMENSION * 2 });
    mGameState.enemies[0].setScale({ TILE_DIMENSION * 0.8f, TILE_DIMENSION * 0.8f });
    mGameState.enemies[0].setTexture("assets/enemy_wander.png");
    mGameState.enemies[0].setEntityType(NPC);
    mGameState.enemies[0].setAIType(WANDERER);
    mGameState.enemies[0].setAIState(WALKING);
    mGameState.enemies[0].setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    mGameState.enemies[0].setSpeed(100);

   // Spawn a FOLLOWER
    mGameState.enemies[1].setPosition({ mOrigin.x + TILE_DIMENSION * 3, mOrigin.y - TILE_DIMENSION * 4 });
    mGameState.enemies[1].setScale({ TILE_DIMENSION * 0.8f, TILE_DIMENSION * 0.8f });
    mGameState.enemies[1].setTexture("assets/enemy_follow.png");
    mGameState.enemies[1].setEntityType(NPC);
    mGameState.enemies[1].setAIType(FOLLOWER);
    mGameState.enemies[1].setAIState(IDLE); 
    mGameState.enemies[1].setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    mGameState.enemies[1].setSpeed(120);
   
   // Spawn a FLYER
    mGameState.enemies[2].setPosition({ mOrigin.x + TILE_DIMENSION * 6, mOrigin.y - TILE_DIMENSION * 3 });
    mGameState.enemies[2].setScale({ TILE_DIMENSION * 0.8f, TILE_DIMENSION * 0.8f });
    mGameState.enemies[2].setTexture("assets/enemy_fly.png");
    mGameState.enemies[2].setEntityType(NPC);
    mGameState.enemies[2].setAIType(FLYER);
    mGameState.enemies[2].setAIState(WALKING);
    mGameState.enemies[2].setSpeed(150);
   
   /*
      ----------- CAMERA -----------
   */
    mGameState.camera = { 0 };                                    
    mGameState.camera.target = mGameState.player->getPosition(); 
    mGameState.camera.offset = mOrigin;                           
    mGameState.camera.rotation = 0.0f;                            
    mGameState.camera.zoom = 1.0f;                                
}

void LevelTwo::update(float deltaTime)
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

    for (int i = 0; i < mGameState.enemyCount; i++)
    {
        if (mGameState.enemies[i].isActive() && mGameState.player->isColliding(&mGameState.enemies[i]))
        {
            handlePlayerDeath(); 
            return; 
        }
    }

    if (mGameState.player->getPosition().y > END_GAME_THRESHOLD)
    {
        handlePlayerDeath();
        return; 
    }

    float levelEndX = mGameState.map->getRightBoundary() - TILE_DIMENSION;
    if (mGameState.player->getPosition().x > levelEndX)
    {
        mGameState.nextSceneID = 3; // Go to WinScene (index 3)
    }

    Vector2 currentPlayerPosition = { mGameState.player->getPosition().x, mOrigin.y };
    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelTwo::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    for (int i = 0; i < mGameState.enemyCount; i++)
    {
        mGameState.enemies[i].render();
    }
    
    mGameState.player->render();
    mGameState.map->render();
}

void LevelTwo::shutdown()
{
    delete mGameState.player;
    delete[] mGameState.enemies; 
    delete mGameState.map;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.deathSound);
    UnloadSound(mGameState.winSound);
}