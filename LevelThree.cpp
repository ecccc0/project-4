#include "LevelThree.h"

LevelThree::LevelThree()                                      : Scene { {0.0f}, nullptr   } {}
LevelThree::LevelThree(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelThree::~LevelThree() { shutdown(); }

void LevelThree::initialise()
{
    // Start with no automatic scene change; let gameplay decide when to switch
    mGameState.nextSceneID = -1;

    mGameState.bgm = LoadMusicStream("");
    SetMusicVolume(mGameState.bgm, 0.33f);
    // PlayMusicStream(gState.bgm);

    mGameState.jumpSound = LoadSound("");
    mGameState.deathSound = LoadSound("");
    mGameState.winSound  = LoadSound("");

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
        nullptr,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );

    Vector2 currentPlayerPosition = { mGameState.player->getPosition().x, mOrigin.y };

    if (mGameState.player->getPosition().y > 800.0f) mGameState.nextSceneID = 4;

    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelThree::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

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