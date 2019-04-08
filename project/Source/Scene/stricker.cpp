//
//  stricker.cpp
//  GDCPhysics
//
//  Created by Arun A on 03/11/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#include "stricker.hpp"
#include "../GUI/OSUtils.h"

Stricker::Stricker() : Ball() {
    this->SetGrabed(false);
    this->inputIsAim = false;
    this->inputIsMove = false;
    this->moveToolRotation = 0.0f;
    this->powerScale = 1.0f;
    this->breakShot = true;
    SetStrickerInputOption(OPTION1);
    // Aim Cone
    memset(this->aimConeVertexBuffer, 0 , sizeof(this->aimConeVertexBuffer));
    memset(this->aimConeColorBuffer, 0 , sizeof(this->aimConeColorBuffer));
    memset(this->aimConeLineColorBuffer, 0 , sizeof(this->aimConeLineColorBuffer));
    
    // Grab circle
    memset(this->grabCircleVertexBuffer, 0 , sizeof(this->grabCircleVertexBuffer));
    memset(this->grabCircleColorBuffer, 0 , sizeof(this->grabCircleColorBuffer));
    memset(this->grabCircleLineColorBuffer, 0 , sizeof(this->grabCircleLineColorBuffer));
    
    // Power circle
    memset(this->powerCircleVertexBuffer, 0 , sizeof(this->powerCircleVertexBuffer));
    memset(this->powerCircleColorBuffer, 0, sizeof(this->powerCircleColorBuffer));
    memset(this->powerCircleLineColorBuffer, 0 , sizeof(this->powerCircleLineColorBuffer));
}

Stricker::~Stricker() {
}

void Stricker::InitStricker(float size, float mass, float frictionfactor, const vector2x& pos, CGETextureManager& textureManager, SoundEngine* soundEnginePtr) {
    this->strickerSprite.setOffset(0.0f, 0.0f);
    this->strickerSprite.loadTexture(&textureManager, cpp_getPath("/res/sprites/Sticker.png").c_str());
    
    this->directionSprite.setOffset(0, 1);
    this->directionSprite.loadTexture(&textureManager, cpp_getPath("/res/sprites/arrow.png").c_str());
    
    this->moveToolSprite.setOffset(0, 0);
    this->moveToolSprite.loadTexture(&textureManager, cpp_getPath("/res/sprites/MoveTool.png").c_str());
    
    
    this->initBall(size, mass, frictionfactor, pos, &this->strickerSprite, soundEnginePtr);
    this->SetColor(0.5f, 0.58f, 0.4f);
    this->SetTag("Stricker");
    this->SetRestituition(FTOX(0.09f));
}

