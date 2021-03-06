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
    this->SetAimMode(false);
    this->SetMoveMode(false);
    this->moveToolRotation = 0.0f;
    this->movetoolArrowAnimation = 0.0f;
    this->powerScale = 1.0f;
    this->breakShot = true;
    this->observer = nullptr;
    this->overlapWithCoins = false;
    this->textureManager = nullptr;
    SetStrickerInputOption(OPTION5);
    // Aim Cone
    memset(this->aimConeVertexBuffer, 0 , sizeof(this->aimConeVertexBuffer));
    memset(this->aimConeColorBuffer, 0 , sizeof(this->aimConeColorBuffer));
    memset(this->aimConeLineColorBuffer, 0 , sizeof(this->aimConeLineColorBuffer));
    
    // Aim ring
    memset(this->aimRingVertexBuffer, 0 , sizeof(this->aimRingVertexBuffer));
    memset(this->aimRingColorBuffer, 0 , sizeof(this->aimRingColorBuffer));
    
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

void Stricker::InitStricker(intx size, intx mass, intx frictionfactor, const vector2x& pos, CGETextureManager& textureManager,
                            SoundEngine* soundEnginePtr, const std::string& name, MStrickerObserver* observer) {
    this->textureManager = &textureManager;
    this->observer = observer;
    this->strickerSprite.setOffset(0.0f, 0.0f);
    this->strickerSprite.loadTexture(&textureManager, OSUtils::cpp_getPath("res/sprites/Striker.png").c_str());
    this->strickerSprite.setAlpha(0.5f);
    this->directionSprite.setOffset(0, 1);
    this->directionSprite.loadTexture(&textureManager, OSUtils::cpp_getPath("res/sprites/arrow.png").c_str());
    
    this->moveToolSprite.setOffset(0, 0);
    if (this->inputOption < OPTION4) {
        this->moveToolSprite.loadTexture(&textureManager, OSUtils::cpp_getPath("res/sprites/MoveTool_V1.png").c_str());
    } else {
        this->moveToolSprite.loadTexture(&textureManager, OSUtils::cpp_getPath("res/sprites/MoveTool_V2.png").c_str());
    }
    
    this->moveToolInActionSprite.setOffset(0, 0);
    this->moveToolInActionSprite.loadTexture(&textureManager, OSUtils::cpp_getPath("res/sprites/MoveToolInAction.png").c_str());
    

    this->aimToolSprite.setOffset(0, 0);
    if (this->inputOption < OPTION4) {
        this->aimToolSprite.loadTexture(&textureManager, OSUtils::cpp_getPath("res/sprites/AimTool_V1.png").c_str());
    } else {
        this->aimToolSprite.loadTexture(&textureManager, OSUtils::cpp_getPath("res/sprites/AimTool_V2.png").c_str());
    }
    
    for (int x=0;x<sizeof(moveArrows)/sizeof(Sprite2Dx);x++) {
        this->moveArrows[x].setOffset(0, 0);
        if (this->inputOption==OPTION4) {
            this->moveArrows[x].loadTexture(&textureManager, OSUtils::cpp_getPath("res/sprites/MoveToolArrow_V2.png").c_str());
        } else if (this->inputOption==OPTION5) {
            this->moveArrows[x].loadTexture(&textureManager, OSUtils::cpp_getPath("res/sprites/MovingArrow.png").c_str());
        }
        if (x%2==1) {
            this->moveArrows[x].setRotation(180);
        }
    }
    
    //shadowSprite
    this->shadowSprite.setOffset(0, 0);
    this->shadowSprite.loadTexture(&textureManager, OSUtils::cpp_getPath("res/sprites/StickerShadow.png").c_str());
    
    this->initBall(size, mass, frictionfactor, pos, &this->strickerSprite, &this->shadowSprite, soundEnginePtr);
    this->SetColor(0.5f, 0.58f, 0.4f);
    this->SetTag("ST");
    this->SetRBName(name);
    this->SetRestituition(STRICKER_BOUNCE_FACTORx);   //=FTOX(0.09f)

    this->SetStrickerInputOption(OPTION5);
}

