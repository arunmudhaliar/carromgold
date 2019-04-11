#pragma once
//
//  hole.h
//  Copyright 2018 GDCPhysics
//  GDCPhysics
//
//  Created by Arun Mudaliar.
//
//

#include "../Physics/rigidBody.h"
#include "../Physics/renderObject.h"
#include "../GUI/Sprite/Sprite2Dx.h"

#define SEGMENTS    36

class Hole : public RenderObject {
public:
    Hole();
    virtual ~Hole();
    
    void initHole(intx size, const vector2x& pos);
    void SetColor(float r, float g, float b, float a=1.0f);
    
    bool IsOverlap(vector2x& pos, const intx radius, intx penitration=FTOX(0.75f));
protected:
    float vertexBuffer[(SEGMENTS+2)*2];
    intx size;
    virtual void OnRender(const matrix4x4f& viewProjection);
    
    float color[4];
};
