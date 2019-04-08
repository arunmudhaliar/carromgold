//
//  Input.hpp
//  GDCPhysics
//
//  Created by Arun A on 09/11/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#ifndef Input_hpp
#define Input_hpp

#include "../Physics/core/vector2x.h"

struct Input {
public:
    enum INPUT_TYPE {
        STRIKER,
        BALL,
        INPUT_TYPE_MAX
    };
    
    Input();
    
    unsigned long actualInputTime;
    unsigned long triggerAfter;
    INPUT_TYPE type;
    vector2x force;
    vector2x position;
    vector2x velocity;
};
#endif /* Input_hpp */
