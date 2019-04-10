//
//  scene.cpp
//  Copyright 2018 GDCPhysics
//  GDCPhysics
//
//  Created by Arun Mudaliar.
//
//

#include "scene.h"
#if GEAR_APPLE
    #include <OpenGL/gl.h>
#elif GEAR_ANDROID
    #include <GLES2/gl2.h>
#else
    #include <gl.h>
#endif
#include "../Physics/core/Timer.h"
#include "../Physics/core/util.h"
#include "../GUI/OSUtils.h"

Scene& Scene::GetInstance() {
    static Scene instance;
    return instance;
}

Scene::Scene() {
#if GEAR_APPLE
    this->window = nullptr;
#endif
    this->statusMsg = "Connecting...";
    this->player1Score = "0";
    this->player2Score = "0";
}

Scene::~Scene() {
    soundEngine.destroyEngine();
}

#if GEAR_APPLE
void Scene::InitScene(SDL_Window* window, float cx, float cy) {
    this->window = window;
#else
void Scene::InitScene(float cx, float cy) {
#endif
    cx*=0.5f; cy*=0.5f;     // for retina display.
    windowSize.set(cx, cy);
    Timer::init();
    
    renderer.setViewPort(0, 0, cx, cy);
    HWShaderManager::GetHWShaderManager().Init();
    
    geFontManager::g_cFontManager.setRenderer(&renderer);
    geFontManager::InitializeFonts();
    
    Resize(cx*2.0f, cy*2.0f);
    
    // load default states for ogl
    InternalGLStates();
    
    // load default shader vals
    auto guishader = HWShaderManager::GetHWShaderManager().GetHWShader(2);
    guishader->enableProgram();
    const float* u_mvp_m4x4= renderer.getViewProjectionMatrix()->getMatrix();
    guishader->sendUniformTMfv("u_mvp_m4x4", u_mvp_m4x4, false, 4);
    guishader->sendUniform2f("u_uvtile", 1.0f, 1.0f);
    guishader->disableProgram();
    //

    soundEngine.init();
    for (auto sfx : sfxSounds) {
        auto sfxSrc = soundEngine.load(cpp_getPath(sfx.filename));
        if (!sfxSrc) {
            DEBUG_PRINT("SFX load error - %s", sfx.filename.c_str());
        }
    }
    
    board.InitBoard(vector2i(cx, cy), textureManager, &this->soundEngine);
    
    
    // DEBUG PANEL
    float btn_width = 200.0f;
    float btn_height = 80.0f;
    debugButton1.initButton(worldScale*vector2f(-cx*0.48f+btn_width*0.5f, cy*0.45f-btn_height*0.5f), vector2f(btn_width, btn_height), "test1", [this](){
        this->board.GetPlayerStricker().SetStrickerInputOption(Stricker::OPTION1);
    });
    //debugButton2
    debugButton2.initButton(worldScale*vector2f(cx*0.48f-btn_width*0.5f, cy*0.45f-btn_height*0.5f), vector2f(btn_width, btn_height), "test2", [this](){
        this->board.GetPlayerStricker().SetStrickerInputOption(Stricker::OPTION2);
    });
    debugButton3.initButton(worldScale*vector2f(-cx*0.48f+btn_width*0.5f,  cy*0.45f-btn_height*0.5f - btn_height*2.5f), vector2f(btn_width, btn_height), "test3", [this](){
        this->board.GetPlayerStricker().SetStrickerInputOption(Stricker::OPTION3);
    });
    
    debugBtnList.push_back(&debugButton1);
    debugBtnList.push_back(&debugButton2);
    debugBtnList.push_back(&debugButton3);
    
#if ENABLE_MULTIPLAYER
    NetworkManager::GetInstance().InitNetwork(this);
#endif
}

void Scene::InternalGLStates() {
    glClearColor(1.0f, 0.957f, 0.9f, 1.0f);
#if GEAR_APPLE
    glClearDepth(1.0f);                                 // Depth Buffer Setup
#else
    glClearDepthf(1.0f);                                 // Depth Buffer Setup
#endif
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);                             // Enable culling
    glDepthFunc(GL_LEQUAL);                             // The Type Of Depth Testing To Do (GL_LEQUAL is must for shadow fonts)
    glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
}

