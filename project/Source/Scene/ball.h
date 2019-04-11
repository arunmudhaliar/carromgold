#pragma once
//
//  ball.h
//  Copyright 2018 GDCPhysics
//  GDCPhysics
//
//  Created by Arun Mudaliar.
//
//

#include "../Physics/rigidBody.h"
#include "../Physics/renderObject.h"
#include "../GUI/Sprite/Sprite2Dx.h"
#include "../audio/SoundEngine.h"

#define SEGMENTS    36

class Ball : public RenderObject, public RigidBody {
public:
    Ball();
    virtual ~Ball();
    
    void initBall(intx size, intx mass, intx frictionFactor, const vector2x& pos, Sprite2Dx* sprite, SoundEngine* soundEnginePtr);
    void SetColor(float r, float g, float b, float a=1.0f);
protected:
    float vertexBuffer[(SEGMENTS+2)*2];
    float size;
    virtual void OnRender(const matrix4x4f& viewProjection);
    virtual void OnPostInitBall();
    
    // rb
    virtual void UpdatePositionFromRB(const vector2x& displacement);
    virtual void SetPositionFromRB(const vector2x& pos);
    virtual void OnCollision(std::vector<Collider*>& colliders);
    virtual void OnPhysicsUpdate();
    virtual void OnStartedMoving();
    virtual void OnCameToHalt();

    float color[4];
    Sprite2Dx* sprite;
    
    SoundEngine* soundEnginePtr;
    SoundSource* sfxDragInstance;
    
    static float maximumVelocity;
};
