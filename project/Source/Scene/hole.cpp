//
//  hole.cpp
//  Copyright 2018 GDCPhysics
//  GDCPhysics
//
//  Created by Arun Mudaliar.
//
//

#include "hole.h"
#include "../Physics/core/util.h"

Hole::Hole() {
    this->size = 0;
    memset(vertexBuffer, 0 , sizeof(vertexBuffer));
    SetColor(0.75f, 0, 0.2f);
}

Hole::~Hole() {
}

void Hole::SetColor(float r, float g, float b, float a) {
    this->color[0] = r;
    this->color[1] = g;
    this->color[2] = b;
    this->color[3] = a;
}

void Hole::initHole(float size, const vector2x& pos) {
    this->size = size;
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
    setPositionx(pos);
}

void Hole::OnRender(const matrix4x4f& viewProjection) {
#if !USE_ProgrammablePipeLine
    auto p = getOpenGLMatrix();
    glPushMatrix();
    glTranslatef(XTOF(p[12]), XTOF(p[13]), 0);
    glColor3fv(this->color);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertexBuffer);
    glDrawArrays(GL_TRIANGLE_FAN, 0, SEGMENTS+2);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
#endif
}

bool Hole::IsOverlap(vector2x& pos, const intx radius, intx penitration) {
    vector2x v = pos - getPosition2x();
//    intx bothradius = radius + FTOX(this->size);
    intx distance = v.lengthx();
    if (distance < FTOX(this->size)) {
        intx ratio = DIVX(distance, FTOX(this->size));
        if (ratio < penitration) {
            return true;
        }
    }
    
    return false;
}

