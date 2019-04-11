//
//  board.cpp
//  GDCPhysics
//
//  Created by Arun A on 06/04/19.
//  Copyright © 2019 Arun A. All rights reserved.
//

#include "board.hpp"
#include "../GUI/OSUtils.h"

#define BOTTOM_TOP_WALL_HEIGHTx  ITOX(40)
#define LEFT_RIGHT_WALL_WIDTHx  ITOX(40)
#define BALL_SIZE 17
#define PLAYER_WIDTHx ITOX(30)
#define PLAYER_START_X_POSx ITOX(45)
#define BALL_INIT_LATERAL_FORCEx vector2x(0, -ITOX(5000))

#define AIM_FROM_GRAB 0

Board::Board() {
    this->gameTurn = TURN_MAX;
    this->playerType = PLAYER_TYPE_MAX;
    this->gameState = GAME_STATE_MAX;
    this->elapsedTimeSinceFire = 0;
    this->soundEnginePtr = nullptr;
}

Board::~Board() {
    for (auto c : this->coins) {
        delete c.second;
    }
    this->coins.clear();
}

void Board::InitBoard(const vector2i& viewPort, CGETextureManager& textureManager, SoundEngine* soundEnginePtr) {
    this->soundEnginePtr = soundEnginePtr;
    this->bgSprite.setOffset(0, 0);
    this->bgSprite.loadTexture(&textureManager, cpp_getPath("/res/sprites/bg.png").c_str());
    this->bgSprite.setScale(viewPort.x/bgSprite.getClipWidth(), viewPort.y/bgSprite.getClipHeight());
    
    
    this->boardSprite.setOffset(0, 0);
    this->boardSprite.loadTexture(&textureManager, cpp_getPath("/res/sprites/CarromBoard_1.png").c_str());
    
    this->blackCoinSprite.setOffset(0.0f, 0.0f);
    this->blackCoinSprite.loadTexture(&textureManager, cpp_getPath("/res/sprites/BlackP.png").c_str());
    this->whiteCoinSprite.setOffset(0.0f, 0.0f);
    this->whiteCoinSprite.loadTexture(&textureManager, cpp_getPath("/res/sprites/WhiteP.png").c_str());
    this->queenSprite.setOffset(0.0f, 0.0f);
    this->queenSprite.loadTexture(&textureManager, cpp_getPath("/res/sprites/RedP.png").c_str());
    
    // stricker
    vector2x leftOriginPosx=-(BOARD_SIZEx*FTOX(0.5f));
    vector2x strickerPos = BOARD_SIZEx*FTOX(0.5f);
    strickerPos.y=0;
    strickerPos.y+=MULTX(BOARD_SIZEx.y, FTOX(0.172f));
    playerStricker.InitStricker(FTOX(BALL_SIZE*1.15f), FTOX(0.3f), FTOX(0.029f), leftOriginPosx+strickerPos, textureManager, this->soundEnginePtr);
    
    this->SetGameState(GAME_INIT);
#if !ENABLE_MULTIPLAYER
    this->SetGameState(GAME_START);
#endif
    // TODO : need to set from the network data.
//    this->boardMatrix.setRotationMatrix(180, 0, 0, true);
    this->boardMatrix.setPosition(BOARD_OFFSET.x, BOARD_OFFSET.y, 0);
    this->boardMatrixInv = this->boardMatrix.getInverse();
}

void Board::UpdateBoard() {
    if (this->gameState >= GAME_START && this->gameState < GAME_RESET) {
        physicsSolver.UpdateSolver();
        
        if (this->gameState==GAME_PLAYER_FIRE && this->elapsedTimeSinceFire>FTOX(0.2f) && physicsSolver.IsAllRigidBodiesStopped()) {
            vector2x strickerPos = BOARD_SIZEx*FTOX(0.5f);
            strickerPos.y=0;
            strickerPos.y+=MULTX(BOARD_SIZEx.y, FTOX(0.172f));
            vector2x leftOriginPosx=-(BOARD_SIZEx*FTOX(0.5f));
            playerStricker.SetRBPosition(leftOriginPosx+strickerPos, true);
            SetGameState(GAME_PLAYER_PLACE_STRICKER);
        }
    }
}

