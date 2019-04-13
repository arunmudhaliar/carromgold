//
//  stricker.hpp
//  GDCPhysics
//
//  Created by Arun A on 03/11/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#ifndef stricker_hpp
#define stricker_hpp

#include "ball.h"
#include "../GUI/Sprite/Sprite2Dx.h"
#include "boardConfig.h"

#include <functional>

#define STRICKER_GRAB_INNER_RADIUS_SCALE  1.5f
#define STRICKER_GRAB_OUTER_RADIUS_SCALE  3.5f
// AIM CONE
#define AIMCONE_ANGLE    180
#define AIMCONE_ANGLE_DELTA    10
#define CONE_VERT_ARRAY_SZ ((AIMCONE_ANGLE/AIMCONE_ANGLE_DELTA)*2 + 2)*2
#define CONE_COLOR_ARRAY_SZ ((AIMCONE_ANGLE/AIMCONE_ANGLE_DELTA)*2 + 2)*4

// AIM RING
#define AIMCONE_RING_ANGLE    180
#define AIMCONE_RING_ANGLE_DELTA    10
#define AIMCONE_RING_VERT_ARRAY_SZ ((AIMCONE_RING_ANGLE/AIMCONE_RING_ANGLE_DELTA)*4 + 2)*2
#define AIMCONE_RING_COLOR_ARRAY_SZ ((AIMCONE_RING_ANGLE/AIMCONE_RING_ANGLE_DELTA)*4 + 2)*4

//GRAB CIRCLE
#define GRAB_CIRCLE_ANGLE    180
#define GRAB_CIRCLE_ANGLE_DELTA    5
#define GRAB_CIRCLE_VERT_ARRAY_SZ ((GRAB_CIRCLE_ANGLE/GRAB_CIRCLE_ANGLE_DELTA)*2 + 2)*2
#define GRAB_CIRCLE_COLOR_ARRAY_SZ ((GRAB_CIRCLE_ANGLE/GRAB_CIRCLE_ANGLE_DELTA)*2 + 2)*4

#define POWER_CIRCLE_ANGLE  180
#define POWER_CIRCLE_ANGLE_DELTA    2
#define POWER_CIRCLE_VERT_ARRAY_SZ ((POWER_CIRCLE_ANGLE/POWER_CIRCLE_ANGLE_DELTA)*2 + 2)*2
#define POWER_CIRCLE_COLOR_ARRAY_SZ ((POWER_CIRCLE_ANGLE/POWER_CIRCLE_ANGLE_DELTA)*2 + 2)*4

class Stricker;
class MStrickerObserver {
public:
    virtual void OnStricker_StateChangeTo_Grab(Stricker*) = 0;
    virtual void OnStricker_StateChangeTo_Aim(Stricker*) = 0;
    virtual void OnStricker_StateChangeTo_Move(Stricker*) = 0;
    virtual void OnStricker_StateChangeTo_Shoot(Stricker*) = 0;
    virtual void OnStricker_Move(Stricker*) = 0;
    virtual void OnStricker_Aim(Stricker*) = 0;
    virtual void OnStricker_StateChangeTo_PlaceStricker(Stricker*) = 0;
};

class Stricker : public Ball {
public:
    enum STRICKER_INPUT_METHOD {
        OPTION1,
        OPTION2,
        OPTION3,
        OPTION_MAX
    };
    
    Stricker();
    virtual ~Stricker();
    void InitStricker(intx size, intx mass, intx frictionfactor, const vector2x& pos, CGETextureManager& textureManager,
                      SoundEngine* soundEnginePtr, const std::string& name, MStrickerObserver* observer=nullptr);
    bool IsOverlap(const vector2x& pos);
    bool IsGrabed() { return this->grabed; }
    bool IsAim()    { return this->inputIsAim; }
    bool IsMove()   { return this->inputIsMove; }
    
    void DrawPreHelperSprites(const matrix4x4f& viewProjection);
    void DrawPostHelperSprites(const matrix4x4f& viewProjection);
    
    void DrawAimCone(const matrix4x4f& viewProjection);
    void DrawAimRing(const matrix4x4f& viewProjection);
    void DrawGrabCircle(const matrix4x4f& viewProjection);
    void DrawPowerCircle(const matrix4x4f& viewProjection);
    
    void UpdateStricker(intx fixedDT);
    
    void Cmd_PlaceStricker();
    void Cmd_TryGrab(const vector2x& pos);
    void Cmd_TryShoot(const vector2x& pos, std::function<void()> callback);
    void Cmd_TryMove(const vector2x& pos);
    
    void SetStrickerInputOption(STRICKER_INPUT_METHOD option) { this->inputOption = option; }
    
    void SetStrickerPosition(const vector2x& pos);
    
    // Remote
    void Remote_SetGrabbed(bool flag);
    void Remote_SetAimMode(bool flag);
    void Remote_SetMoveMode(bool flag);
    
protected:
    void UpdateStricker1(intx fixedDT);
    void UpdateStricker2(intx fixedDT);
    void UpdateStricker3(intx fixedDT);
    
    void OnPostInitBall() override;
    void OnRender(const matrix4x4f& viewProjection) override;
    void OnCollision(std::vector<Collider*>& colliders) override;
    void OnPhysicsUpdate() override;
    void OnStartedMoving() override;
    void OnCameToHalt() override;
    
    void SetGrabed(bool flag);
    void SetAimMode(bool flag);
    void SetMoveMode(bool flag);
    
    void MoveStricker(intx fixedDT, Ball& ball, vector2x& delta);
    void Fire();
    
    void SetMoveArrowPositions(const vector2x& pos);
    bool grabed;
    
    // vars
    Sprite2Dx strickerSprite;
    Sprite2Dx directionSprite;
    Sprite2Dx moveToolSprite;
    Sprite2Dx moveArrows[4];
    
    steTexturePacket* ringTexture;
    
    float moveToolRotation;
    bool inputIsAim;
    bool inputIsMove;
    float powerScale;

    bool breakShot;
    
    STRICKER_INPUT_METHOD inputOption;
    vector2x inputCurrentPos;
    vector2x inputPrevPos;
    vector2x inputStartPos;
    
    MStrickerObserver* observer;
    // Aim cone
    float aimConeVertexBuffer[CONE_VERT_ARRAY_SZ];
    float aimConeColorBuffer[CONE_COLOR_ARRAY_SZ];
    float aimConeLineColorBuffer[CONE_COLOR_ARRAY_SZ];
    
    // Aim ring
    float aimRingVertexBuffer[AIMCONE_RING_VERT_ARRAY_SZ];
    float aimRingColorBuffer[AIMCONE_RING_COLOR_ARRAY_SZ];
    
    // Grab circle
    float grabCircleVertexBuffer[GRAB_CIRCLE_VERT_ARRAY_SZ];
    float grabCircleColorBuffer[GRAB_CIRCLE_COLOR_ARRAY_SZ];
    float grabCircleLineColorBuffer[GRAB_CIRCLE_COLOR_ARRAY_SZ];
    
    // Power circle
    float powerCircleVertexBuffer[POWER_CIRCLE_VERT_ARRAY_SZ];
    float powerCircleColorBuffer[POWER_CIRCLE_COLOR_ARRAY_SZ];
    float powerCircleLineColorBuffer[POWER_CIRCLE_COLOR_ARRAY_SZ];
};
#endif /* stricker_hpp */