void Scene::Resize(float cx, float cy) {
    cx*=0.5f; cy*=0.5f;     // for retina display.
    windowSize.set(cx, cy);

    renderer.setViewPort(0, 0, cx, cy);
    gxRectf viewportRect(renderer.getViewPortRect());
    vector2f centerAlignedPos(viewportRect.m_pos-viewportRect.m_size*0.5f);
    projectionMatrix.setOrtho(centerAlignedPos.x, centerAlignedPos.x+viewportRect.m_size.x, centerAlignedPos.y, centerAlignedPos.y+viewportRect.m_size.y, 0, 100);
    renderer.setProjectionMatrixToGL(&projectionMatrix);

    // for GUI fonts
    matrix4x4f orthoMatrix;
    orthoMatrix.setOrtho(0, cx, cy, 0, 0, 100);
    //
    
    float scale = BOARD_SIZE.x/cx;
    worldScale.setScale(scale, scale, scale);
    inverseTransformationMatrix = worldScale.getInverse();
    viewProjectionMatrix = projectionMatrix * inverseTransformationMatrix;
    
    renderer.setOrthoProjectionMatrix(&orthoMatrix);
    renderer.setViewMatrixToGL(&inverseTransformationMatrix);
    renderer.setViewProjectionMatrix(&viewProjectionMatrix);
    
#if !(USE_ProgrammablePipeLine)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(centerAlignedPos.x, centerAlignedPos.x+viewportRect.m_size.x, centerAlignedPos.y, centerAlignedPos.y+viewportRect.m_size.y, 0.0f, 10.0f);
//    glOrtho(0.0f, (int)cx, 0.0f, (int)cy, 0.0f, 10.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#endif
}

vector2x Scene::WindowToBoard(const vector2x& windowCoord) {
    gxRectf viewportRect(renderer.getViewPortRect());
    vector2f boardCenterf(viewportRect.m_size*0.5f - viewportRect.m_pos);
    vector2x boardCenterx(FTOX(boardCenterf.x), FTOX(boardCenterf.y));
    //boardCenterx-=(BOARD_OFFSETx*FTOX(0.5f));
    
    float scale = BOARD_SIZE.x/this->windowSize.x;
    
    return (windowCoord - boardCenterx)*scale;
}

void Scene::Update() {
    Timer::update();
    this->board.UpdateBoard();
}

void Scene::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
#if !USE_ProgrammablePipeLine
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
#endif
    
    this->board.DrawBoard(*renderer.getViewProjectionMatrix());
    
     DrawStats();
    
    // Debug panel
    for(auto b : debugBtnList) {
        b->drawButton(*renderer.getViewProjectionMatrix());
    }
    //
    
#if !USE_ProgrammablePipeLine
    glPopMatrix();
#endif
}