void Stricker::SetOverlapWithCoins(bool flag) {
    this->overlapWithCoins = flag;
}

void Stricker::SetStrickerPosition(const vector2x& pos) {
    SetRBPosition(pos, true);
//    this->moveToolSprite.setPositionx(pos);
//    this->aimToolSprite.setPositionx(pos);
//    this->directionSprite.setPositionx(pos);
}

void Stricker::Remote_SetGrabbed(bool flag) {
    this->SetGrabed(flag);
}

void Stricker::Remote_SetAimMode(bool flag) {
    this->SetAimMode(flag);
}

void Stricker::Remote_SetMoveMode(bool flag) {
    this->SetMoveMode(flag);
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
    
    // Aim ring
    cntr=0;
    float innerRingRadius = this->size*STRICKER_GRAB_INNER_RADIUS_SCALE;
    float outerRingRadius = this->size*STRICKER_GRAB_OUTER_RADIUS_SCALE;
    for(int xx=-AIMCONE_RING_ANGLE; xx<=AIMCONE_RING_ANGLE; xx+=AIMCONE_RING_ANGLE_DELTA) {
        float cso=outerRingRadius*cos(DEG2RAD((float)(xx-90)));
        float sno=outerRingRadius*sin(DEG2RAD((float)(xx-90)));
        float csi=innerRingRadius*cos(DEG2RAD((float)(xx-90)));
        float sni=innerRingRadius*sin(DEG2RAD((float)(xx-90)));
        this->aimRingVertexBuffer[cntr*4+0]=csi;
        this->aimRingVertexBuffer[cntr*4+1]=sni;
        this->aimRingVertexBuffer[cntr*4+2]=cso;
        this->aimRingVertexBuffer[cntr*4+3]=sno;
        
        float clr_i = 1.0f;
        float clr_o = 1.0f;
        this->aimRingColorBuffer[cntr*8+0] = clr_i;
        this->aimRingColorBuffer[cntr*8+1] = clr_i;
        this->aimRingColorBuffer[cntr*8+2] = clr_i;
        this->aimRingColorBuffer[cntr*8+3] = 0.3f;
        
        this->aimRingColorBuffer[cntr*8+4] = clr_o;
        this->aimRingColorBuffer[cntr*8+5] = clr_o;
        this->aimRingColorBuffer[cntr*8+6] = clr_o;
        this->aimRingColorBuffer[cntr*8+7] = 0.1f;
        cntr++;
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

bool Stricker::IsOverlap(const vector2x& pos) {
    if ((pos-GetRBPosition()).lengthx()<this->GetRadius()) {
        return true;
    }
    return false;
}

void Stricker::OnRender(const matrix4x4f& viewProjection) {
    if (this->overlapWithCoins) {
        this->sprite->setRGB(0.75f, 0.0f, 0.0f);
        this->sprite->setAlpha(0.5f);
    } else {
        this->sprite->setRGB(1.0f, 1.0f, 1.0f);
        this->sprite->setAlpha(1.0f);
    }
    
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
    if (!this->inputIsAim) {
        if (this->inputOption==OPTION3 || this->inputOption==OPTION4 || this->inputOption==OPTION5) {
            if (!this->IsGrabed()) {
                if (this->inputOption!=OPTION5) {
                    this->moveToolSprite.draw(shader, viewProjection);
                }
                if (!this->overlapWithCoins) {
                    this->aimToolSprite.draw(shader, viewProjection);
                }
            } else {
                this->moveToolInActionSprite.draw(shader, viewProjection);
            }
        } else {
            this->moveToolSprite.draw(shader, viewProjection);
        }
    }
    
    if (this->inputOption==OPTION4 || this->inputOption==OPTION5) {
        if (!this->IsGrabed()/* || this->inputIsMove*/) {
            for(int x=0;x<sizeof(moveArrows)/sizeof(Sprite2Dx);x++) {
                vector2x oldPos = this->moveArrows[x].getPosition2x();
                float displacement = (x%2==0)?-60.0f:60.0f;
                float alpha = (this->movetoolArrowAnimation<0.5f)?this->movetoolArrowAnimation*2.0f:(0.5f-(this->movetoolArrowAnimation-0.5f))*2.0f;
                if (this->inputOption==OPTION5) {
                    vector2x leftOriginPosx=-(BOARD_SIZEx*FTOX(0.5f));
//                    vector2x rightMargin = leftOriginPosx+(BOARD_SIZEx*FTOX(1.0f-0.24f));
                    vector2x leftMargin = leftOriginPosx+(BOARD_SIZEx*FTOX(0.24f));
                    vector2x newPosx = oldPos+vector2x(FTOX(this->movetoolArrowAnimation*displacement), 0);
                    intx ratio = ABS(DIVX(newPosx.x, leftMargin.x));
                    alpha = XTOF(FX_ONE-ratio);
                }
                this->moveArrows[x].setAlpha(alpha);
                this->moveArrows[x].setPositionx(oldPos+vector2x(FTOX(this->movetoolArrowAnimation*displacement), 0));
                this->moveArrows[x].draw(shader, viewProjection);
                this->moveArrows[x].setPositionx(oldPos);
            }
        }
    }
    shader->disableProgram();
    
    
    // circles
    if (this->inputIsAim) {
        this->DrawPowerCircle(viewProjection);
        this->DrawGrabCircle(viewProjection);
    } else if (this->inputIsMove){
//        if (this->inputOption == OPTION3) {
//            this->DrawAimRing(viewProjection);
//        }
    } else {
//        if (this->inputOption == OPTION3) {
//            this->DrawAimRing(viewProjection);
//        }
//        this->DrawGrabCircle(viewProjection);
//        this->DrawAimCone(viewProjection);
    }
}

void Stricker::DrawPostHelperSprites(const matrix4x4f& viewProjection) {
    auto shader = HWShaderManager::GetHWShaderManager().GetHWShader(2);
    shader->enableProgram();
    if (this->inputIsAim) {
        this->directionSprite.draw(shader, viewProjection);
    }
    shader->disableProgram();
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

void Stricker::DrawAimRing(const matrix4x4f& viewProjection) {
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
    
//    glDisable(GL_CULL_FACE);
    glVertexAttribPointer(shader->getAttribLoc("a_vertex_coord_v4"), 2, GL_FLOAT, GL_FALSE, 0, this->aimRingVertexBuffer);
    glEnableVertexAttribArray(shader->getAttribLoc("a_vertex_coord_v4"));
    
    glVertexAttribPointer(shader->getAttribLoc("a_color_coord_v4"), 4, GL_FLOAT, GL_FALSE, 0, this->aimRingColorBuffer);
    glEnableVertexAttribArray(shader->getAttribLoc("a_color_coord_v4"));
    int points = ((AIMCONE_RING_ANGLE/AIMCONE_RING_ANGLE_DELTA)*4 + 2);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, points);
    //    glBlendFunc(GL_ONE, GL_ONE);
    
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

void Stricker::transformationChangedx() {
    auto pos = getPosition2x();
    this->moveToolSprite.setPositionx(pos);
    this->moveToolInActionSprite.setPositionx(pos);
    this->aimToolSprite.setPositionx(pos);
//    this->SetMoveArrowPositions(pos);
    this->directionSprite.setPositionx(pos);
    
    auto radiusOuter = 0;
    if (this->inputOption==OPTION4) {
        radiusOuter = MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_OUTER_RADIUS_SCALE))+ITOX(3);
    } else {
        radiusOuter = this->GetRadius(); //MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_OUTER_RADIUS_SCALE))+ITOX(3);
    }
    for(int x=0;x<sizeof(moveArrows)/sizeof(Sprite2Dx);x++) {
        if (x%2==0) {
            this->moveArrows[x].setPositionx(pos-vector2x(radiusOuter, 0));
        } else {
            this->moveArrows[x].setPositionx(pos+vector2x(radiusOuter, 0));
            radiusOuter+=FTOX(this->moveArrows[x].getClipWidth()+10);
        }
    }

}

void Stricker::UpdateStricker5(intx fixedDT) {
    float rotationSpeed = 60.0f;
    float moveArrowSpeed = 1.0f;
    this->moveToolRotation += (rotationSpeed*XTOF(fixedDT));
    this->movetoolArrowAnimation += (moveArrowSpeed*XTOF(fixedDT));
    if (this->moveToolRotation>=360.0f) {
        this->moveToolRotation = 0.0f;
    }
    
    if (this->movetoolArrowAnimation>2.0f) {
        this->movetoolArrowAnimation = 0.0f;
    }
    auto strickerPosx = GetRBPosition();
    vector2x inputDiffx = strickerPosx - this->inputCurrentPos;
    vector2x inputCurrentToPrevDiffx =  this->inputPrevPos - this->inputCurrentPos;
    this->inputPrevPos = this->inputCurrentPos;
    auto inputDiff_magx = inputDiffx.lengthx();
    auto scaled_inner_radiusx = MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_INNER_RADIUS_SCALE));
    
    if (!this->IsGrabed()) {
        this->aimToolSprite.setRotation(this->moveToolRotation);
    }
    
    const intx speedToMoveUnderGrabPosx = FTOX(24.0f);
    if (this->IsGrabed() && this->inputIsMove) {
        //        this->aimToolSprite.setRotation(this->moveToolRotation);
        // just move
        vector2x inputDeltax = inputCurrentToPrevDiffx;
        inputDeltax.y=0;
        auto toMove = inputDeltax*MULTX(fixedDT, speedToMoveUnderGrabPosx);
        MoveStricker(fixedDT, *this, inputDeltax);
        auto updatedStrickerPos = this->GetRBPosition();
        //        this->moveToolSprite.setPositionx(updatedStrickerPos);
        //        this->aimToolSprite.setPositionx(updatedStrickerPos);
        //        this->SetMoveArrowPositions(updatedStrickerPos);
    } else if (this->IsGrabed() && this->inputIsAim) {
        if (this->inputAimWentOutOfinnerCircle && inputDiff_magx <  scaled_inner_radiusx) {
            this->SetMoveMode(true);
            this->SetAimMode(false);
            this->inputAimWentOutOfinnerCircle = false;
        } else {
            // just aim
            vector2f diff = vector2f(XTOF(inputDiffx.x), XTOF(inputDiffx.y));
            float angle = RAD2DEG(atan2(-diff.x, diff.y));
            float powerLength = diff.Length()-XTOF(scaled_inner_radiusx);
            powerLength = MIN(powerLength, MAX_STRICK_LENGHT);
            this->powerScale = 1.0f + powerLength/MAX_STRICK_LENGHT;
            float dirSpriteScale = powerLength/directionSprite.getClipHeight();
            directionSprite.setTile(1.0f, dirSpriteScale*1.75f);
            directionSprite.setScale(1.0f, dirSpriteScale*1.75f);
            directionSprite.setRotation(angle);
            
            if (!this->inputAimWentOutOfinnerCircle && inputDiff_magx >  scaled_inner_radiusx) {
                this->inputAimWentOutOfinnerCircle = true;
            }
            //            aimToolSprite.setPositionx(strickerPosx);
            //            directionSprite.setPos(XTOF(strickerPosx.x), XTOF(strickerPosx.y));
        }
    }
}

