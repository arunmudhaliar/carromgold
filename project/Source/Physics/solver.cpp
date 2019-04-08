//
//  solver.cpp
//  GDCPhysics
//
//  Created by Arun A on 31/10/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#include "solver.hpp"
#include "core/util.h"
#include "core/pxMath.h"
#include "core/Timer.h"

#define FIXED_DT_READABLE    16             // 60 fps
#define FIXED_DT_THRESHOLD_READABLE    40   // 25 fps
#define FIXED_DT    FTOX(0.016f)            // 60 fps
#define GRAVITY -ITOX(100)
#define ISGRAVITY   0

Solver::Solver() {
    this->fixedDTObserver = nullptr;
    this->elapsedTime = 0;
    this->currentTime = 0;
    this->accumulator = 0;
    this->simSteps = 0;
}

Solver::~Solver() {
}

void Solver::InitSolver(FixedUpdateObserver* fixedDTObserver) {
    this->fixedDTObserver = fixedDTObserver;
    this->elapsedTime = 0;
    this->currentTime = Timer::getCurrentTimeInMilliSec();
    this->accumulator = 0;
    this->simSteps = 0;
}

void Solver::AddRigidBody(RigidBody* rb) {
    if (std::find(this->rigidBodies.begin(), this->rigidBodies.end(), rb) == this->rigidBodies.end()) {
        this->rigidBodies.push_back(rb);
    }
}

void Solver::AddBoxCollider(BoxCollider* collider) {
    if (std::find(this->boxColliders.begin(), this->boxColliders.end(), collider) == this->boxColliders.end()) {
        this->boxColliders.push_back(collider);
    }
}

void Solver::RemoveRigidBody(RigidBody* rb) {
    this->rigidBodies.erase(std::remove(this->rigidBodies.begin(), this->rigidBodies.end(), rb), this->rigidBodies.end());
}

void Solver::RemoveBoxCollider(BoxCollider* collider) {
    this->boxColliders.erase(std::remove(this->boxColliders.begin(), this->boxColliders.end(), collider), this->boxColliders.end());
}

void Solver::UpdateSolver() {
    unsigned long newTime = Timer::getCurrentTimeInMilliSec();
    unsigned long frameTime = newTime - this->currentTime;
    if ( frameTime > FIXED_DT_THRESHOLD_READABLE ) {
        frameTime = FIXED_DT_THRESHOLD_READABLE;
    }
    this->currentTime = newTime;
    this->accumulator += frameTime;
    
    while ( this->accumulator >= FIXED_DT_READABLE ) {
        this->simSteps++;
        if (this->fixedDTObserver) {
            this->fixedDTObserver->OnFixedUpdate(FIXED_DT);
        }
        UpdatePhysics(this->simSteps, FIXED_DT);
        this->elapsedTime += FIXED_DT_READABLE;
        this->accumulator -= FIXED_DT_READABLE;
    }
}

void Solver::UpdatePhysics(__int64_t step, intx fixedDT) {
    // Euler integrator.
    vector2x outDisplacement;
    vector2x outVelocity;
    std::vector<Collider*> outColliders;
    for (auto rb : this->rigidBodies) {
        rb->SimulateStep(fixedDT, outDisplacement, outVelocity);
        rb->ClearForce();
        auto oldVel = rb->GetRBVelocity();
        auto oldPos = rb->GetRBPosition();
        auto newPos = oldPos+outDisplacement;
        bool collisionHappened = false;
        vector2x contactNormal;
        rb->SetStateInActiveCollision(false);
        CheckCollisions(rb, newPos, rb->GetRadiusSq(), collisionHappened, contactNormal, outColliders);
        if (collisionHappened) {
            rb->SetStateInActiveCollision(true);
            
            const intx degenerationFactor = FTOX(0.05f);  //0.1f
            newPos = newPos + contactNormal*degenerationFactor;
            rb->SetRBPosition(newPos, true);
            
            rb->TriggerCollisionEvent(outColliders);
        } else {
            rb->SetRBVelocity((oldVel+outVelocity)*rb->GetFrictionFactor());
            rb->SetRBPosition(newPos, true);
        }
        
        rb->PhysicsUpdate();
        
#if ISGRAVITY
        intx gravity = MULTX(GRAVITY, rb->GetRBMass());
        rb->AddForce(vector2x(0, gravity));
#endif
    }
}

void CalculateImpulse(RigidBody& A, RigidBody& B, vector2x& Avel, vector2x& Bvel) {
    // Calculate relative velocity
    vector2x rv = B.GetRBVelocity() - A.GetRBVelocity();
    vector2x normal = B.GetRBPosition() - A.GetRBPosition();
    normal.normalizex();
    
    // Calculate relative velocity in terms of the normal direction
    auto velAlongNormal = rv.dotx(normal);
    
    
    // Do not resolve if velocities are separating
    if(velAlongNormal > 0)
        return;
//    velAlongNormal = MAX(velAlongNormal, -ITOX(1000));
//    printf("velAlongNormal %5.2f\n", XTOF(velAlongNormal));
    
    // Calculate restitution
    intx ea = A.GetRestitution();
    intx eb = B.GetRestitution();
    
    // Calculate impulse scalar
    intx ja = MULTX(-(FX_ONE + ea), velAlongNormal);
    intx jb = MULTX(-(FX_ONE + eb), velAlongNormal);
    intx massAinv = DIVX(FX_ONE, A.GetRBMass());
    intx massBinv = DIVX(FX_ONE, B.GetRBMass());
    
    ja = DIVX(ja, massAinv + massBinv);
    jb = DIVX(jb, massAinv + massBinv);
    
    // Apply impulse
    vector2x impulseA = normal * ja;
    vector2x impulseB = normal * jb;
    Avel = -impulseA * massAinv;
    Bvel = impulseB * massBinv;
}

