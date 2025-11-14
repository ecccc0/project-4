#include "LevelThree.h"
#include "cs3113.h" // For types/helpers if needed

// Function from main.cpp that we will call
extern void handlePlayerDeath();

LevelThree::LevelThree()                                      : Scene { {0.0f}, nullptr   } {}
LevelThree::LevelThree(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelThree::~LevelThree() { shutdown(); }

void LevelThree::initialise()
{
    // Start with no automatic scene change; let gameplay decide when to switch
    mGameState.nextSceneID = -1;

    // BGM is handled globally in main.cpp now

    // Load SFX using shared assets
    mGameState.jumpSound = LoadSound("assets/Dirt Jump.wav");
    mGameState.deathSound = LoadSound("assets/Death Sound.ogg");
    mGameState.winSound  = LoadSound("assets/level_win.wav");

    /*
        ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVELTHREE_WIDTH, LEVELTHREE_HEIGHT,   // map grid cols & rows
        (unsigned int *) mLevelData, // grid data
        "assets/tileset.png",   // texture filepath
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
    mGameState.player->setJumpingPower(500.0f); 

    // ----------- ENEMIES -----------
    mGameState.enemyCount = 2;
    mGameState.enemies = new Entity[mGameState.enemyCount];

    // Enemy 0: WANDERER on a platform
    mGameState.enemies[0].setPosition({ mOrigin.x - TILE_DIMENSION * 2, mOrigin.y - TILE_DIMENSION * 2 });
    mGameState.enemies[0].setScale({ TILE_DIMENSION * 0.8f, TILE_DIMENSION * 0.8f });
    mGameState.enemies[0].setColliderDimensions({ TILE_DIMENSION * 0.8f, TILE_DIMENSION * 0.8f });
    mGameState.enemies[0].setTexture("assets/enemy_wander.png");
    mGameState.enemies[0].setEntityType(NPC);
    mGameState.enemies[0].setAIType(WANDERER);
    mGameState.enemies[0].setAIState(WALKING);
    mGameState.enemies[0].setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    mGameState.enemies[0].setSpeed(100);

    // Enemy 1: FOLLOWER on a higher ledge
    mGameState.enemies[1].setPosition({ mOrigin.x + TILE_DIMENSION * 4, mOrigin.y - TILE_DIMENSION * 3 });
    mGameState.enemies[1].setScale({ TILE_DIMENSION * 0.8f, TILE_DIMENSION * 0.8f });
    mGameState.enemies[1].setColliderDimensions({ TILE_DIMENSION * 0.8f, TILE_DIMENSION * 0.8f });
    mGameState.enemies[1].setTexture("assets/enemy_follow.png");
    mGameState.enemies[1].setEntityType(NPC);
    mGameState.enemies[1].setAIType(FOLLOWER);
    mGameState.enemies[1].setAIState(IDLE);
    mGameState.enemies[1].setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    mGameState.enemies[1].setSpeed(120);
    /*
        ----------- CAMERA -----------
    */
    mGameState.camera = { 0 };                                    // zero initialize
    mGameState.camera.target = mGameState.player->getPosition(); // camera follows player
    mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
    mGameState.camera.rotation = 0.0f;                            // no rotation
    mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelThree::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    mGameState.player->update(
        deltaTime,      // delta time / fixed timestep
        mGameState.player,        // player
        mGameState.map, // map
        mGameState.enemies,        // collidable entities
        mGameState.enemyCount               // col. entity count
    );

    // Update enemies
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

    // Player/enemy collision check
    for (int i = 0; i < mGameState.enemyCount; i++)
    {
        if (mGameState.enemies[i].isActive() && mGameState.player->isColliding(&mGameState.enemies[i]))
        {
            handlePlayerDeath();
            return;
        }
    }

    Vector2 currentPlayerPosition = { mGameState.player->getPosition().x, mOrigin.y };

    if (mGameState.player->getPosition().y > 800.0f) mGameState.nextSceneID = 4;

    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelThree::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    for (int i = 0; i < mGameState.enemyCount; i++)
    {
        mGameState.enemies[i].render();
    }
    
    mGameState.player->render();
    mGameState.map->render();
}

void LevelThree::shutdown()
{
    // Clean up player/enemies if present
    if (mGameState.player) delete mGameState.player;
    if (mGameState.enemies) delete[] mGameState.enemies;
    delete mGameState.map;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.deathSound);
    UnloadSound(mGameState.winSound);
}