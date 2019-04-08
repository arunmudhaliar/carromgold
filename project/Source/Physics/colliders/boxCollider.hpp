//
//  boxCollider.hpp
//  GDCPhysics
//
//  Created by Arun A on 31/10/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#ifndef boxCollider_hpp
#define boxCollider_hpp

#include "../core/vector2x.h"
#include "../renderObject.h"
#include "../rigidBody.h"

class RigidBody;
class Collider;
class BoxCollider : public Collider, public RenderObject {
public:
    BoxCollider();
    virtual ~BoxCollider();
    
    void InitBoxCollider(const vector2x& size, const vector2x& pos);
    
    inline vector2x GetBottomLeft()    { return this->position + vector2x(FTOX(vertexBuffer[0]), FTOX(vertexBuffer[1])); }
    inline vector2x GetBottomRight()   { return this->position + vector2x(FTOX(vertexBuffer[2]), FTOX(vertexBuffer[3])); }
    inline vector2x GetTopLeft()       { return this->position + vector2x(FTOX(vertexBuffer[6]), FTOX(vertexBuffer[7])); }
    inline vector2x GetTopRight()      { return this->position + vector2x(FTOX(vertexBuffer[4]), FTOX(vertexBuffer[5])); }
    
    vector2x GetPosition() { return position; }
    void SetPosition(const vector2x& pos)   { position = pos; }
    
    void CollidedWithRB(RigidBody* rb, const vector2x& contactPt, const vector2x& contactNormal) {
        OnCollidedWithRB(rb, contactPt, contactNormal);
    }
    
    void setDebugPos(vector2x p, int index) { debugPos[index] = p; }
    
protected:
    virtual void OnRender();
    
    virtual void OnCollidedWithRB(RigidBody* rb, const vector2x& contactPt, const vector2x& contactNormal) {}
    
    vector2x size;
    vector2x position;
    float vertexBuffer[4*2];
    
    vector2x debugPos[4];
};
#endif /* boxCollider_hpp */
