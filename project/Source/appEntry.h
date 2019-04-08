#pragma once
//
//  appentry.h
//  Copyright 2017 GEAR3D
//  Gear3D
//
//  Created by Arun Mudaliar.
//
//

#include <SDL2/SDL.h>

class GDCPhysics {
public:
    int appEntry();
    void processSDLEvent(SDL_Window * window, SDL_Event& e, void* userData);
};

