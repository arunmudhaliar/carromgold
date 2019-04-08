//
//  rigidBody.cpp
//  Copyright 2018 GDCPhysics
//  GDCPhysics
//
//  Created by Arun Mudaliar.
//
//

#include "rigidBody.h"

Collider::Collider(bool isRigidBody) :
rigidBody(isRigidBody) {
    SetColliderActive(true);
}

Collider::~Collider() {
}

RigidBody::RigidBody() : Collider(true) {
    SetMass(FX_ONE);
    SetFrictionFactor(0);
    SetRadius(ITOX(20));
    SetRestituition(FTOX(0.09f));
    
    SetStateInActiveCollision(false);
    this->isMoving = false;
    
    float size = 5.0f;
    int delta = (360/SEGMENTS_DEBUG);
    //    vertexBufferDebug[0] = 0.0f;
    //    for(int xx=0; xx<SEGMENTS_DEBUG; xx++) {
    //        float cs=size*cos(DEG2RAD((float)xx*delta));
    //        float sn=size*sin(DEG2RAD((float)xx*delta));
    //        vertexBufferDebug[(xx+1)*2+0]=cs;
    //        vertexBufferDebug[(xx+1)*2+1]=sn;
    //    }
    //    vertexBufferDebug[SEGMENTS_DEBUG*2+0]=vertexBuffer[(1)*2+0];
    //    vertexBufferDebug[SEGMENTS_DEBUG*2+1]=vertexBuffer[(1)*2+1];
    //
    
    vertexBufferDebug[0]=vertexBufferDebug[1] = 0.0f;
    for(int xx=0; xx<SEGMENTS_DEBUG; xx++) {
        float cs=size*cos(DEG2RAD((float)xx*delta));
        float sn=size*sin(DEG2RAD((float)xx*delta));
        vertexBufferDebug[(xx+1)*2+0]=cs;
        vertexBufferDebug[(xx+1)*2+1]=sn;
    }
    vertexBufferDebug[SEGMENTS_DEBUG*2+0]=vertexBufferDebug[(1)*2+0];
    vertexBufferDebug[SEGMENTS_DEBUG*2+1]=vertexBufferDebug[(1)*2+1];
}

RigidBody::~RigidBody() {
}

void RigidBody::SimulateStep(intx fixedDT, vector2x& displacement, vector2x& vel) {
    vector2x acceleration = this->force * invMass;
    vel = acceleration * fixedDT;     // fixed dt for 45 FPS
    displacement = (vel + this->velocity) * fixedDT;
}

void RigidBody::SetMass(intx mass) {
    this->mass = mass;
    this->invMass = DIVX(FX_ONE, mass);
}

void RigidBody::SetRadius(intx radius) {
    this->radius = radius;
    this->radiusSq = MULTX(radius, radius);
}

void RigidBody::AddForce(vector2x force) {
    this->force += force;
}

void RigidBody::AddRBVelocity(const vector2x& velocity) {
    this->velocity += velocity;
}

void RigidBody::AddRBAccumulatedVelocity(const vector2x& velocity) {
    this->accumulatedVel += velocity;
}

void RigidBody::ApplyRBAccumulatedVelocity() {
    this->velocity += this->accumulatedVel;
    this->accumulatedVel.zerox();
}

void RigidBody::ClearForce() {
    this->force.zerox();
}

void RigidBody::SetRBPosition(const vector2x pos, bool updateTransformImmediate) {
    this->position = pos;
    if (updateTransformImmediate) {
        SetPositionFromRB(this->position);
    }
}

void RigidBody::SetRBVelocity(const vector2x& velocity) {
    this->velocity = velocity;
}

void RigidBody::SetFrictionFactor(intx factor) {
    frictionFactor = FX_ONE - factor;
}

float RigidBody::GetKineticEnergy() {
    // mass in kg, velocity in meters per second, result is joules
    float vel_magf = XTOF(this->velocity.lengthx());
    float massf = XTOF(this->mass);
    return 0.5f * massf * pow(vel_magf, 2.0f);
}

void RigidBody::DrawDebugPos() {
#if !USE_ProgrammablePipeLine
    glPushMatrix();
    glTranslatef(XTOF(this->debugPos.x), XTOF(this->debugPos.y), 0);
    glColor3f(1.0f, 0.3f, 0.3f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, this->vertexBufferDebug);
    glDrawArrays(GL_TRIANGLE_FAN, 0, SEGMENTS_DEBUG+2);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
#endif
}

void RigidBody::PhysicsUpdate() {
    bool isMovingOld = this->isMoving;
    this->isMoving = this->velocity.lengthSquaredx()>0;
    
    if (!isMovingOld && this->isMoving) {
        this->OnStartedMoving();
    } else if (isMovingOld && !this->isMoving) {
        this->OnCameToHalt();
    }
    
    this->OnPhysicsUpdate();
}