void Stricker::OnPostInitBall() {
    Ball::OnPostInitBall();
    
    // Aim Cone
    int cntr=0;
    float coneHeight = this->size*STRICKER_GRAB_OUTER_RADIUS_SCALE;
    this->aimConeVertexBuffer[cntr*2+0]=this->aimConeVertexBuffer[cntr*2+1] = 0.0f;
    
    this->aimConeColorBuffer[cntr*4+0]=this->aimConeColorBuffer[cntr*4+1]=this->aimConeColorBuffer[cntr*4+2]=1.0f;
    this->aimConeColorBuffer[cntr*4+3] = 0.0f;
    
    this->aimConeLineColorBuffer[cntr*4+0]=this->aimConeLineColorBuffer[cntr*4+1]=this->aimConeLineColorBuffer[cntr*4+2]=1.0f;
    this->aimConeLineColorBuffer[cntr*4+3] = 0.0f;
    for(int xx=-AIMCONE_ANGLE; xx<=AIMCONE_ANGLE; xx+=AIMCONE_ANGLE_DELTA) {
        cntr++;
        float cs=coneHeight*cos(DEG2RAD((float)(xx-90)));
        float sn=coneHeight*sin(DEG2RAD((float)(xx-90)));
        this->aimConeVertexBuffer[cntr*2+0]=cs;
        this->aimConeVertexBuffer[cntr*2+1]=sn;
        
        this->aimConeColorBuffer[cntr*4+0] = 1.0f;
        this->aimConeColorBuffer[cntr*4+1] = 1.0f;
        this->aimConeColorBuffer[cntr*4+2] = 1.0f;
        this->aimConeColorBuffer[cntr*4+3] = 0.25f;
        
        this->aimConeLineColorBuffer[cntr*4+0] = 1.0f;
        this->aimConeLineColorBuffer[cntr*4+1] = 1.0f;
        this->aimConeLineColorBuffer[cntr*4+2] = 1.0f;
        this->aimConeLineColorBuffer[cntr*4+3] = 0.25f;
    }
    
    
    // Grab circle
    cntr=0;
    float circleRadius = this->size*STRICKER_GRAB_INNER_RADIUS_SCALE;
    this->grabCircleVertexBuffer[cntr*2+0]=this->grabCircleVertexBuffer[cntr*2+1] = 0.0f;
    
    this->grabCircleColorBuffer[cntr*4+0]=this->grabCircleColorBuffer[cntr*4+1]=this->grabCircleColorBuffer[cntr*4+2]=1.0f;
    this->grabCircleColorBuffer[cntr*4+3] = 0.0f;
    
    this->grabCircleLineColorBuffer[cntr*4+0]=1.0f;
    this->grabCircleLineColorBuffer[cntr*4+1]=1.0f;
    this->grabCircleLineColorBuffer[cntr*4+2]=1.0f;
    this->grabCircleLineColorBuffer[cntr*4+3] = 0.4f;
    for(int xx=-GRAB_CIRCLE_ANGLE; xx<=GRAB_CIRCLE_ANGLE; xx+=GRAB_CIRCLE_ANGLE_DELTA) {
        cntr++;
        float cs=circleRadius*cos(DEG2RAD((float)(xx-90)));
        float sn=circleRadius*sin(DEG2RAD((float)(xx-90)));
        this->grabCircleVertexBuffer[cntr*2+0]=cs;
        this->grabCircleVertexBuffer[cntr*2+1]=sn;
        
        this->grabCircleColorBuffer[cntr*4+0] = 1.0f;
        this->grabCircleColorBuffer[cntr*4+1] = 1.0f;
        this->grabCircleColorBuffer[cntr*4+2] = 1.0f;
        this->grabCircleColorBuffer[cntr*4+3] = 0.15f;
        
        this->grabCircleLineColorBuffer[cntr*4+0] = 1.0f;
        this->grabCircleLineColorBuffer[cntr*4+1] = 1.0f;
        this->grabCircleLineColorBuffer[cntr*4+2] = 1.0f;
        this->grabCircleLineColorBuffer[cntr*4+3] = 0.4f;
    }

    // Power circle
    cntr=0;
    circleRadius = this->size*STRICKER_GRAB_INNER_RADIUS_SCALE;
    this->powerCircleVertexBuffer[cntr*2+0]=this->powerCircleVertexBuffer[cntr*2+1] = 0.0f;
    
    this->powerCircleLineColorBuffer[cntr*4+0]=0.1f;
    this->powerCircleLineColorBuffer[cntr*4+1]=0.1f;
    this->powerCircleLineColorBuffer[cntr*4+2]=0.1f;
    this->powerCircleLineColorBuffer[cntr*4+3] = 1.0f;
    
    this->powerCircleColorBuffer[cntr*4+0] = 0.1f;
    this->powerCircleColorBuffer[cntr*4+1] = 0.1f;
    this->powerCircleColorBuffer[cntr*4+2] = 0.1f;
    this->powerCircleColorBuffer[cntr*4+3] = 0.09f;
    
    for(int xx=-POWER_CIRCLE_ANGLE; xx<=POWER_CIRCLE_ANGLE; xx+=POWER_CIRCLE_ANGLE_DELTA) {
        cntr++;
        float cs=circleRadius*cos(DEG2RAD((float)(xx-90)));
        float sn=circleRadius*sin(DEG2RAD((float)(xx-90)));
        
        this->powerCircleVertexBuffer[cntr*2+0]=cs;
        this->powerCircleVertexBuffer[cntr*2+1]=sn;
        
        float alpha = (cntr%2==0)?1.0f:0.0f;
        this->powerCircleLineColorBuffer[cntr*4+0] = 0.1f;
        this->powerCircleLineColorBuffer[cntr*4+1] = 0.1f;
        this->powerCircleLineColorBuffer[cntr*4+2] = 0.1f;
        this->powerCircleLineColorBuffer[cntr*4+3] = alpha;
        
        this->powerCircleColorBuffer[cntr*4+0] = 0.1f;
        this->powerCircleColorBuffer[cntr*4+1] = 0.1f;
        this->powerCircleColorBuffer[cntr*4+2] = 0.1f;
        this->powerCircleColorBuffer[cntr*4+3] = 0.09f;
    }
}

