//
//  boardConfig.h
//  GDCPhysics
//
//  Created by Arun A on 06/04/19.
//  Copyright © 2019 Arun A. All rights reserved.
//

#ifndef boardConfig_h
#define boardConfig_h

#include <string>

#define BOARD_SIZE vector2f(600, 600)
#define BOARD_SIZEx vector2x(ITOX(600), ITOX(600))

#define BOARD_OFFSET vector2f(0, 60)
#define BOARD_OFFSETx vector2x(0, ITOX(60))

#define MAX_STRICK_LENGHT   130
#define MAX_STRICK_POWER   50000
#define MAX_COIN_VELOCITY   1000
#define MAX_COIN_VELOCITYx   ITOX(MAX_COIN_VELOCITY)

enum SFX_SOUND {
    sfx_breakshot_1,
    sfx_breakshot_2,
    sfx_puck_hits_edge_1,
    sfx_puck_hits_edge_2,
    sfx_puck_hits_edge_3,
    sfx_puck_hits_edge_4,
    sfx_puck_hits_edge_5,
    sfx_puck_hits_puck_1,
    sfx_puck_hits_puck_2,
    sfx_puck_hits_puck_3,
    sfx_puck_hits_puck_4,
    sfx_puck_hits_puck_5,
    sfx_puck_pot_1,
    sfx_puck_pot_2,
    sfx_puck_pot_3,
    sfx_puck_pot_4,
    sfx_striker_drag_LOOP,
    sfx_striker_hits_puck_1,
    sfx_striker_hits_puck_2,
    sfx_striker_hits_puck_3,
    sfx_striker_hits_puck_4,
    sfx_striker_hits_puck_5,
    sfx_striker_shoot_1,
    sfx_striker_shoot_2,
    sfx_striker_shoot_3,
};

struct sfxStruct {
    SFX_SOUND sfxID;
    std::string filename;
};

static sfxStruct sfxSounds[] = {
    {sfx_breakshot_1, "/res/sound/sfx_breakshot_1.gsx"},
    {sfx_breakshot_2, "/res/sound/sfx_breakshot_2.gsx"},
    {sfx_puck_hits_edge_1, "/res/sound/sfx_puck_hits_edge_1.gsx"},
    {sfx_puck_hits_edge_2, "/res/sound/sfx_puck_hits_edge_2.gsx"},
    {sfx_puck_hits_edge_3, "/res/sound/sfx_puck_hits_edge_3.gsx"},
    {sfx_puck_hits_edge_4, "/res/sound/sfx_puck_hits_edge_4.gsx"},
    {sfx_puck_hits_edge_5, "/res/sound/sfx_puck_hits_edge_5.gsx"},
    {sfx_puck_hits_puck_1, "/res/sound/sfx_puck_hits_puck_1.gsx"},
    {sfx_puck_hits_puck_2, "/res/sound/sfx_puck_hits_puck_2.gsx"},
    {sfx_puck_hits_puck_3, "/res/sound/sfx_puck_hits_puck_3.gsx"},
    {sfx_puck_hits_puck_4, "/res/sound/sfx_puck_hits_puck_4.gsx"},
    {sfx_puck_hits_puck_5, "/res/sound/sfx_puck_hits_puck_5.gsx"},
    {sfx_puck_pot_1, "/res/sound/sfx_puck_pot_1.gsx"},
    {sfx_puck_pot_2, "/res/sound/sfx_puck_pot_2.gsx"},
    {sfx_puck_pot_3, "/res/sound/sfx_puck_pot_3.gsx"},
    {sfx_puck_pot_4, "/res/sound/sfx_puck_pot_4.gsx"},
    {sfx_striker_drag_LOOP, "/res/sound/sfx_striker_drag_LOOP.gsx"},
    {sfx_striker_hits_puck_1, "/res/sound/sfx_striker_hits_puck_1.gsx"},
    {sfx_striker_hits_puck_2, "/res/sound/sfx_striker_hits_puck_2.gsx"},
    {sfx_striker_hits_puck_3, "/res/sound/sfx_striker_hits_puck_3.gsx"},
    {sfx_striker_hits_puck_4, "/res/sound/sfx_striker_hits_puck_4.gsx"},
    {sfx_striker_hits_puck_5, "/res/sound/sfx_striker_hits_puck_5.gsx"},
    {sfx_striker_shoot_1, "/res/sound/sfx_striker_shoot_1.gsx"},
    {sfx_striker_shoot_2, "/res/sound/sfx_striker_shoot_2.gsx"},
    {sfx_striker_shoot_3, "/res/sound/sfx_striker_shoot_3.gsx"}
};

#endif /* boardConfig_h */