void Board::OnFixedUpdate(intx fixedDT) {
    if (this->gameState >= GAME_START && this->gameState < GAME_RESET) {
        this->elapsedTimeSinceFire += fixedDT;
        
        if (this->gameState == GAME_PLAYER_PLACE_STRICKER) {
            // stricker update
            this->playerStricker.UpdateStricker(fixedDT);
        } else if (this->gameState == GAME_PLAYER_FIRE) {
            std::vector<int> removeCoins;
            for(auto c : this->coins) {
                for (int x=0;x<4;x++) {
                    if (holes[x].IsOverlap(c.second->GetRBPosition(), c.second->GetRadius())) {
                        removeCoins.push_back(c.first);
                    }
                }
            }
            
            for(auto c : removeCoins) {
                auto rb = this->coins[c];
                if (rb->GetTag() == "Coin" || rb->GetTag() =="Queen") {
                    int sfxID = getRandom(SFX_SOUND::sfx_puck_pot_1, SFX_SOUND::sfx_puck_pot_4+1);
                    auto sfxSrc = this->soundEnginePtr->getSource(sfxID);
                    sfxSrc->play(false);
                } else if (rb->GetTag() == "Stricker") {
                    auto sfxSrc = this->soundEnginePtr->getSource(SFX_SOUND::sfx_puck_pot_2);
                    sfxSrc->play(false);
                }
                
                this->physicsSolver.RemoveRigidBody(rb);
                this->coins.erase(this->coins.find(c));
                GX_DELETE(rb);
            }
        }
    }
}

void Board::DrawBoard(const matrix4x4f& viewProjection) {
    auto mvp = viewProjection * boardMatrix;
#if USE_ProgrammablePipeLine
    auto guishader = HWShaderManager::GetHWShaderManager().GetHWShader(2);
    
    // bg
    guishader->enableProgram();
    bgSprite.draw(guishader, viewProjection);
    boardSprite.draw(guishader, mvp);
    guishader->disableProgram();
#else
    boardSprite.draw();
#endif
    
//#if USE_ProgrammablePipeLine
//    if (this->gameState==GAME_PLAYER_PLACE_STRICKER) {
//        this->stricker.DrawHelperSprites(viewProjection);
//    }
//#else
//    directionSprite.draw();
//#endif
    
    // render objects here
    if (this->gameState==GAME_PLAYER_PLACE_STRICKER) {
        this->playerStricker.DrawPreHelperSprites(mvp);
    }
    playerStricker.draw(mvp);
    if (this->gameState==GAME_PLAYER_PLACE_STRICKER) {
        this->playerStricker.DrawPostHelperSprites(mvp);
    }
    
    ground.draw(mvp);
    leftWall.draw(mvp);
    rightWall.draw(mvp);
    topWall.draw(mvp);
    
    for(auto c : coins) {
        c.second->draw(mvp);
    }
    
    //holes
    holes[0].draw(mvp);
    holes[1].draw(mvp);
    holes[2].draw(mvp);
    holes[3].draw(mvp);
}

void Board::TryStartGame() {
    SetGameState(GAME_START);
}

void Board::TryTurnPlayer(bool force/* = false*/) {
    if (this->IsMyTurn() && !force) {
        DEBUG_PRINT("TryTurnPlayer failed. Already players turn.");
        return;
    }
    
    this->gameTurn = TURN_PLAYER;
    OnPlayerTurn();
}

void Board::TryTurnOpponent(bool force/* = false*/) {
    if (!this->IsMyTurn() && !force) {
        DEBUG_PRINT("TryTurnOpponent failed. Already opponents turn.");
        return;
    }
    
    this->gameTurn = TURN_OPPONENT;
    OnOpponentTurn();
}

void Board::OnPlayerTurn() {
    this->SetGameState(GAME_PLAYER_PLACE_STRICKER);
}

void Board::OnOpponentTurn() {
    this->SetGameState(GAME_PLAYER_PLACE_STRICKER);
}

void Board::SetGameState(GAME_STATE state) {
    if (this->gameState!=state) {
        auto prevState = this->gameState;
        this->gameState = state;
        OnGameStateChange(prevState);
    }
}

