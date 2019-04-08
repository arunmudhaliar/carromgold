#pragma once
//
//  rigidBody.h
//  Copyright 2018 GDCPhysics
//  GDCPhysics
//
//  Created by Arun Mudaliar.
//
//

#include "core/transformx.h"
#include "core/vector2x.h"
#include <vector>
#include <string>

#define SEGMENTS_DEBUG 36
class Collider {
public:
    Collider(bool isRigidBody = false);
    virtual ~Collider();
    bool IsColliderActive() { return this->colliderActive; }
    void SetColliderActive(bool flag) { this->colliderActive = flag; }
    bool IsRigidBody() { return this->rigidBody; }
    
protected:
    bool colliderActive;
    bool rigidBody;
};

class RigidBody : public Collider {
public:
    RigidBody();
    virtual ~RigidBody();
    
    void SetMass(intx mass);
    void SetFrictionFactor(intx factor);
    void SetRadius(intx radius);
    void AddForce(vector2x force);
    void ClearForce();
    void SimulateStep(intx fixedDT, vector2x& displacement, vector2x& velocity);

    void SetRBPosition(const vector2x pos, bool updateTransformImmediate = false);
    void SetRBVelocity(const vector2x& velocity);
    void AddRBVelocity(const vector2x& velocity);
    void AddRBAccumulatedVelocity(const vector2x& velocity);
    void ApplyRBAccumulatedVelocity();
    
    float GetKineticEnergy();
    inline vector2x& GetRBPosition()   { return this->position; }
    inline vector2x& GetRBVelocity()   { return this->velocity; }
    inline intx GetRBMass() { return this->mass; }
    inline intx GetRadius() { return this->radius; }
    inline intx GetRadiusSq() { return this->radiusSq; }
    inline vector2x& GetRBForce()   { return this->force; }
    inline intx GetFrictionFactor() { return this->frictionFactor; }
    inline intx GetRestitution() { return this->restitution; }
    
    inline void SetTag(const std::string& tag) { this->tag = tag; }
    inline const std::string& GetTag() { return this->tag; }
    
    void setDebugPos(vector2x p) { debugPos = p; }
    void TriggerCollisionEvent(std::vector<Collider*>& colliders) { OnCollision(colliders); }
    
    void DrawDebugPos();
    
    void SetStateInActiveCollision(bool flag) { this->isInActiveCollision = flag; }
    bool IsStateInActiveCollision() { return this->isInActiveCollision; }
    void SetRestituition(intx e) { this->restitution = e; }
    
    void PhysicsUpdate();
    
protected:
    vector2x force;
    vector2x velocity;
    intx mass;
    intx invMass;
    vector2x position;
    intx radius;
    intx radiusSq;
    intx frictionFactor;
    std::string tag;
    vector2x debugPos;
    vector2x accumulatedVel;
    intx restitution;
    virtual void UpdatePositionFromRB(const vector2x& displacement) = 0;
    virtual void SetPositionFromRB(const vector2x& pos) = 0;
    virtual void OnCollision(std::vector<Collider*>& colliders) = 0;
    virtual void OnPhysicsUpdate() = 0;
    virtual void OnStartedMoving() = 0;
    virtual void OnCameToHalt() = 0;
    
    float vertexBufferDebug[(SEGMENTS_DEBUG+2)*2];
    bool isInActiveCollision;
    bool isMoving;
};