bool Stricker::IsOverlap(intx x, intx y) {
    vector2x input(x, y);
    if ((input-GetRBPosition()).lengthx()<this->GetRadius()) {
        return true;
    }
    return false;
}

void Stricker::OnRender(const matrix4x4f& viewProjection) {
    Ball::OnRender(viewProjection);
}

void Stricker::OnCollision(std::vector<Collider*>& colliders) {
    Ball::OnCollision(colliders);
    
    if (this->breakShot && colliders.size()) {
        this->breakShot = false;
        float currentVel = XTOF(GetRBVelocity().lengthx());
        float ratio = currentVel/MAX_COIN_VELOCITY;
        ratio = MIN(ratio, 1.0f);
        
        if (ratio>0.02f) {
            int sfxID = getRandom(SFX_SOUND::sfx_breakshot_1, SFX_SOUND::sfx_breakshot_2+1);
            auto sfxSrc = this->soundEnginePtr->getSource(sfxID);
            sfxSrc->setVolume(ratio);
            sfxSrc->play(false);
        }
    } else if (colliders.size()) {
        float currentVel = XTOF(GetRBVelocity().lengthx());
        float ratio = currentVel/MAX_COIN_VELOCITY;
        ratio = MIN(ratio, 1.0f);
        
        if (ratio>0.02f) {
            int sfxID = getRandom(SFX_SOUND::sfx_puck_hits_puck_1, SFX_SOUND::sfx_puck_hits_puck_5+1);
            auto sfxSrc = this->soundEnginePtr->getSource(sfxID);
            sfxSrc->setVolume(ratio*0.5f);
            sfxSrc->play(false);
        }
    }
}

void Stricker::OnPhysicsUpdate() {
    Ball::OnPhysicsUpdate();
    
    if (this->isMoving) {
        float currentVel = XTOF(GetRBVelocity().lengthx());
        float ratio = currentVel/MAX_COIN_VELOCITY;
        ratio = MIN(ratio, 1.0f);
        this->sfxDragInstance->setVolume(ratio*0.85f);
    }
}

void Stricker::OnStartedMoving() {
    Ball::OnStartedMoving();
    
    float currentVel = XTOF(GetRBVelocity().lengthx());
    float ratio = currentVel/MAX_COIN_VELOCITY;
    ratio = MIN(ratio, 1.0f);
    
    if (ratio>0.02f) {
        //sfx_striker_drag_LOOP
        this->sfxDragInstance->setVolume(ratio*0.85f);
        this->sfxDragInstance->play();
    }
}

void Stricker::OnCameToHalt() {
    Ball::OnCameToHalt();
    this->sfxDragInstance->stop();
}


void Stricker::DrawPreHelperSprites(const matrix4x4f& viewProjection) {
    auto shader = HWShaderManager::GetHWShaderManager().GetHWShader(2);
    shader->enableProgram();
    if (this->inputIsAim) {
        this->directionSprite.draw(shader, viewProjection);
    } else {
        this->moveToolSprite.draw(shader, viewProjection);
    }
    shader->disableProgram();
    
    
    // circles
    if (this->inputIsAim) {
        this->DrawPowerCircle(viewProjection);
        this->DrawGrabCircle(viewProjection);
    } else if (this->inputIsMove){
//        this->DrawAimCone(viewProjection);
    } else {
//        this->DrawGrabCircle(viewProjection);
//        this->DrawAimCone(viewProjection);
    }
}

void Stricker::DrawPostHelperSprites(const matrix4x4f& viewProjection) {
}