void Board::OnGameStateChange(GAME_STATE from) {
    switch (this->gameState) {
        case GAME_INIT: {
            OnGameInit();
        }
            break;
        case GAME_START: {
            OnGameStart();
        }
            break;
            
        case GAME_PLAYER_PLACE_STRICKER: {
            OnGamePlayerPlaceStricker();
        }
            break;
        case GAME_PLAYER_PLACE_AIM: {
            OnGamePlayerPlaceAim();
        }
            break;
        case GAME_PLAYER_FIRE: {
            OnGamePlayerFire();
        }
            break;
        case GAME_RESET: {
            OnGameReset();
        }
            break;
        default:
            break;
    }
}

void Board::OnGameInit() {
    vector2x leftOriginPosx=-(BOARD_SIZEx*FTOX(0.5f));
    
    // init scene
    physicsSolver.InitSolver(this);

    ground.InitWall(vector2x(BOARD_SIZEx.x, BOTTOM_TOP_WALL_HEIGHTx), leftOriginPosx+(- vector2x(0, ITOX(25))), this->soundEnginePtr);
    leftWall.InitWall(vector2x(LEFT_RIGHT_WALL_WIDTHx, BOARD_SIZEx.y), leftOriginPosx+vector2x(-ITOX(25), 0), this->soundEnginePtr);
    rightWall.InitWall(vector2x(LEFT_RIGHT_WALL_WIDTHx, BOARD_SIZEx.y), leftOriginPosx+vector2x(BOARD_SIZEx.x-LEFT_RIGHT_WALL_WIDTHx+ITOX(25), 0), this->soundEnginePtr);
    topWall.InitWall(vector2x(BOARD_SIZEx.x, BOTTOM_TOP_WALL_HEIGHTx), leftOriginPosx+vector2x(0, BOARD_SIZEx.y-BOTTOM_TOP_WALL_HEIGHTx+ITOX(25)), this->soundEnginePtr);
    
    
    holes[0].initHole(ITOX(24), leftOriginPosx + vector2x(ITOX(36), ITOX(36)));
    holes[1].initHole(ITOX(24), leftOriginPosx + vector2x(ITOX(36), BOARD_SIZEx.y-ITOX(36)));
    holes[2].initHole(ITOX(24), leftOriginPosx + vector2x(BOARD_SIZEx.x-ITOX(36), ITOX(36)));
    holes[3].initHole(ITOX(24), leftOriginPosx + vector2x(BOARD_SIZEx.x-ITOX(36), BOARD_SIZEx.y-ITOX(36)));
    
    physicsSolver.AddRigidBody(&playerStricker);
    
    ResetCoins();
    
    ground.SetWallType(Wall::BOTTOM);
    leftWall.SetWallType(Wall::LEFT);
    rightWall.SetWallType(Wall::RIGHT);
    topWall.SetWallType(Wall::TOP);
    
    physicsSolver.AddBoxCollider(&ground);
    physicsSolver.AddBoxCollider(&leftWall);
    physicsSolver.AddBoxCollider(&rightWall);
    physicsSolver.AddBoxCollider(&topWall);
}

