#pragma once
//
//  renderObject.h
//  Copyright 2018 GDCPhysics
//  GDCPhysics
//
//  Created by Arun Mudaliar.
//
//

#include "core/transformx.h"
#include "../GUI/hwShader/HWShaderManager.h"
#include "core/matrix4x4f.h"

class RenderObject : public transformx {
public:
    RenderObject() {}
    virtual ~RenderObject() {}
    
    void draw(const matrix4x4f& viewProjection) {
        OnRender(viewProjection);
    }
    
protected:
    virtual void OnRender(const matrix4x4f& viewProjection){}
};
