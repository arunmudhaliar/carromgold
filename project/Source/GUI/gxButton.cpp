//
//  gxButton.cpp
//  GDCPhysics
//
//  Created by Arun A on 09/04/19.
//  Copyright © 2019 Arun A. All rights reserved.
//

#include "gxButton.hpp"
#include "hwShader/HWShaderManager.h"

void gxButton::initButton(const vector2f& pos, const vector2f& size, const std::string& name, std::function<void()> callback) {
    this->position = pos;
    this->size = size;
    this->button_click_callback = callback;
    this->isPressed = false;
    
    float hx = size.x*0.5f;
    float hy = size.y*0.5f;
    
    vertArray[0] = hx; vertArray[1] = -hy;
    vertArray[2] = hx; vertArray[3] = hy;
    vertArray[4] = -hx; vertArray[5] = -hy;
    vertArray[6] = -hx; vertArray[7] = hy;
    
    float rgba[4] = {0.1f, 0.1f, 0.1f, 0.3f};
    for (int x=0;x<4;x++) {
        colorArray[x*4+0] = rgba[0];
        colorArray[x*4+1] = rgba[1];
        colorArray[x*4+2] = rgba[2];
        colorArray[x*4+3] = rgba[3];
    }
}

void gxButton::drawButton(const matrix4x4f& viewProjection) {
#if USE_ProgrammablePipeLine
    auto shader = HWShaderManager::GetHWShaderManager().GetHWShader(1);
    shader->enableProgram();
    matrix4x4f dummyTM;
    dummyTM.setPosition(position.x, position.y, 0);
    if (isPressed) {
        dummyTM.setScale(1.2f, 1.2f, 1.2f);
    }
    const float* u_mvp_m4x4= (viewProjection * dummyTM).getMatrix();
    shader->sendUniformTMfv("u_mvp_m4x4", u_mvp_m4x4, false, 4);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glVertexAttribPointer(shader->getAttribLoc("a_vertex_coord_v4"), 2, GL_FLOAT, GL_FALSE, 0, this->vertArray);
    glEnableVertexAttribArray(shader->getAttribLoc("a_vertex_coord_v4"));
    
    glVertexAttribPointer(shader->getAttribLoc("a_color_coord_v4"), 4, GL_FLOAT, GL_FALSE, 0, this->colorArray);
    glEnableVertexAttribArray(shader->getAttribLoc("a_color_coord_v4"));
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glDisableVertexAttribArray(shader->getAttribLoc("a_color_coord_v4"));
    glDisableVertexAttribArray(shader->getAttribLoc("a_vertex_coord_v4"));
    
    glDisable(GL_BLEND);
    shader->disableProgram();
#endif
}

bool gxButton::isOverlap(const vector2f& pos) {
    float hx = this->size.x*0.5f;
    float hy = this->size.y*0.5f;
    
    if (this->position.x-hx < pos.x && this->position.x+hx > pos.x) {
        if (this->position.y-hy < pos.y && this->position.y+hy > pos.y) {
            return true;
        }
    }
    
    return false;
}

bool gxButton::mouseDown(vector2f pos) {
    isPressed = isOverlap(pos);
    return isPressed;
}

bool gxButton::mouseUp(vector2f pos) {
    bool consumed = false;
    if (isPressed && isOverlap(pos)) {
        button_click_callback();
        consumed = true;
    }
    isPressed = false;
    
    return consumed;
}

void gxButton::mouseMove(vector2f pos) {
    
}
