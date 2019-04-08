//
//  gxButton.hpp
//  GDCPhysics
//
//  Created by Arun A on 09/04/19.
//  Copyright © 2019 Arun A. All rights reserved.
//

#ifndef gxButton_hpp
#define gxButton_hpp

#include <stdio.h>
#include <string>
#include <functional>
#include "../Physics/core/vector2.h"
#include "../Physics/core/matrix4x4f.h"

class gxButton {
public:
    void initButton(const vector2f& pos, const vector2f& size, const std::string& name, std::function<void()> callback);
    void drawButton(const matrix4x4f& viewProjection);
    bool mouseDown(vector2f pos);
    bool mouseUp(vector2f pos);
    void mouseMove(vector2f pos);
    
    bool isOverlap(const vector2f& pos);
protected:
    bool isPressed;
    vector2f position;
    vector2f size;
    std::function<void()> button_click_callback;
    
    float vertArray[4*2];
    float colorArray[4*4];
};
#endif /* gxButton_hpp */