void Stricker::DrawGrabCircle(const matrix4x4f& viewProjection) {
#if USE_ProgrammablePipeLine
    auto shader = HWShaderManager::GetHWShaderManager().GetHWShader(1);
    shader->enableProgram();
    matrix4x4f dummyTM;
    dummyTM.copyMatrix(*this);
    
    matrix4x4f rotationTM;
    //    rotationTM.setRotationMatrix(rotation, 0, 0, true);
    const float* u_mvp_m4x4= (viewProjection * (dummyTM*rotationTM)).getMatrix();
    shader->sendUniformTMfv("u_mvp_m4x4", u_mvp_m4x4, false, 4);
    
#if GEAR_APPLE
    glEnable(GL_LINE_SMOOTH);
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
#endif
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glVertexAttribPointer(shader->getAttribLoc("a_vertex_coord_v4"), 2, GL_FLOAT, GL_FALSE, 0, this->grabCircleVertexBuffer);
    glEnableVertexAttribArray(shader->getAttribLoc("a_vertex_coord_v4"));
    
    glVertexAttribPointer(shader->getAttribLoc("a_color_coord_v4"), 4, GL_FLOAT, GL_FALSE, 0, this->grabCircleColorBuffer);
    glEnableVertexAttribArray(shader->getAttribLoc("a_color_coord_v4"));
    int points = (GRAB_CIRCLE_ANGLE/GRAB_CIRCLE_ANGLE_DELTA)*2 + 2;
    glDrawArrays(GL_TRIANGLE_FAN, 0, points);
    //    glBlendFunc(GL_ONE, GL_ONE);
    
    glVertexAttribPointer(shader->getAttribLoc("a_color_coord_v4"), 4, GL_FLOAT, GL_FALSE, 0, this->grabCircleLineColorBuffer);
    glEnableVertexAttribArray(shader->getAttribLoc("a_color_coord_v4"));
    //aimConeLineColorBuffer
#if GRAB_CIRCLE_ANGLE==180
    glDrawArrays(GL_LINE_LOOP, 1, points-1);
#else
    glDrawArrays(GL_LINE_LOOP, 0, points);
#endif
    glDisableVertexAttribArray(shader->getAttribLoc("a_color_coord_v4"));
    glDisableVertexAttribArray(shader->getAttribLoc("a_vertex_coord_v4"));
    
    glDisable(GL_BLEND);
#if GEAR_APPLE
    glDisable(GL_LINE_SMOOTH);
#endif
    shader->disableProgram();
#endif
}

void Stricker::DrawAimCone(const matrix4x4f& viewProjection) {
#if USE_ProgrammablePipeLine
    auto shader = HWShaderManager::GetHWShaderManager().GetHWShader(1);
    shader->enableProgram();
    matrix4x4f dummyTM;
    dummyTM.copyMatrix(*this);
    
    matrix4x4f rotationTM;
    //    rotationTM.setRotationMatrix(rotation, 0, 0, true);
    const float* u_mvp_m4x4= (viewProjection * (dummyTM*rotationTM)).getMatrix();
    shader->sendUniformTMfv("u_mvp_m4x4", u_mvp_m4x4, false, 4);
    
#if GEAR_APPLE
    glEnable(GL_LINE_SMOOTH);
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
#endif
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glVertexAttribPointer(shader->getAttribLoc("a_vertex_coord_v4"), 2, GL_FLOAT, GL_FALSE, 0, this->aimConeVertexBuffer);
    glEnableVertexAttribArray(shader->getAttribLoc("a_vertex_coord_v4"));
    
    glVertexAttribPointer(shader->getAttribLoc("a_color_coord_v4"), 4, GL_FLOAT, GL_FALSE, 0, this->aimConeColorBuffer);
    glEnableVertexAttribArray(shader->getAttribLoc("a_color_coord_v4"));
    int points = ((AIMCONE_ANGLE/AIMCONE_ANGLE_DELTA)*2 + 2);
    glDrawArrays(GL_TRIANGLE_FAN, 0, points);
    //    glBlendFunc(GL_ONE, GL_ONE);
    
    glVertexAttribPointer(shader->getAttribLoc("a_color_coord_v4"), 4, GL_FLOAT, GL_FALSE, 0, this->aimConeLineColorBuffer);
    
#if AIMCONE_ANGLE==180
    glDrawArrays(GL_LINE_LOOP, 1, points-1);
#else
    glDrawArrays(GL_LINE_LOOP, 0, points);
#endif
    
    glDisableVertexAttribArray(shader->getAttribLoc("a_color_coord_v4"));
    glDisableVertexAttribArray(shader->getAttribLoc("a_vertex_coord_v4"));
    
    glDisable(GL_BLEND);
#if GEAR_APPLE
    glDisable(GL_LINE_SMOOTH);
#endif
    shader->disableProgram();
#endif
}

