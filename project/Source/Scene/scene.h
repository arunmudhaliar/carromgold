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
#include "../Connection/NetworkManager.hpp"
#include <SDL2/SDL.h>
#endif

class Scene
#if ENABLE_MULTIPLAYER
: protected NetworkManagerDelegate
#endif
{
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
    void Resize(float cx, float cy);
    void Update();
    void Render();
    
    void StartGameFromMenu();
    void MouseBtnDown(int mx, int my);
    void MouseBtnUp(int mx, int my);
    void MouseMove(int mx, int my);
    
    vector2x WindowToBoard(const vector2x& windowCoord);
    
protected:
    vector2i windowSize;
    Board board;
    
#if ENABLE_MULTIPLAYER
    // NetworkManagerDelegate
    void OnNetworkMessage(const std::string& msg) override;
    void OnNetworkFail() override;
    void OnNetworkOpen() override;
    void OnNetworkClose() override;
#endif
    
    void DrawStats();
    
    void SendPing();
    
private:
    
    void InternalGLStates();

#if GEAR_APPLE
    SDL_Window* window;
#endif

    std::string player1Score;
    std::string player2Score;
    std::string statusMsg;

    CGETextureManager textureManager;
    SoundEngine soundEngine;
    
    SoundSource* sfx_breakShot;
    
    //renderer
    matrix4x4f worldScale;
    gxRenderer renderer;
    matrix4x4f projectionMatrix;
    matrix4x4f inverseTransformationMatrix;
    matrix4x4f viewProjectionMatrix;
    
    gxButton debugButton1;
    gxButton debugButton2;
    gxButton debugButton3;
    
    std::vector<gxButton*> debugBtnList;
};