void Stricker::UpdateStricker4(intx fixedDT) {
    float rotationSpeed = 60.0f;
    float moveArrowSpeed = 1.0f;
    this->moveToolRotation += (rotationSpeed*XTOF(fixedDT));
    this->movetoolArrowAnimation += (moveArrowSpeed*XTOF(fixedDT));
    if (this->moveToolRotation>=360.0f) {
        this->moveToolRotation = 0.0f;
    }
    if (this->movetoolArrowAnimation>1.0f) {
        this->movetoolArrowAnimation = 0.0f;
    }
    auto strickerPosx = GetRBPosition();
    vector2x inputDiffx = strickerPosx - this->inputCurrentPos;
    vector2x inputCurrentToPrevDiffx =  this->inputPrevPos - this->inputCurrentPos;
    this->inputPrevPos = this->inputCurrentPos;
    auto inputDiff_magx = inputDiffx.lengthx();
    auto scaled_inner_radiusx = MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_INNER_RADIUS_SCALE));
    
    if (!this->IsGrabed()) {
        this->aimToolSprite.setRotation(this->moveToolRotation);
    }
    
    const intx speedToMoveUnderGrabPosx = FTOX(24.0f);
    if (this->IsGrabed() && this->inputIsMove) {
        //        this->aimToolSprite.setRotation(this->moveToolRotation);
        // just move
        vector2x inputDeltax = inputCurrentToPrevDiffx;
        inputDeltax.y=0;
        auto toMove = inputDeltax*MULTX(fixedDT, speedToMoveUnderGrabPosx);
        MoveStricker(fixedDT, *this, inputDeltax);
        auto updatedStrickerPos = this->GetRBPosition();
        //        this->moveToolSprite.setPositionx(updatedStrickerPos);
        //        this->aimToolSprite.setPositionx(updatedStrickerPos);
        //        this->SetMoveArrowPositions(updatedStrickerPos);
    } else if (this->IsGrabed() && this->inputIsAim) {
        if (this->inputAimWentOutOfinnerCircle && inputDiff_magx <  scaled_inner_radiusx) {
            this->SetMoveMode(true);
            this->SetAimMode(false);
            this->inputAimWentOutOfinnerCircle = false;
        } else {
            // just aim
            vector2f diff = vector2f(XTOF(inputDiffx.x), XTOF(inputDiffx.y));
            float angle = RAD2DEG(atan2(-diff.x, diff.y));
            float powerLength = diff.Length()-XTOF(scaled_inner_radiusx);
            powerLength = MIN(powerLength, MAX_STRICK_LENGHT);
            this->powerScale = 1.0f + powerLength/MAX_STRICK_LENGHT;
            float dirSpriteScale = powerLength/directionSprite.getClipHeight();
            directionSprite.setTile(1.0f, dirSpriteScale*1.75f);
            directionSprite.setScale(1.0f, dirSpriteScale*1.75f);
            directionSprite.setRotation(angle);
            
            if (!this->inputAimWentOutOfinnerCircle && inputDiff_magx >  scaled_inner_radiusx) {
                this->inputAimWentOutOfinnerCircle = true;
            }
            //            aimToolSprite.setPositionx(strickerPosx);
            //            directionSprite.setPos(XTOF(strickerPosx.x), XTOF(strickerPosx.y));
        }
    }
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
        this->aimToolSprite.setRotation(this->moveToolRotation);
    }
    
    const intx speedToMoveUnderGrabPosx = FTOX(24.0f);
    if (this->IsGrabed() && this->inputIsMove) {
//        this->aimToolSprite.setRotation(this->moveToolRotation);
        // just move
        vector2x inputDeltax = inputCurrentToPrevDiffx;
        inputDeltax.y=0;
        auto toMove = inputDeltax*MULTX(fixedDT, speedToMoveUnderGrabPosx);
        MoveStricker(fixedDT, *this, inputDeltax);
        auto updatedStrickerPos = this->GetRBPosition();
//        this->moveToolSprite.setPositionx(updatedStrickerPos);
//        this->aimToolSprite.setPositionx(updatedStrickerPos);
//        this->SetMoveArrowPositions(updatedStrickerPos);
    } else if (this->IsGrabed() && this->inputIsAim) {
        if (inputDiff_magx <  scaled_inner_radiusx) {
            this->SetMoveMode(true);
            this->SetAimMode(false);
        } else {
            // just aim
            vector2f diff = vector2f(XTOF(inputDiffx.x), XTOF(inputDiffx.y));
            float angle = RAD2DEG(atan2(-diff.x, diff.y));
            float powerLength = diff.Length()-XTOF(scaled_inner_radiusx);
            powerLength = MIN(powerLength, MAX_STRICK_LENGHT);
            this->powerScale = 1.0f + powerLength/MAX_STRICK_LENGHT;
            float dirSpriteScale = powerLength/directionSprite.getClipHeight();
            directionSprite.setTile(1.0f, dirSpriteScale*1.75f);
            directionSprite.setScale(1.0f, dirSpriteScale*1.75f);
            directionSprite.setRotation(angle);
            
//            aimToolSprite.setPositionx(strickerPosx);
//            directionSprite.setPos(XTOF(strickerPosx.x), XTOF(strickerPosx.y));
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
        if (inputDiff_magx >  scaled_inner_radiusx) {
            this->SetMoveMode(false);
            this->SetAimMode(true);
            // calculate the direction and set for the sprite
            vector2f diff = vector2f(XTOF(inputDiffx.x), XTOF(inputDiffx.y));
            float angle = RAD2DEG(atan2(-diff.x, diff.y));
            float scale = diff.Length()/directionSprite.getClipHeight();
            directionSprite.setTile(1.0f, scale);
            directionSprite.setScale(1.0f, scale);
            directionSprite.setRotation(angle);
//            directionSprite.setPos(XTOF(strickerPosx.x), XTOF(strickerPosx.y));
        } else {
            this->moveToolSprite.setRotation(this->moveToolRotation);
            // just move
            vector2x inputDeltax = inputCurrentToPrevDiffx;
            inputDeltax.y=0;
            auto toMove = inputDeltax*MULTX(fixedDT, speedToMoveUnderGrabPosx);
            MoveStricker(fixedDT, *this, inputDeltax);
            auto updatedStrickerPos = this->GetRBPosition();
//            this->moveToolSprite.setPos(XTOF(updatedStrickerPos.x), XTOF(updatedStrickerPos.y));
        }
    } else if (this->IsGrabed() && this->inputIsAim) {
        if (inputDiff_magx <  scaled_inner_radiusx) {
            this->SetMoveMode(true);
            this->SetAimMode(false);
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
            
//            directionSprite.setPos(XTOF(strickerPosx.x), XTOF(strickerPosx.y));
        }
    }
}

