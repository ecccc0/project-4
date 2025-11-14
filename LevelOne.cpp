#include "LevelOne.h"
#include "cs3113.h" // For LOG

// Function from main.cpp that we will call
extern void handlePlayerDeath();

LevelOne::LevelOne()                                      : Scene { {0.0f}, nullptr   } {}
LevelOne::LevelOne(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelOne::~LevelOne() { shutdown(); }

void LevelOne::initialise()
{
    mGameState.nextSceneID = -1; // -1 means "no change"
    mGameState.enemyCount = 0;
    mGameState.enemies = nullptr;

    mGameState.bgm = LoadMusicStream("assets/game/04 - Silent Forest.wav");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");
    mGameState.deathSound = LoadSound("assets/game/Death Sound.wav");
    mGameState.winSound  = LoadSound("assets/game/level_win.wav");

   /*
      ----------- MAP -----------
   */
    mGameState.map = new Map(
        LEVELONE_WIDTH, LEVELONE_HEIGHT,   // map grid cols & rows
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
    mGameState.enemyCount = 1;
    mGameState.enemies = new Entity[mGameState.enemyCount];
    
    // Spawn a WANDERER using default constructor then configure
    // The array allocation already default-constructs each Entity
    mGameState.enemies[0].setPosition({ mOrigin.x + TILE_DIMENSION, mOrigin.y - TILE_DIMENSION * 2 });
    mGameState.enemies[0].setScale({ TILE_DIMENSION * 0.8f, TILE_DIMENSION * 0.8f });
    mGameState.enemies[0].setTexture("assets/enemy_wander.png");
    mGameState.enemies[0].setEntityType(NPC);
    mGameState.enemies[0].setAIType(WANDERER);
    mGameState.enemies[0].setAIState(WALKING);
    mGameState.enemies[0].setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    mGameState.enemies[0].setSpeed(100); // Slower than player
    
    /*
        ----------- CAMERA -----------
    */
    mGameState.camera = { 0 };                                    
    mGameState.camera.target = mGameState.player->getPosition(); 
    mGameState.camera.offset = mOrigin;                           
    mGameState.camera.rotation = 0.0f;                            
    mGameState.camera.zoom = 1.0f;                                
}

void LevelOne::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    // Update Player, passing in enemies for collision
    mGameState.player->update(
        deltaTime,      
        mGameState.player, 
        mGameState.map, 
        mGameState.enemies,    // Pass enemies
        mGameState.enemyCount  // Pass enemy count
    );

    // Update all enemies
    for (int i = 0; i < mGameState.enemyCount; i++)
    {
        mGameState.enemies[i].update(
            deltaTime,
            mGameState.player,
            mGameState.map,
            nullptr, // Enemies don't collide with other enemies (for now)
            0
        );
    }

    // Check for player collision with enemies
    for (int i = 0; i < mGameState.enemyCount; i++)
    {
        // Check if the enemy is active AND player is colliding with it
        if (mGameState.enemies[i].isActive() && mGameState.player->isColliding(&mGameState.enemies[i]))
        {
            handlePlayerDeath(); // Call death function
            return; // Exit update early to prevent further logic
        }
    }

    // Check for falling off the map
    if (mGameState.player->getPosition().y > END_GAME_THRESHOLD)
    {
        handlePlayerDeath();
        return; // Stop updating this frame
    }

    // Check for reaching the end of the level
    float levelEndX = mGameState.map->getRightBoundary() - TILE_DIMENSION;
    if (mGameState.player->getPosition().x > levelEndX)
    {
        mGameState.nextSceneID = 2; // Go to LevelTwo
    }

    // Update camera target
    Vector2 currentPlayerPosition = { mGameState.player->getPosition().x, mOrigin.y };
    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelOne::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    // Render all enemies
    for (int i = 0; i < mGameState.enemyCount; i++)
    {
        mGameState.enemies[i].render();
    }
    
    mGameState.player->render();
    mGameState.map->render();
}

void LevelOne::shutdown()
{
    delete mGameState.player;
    delete[] mGameState.enemies; // Clean up enemy array
    delete mGameState.map;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.deathSound); // Added
    UnloadSound(mGameState.winSound);
}