void Stricker::DrawPowerCircle(const matrix4x4f& viewProjection) {
#if USE_ProgrammablePipeLine
    auto shader = HWShaderManager::GetHWShaderManager().GetHWShader(1);
    shader->enableProgram();
    matrix4x4f dummyTM;
    dummyTM.copyMatrix(*this);
    
    matrix4x4f rotationTM;
    rotationTM.setScale(this->powerScale, this->powerScale, this->powerScale);
    
    //    rotationTM.setRotationMatrix(rotation, 0, 0, true);
    const float* u_mvp_m4x4= (viewProjection * (dummyTM*rotationTM)).getMatrix();
    shader->sendUniformTMfv("u_mvp_m4x4", u_mvp_m4x4, false, 4);
    
#if GEAR_APPLE
    glEnable(GL_LINE_SMOOTH);
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
#endif
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glVertexAttribPointer(shader->getAttribLoc("a_vertex_coord_v4"), 2, GL_FLOAT, GL_FALSE, 0, this->powerCircleVertexBuffer);
    glEnableVertexAttribArray(shader->getAttribLoc("a_vertex_coord_v4"));
    
    glVertexAttribPointer(shader->getAttribLoc("a_color_coord_v4"), 4, GL_FLOAT, GL_FALSE, 0, this->powerCircleColorBuffer);
    glEnableVertexAttribArray(shader->getAttribLoc("a_color_coord_v4"));
    
    int points = (POWER_CIRCLE_ANGLE/POWER_CIRCLE_ANGLE_DELTA)*2 + 2;
#if POWER_CIRCLE_ANGLE==180
    glDrawArrays(GL_TRIANGLE_FAN, 1, points-1);
#else
    glDrawArrays(GL_TRIANGLE_FAN, 0, points);
#endif
    
    glVertexAttribPointer(shader->getAttribLoc("a_color_coord_v4"), 4, GL_FLOAT, GL_FALSE, 0, this->powerCircleLineColorBuffer);
    
#if POWER_CIRCLE_ANGLE==180
    glDrawArrays(GL_POINTS, 1, points-1);
#else
    glDrawArrays(GL_POINTS, 0, points);
#endif
    
    glDisableVertexAttribArray(shader->getAttribLoc("a_color_coord_v4"));
    glDisableVertexAttribArray(shader->getAttribLoc("a_vertex_coord_v4"));
    
    glDisable(GL_BLEND);
#if GEAR_APPLE
    glDisable(GL_LINE_SMOOTH);
#endif
    shader->disableProgram();
#endif
}