void Scene::DrawStats() {
    if (geFontManager::g_pFontArial10_84Ptr == nullptr) {
        return;
    }
    
#if !USE_ProgrammablePipeLine
    // Stats
    glPushMatrix();
    glScalef(1, -1, 1);
#endif
    int iterator = 0;
    int yOffset = 20;
#if GEAR_ANDROID
    yOffset = 80;
#endif
//    geFontManager::g_pFontArial10_84Ptr->drawString(util::stringFormat("FPS %3.2f", Timer::getFPS()).c_str(), 45, -(60+(iterator++)*20), 200);
//    geFontManager::g_pFontArial10_84Ptr->drawString(util::stringFormat("PING %d ms", this->pingTimeFromOtherPlayer).c_str(), 45, -(60+(iterator++)*20), 200);
//    if (this->gameState == GAME_START) {
//        geFontManager::g_pFontArial10_84Ptr->drawString((this->playerType == PLAYER_FIRST) ? "PLAYER 1" : "PLAYER 2", 45, -(60+(iterator++)*20), 200);
//    }
//    geFontManager::g_pFontArial10_84Ptr->drawString(util::stringFormat("ELAPSED %lu ms", this->physicsSolver.GetElapsedTime()).c_str(), 45, -(60+(iterator++)*20), 200);
    geFontManager::g_pFontArial10_84Ptr->drawString(util::stringFormat("STATUS : %s", this->statusMsg.c_str()).c_str(), 20, yOffset + ++iterator*20, 200);
    
    geFontManager::g_pFontArial10_84Ptr->drawString(this->board.IsMyTurn()?"YOUR TURN":"OPPONENTS TURN", windowSize.x*0.5f, yOffset + iterator*20, 200, true);
    
    geFontManager::g_pFontArial10_84Ptr->drawString(util::stringFormat("FPS : %3.2f", Timer::getFPS()).c_str(), 20, windowSize.y*0.94f, 200);
//    geFontManager::g_pFontArial10_84Ptr->drawString(util::stringFormat("BALL VEL : %d", XTOI(this->ball.GetRBVelocity().lengthx())).c_str(), 45, -(60+(iterator++)*20), 200);

//        geFontManager::g_pFontArial10_84Ptr->drawString(util::stringFormat("PLAYER 1: %s", player1Score.c_str()).c_str(), 0, 0, 200);
//    geFontManager::g_pFontArial10_84Ptr->drawString(util::stringFormat("PLAYER 1: %s", player1Score.c_str()).c_str(), windowSize.x*0.09f, -(windowSize.y*0.85f), 200);
//    geFontManager::g_pFontArial10_84Ptr->drawString(util::stringFormat("PLAYER 2: %s", player2Score.c_str()).c_str(), windowSize.x*0.82f, -(windowSize.y*0.85f), 200);

//    if (this->stricker.IsGrabed()) {
//        geFontManager::g_pFontArial10_84Ptr->drawString(util::stringFormat("GRABBED").c_str(), windowSize.x*0.82f, -(windowSize.y*0.8f), 200);
//    }
    
#if !USE_ProgrammablePipeLine
    glPopMatrix();
    //
#endif
}

void Scene::MouseBtnUp(int mx, int my) {
    // Debug panel
    bool consumedByGUI = false;
    auto w2bPos = WindowToBoard(vector2x(ITOX(mx), ITOX(windowSize.y-my)));
    for(auto b : debugBtnList) {
        if (b->mouseUp(vector2f(XTOF(w2bPos.x), XTOF(w2bPos.y)))) {
            consumedByGUI = true;
        }
    }
    //
    
    if (!consumedByGUI) {
        this->board.MouseBtnUp(WindowToBoard(vector2x(ITOX(mx), ITOX(windowSize.y-my))));
    }
}

void Scene::MouseBtnDown(int mx, int my) {
    // Debug panel
    bool consumedByGUI = false;
    auto w2bPos = WindowToBoard(vector2x(ITOX(mx), ITOX(windowSize.y-my)));
    for(auto b : debugBtnList) {
        if (b->mouseDown(vector2f(XTOF(w2bPos.x), XTOF(w2bPos.y)))) {
            consumedByGUI = true;
        }
    }
    //
    
    if (!consumedByGUI) {
        this->board.MouseBtnDown(WindowToBoard(vector2x(ITOX(mx), ITOX(windowSize.y-my))));
    }
}

void Scene::MouseMove(int mx, int my) {
    this->board.MouseMove(WindowToBoard(vector2x(ITOX(mx), ITOX(windowSize.y-my))));
    
    // Debug panel
    auto w2bPos = WindowToBoard(vector2x(ITOX(mx), ITOX(windowSize.y-my)));
    for(auto b : debugBtnList) {
        b->mouseMove(vector2f(XTOF(w2bPos.x), XTOF(w2bPos.y)));
    }
    //
}

void Scene::StartGameFromMenu() {
//    if (this->gameState == GAME_INIT || this->gameState == GAME_RESET) {
////        NetworkManager::GetInstance().SendMessage("ping");
//    }
}

void Scene::SendPing() {
//    if (this->gameState == GAME_START ) {
////        NetworkManager::GetInstance().SendMessage("ping");
//    }
}

