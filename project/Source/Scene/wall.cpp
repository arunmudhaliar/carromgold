//
//  wall.cpp
//  GDCPhysics
//
//  Created by Arun A on 03/11/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#include "wall.hpp"
#include "../Physics/core/Timer.h"
#include "ball.h"
#include "scene.h"

Wall::Wall() : BoxCollider() {
    this->soundEnginePtr = nullptr;
    float size = 5.0f;
    int delta = (360/SEGMENTS_DEBUG);
    vertexBufferDebug[0]=vertexBuffer[1] = 0.0f;
    for(int xx=0; xx<SEGMENTS_DEBUG; xx++) {
        float cs=size*cos(DEG2RAD((float)xx*delta));
        float sn=size*sin(DEG2RAD((float)xx*delta));
        vertexBufferDebug[(xx+1)*2+0]=cs;
        vertexBufferDebug[(xx+1)*2+1]=sn;
    }
    vertexBufferDebug[SEGMENTS_DEBUG*2+0]=vertexBuffer[(1)*2+0];
    vertexBufferDebug[SEGMENTS_DEBUG*2+1]=vertexBuffer[(1)*2+1];
}

Wall::~Wall() {
}

void Wall::InitWall(const vector2x& size, const vector2x& pos, SoundEngine* soundEnginePtr) {
    BoxCollider::InitBoxCollider(size, pos);
    this->soundEnginePtr = soundEnginePtr;
}

void Wall::OnCollidedWithRB(RigidBody* rb, const vector2x& contactPt, const vector2x& contactNormal) {
    float currentVel = XTOF(rb->GetRBVelocity().lengthx());
    float ratio = currentVel/MAX_COIN_VELOCITY;
    ratio = MIN(ratio, 1.0f);
    
    Ball* coin = dynamic_cast<Ball*>(rb);
    if (ratio>0.02f && coin) {
        if (Ball::IsCoin(*coin)) {
            int sfxID = getRandom(SFX_SOUND::sfx_puck_hits_edge_1, SFX_SOUND::sfx_puck_hits_edge_5+1);
            auto sfxSrc = this->soundEnginePtr->getSource(sfxID);
            sfxSrc->setVolume(ratio*0.75f);
            sfxSrc->play(false);
        } else if (Ball::IsStricker(*coin)) {
            auto sfxSrc = this->soundEnginePtr->getSource(SFX_SOUND::sfx_puck_hits_edge_3);
            sfxSrc->setVolume(ratio*0.75f);
            sfxSrc->play(false);
        }
    }
}

void Wall::OnRender(const matrix4x4f& viewProjection) {
#if !USE_ProgrammablePipeLine
    glPushMatrix();
    glTranslatef(XTOF(this->position.x), XTOF(this->position.y), 0);
    glColor3f(0.3f, 0.3f, 0.3f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertexBuffer);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
    
    for (int x=0;x<4;x++) {
        glPushMatrix();
        glTranslatef(XTOF(this->debugPos[x].x), XTOF(this->debugPos[x].y), 0);
        glColor3f(1.0f, 0.3f, 0.3f);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, this->vertexBufferDebug);
        glDrawArrays(GL_TRIANGLE_FAN, 0, SEGMENTS_DEBUG+2);
        glDisableClientState(GL_VERTEX_ARRAY);
        glPopMatrix();
    }
#endif
}