void Stricker::UpdateStricker3(intx fixedDT) {
    float rotationSpeed = 60.0f;
    this->moveToolRotation += (rotationSpeed*XTOF(fixedDT));
    if (this->moveToolRotation>=360.0f) {
        this->moveToolRotation = 0.0f;
    }
    auto strickerPosx = GetRBPosition();
    vector2x inputDiffx = strickerPosx - this->inputCurrentPos;
    vector2x inputCurrentToPrevDiffx =  this->inputPrevPos - this->inputCurrentPos;
    this->inputPrevPos = this->inputCurrentPos;
    auto inputDiff_magx = inputDiffx.lengthx();
    auto scaled_inner_radiusx = MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_INNER_RADIUS_SCALE));
    
    if (!this->IsGrabed()) {
        this->moveToolSprite.setRotation(this->moveToolRotation);
    }
    const intx speedToMoveUnderGrabPosx = FTOX(24.0f);
    if (this->IsGrabed() && this->inputIsMove) {
        if (inputDiff_magx >  scaled_inner_radiusx) {
            this->inputIsMove = false;
            this->inputIsAim = true;
            // calculate the direction and set for the sprite
            vector2f diff = vector2f(XTOF(inputDiffx.x), XTOF(inputDiffx.y));
            float angle = RAD2DEG(atan2(-diff.x, diff.y));
            float scale = diff.Length()/directionSprite.getClipHeight();
            directionSprite.setTile(1.0f, scale);
            directionSprite.setScale(1.0f, scale);
            directionSprite.setRotation(angle);
            directionSprite.setPos(XTOF(strickerPosx.x), XTOF(strickerPosx.y));
        } else {
            this->moveToolSprite.setRotation(this->moveToolRotation);
            // just move
            vector2x inputDeltax = inputCurrentToPrevDiffx;
            inputDeltax.y=0;
            auto toMove = inputDeltax*MULTX(fixedDT, speedToMoveUnderGrabPosx);
            MoveStricker(fixedDT, *this, inputDeltax);
            auto updatedStrickerPos = this->GetRBPosition();
            this->moveToolSprite.setPos(XTOF(updatedStrickerPos.x), XTOF(updatedStrickerPos.y));
        }
    } else if (this->IsGrabed() && this->inputIsAim) {
        if (inputDiff_magx <  scaled_inner_radiusx) {
            this->inputIsMove = true;
            this->inputIsAim = false;
        } else {
            // just aim
            vector2f diff = vector2f(XTOF(inputDiffx.x), XTOF(inputDiffx.y));
            float angle = RAD2DEG(atan2(-diff.x, diff.y));
            float powerLength = diff.Length();
            powerLength = MIN(powerLength, MAX_STRICK_LENGHT);
            this->powerScale = 1.0f + powerLength/MAX_STRICK_LENGHT;
            float dirSpriteScale = powerLength/directionSprite.getClipHeight();
            directionSprite.setTile(1.0f, dirSpriteScale*1.75f);
            directionSprite.setScale(1.0f, dirSpriteScale*1.75f);
            directionSprite.setRotation(angle);
            
            directionSprite.setPos(XTOF(strickerPosx.x), XTOF(strickerPosx.y));
        }
    }

}

void Stricker::UpdateStricker2(intx fixedDT) {
    float rotationSpeed = 60.0f;
    this->moveToolRotation += (rotationSpeed*XTOF(fixedDT));
    if (this->moveToolRotation>=360.0f) {
        this->moveToolRotation = 0.0f;
    }
    auto strickerPosx = GetRBPosition();
    vector2x inputDiffx = strickerPosx - this->inputCurrentPos;
    vector2x inputCurrentToPrevDiffx =  this->inputPrevPos - this->inputCurrentPos;
    this->inputPrevPos = this->inputCurrentPos;
    auto inputDiff_magx = inputDiffx.lengthx();
    auto scaled_inner_radiusx = MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_INNER_RADIUS_SCALE));
    
    if (!this->IsGrabed()) {
        this->moveToolSprite.setRotation(this->moveToolRotation);
    }
    const intx speedToMoveUnderGrabPosx = FTOX(24.0f);
    if (this->IsGrabed() && this->inputIsMove) {
        this->moveToolSprite.setRotation(this->moveToolRotation);
        // just move
        vector2x inputDeltax = inputCurrentToPrevDiffx;
        inputDeltax.y=0;
        auto toMove = inputDeltax*MULTX(fixedDT, speedToMoveUnderGrabPosx);
        MoveStricker(fixedDT, *this, inputDeltax);
        auto updatedStrickerPos = this->GetRBPosition();
        this->moveToolSprite.setPos(XTOF(updatedStrickerPos.x), XTOF(updatedStrickerPos.y));
    } else if (this->IsGrabed() && this->inputIsAim) {
        if (inputDiff_magx <  scaled_inner_radiusx) {
            this->inputIsMove = true;
            this->inputIsAim = false;
        } else {
            // just aim
            vector2f diff = vector2f(XTOF(inputDiffx.x), XTOF(inputDiffx.y));
            float angle = RAD2DEG(atan2(-diff.x, diff.y));
            float powerLength = diff.Length();
            powerLength = MIN(powerLength, MAX_STRICK_LENGHT);
            this->powerScale = 1.0f + powerLength/MAX_STRICK_LENGHT;
            float dirSpriteScale = powerLength/directionSprite.getClipHeight();
            directionSprite.setTile(1.0f, dirSpriteScale*1.75f);
            directionSprite.setScale(1.0f, dirSpriteScale*1.75f);
            directionSprite.setRotation(angle);
            
            directionSprite.setPos(XTOF(strickerPosx.x), XTOF(strickerPosx.y));
        }
    }
}

