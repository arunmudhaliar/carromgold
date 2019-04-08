//
//  boxCollider.cpp
//  GDCPhysics
//
//  Created by Arun A on 31/10/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#include "boxCollider.hpp"
#if GEAR_APPLE
    #include <OpenGL/gl.h>
#elif GEAR_ANDROID
    #include <GLES2/gl2.h>
#else
    #include <gl.h>
#endif
#include <string.h>

BoxCollider::BoxCollider() :
Collider(),
RenderObject() {
    memset(vertexBuffer, 0 , sizeof(vertexBuffer));
}

BoxCollider::~BoxCollider() {
}

void BoxCollider::InitBoxCollider(const vector2x& size, const vector2x& pos) {
    this->size = size;
    this->position = pos;
    
    // bottom-left
    vertexBuffer[0] = 0;
    vertexBuffer[1] = 0;
    
    // bottom-right
    vertexBuffer[2] = XTOF(size.x);
    vertexBuffer[3] = 0;
    
    // top-right
    vertexBuffer[4] = XTOF(size.x);
    vertexBuffer[5] = XTOF(size.y);
    
    // top-left
    vertexBuffer[6] = 0;
    vertexBuffer[7] = XTOF(size.y);
}

void BoxCollider::OnRender() {
#if !USE_ProgrammablePipeLine
    glPushMatrix();
    glTranslatef(XTOF(this->position.x), XTOF(this->position.y), 0);
    glColor3f(0.6f, 0.6f, 0.6f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertexBuffer);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
#endif
}