void Stricker::UpdateStricker1(intx fixedDT) {
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
//        this->moveToolSprite.setPos(XTOF(updatedStrickerPos.x), XTOF(updatedStrickerPos.y));
    } else if (this->IsGrabed() && this->inputIsAim) {
        if (inputDiff_magx <  scaled_inner_radiusx) {
            this->SetMoveMode(true);
            this->SetAimMode(false);
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
            
//            directionSprite.setPos(XTOF(strickerPosx.x), XTOF(strickerPosx.y));
        }
    }
}

void Stricker::UpdateStricker(intx fixedDT) {
    switch (this->inputOption) {
        case OPTION1:
            UpdateStricker1(fixedDT);
            break;
        case OPTION2:
            UpdateStricker2(fixedDT);
            break;
        case OPTION3:
            UpdateStricker3(fixedDT);
            break;
        case OPTION4:
            UpdateStricker4(fixedDT);
            break;
        case OPTION5:
            UpdateStricker5(fixedDT);
            break;
        default:
            UpdateStricker2(fixedDT);
            break;
    }
}

void Stricker::SetGrabed(bool flag) {
    auto oldValue = this->grabed;
    this->grabed = flag;
    if (this->grabed && !oldValue && observer) {
        observer->OnStricker_StateChangeTo_Grab(this);
    }
}