void Stricker::UpdateStricker(intx fixedDT) {
    switch (this->inputOption) {
        case OPTION1:
            UpdateStricker2(fixedDT);
            break;
        case OPTION2:
            UpdateStricker3(fixedDT);
            break;
        default:
            UpdateStricker3(fixedDT);
            break;
    }
#if OLD_METHOD_NOT_OPTIMISED
    float rotationSpeed = 60.0f;
    this->moveToolRotation += (rotationSpeed*XTOF(fixedDT));
    if (this->moveToolRotation>=360.0f) {
        this->moveToolRotation = 0.0f;
    }
    auto strickerPosx = GetRBPosition();
    vector2x inputCurrentToPrevDiffx =  this->inputPrevPos - this->inputCurrentPos;
    this->inputPrevPos = this->inputCurrentPos;
    vector2x inputDiffx = strickerPosx - this->inputCurrentPos;
    auto inputDiff_magx = inputDiffx.lengthx();
    auto scaled_inner_radiusx = MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_INNER_RADIUS_SCALE));
    auto scaled_outer_radiusx = MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_OUTER_RADIUS_SCALE));
    if (inputDiff_magx>0) {
        if (!this->IsGrabed() && inputDiff_magx <  scaled_inner_radiusx) {
            this->SetGrabed(true);
        }
        this->moveToolSprite.setRotation(this->moveToolRotation);
        if (this->IsGrabed()) {
            vector2f diff = vector2f(XTOF(inputDiffx.x), XTOF(inputDiffx.y));
            float angle = RAD2DEG(atan2(-diff.x, diff.y));
            auto canGrab = inputDiffx.lengthx() < scaled_inner_radiusx;
            if (inputDiffx.lengthx() < scaled_outer_radiusx && !this->inputIsAim && !this->inputIsMove) {
                canGrab = true;
            }
            const intx speedToMoveUnderGrabPosx = FTOX(24.0f);
            if (canGrab) {
                // just move
                vector2x inputDeltax = inputCurrentToPrevDiffx;
                inputDeltax.y=0;
                this->inputIsAim = false;
                this->inputIsMove = false;
                auto toMove = inputDeltax*MULTX(fixedDT, speedToMoveUnderGrabPosx);
                MoveStricker(fixedDT, *this, inputDeltax);
                auto updatedStrickerPos = this->GetRBPosition();
                this->moveToolSprite.setPos(XTOF(updatedStrickerPos.x), XTOF(updatedStrickerPos.y));
                //                        this->inputPrevPos = this->inputCurrentPos;
            } else {
                if (this->inputIsAim) {
                    // just aim
                    float powerLength = diff.Length();
                    powerLength = MIN(powerLength, MAX_STRICK_LENGHT);
                    this->powerScale = 1.0f + powerLength/MAX_STRICK_LENGHT;
                    float dirSpriteScale = powerLength/directionSprite.getClipHeight();
                    directionSprite.setTile(1.0f, dirSpriteScale*1.75f);
                    directionSprite.setScale(1.0f, dirSpriteScale*1.75f);
                    directionSprite.setRotation(angle);
                    
                    directionSprite.setPos(XTOF(strickerPosx.x), XTOF(strickerPosx.y));
                    this->inputIsAim = true;
                } else if (this->inputIsMove) {
                    // just move
                    vector2x inputDeltax = inputDiffx;
                    inputDeltax.y=0;
                    this->inputIsAim = false;
                    
                    auto toMove = inputDeltax*MULTX(fixedDT, speedToMoveUnderGrabPosx);
                    MoveStricker(fixedDT, *this, toMove);
                    auto updatedStrickerPos = this->GetRBPosition();
                    this->moveToolSprite.setPos(XTOF(updatedStrickerPos.x), XTOF(updatedStrickerPos.y));
                } else {
                    if (ABS(angle)>AIMCONE_ANGLE) {
                        this->inputIsMove = true;
                        this->inputIsAim = false;
                        // set the position for the sprite
                        this->moveToolSprite.setPos(XTOF(strickerPosx.x), XTOF(strickerPosx.y));
                    } else {
                        this->inputIsMove = false;
                        this->inputIsAim = true;
                        // calculate the direction and set for the sprite
                        float scale = diff.Length()/directionSprite.getClipHeight();
                        directionSprite.setTile(1.0f, scale);
                        directionSprite.setScale(1.0f, scale);
                        directionSprite.setRotation(angle);
                        directionSprite.setPos(XTOF(strickerPosx.x), XTOF(strickerPosx.y));
                    }
                }
            }
        }
    }
