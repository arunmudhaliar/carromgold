//
//  board.hpp
//  GDCPhysics
//
//  Created by Arun A on 06/04/19.
//  Copyright © 2019 Arun A. All rights reserved.
//

#ifndef board_hpp
#define board_hpp

#include "stricker.hpp"
#include "wall.hpp"
#include "hole.h"
#include "../Physics/solver.hpp"
#include <map>
#include <stdio.h>

class Board : protected FixedUpdateObserver {
public:
    enum PLAYER_TYPE {
        PLAYER_FIRST,
        PLAYER_SECOND,
        PLAYER_TYPE_MAX
    };
    
    enum GAME_STATE {
        GAME_INIT,
        GAME_START,
        GAME_PLAYER_PLACE_STRICKER,
        GAME_PLAYER_PLACE_AIM,
        GAME_PLAYER_FIRE,
        GAME_RESET,
        GAME_STATE_MAX
    };
    
    Board();
    ~Board();
    
    void InitBoard(const vector2i& viewPort, CGETextureManager& textureManager, SoundEngine* soundEnginePtr);
    void UpdateBoard();
    void DrawBoard(const matrix4x4f& viewProjection);
    
    // Mouse inputs
    void MouseBtnUp(const vector2x& pos);
    void MouseBtnDown(const vector2x& pos);
    void MouseMove(const vector2x& pos);
    
    Stricker& GetStricker() { return this->stricker; }
    GAME_STATE GetBoardState() { return this-> gameState; }
    PLAYER_TYPE GetPlayerType() { return this->playerType; }
    void SetPlayerType(PLAYER_TYPE type) { this->playerType = type; }
    
    void TryStartGame();
    
protected:
    
    void ApplyBoost(int mx, int my);
    void SetGameState(GAME_STATE state);
    void ResetCoins();
    void MoveStricker(intx fixedDT, Ball& ball, vector2x& delta);
    
    void OnGameStateChange(GAME_STATE from);
    void OnGameInit();
    void OnGameStart();
    void OnGamePlayerPlaceStricker();
    void OnGamePlayerPlaceAim();
    void OnGamePlayerFire();
    void OnGameReset();
    void OnFixedUpdate(intx fixedDT) override;
    
    PLAYER_TYPE playerType;
    
    matrix4x4f boardMatrix;
    matrix4x4f boardMatrixInv;
    Solver physicsSolver;
    GAME_STATE gameState;
    Stricker stricker;
    std::map<int, Ball*> coins;
    intx elapsedTimeSinceFire;
    
    Hole holes[4];
    Wall ground;
    Wall topWall;
    Wall leftWall;
    Wall rightWall;
    
    Sprite2Dx bgSprite;
    Sprite2Dx boardSprite;
    Sprite2Dx whiteCoinSprite;
    Sprite2Dx blackCoinSprite;
    Sprite2Dx queenSprite;
    
    SoundEngine* soundEnginePtr;
};

#endif /* board_hpp */
