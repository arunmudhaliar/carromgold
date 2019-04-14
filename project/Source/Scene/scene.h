#pragma once
//
//  scene.h
//  Copyright 2018 GDCPhysics
//  GDCPhysics
//
//  Created by Arun Mudaliar.
//
//

#include "ball.h"
#include "stricker.hpp"
#include "wall.hpp"
#include "hole.h"
#include "board.hpp"
#include "../Physics/colliders/boxCollider.hpp"
#include "../GUI/geFontManager.h"
#include "../GUI/Sprite/Sprite2Dx.h"
#include "../GUI/hwShader/HWShaderManager.h"
#include "gxRenderer.h"
#include "../audio/SoundEngine.h"
#include "../GUI/gxButton.hpp"
#include <map>
#if GEAR_APPLE
#include <SDL2/SDL.h>
#endif

#if ENABLE_MULTIPLAYER
#include "../Connection/NetworkManager.hpp"
#endif

#define SHOW_DEBUG_PANEL 0

class Scene : protected MStrickerObserver
#if ENABLE_MULTIPLAYER
, protected NetworkManagerDelegate
#endif
, protected MBoardObserver {
private:
    Scene();
    
public:
    
    enum MOUSE_EVENT {
        MOUSE_UP,
        MOUSE_DOWN,
        MOUSE_MOVE,
        MOUSE_MAX_EVENT
    };
    
    static Scene& GetInstance();
    
    ~Scene();
#if GEAR_APPLE
    void InitScene(SDL_Window* window, float cx, float cy);
#else
    void InitScene(float cx, float cy);
#endif
    void StartGameFromMenu();
    
    void Resize(float cx, float cy);
    void Update();
    void Render();
    
    // input
    void MouseBtnDown(int mx, int my);
    void MouseBtnUp(int mx, int my);
    void MouseMove(int mx, int my);
    
    // util
    vector2x WindowToBoard(const vector2x& windowCoord);
    
protected:
    
#if ENABLE_MULTIPLAYER
    // NetworkManagerDelegate
    void OnNetworkMessage(const std::string& msg) override;
    void OnNetworkFail() override;
    void OnNetworkOpen() override;
    void OnNetworkClose() override;
#endif
    
    // Stricker observer
    void OnStricker_StateChangeTo_Grab(Stricker*) override;
    void OnStricker_StateChangeTo_Aim(Stricker*) override;
    void OnStricker_StateChangeTo_Move(Stricker*) override;
    void OnStricker_StateChangeTo_Shoot(Stricker*) override;
    void OnStricker_Move(Stricker*) override;
    void OnStricker_Aim(Stricker*) override;
    void OnStricker_StateChangeTo_PlaceStricker(Stricker*) override;
    
    // Board observer
    void OnFinishTurn() override;
    
    void DrawStats();
    void SendPing();
    
private:
    
    void InternalGLStates();
    bool CheckShas();
    const std::string GetCurrentSha();
#if ENABLE_MULTIPLAYER
    void SendSha();
#endif
    
    // window
    vector2i windowSize;
#if GEAR_APPLE
    SDL_Window* window;
#endif

    // global
    CGETextureManager textureManager;
    SoundEngine soundEngine;
    
    // game play
    Board board;
    std::string player1Score;
    std::string player2Score;
    std::string statusMsg;

    // debug
    gxButton debugButton1;
    gxButton debugButton2;
    gxButton debugButton3;
    gxButton debugButton4;  //sha check
    
    std::vector<gxButton*> debugBtnList;
    std::string debugShootValSend;
    std::string debugShootValReceived;

    //renderer
    matrix4x4f worldScale;
    gxRenderer renderer;
    matrix4x4f projectionMatrix;
    matrix4x4f inverseTransformationMatrix;
    matrix4x4f viewProjectionMatrix;
    
    // checksum
    std::vector<std::string> sha256Array;
    std::string sha256Str;
    std::string sha256Source;
    
    // incoming
    std::vector<std::string> sha256Array_incoming;
    std::string sha256Str_incoming;
};