#endif
}

void Stricker::Cmd_PlaceStricker() {
    this->inputIsAim = false;
    this->inputIsMove = false;
    this->powerScale = 1.0f;
    this->SetGrabed(false);
    auto strickerPos = this->GetRBPosition();
    this->moveToolSprite.setPos(XTOF(strickerPos.x), XTOF(strickerPos.y));
}

void Stricker::Cmd_TryGrab(const vector2x& pos) {
    this->inputCurrentPos = pos;
    this->inputPrevPos = this->inputCurrentPos;
    this->inputStartPos = this->inputCurrentPos;
    
    auto strickerPosx = GetRBPosition();
    vector2x inputDiffx = strickerPosx - this->inputCurrentPos;
    auto inputDiff_magx = inputDiffx.lengthx();
    auto scaled_inner_radiusx = MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_INNER_RADIUS_SCALE));
    if (!this->IsGrabed() && inputDiff_magx <  scaled_inner_radiusx) {
        this->SetGrabed(true);
        this->inputIsMove = true;
    } else if (!this->IsGrabed()) {
        this->SetGrabed(true);
        this->inputIsAim  = true;
    }
}

void Stricker::Cmd_TryFire(const vector2x& pos, std::function<void()> callback) {
    this->inputCurrentPos = pos;
    
    if (this->inputIsAim) {
        this->ApplyBoost();
        callback();
        
    }
    
    this->SetGrabed(false);
    this->inputIsAim = false;
    this->inputIsMove = false;
}

void Stricker::Cmd_TryMove(const vector2x& pos) {
    this->inputCurrentPos = pos;
}

void Stricker::MoveStricker(intx fixedDT, Ball& ball, vector2x& delta) {
    //    intx speed = ITOX(300);
    auto position = ball.GetRBPosition();
    position -= delta;//vector2x(MULTX(speed, fixedDT), 0);
    
    vector2x leftOriginPosx=-(BOARD_SIZEx*FTOX(0.5f));
    vector2x rightMargin = leftOriginPosx+(BOARD_SIZEx*FTOX(1.0f-0.24f));
    vector2x leftMargin = leftOriginPosx+(BOARD_SIZEx*FTOX(0.24f));
    if (position.x > rightMargin.x) {
        position.x = rightMargin.x;
    }
    if (position.x < leftMargin.x) {
        position.x = leftMargin.x;
    }
    ball.SetRBPosition(position);
}

void Stricker::ApplyBoost() {
    auto vel = GetRBVelocity();
    auto strickerPos = GetRBPosition();
    
    vector2x diffStartToCurrent = strickerPos - this->inputCurrentPos;
    auto diff_mag = diffStartToCurrent.lengthx();
    diffStartToCurrent.normalizex();
    
    // 130 is the top power
    const int MAX_ALLOWED_LENGTH = ITOX(MAX_STRICK_LENGHT);
    const int MAX_ALLOWED_POWER = ITOX(MAX_STRICK_POWER);
    const int POWER_DELTA = ITOX(MAX_STRICK_POWER/MAX_STRICK_LENGHT);
    
    diff_mag = MIN(diff_mag, MAX_ALLOWED_LENGTH);
    
    intx scaled_diff = MULTX64(diff_mag, POWER_DELTA);
    scaled_diff = MIN(scaled_diff, MAX_ALLOWED_POWER);
    //    DEBUG_PRINT("scaled_diff %d", XTOI(scaled_diff));
    //    scaled_diff = FTOX(40000);
    AddForce(diffStartToCurrent*scaled_diff);
}