void Stricker::SetAimMode(bool flag) {
    auto oldValue = this->inputIsAim;
    this->inputIsAim = flag;
    if (this->inputIsAim && !oldValue && observer) {
        observer->OnStricker_StateChangeTo_Aim(this);
    }
}

void Stricker::SetMoveMode(bool flag) {
    auto oldValue = this->inputIsMove;
    this->inputIsMove = flag;
    if (this->inputIsMove && !oldValue && observer) {
        observer->OnStricker_StateChangeTo_Move(this);
    }
}

void Stricker::Cmd_PlaceStricker() {
    this->SetAimMode(false);
    this->SetMoveMode(false);
    this->powerScale = 1.0f;
    this->SetGrabed(false);
    this->inputAimWentOutOfinnerCircle = false;
//    auto strickerPos = this->GetRBPosition();
//    this->moveToolSprite.setPositionx(strickerPos);
//    this->aimToolSprite.setPositionx(strickerPos);
//    this->SetMoveArrowPositions(strickerPos);
    if (this->observer) {
        this->observer->OnStricker_StateChangeTo_PlaceStricker(this);
    }
}

void Stricker::Cmd_TryGrab(const vector2x& pos) {
    this->inputCurrentPos = pos;
    this->inputPrevPos = this->inputCurrentPos;
    this->inputStartPos = this->inputCurrentPos;
    
    auto strickerPosx = GetRBPosition();
    vector2x inputDiffx = strickerPosx - this->inputCurrentPos;
    auto inputDiff_magx = inputDiffx.lengthx();
    auto scaled_inner_radiusx = MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_INNER_RADIUS_SCALE));
    if (this->inputOption <= OPTION3) {
        if (!this->IsGrabed() && inputDiff_magx <  scaled_inner_radiusx) {
            this->SetGrabed(true);
            this->SetMoveMode(true);
        } else if (!this->IsGrabed() && !this->overlapWithCoins) {
            if (this->inputOption == OPTION3) {
                auto scaled_outer_radiusx = MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_OUTER_RADIUS_SCALE));
                if (inputDiff_magx > scaled_inner_radiusx && inputDiff_magx < scaled_outer_radiusx) {
                    this->SetGrabed(true);
                    this->SetAimMode(true);
                }
            } else {
                this->SetGrabed(true);
                this->SetAimMode(true);
            }
        }
    } else if (this->inputOption == OPTION4){
        if (!this->IsGrabed() && inputDiff_magx <  scaled_inner_radiusx && !this->overlapWithCoins) {
            this->SetGrabed(true);
            this->SetAimMode(true);
            this->inputAimWentOutOfinnerCircle = false;
        } else if (!this->IsGrabed()) {
            auto scaled_outer_radiusx = MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_OUTER_RADIUS_SCALE));
            if (inputDiff_magx > scaled_inner_radiusx && inputDiff_magx < scaled_outer_radiusx) {
                this->SetGrabed(true);
                this->SetMoveMode(true);
                this->inputAimWentOutOfinnerCircle = false;
            }
        }
    } else if (this->inputOption == OPTION5) {
        if (!this->IsGrabed() && inputDiff_magx <  scaled_inner_radiusx && !this->overlapWithCoins) {
            this->SetGrabed(true);
            this->SetAimMode(true);
            this->inputAimWentOutOfinnerCircle = false;
        } else if (!this->IsGrabed()) {
            vector2x leftOriginPosx=-(BOARD_SIZEx*FTOX(0.5f));
            vector2x rightMargin = leftOriginPosx+(BOARD_SIZEx*FTOX(1.0f-0.24f));
            vector2x leftMargin = leftOriginPosx+(BOARD_SIZEx*FTOX(0.24f));
            if (!(this->inputCurrentPos.x<leftMargin.x || this->inputCurrentPos.x>rightMargin.x
                  || ABS(this->inputCurrentPos.y - strickerPosx.y)>this->GetRadius())) {
                this->SetGrabed(true);
                this->SetMoveMode(true);
                this->inputAimWentOutOfinnerCircle = false;
            }
//            auto scaled_outer_radiusx = MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_OUTER_RADIUS_SCALE));
//            if (inputDiff_magx > scaled_inner_radiusx && inputDiff_magx < scaled_outer_radiusx) {
//                this->SetGrabed(true);
//                this->SetMoveMode(true);
//                this->inputAimWentOutOfinnerCircle = false;
//            }
        }
    }
}

