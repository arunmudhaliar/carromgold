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

class Board;
class MBoardObserver {
public:
    virtual void OnPocketed(Ball& coin) = 0;
    virtual void OnFinishTurn() = 0;
};

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
        GAME_FINISH_TURN,
        GAME_RESET,
        GAME_STATE_MAX
    };
    
    enum PLAYER_TURN {
        TURN_PLAYER,
        TURN_OPPONENT,
        TURN_MAX
    };
    
    Board();
    ~Board();
    
    void InitBoard(const vector2i& viewPort, CGETextureManager& textureManager,
                   SoundEngine* soundEnginePtr, MStrickerObserver* observer, MBoardObserver* boardObserver);
    void UpdateBoard();
    void DrawBoard(const matrix4x4f& viewProjection);
    
    // Mouse inputs
    void MouseBtnUp(const vector2x& pos);
    void MouseBtnDown(const vector2x& pos);
    void MouseMove(const vector2x& pos);
    
    inline Stricker& GetPlayerStricker()            { return this->playerStricker; }
    inline Stricker& GetOpponentStricker()          { return this->opponentStricker; }
    inline GAME_STATE GetBoardState()               { return this-> gameState; }
    inline PLAYER_TYPE GetPlayerType()              { return this->playerType; }
    inline void SetPlayerType(PLAYER_TYPE type)     { this->playerType = type; }
    inline bool IsMyTurn()                          { return this->gameTurn==TURN_PLAYER; }
    
    void TryStartGame();
    void TryTurnPlayer(bool force = false);
    void TryTurnOpponent(bool force = false);
    bool CanShowBoard() { return this->gameState>=GAME_START && this->gameState<=GAME_RESET; }
    bool IsPlayerStricker(Stricker* stricker);
    
    // For opponent stricker
    void Remote_Fire(const vector2x& force);
    
    inline Solver& GetPhysicsSolver() { return this->physicsSolver; }
    inline const std::map<int, Ball*>& GetCoins() { return this->coins; }
    void ResetCoins();
    
protected:
    
    void StopAllCoins();
    void SetGameState(GAME_STATE state);
    
    void OnGameStateChange(GAME_STATE from);
    void OnGameInit();
    void OnGameStart();
    void OnGamePlayerPlaceStricker();
    void OnGamePlayerPlaceAim();
    void OnGamePlayerFire();
    void OnGameFinishTurn();
    void OnGameReset();
    void OnFixedUpdate(intx fixedDT) override;
    
    // GamePlay observers
    void OnPlayerTurn();
    void OnOpponentTurn();
    
    MBoardObserver* boardObserver;
    
    PLAYER_TYPE playerType;
    PLAYER_TURN gameTurn;
    vector2x bottomStrickerInitPosition;
    vector2x topStrickerInitPosition;
    
    Solver physicsSolver;
    GAME_STATE gameState;
    Stricker playerStricker;
    Stricker opponentStricker;
    std::map<int, Ball*> coins;
    intx elapsedTimeSinceFire;
    
    Sprite2Dx moveAreaBgSprite;
    Sprite2Dx barBGSprite;
    
    Hole holes[4];
    Wall ground;
    Wall topWall;
    Wall leftWall;
    Wall rightWall;
    
    // only used for rendering purpose
    matrix4x4f boardMatrix;
    matrix4x4f boardMatrixInv;
    Sprite2Dx bgSprite;
    Sprite2Dx boardSprite;
    Sprite2Dx whiteCoinSprite;
    Sprite2Dx blackCoinSprite;
    Sprite2Dx queenSprite;
    Sprite2Dx coinShadowSprite;
    
    // dont delete this pointer
    SoundEngine* soundEnginePtr;
};

#endif /* board_hpp */