bool Solver::IsAllRigidBodiesStopped() {
    for (auto rb : this->rigidBodies) {
        if (rb->GetRBVelocity().lengthSquaredx()>=FTOX(2.0f)) {
            return false;
        }
    }
    
    return true;
}

void Solver::CheckCollisions(RigidBody* rb, vector2x& newPos, intx radiusSq, bool& collisionHappened, vector2x& contactNormal, std::vector<Collider*>& colliders) {
    collisionHappened = false;
    int collision_check_cntr=5;
    colliders.clear();
    while(collision_check_cntr--) {
        vector2x avgPos;
        vector2x avgNrml;
        int cnt=0;
        
        for(auto boxCollider : this->boxColliders) {
            vector2x bottomLeft(boxCollider->GetBottomLeft());
            vector2x bottomRight(boxCollider->GetBottomRight());
            vector2x topLeft(boxCollider->GetTopLeft());
            vector2x topRight(boxCollider->GetTopRight());

            vector2x borders[] = {
                bottomLeft, bottomRight,
                bottomRight, topRight,
                topRight, topLeft,
                topLeft, bottomLeft
            };
            
            vector2x normals[] = {
                vector2x(0, FX_ONE),
                vector2x(FX_ONE, 0),
                vector2x(0, -FX_ONE),
                vector2x(-FX_ONE, 0)
            };
            
            for (int x = 0; x<4; x++) {
                vector2x i1, i2;
                intx s, t;
                auto velocity = rb->GetRBPosition() - newPos;
                if (velocity.lengthx()>0) {
                    util::twoLineSegmentIntersection(rb->GetRBPosition(), newPos, borders[x*2+0], borders[x*2+1], i1, i2, s, t);
                } else {
                    i2 = util::closestPointOnLine(newPos, borders[x*2+0], borders[x*2+1]);
                }
                
                auto diff = newPos-i2;
                boxCollider->setDebugPos(i2, x);
                if (diff.lengthSquaredx()<=radiusSq) {
                    diff= diff+velocity;
                    diff.normalizex();
                    auto calcPos = i2 + diff*(rb->GetRadius());
                    avgPos+=calcPos;
                    avgNrml+=normals[x];
                    if (std::find(colliders.begin(), colliders.end(), boxCollider)==colliders.end()) {
                        colliders.push_back(boxCollider);
                        auto rbVel = rb->GetRBVelocity();
//                        intx rbVelMag = MULTX(rbVel.lengthx(), FTOX(1.15f));
                        intx rbVelMag = rbVel.lengthx();
                        rbVel.normalizex();
                        
                        //r = 2(n.l)n-l;
                        vector2x refl = normals[x] * MULTX(normals[x].dotx(-rbVel), ITOX(2)) + rbVel;
                        rb->SetRBVelocity((refl)*rbVelMag);
                    }
                    boxCollider->CollidedWithRB(rb, i2, diff);
                    cnt++;
                }
            }
            
//            // Check for penetration
//            // Note: We are not checking penitration for now.
//            bPenitration = false;
//
//            //which one is the closest
//            __int64_t closest_length=GX_MAX_INT;
//            int closest_index=-1;
//
//            for (int l=0; l<4; l++) {
//                vector2x diff(newPos-closestPt[l]);
//                __int64_t length=diff.lengthSquaredx();
//                if(length<closest_length) {
//                    closest_length=length;
//                    closest_index=l;
//
//                    if(bPenitration) {
//                        diff=-diff;
//                    }
//
//                    if(closest_length <= radiusSq) {
//                        // Collision occured
//                        diff.normalizex();
//                        intx radius = (intx)pxMath::SQRT((__int64_t)radiusSq);
//                        auto contactPt = closestPt[closest_index];
//                        vector2x calc_Pos(contactPt + diff*radius);
//                        avgPos+=calc_Pos;
//                        avgNrml+=diff;
//                        cnt++;
//                        colliders.push_back(boxCollider);
//                        boxCollider->CollidedWithRB(rb, contactPt, diff);
//                    }
//                }
//            }//for
//            //~circle-rectangle collision
        }
        
        // all rigid bodies
        vector2x startP = rb->GetRBPosition();
        for (auto orb : this->rigidBodies) {
            if (orb == rb) {
                continue;
            }
            // sweap test
            vector2x closestPt;
            closestPt=(util::closestPointOnLine(orb->GetRBPosition(), startP, newPos));
            auto diff = closestPt-orb->GetRBPosition();
            intx r2 = rb->GetRadius()+orb->GetRadius();
            intx r2sq = MULTX(r2, r2);
            if (diff.lengthSquaredx()<=r2sq) {
                diff.normalizex();
                auto calcPos = orb->GetRBPosition() + diff*(orb->GetRadius()+rb->GetRadius());
                avgPos+=calcPos;
                avgNrml+=diff;
                if (std::find(colliders.begin(), colliders.end(), orb)==colliders.end()) {
                    colliders.push_back(orb);
                    vector2x rbVelToAdd;
                    vector2x orbVelToAdd;
                    CalculateImpulse(*rb, *orb, rbVelToAdd, orbVelToAdd);
                    rb->AddRBVelocity(rbVelToAdd);
                    orb->SetRBVelocity(orbVelToAdd);
                    rb->setDebugPos(closestPt);
                }
                cnt++;
            }
        }
        
        if(cnt) {
            avgPos.x=DIVX(avgPos.x,ITOX(cnt));
            avgPos.y=DIVX(avgPos.y,ITOX(cnt));
            // normali
            avgNrml.normalizex();
            contactNormal = avgNrml;
            newPos=avgPos;
            collisionHappened = true;
        } else {
            break;  //break the loop
        }
    }
}