#if ENABLE_MULTIPLAYER
void Scene::OnNetworkMessage(const std::string& msg) {
//#if 0
//    printf("Msg from other player %s\n", msg.c_str());
    auto gameState = this->board.GetBoardState();
    if (msg == "first" || msg == "second") {
        if (gameState == Board::GAME_INIT || gameState == Board::GAME_RESET) {
//            this->physicsSolver.RemoveBoxCollider(&player1);
//            this->physicsSolver.RemoveBoxCollider(&player2);
            if (msg == "first") {
                this->board.SetPlayerType(Board::PLAYER_FIRST);
//                this->physicsSolver.AddBoxCollider(&player1);
            } else if (msg == "second") {
                this->board.SetPlayerType(Board::PLAYER_SECOND);
//                this->physicsSolver.AddBoxCollider(&player2);
            }
            NetworkManager::GetInstance().SendMessage("ping");
//            pingTimeFromOtherPlayer = 0;
//            pingStartTime = Timer::getCurrentTimeInMilliSec();
        }
    } else if (msg == "ping") {
//        pingTimeFromOtherPlayer = Timer::getCurrentTimeInMilliSec()-pingStartTime;
//        printf("PING TIME %lu ms.\n", pingTimeFromOtherPlayer);
        if (gameState == Board::GAME_INIT || gameState == Board::GAME_RESET) {
            NetworkManager::GetInstance().SendMessage("ping_akn");
        }
    } else if (msg == "startgame") {
        if (gameState == Board::GAME_INIT || gameState == Board::GAME_RESET) {
            this->board.TryStartGame();
        }
    }
    
    /*else if (msg == "boost") {
        ApplyBoost(0, 0);
    } else {
        std::vector<std::string> lines;
        util::splitString(msg, lines);
        if (lines.size()) {
            if (lines[0] == "ball" && lines.size()==2) {
                std::vector<std::string> args;
                util::splitString(lines[1], args, ',');
                if (args.size()==4) {
                    intx px = atoi(args[0].c_str());
                    intx py = atoi(args[1].c_str());
                    intx vx = atoi(args[2].c_str());
                    intx vy = atoi(args[3].c_str());
                    
                    this->stricker.SetRBVelocity(vector2x(vx, vy));
                    this->stricker.SetRBPosition(vector2x(px, py));
                }
            } else if (lines[0] == "stricker" && lines.size()==2) {
                std::vector<std::string> args;
                util::splitString(lines[1], args, ',');
                if (args.size()==2) {
                    intx fx = atoi(args[0].c_str());
                    intx fy = atoi(args[1].c_str());
                    this->stricker.AddForce(vector2x(fx, fy));
                }
            }
//            else if (lines[0] == "stricker_up" && lines.size()==4) {
//                int val = atoi(lines[1].c_str());
//                this->remoteInputMoveUp = (val==1);
//                std::vector<std::string> args;
//                util::splitString(lines[2], args, ',');
//                if (args.size()==2) {
//                    intx px = atoi(args[0].c_str());
//                    intx py = atoi(args[1].c_str());
//                    if (this->playerType == PLAYER_FIRST) {
//                        this->player2.SetPosition(vector2x(px, py));
//                    } else if (this->playerType == PLAYER_SECOND) {
//                        this->player1.SetPosition(vector2x(px, py));
//                    }
//                }
//                unsigned long remoteElapsedTime = atol(lines[3].c_str());
//                remoteElapsedTime;
//            } else if (lines[0] == "stricker_down" && lines.size()==4) {
//                int val = atoi(lines[1].c_str());
//                this->remoteInputMoveDown = (val==1);
//                std::vector<std::string> args;
//                util::splitString(lines[2], args, ',');
//                if (args.size()==2) {
//                    intx px = atoi(args[0].c_str());
//                    intx py = atoi(args[1].c_str());
//                    if (this->playerType == PLAYER_FIRST) {
//                        this->player2.SetPosition(vector2x(px, py));
//                    } else if (this->playerType == PLAYER_SECOND) {
//                        this->player1.SetPosition(vector2x(px, py));
//                    }
//                }
//                unsigned long remoteElapsedTime = atol(lines[3].c_str());
//                remoteElapsedTime;
//            }
            else if (lines[0] == "score" && lines.size()==2) {
                std::vector<std::string> args;
                util::splitString(lines[1], args, ',');
                if (args.size()==2) {
                    player1Score = args[0];
                    player2Score = args[1];
                }
            }
        }
    }
     */
//#endif
}

void Scene::OnNetworkFail() {
    statusMsg = "Network Error...";
}

void Scene::OnNetworkOpen() {
//    NetworkManager::GetInstance().SendMessage("Hello from other player.");
    statusMsg = "CONNECTED";
}

void Scene::OnNetworkClose() {
    statusMsg = "DISCONECTED";
//    SetGameState(GAME_RESET);
}
#endif