void Stricker::Cmd_TryShoot(const vector2x& pos, std::function<void()> callback) {
    this->inputCurrentPos = pos;
    
    if (this->inputIsAim) {
        this->Fire();
        callback();
    }
    
    this->SetGrabed(false);
    this->SetAimMode(false);
    this->SetMoveMode(false);
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
    ball.SetRBPosition(position, true);
}

void Stricker::Fire() {
    auto vel = GetRBVelocity();
    auto strickerPos = GetRBPosition();
    
    vector2x diffStartToCurrent = strickerPos - this->inputCurrentPos;
    auto diff_mag = diffStartToCurrent.lengthx();
    if (this->inputOption == OPTION3 || this->inputOption == OPTION4 || this->inputOption==OPTION5) {
        auto scaled_inner_radiusx = MULTX(this->GetRadius(), FTOX(STRICKER_GRAB_INNER_RADIUS_SCALE));
        diff_mag-=scaled_inner_radiusx;
        if (diff_mag<0) {
            diff_mag=0;
        }
    }
    diffStartToCurrent.normalizex();
    
    // 130 is the top power
    const int MAX_ALLOWED_LENGTH = ITOX(MAX_STRICK_LENGHT);
    const int MAX_ALLOWED_POWER = ITOX(MAX_STRICK_POWER);
    const int POWER_DELTA = ITOX(MAX_STRICK_POWER/MAX_STRICK_LENGHT);
    
    diff_mag = MIN(diff_mag, MAX_ALLOWED_LENGTH);
    
    intx scaled_diff = MULTX64(diff_mag, POWER_DELTA);
    scaled_diff = MIN(scaled_diff, MAX_ALLOWED_POWER);
        DEBUG_PRINT("scaled_diff %d", XTOI(scaled_diff));
    //    scaled_diff = FTOX(40000);
    SetRBVelocity(vector2x());
    AddForce(diffStartToCurrent*scaled_diff);
    if (observer) {
        observer->OnStricker_StateChangeTo_Shoot(this);
    }
}

void Stricker::SetStrickerInputOption(STRICKER_INPUT_METHOD option) {
    this->inputOption = option;
    
    if (!this->textureManager) {
        // called from constructor. So no need to load the textures now.
        return;
    }
    
    switch (this->inputOption) {
        case OPTION1:
        case OPTION2:
        case OPTION3:
        {
            this->moveToolSprite.loadTexture(this->textureManager, OSUtils::cpp_getPath("res/sprites/MoveTool_V1.png").c_str());
            this->aimToolSprite.loadTexture(this->textureManager, OSUtils::cpp_getPath("res/sprites/AimTool_V1.png").c_str());
        }
            break;
        case OPTION4:
        case OPTION5:
        {
            this->moveToolSprite.loadTexture(this->textureManager, OSUtils::cpp_getPath("res/sprites/MoveTool_V2.png").c_str());
            this->aimToolSprite.loadTexture(this->textureManager, OSUtils::cpp_getPath("res/sprites/AimTool_V2.png").c_str());
        }
            break;
        default:
            break;
    }
}
