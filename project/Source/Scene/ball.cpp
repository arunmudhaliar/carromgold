//
//  ball.cpp
//  Copyright 2018 GDCPhysics
//  GDCPhysics
//
//  Created by Arun Mudaliar.
//
//

#include "ball.h"
#include "../Physics/core/util.h"
#if GEAR_APPLE
#include "../Connection/NetworkManager.hpp"
#endif
#include "stricker.hpp"

float Ball::maximumVelocity = 0.0f;

Ball::Ball() {
    this->size = 0;
    memset(vertexBuffer, 0 , sizeof(vertexBuffer));
    SetColor(0.75f, 0, 0.2f);
    this->sprite = nullptr;
    this->shadowSprite = nullptr;
    this->soundEnginePtr = nullptr;
    this->sfxDragInstance = nullptr;
}

Ball::~Ball() {
    this->sfxDragInstance->stop();
}

void Ball::SetColor(float r, float g, float b, float a) {
    this->color[0] = r;
    this->color[1] = g;
    this->color[2] = b;
    this->color[3] = a;
}

void Ball::initBall(intx size, intx mass, intx frictionFactor, const vector2x& pos, Sprite2Dx* sprite, Sprite2Dx* shadowSprite, SoundEngine* soundEnginePtr) {
    this->soundEnginePtr = soundEnginePtr;
    this->size = XTOF(size);
    this->sprite = sprite;
    this->shadowSprite = shadowSprite;
    SetRadius(size);
    SetMass(mass);
    SetFrictionFactor(frictionFactor);
    int delta = (360/SEGMENTS);
    vertexBuffer[0]=vertexBuffer[1] = 0.0f;
    for(int xx=0; xx<SEGMENTS; xx++) {
        float cs=size*cos(DEG2RAD((float)xx*delta));
        float sn=size*sin(DEG2RAD((float)xx*delta));
        vertexBuffer[(xx+1)*2+0]=cs;
        vertexBuffer[(xx+1)*2+1]=sn;
    }
    vertexBuffer[SEGMENTS*2+0]=vertexBuffer[(1)*2+0];
    vertexBuffer[SEGMENTS*2+1]=vertexBuffer[(1)*2+1];
    SetRBPosition(pos, true);
    
    auto sfxDrag = this->soundEnginePtr->getSource(SFX_SOUND::sfx_striker_drag_LOOP);
    this->sfxDragInstance = this->soundEnginePtr->createInstanceOf(sfxDrag);
    
    OnPostInitBall();
}

void Ball::OnPostInitBall() {
}

void Ball::OnRender(const matrix4x4f& viewProjection) {
//    float currentVel = XTOF(GetRBVelocity().lengthx());
//    if (currentVel>800) {
//        Ball::maximumVelocity = currentVel;
//        DEBUG_PRINT("Max Velocity Changed %f", currentVel);
//    }
    auto p = getOpenGLMatrix();
#if !USE_ProgrammablePipeLine
    glPushMatrix();
    glTranslatef(XTOF(p[12]), XTOF(p[13]), XTOF(p[14]));
    glColor3fv(this->color);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertexBuffer);
    glDrawArrays(GL_TRIANGLE_FAN, 0, SEGMENTS+2);
    glDisableClientState(GL_VERTEX_ARRAY);
#endif
    
#if USE_ProgrammablePipeLine
    auto shader = HWShaderManager::GetHWShaderManager().GetHWShader(2);
    shader->enableProgram();
    if (this->shadowSprite) {
        this->shadowSprite->setPositionx(p[12], p[13], p[14]);
        this->shadowSprite->draw(shader, viewProjection);
    }
    this->sprite->setPositionx(p[12], p[13], p[14]);
    this->sprite->draw(shader, viewProjection);
    shader->disableProgram();
#endif
    
#if !USE_ProgrammablePipeLine
    glPopMatrix();
    
    if (IsStateInActiveCollision()) {
        DrawDebugPos();
    }
#endif
}

void Ball::UpdatePositionFromRB(const vector2x& displacement) {
    updatePositionx(displacement.x, displacement.y, 0);
}

void Ball::SetPositionFromRB(const vector2x& pos) {
    setPositionx(pos);
}

void Ball::OnCollision(std::vector<Collider*>& colliders) {
    if (Ball::IsCoin(*this)  && colliders.size()) {
        int sfxID = getRandom(SFX_SOUND::sfx_puck_hits_puck_1, SFX_SOUND::sfx_puck_hits_puck_5+1);
        auto sfxSrc = this->soundEnginePtr->getSource(sfxID);
        
        float currentVel = XTOF(GetRBVelocity().lengthx());
        float ratio = currentVel/MAX_COIN_VELOCITY;
        ratio = MIN(ratio, 1.0f);
        if (ratio>0.02f) {
            sfxSrc->setVolume(ratio*0.45f);
            sfxSrc->play(false);
        }
    }
}

void Ball::OnPhysicsUpdate() {
    if (Ball::IsCoin(*this)) {
        if (this->isMoving) {
            float currentVel = XTOF(GetRBVelocity().lengthx());
            float ratio = currentVel/MAX_COIN_VELOCITY;
            ratio = MIN(ratio, 1.0f);
            this->sfxDragInstance->setVolume(ratio*0.75f);
        }
    }
}

void Ball::OnStartedMoving() {
    if (Ball::IsCoin(*this)) {
        float currentVel = XTOF(GetRBVelocity().lengthx());
        float ratio = currentVel/MAX_COIN_VELOCITY;
        ratio = MIN(ratio, 1.0f);
        
        if (ratio>0.02f) {
            //sfx_striker_drag_LOOP
            this->sfxDragInstance->setVolume(ratio*0.75f);
            this->sfxDragInstance->play();
        }
    }
}

void Ball::OnCameToHalt() {
    if (Ball::IsCoin(*this)) {
        this->sfxDragInstance->stop();
    }
}


bool Ball::IsCoin(const Ball& ball) {
    auto tag = ball.GetTag();
    return tag == "CB" || tag == "CW" || tag == "CQ";
}

bool Ball::IsQueen(const Ball& ball) {
    auto tag = ball.GetTag();
    return tag == "CQ";
}

bool Ball::IsBlackCoin(const Ball& ball) {
    auto tag = ball.GetTag();
    return tag == "CB";
}

bool Ball::IsWhiteCoin(const Ball& ball) {
    auto tag = ball.GetTag();
    return tag == "CW";
}

bool Ball::IsStricker(const Ball& ball) {
    auto tag = ball.GetTag();
    return tag == "ST";
}