void Board::ResetCoins() {
    // add coins
    vector2x center;
    int ballId=0;
    
    const intx COIN_FRICTION_FACTOR = FTOX(0.05f);
    const intx COIN_MASS = FTOX(0.3f);
    const intx COIN_SIZE = FTOX(15);
    intx r2 = MULTX(COIN_SIZE, FTOX(2.05f));
    intx restituition = FTOX(0.09f);
    
    // queen coin
    Ball* queenBall = new Ball();
    queenBall->initBall(COIN_SIZE, COIN_MASS, COIN_FRICTION_FACTOR, center, &this->queenSprite, this->soundEnginePtr);
    queenBall->SetColor(0.75f, 0, 0.2f);
    queenBall->SetTag("Queen");
    queenBall->SetRestituition(restituition);
    this->coins[ballId++] = queenBall;
    physicsSolver.AddRigidBody(queenBall);
    
    int startAngle = -30;
    // inner circle
    for (int x=0;x<6;x++) {
        Ball* newBall = new Ball();
        vector2x newPos;
        newPos.x = MULTX(r2, pxMath::COSX(startAngle+x*60));
        newPos.y = MULTX(r2, pxMath::SINX(startAngle+x*60));
        if (x%2==1) {
            newBall->initBall(COIN_SIZE, COIN_MASS, COIN_FRICTION_FACTOR, center+newPos, &this->blackCoinSprite, this->soundEnginePtr);
            newBall->SetColor(0.15f, 0.15f, 0.15f);
        } else {
            newBall->initBall(COIN_SIZE, COIN_MASS, COIN_FRICTION_FACTOR, center+newPos, &this->whiteCoinSprite, this->soundEnginePtr);
            newBall->SetColor(0.7f, 0.7f, 0.7f);
        }
        newBall->SetTag("Coin");
        newBall->SetRestituition(restituition);
        this->coins[ballId++] = newBall;
        physicsSolver.AddRigidBody(newBall);
    }
    
    // outer circle
    intx r3 = MULTX(COIN_SIZE, FTOX(4.1f));
    intx r32 = MULTX(COIN_SIZE, FTOX(3.51f));
    for (int x=0;x<12;x++) {
        Ball* newBall = new Ball();
        vector2x newPos;
        intx r = (x%2==0)?r3:r32;
        newPos.x = MULTX(r, pxMath::COSX(startAngle+x*30));
        newPos.y = MULTX(r, pxMath::SINX(startAngle+x*30));
        if (x%2==0) {
            newBall->initBall(COIN_SIZE, COIN_MASS, COIN_FRICTION_FACTOR, center+newPos, &this->blackCoinSprite, this->soundEnginePtr);
            newBall->SetColor(0.15f, 0.15f, 0.15f);
        } else {
            newBall->initBall(COIN_SIZE, COIN_MASS, COIN_FRICTION_FACTOR, center+newPos, &this->whiteCoinSprite, this->soundEnginePtr);
            newBall->SetColor(0.7f, 0.7f, 0.7f);
        }
        newBall->SetTag("Coin");
        newBall->SetRestituition(restituition);
        this->coins[ballId++] = newBall;
        physicsSolver.AddRigidBody(newBall);
    }
}

void Board::OnGameStart() {
    switch (this->playerType) {
        case PLAYER_FIRST: {
            this->boardMatrix.setPosition(BOARD_OFFSET.x, BOARD_OFFSET.y, 0);
            this->boardMatrixInv = this->boardMatrix.getInverse();
            this->TryTurnPlayer(true);
        }
        break;
            
        case PLAYER_SECOND: {
            this->boardMatrix.setRotationMatrix(180, 0, 0, true);
            this->boardMatrix.setPosition(BOARD_OFFSET.x, BOARD_OFFSET.y, 0);
            this->boardMatrixInv = this->boardMatrix.getInverse();
            this->TryTurnOpponent(true);
        }
        break;
        default: {
            this->boardMatrix.setPosition(BOARD_OFFSET.x, BOARD_OFFSET.y, 0);
            this->boardMatrixInv = this->boardMatrix.getInverse();
            this->TryTurnPlayer(true);
        }
            break;
    }
}


void Board::OnGamePlayerPlaceStricker() {
    this->playerStricker.Cmd_PlaceStricker();
}

void Board::OnGamePlayerPlaceAim() {
    
}

void Board::OnGamePlayerFire() {
    this->elapsedTimeSinceFire = 0;
}

void Board::OnGameReset() {
}

void Board::MouseBtnUp(const vector2x& pos) {
    if (!(this->gameState >= GAME_PLAYER_PLACE_STRICKER && this->gameState < GAME_RESET)) {
        return;
    }
    
    auto transformedPos = this->boardMatrixInv*vector2f(XTOF(pos.x), XTOF(pos.y));
    this->playerStricker.Cmd_TryShoot(transformedPos, [this](){
        this->SetGameState(GAME_PLAYER_FIRE);
    });
}

void Board::MouseBtnDown(const vector2x& pos) {
    if (!(this->gameState >= GAME_PLAYER_PLACE_STRICKER && this->gameState < GAME_RESET)) {
        return;
    }
    
    auto transformedPos = this->boardMatrixInv*vector2f(XTOF(pos.x), XTOF(pos.y));
    this->playerStricker.Cmd_TryGrab(transformedPos);
}

void Board::MouseMove(const vector2x& pos) {
    if (!(this->gameState >= GAME_PLAYER_PLACE_STRICKER && this->gameState < GAME_RESET)) {
        return;
    }
    auto transformedPos = this->boardMatrixInv*vector2f(XTOF(pos.x), XTOF(pos.y));
    this->playerStricker.Cmd_TryMove(transformedPos);
}
