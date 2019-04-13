//
//  solver.hpp
//  GDCPhysics
//
//  Created by Arun A on 31/10/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#ifndef solver_hpp
#define solver_hpp

#include <vector>

#include "rigidBody.h"
#include "colliders/boxCollider.hpp"

class FixedUpdateObserver {
public:
    virtual void OnFixedUpdate(intx fixedDT) = 0;
};

class Solver {
public:
    Solver();
    ~Solver();
    
    void InitSolver(FixedUpdateObserver* fixedDTObserver);
    void AddRigidBody(RigidBody* rb);
    void AddBoxCollider(BoxCollider* collider);
    void RemoveRigidBody(RigidBody* rb);
    void RemoveBoxCollider(BoxCollider* collider);
    void UpdateSolver();
    
    unsigned long GetElapsedTime()  { return elapsedTime; }
    bool IsAllRigidBodiesStopped();
    
    void ResetTimeVals();
    
private:
    std::vector<RigidBody*> rigidBodies;
    std::vector<BoxCollider*> boxColliders;
    
    void UpdatePhysics(__int64_t step, intx fixedDT);
    void CheckCollisions(RigidBody* rb, vector2x& newPos, intx radiusSq,
                         bool& collisionHappened, vector2x& contactNormal, std::vector<Collider*>& colliders,
                         bool& collidedWithBoxColliders, bool& collidedWithRB);
    
    // timer
    long elapsedTime;
    long currentTime;
    long accumulator;
    __int64_t simSteps;
    
    FixedUpdateObserver* fixedDTObserver;
};
#endif /* solver_hpp */
