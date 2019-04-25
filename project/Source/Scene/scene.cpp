//
//  scene.cpp
//  Copyright 2018 GDCPhysics
//  GDCPhysics
//
//  Created by Arun Mudaliar.
//
//

#include "scene.h"
#include<sstream>
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
#include "../nlohmann/json.hpp"
#include "../sha256/sha256.h"

#define BUILD_VERSION_CODE  1000001
using njson = nlohmann::json;

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
        auto sfxSrc = soundEngine.load(OSUtils::cpp_getPath(sfx.filename));
        if (!sfxSrc) {
            DEBUG_PRINT("SFX load error - %s", sfx.filename.c_str());
        }
    }
    
    board.InitBoard(vector2i(cx, cy), textureManager, &this->soundEngine, this, this);
    
    
    float btn_width = 200.0f;
    float btn_height = 80.0f;
#if SHOW_DEBUG_PANEL
    // DEBUG PANEL
    debugButton1.initButton(worldScale*vector2f(-cx*0.48f+btn_width*0.5f, cy*0.45f-btn_height*0.5f), vector2f(btn_width, btn_height), "test1", [this](){
        this->board.GetPlayerStricker().SetStrickerInputOption(Stricker::OPTION3);
    });
    //debugButton2
    debugButton2.initButton(worldScale*vector2f(cx*0.48f-btn_width*0.5f, cy*0.45f-btn_height*0.5f), vector2f(btn_width, btn_height), "test2", [this](){
        this->board.GetPlayerStricker().SetStrickerInputOption(Stricker::OPTION4);
    });
//    debugButton3.initButton(worldScale*vector2f(-cx*0.48f+btn_width*0.5f,  cy*0.45f-btn_height*0.5f - btn_height*2.5f), vector2f(btn_width, btn_height), "test3", [this](){
//        this->board.GetPlayerStricker().SetStrickerInputOption(Stricker::OPTION3);
//    });
    
    debugBtnList.push_back(&debugButton1);
    debugBtnList.push_back(&debugButton2);
//    debugBtnList.push_back(&debugButton3);
#endif
    
    //sha
//    debugButton4
    debugButton4.initButton(worldScale*vector2f(-cx*0.48f+btn_width*0.5f,  cy*0.4f-btn_height*0.5f - btn_height*2.5f), vector2f(btn_width, btn_height), "shacheck", [this](){
        this->CheckShas();
    });
    debugBtnList.push_back(&debugButton4);
    
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
    
#if SHOW_DEBUG_PANEL
    // Debug panel
    for(auto b : debugBtnList) {
        b->drawButton(*renderer.getViewProjectionMatrix());
    }
    //
#else
    debugButton4.drawButton(*renderer.getViewProjectionMatrix());
#endif
    
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
    geFontManager::g_pFontArial10_84Ptr->drawString(util::stringFormat("STATUS : %s", this->statusMsg.c_str()).c_str(),
                                                    20, yOffset + ++iterator*20, 200);
    
    if (this->board.CanShowBoard()) {
        geFontManager::g_pFontArial10_84Ptr->drawString(this->board.IsMyTurn()?"YOUR TURN":"OPPONENTS TURN",
                                                        windowSize.x*0.5f, yOffset + iterator++*20, 200, true);
    }
    
    geFontManager::g_pFontArial10_84Ptr->drawString(
                    util::stringFormat("FPS : %3.2f", Timer::getFPS()).c_str(), 20, windowSize.y*0.94f, 200);
    geFontManager::g_pFontArial10_84Ptr->drawString(
                    util::stringFormat("ELAPSED : %lu", this->board.GetPhysicsSolver().GetElapsedTime()).c_str(), 20, windowSize.y*0.92f, 200);

    geFontManager::g_pFontArial10_84Ptr->drawString(
                                                    util::stringFormat("SHA : %s", this->sha256Str.c_str()).c_str(), 20, windowSize.y*0.90f, 500);
    geFontManager::g_pFontArial10_84Ptr->drawString(
                                                    util::stringFormat("==>SHA : %s", this->sha256Str_incoming.c_str()).c_str(), 20, windowSize.y*0.88f, 500);
    geFontManager::g_pFontArial10_84Ptr->drawString(
                                                    util::stringFormat("BUILD : %ld", BUILD_VERSION_CODE).c_str(), 20, windowSize.y*0.96f, 500);

    //BUILD_VERSION_CODE

    iterator+=2;
    geFontManager::g_pFontArial10_84Ptr->drawString(
                                                    util::stringFormat("YOU: %s", player1Score.c_str()).c_str(),
                                                    windowSize.x*0.05f, yOffset + iterator*20, 200, 3.0f);
    geFontManager::g_pFontArial10_84Ptr->drawString(
                                                    util::stringFormat("OPPONENT: %s", player2Score.c_str()).c_str(),
                                                    windowSize.x*(0.55f), yOffset + iterator*20, 200, 3.0f);
    
//    geFontManager::g_pFontArial10_84Ptr->drawString(
//                                                    util::stringFormat("<<<< : %s", this->debugShootValReceived.c_str()).c_str(), 20, windowSize.y*0.88f, 500);

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
}

void Scene::SendPing() {
}

#if ENABLE_MULTIPLAYER
void Scene::OnNetworkMessage(const std::string& msg) {
    auto jobj = njson::parse(msg);
    if (jobj.is_discarded()) {
        printf("PARSE ERROR : Invalid incoming msg %s", msg.c_str());
        return;
    }
    
    auto isCommand = jobj.contains("cmd");
    if (!isCommand) {
        printf("Invalid incoming msg %s", msg.c_str());
        return;
    }
    
    auto jcmd = jobj["cmd"];
    if (!jcmd.contains("id")) {
        printf("Invalid incoming msg %s", msg.c_str());
        return;
    }
    
    auto jcmdID = jcmd["id"];
    DEBUG_PRINT("===> json jcmdID:%s", jcmdID.get<std::string>().c_str());
    
    auto gameState = this->board.GetBoardState();
    if (jcmdID == "playerid") {
        if (gameState == Board::GAME_INIT) {
            auto jcmdValue = jcmd["value"];
            if (jcmdValue == "first") {
                this->board.SetPlayerType(Board::PLAYER_FIRST);
            } else if (jcmdValue == "second") {
                this->board.SetPlayerType(Board::PLAYER_SECOND);
            }
            
            njson j;
            j["cmd"]["id"] = "ping";
            NetworkManager::GetInstance().SendMessage(j.dump());
        }
    } else if (jcmdID == "ping") {
        if (gameState == Board::GAME_INIT) {
            njson j;
            j["cmd"]["id"] = "ping_akn";
            NetworkManager::GetInstance().SendMessage(j.dump());
        }
    } else if (jcmdID == "startgame") {
        if (gameState == Board::GAME_INIT) {
            this->board.TryStartGame();
        }
    } else if (jcmdID == "tograb") {
        if (gameState >= Board::GAME_PLAYER_PLACE_STRICKER && gameState <= Board::GAME_FINISH_TURN) {
            auto jcmdValue = jcmd["value"];
            auto px = jcmdValue["px"];
            auto py = jcmdValue["py"];
            intx pxv = px.get<intx>();
            intx pyv = py.get<intx>();
            this->board.GetOpponentStricker().SetStrickerPosition(vector2x(pxv, pyv));
            this->board.GetOpponentStricker().Remote_SetGrabbed(true);
        }
    } else if (jcmdID == "tomove") {
        if (gameState >= Board::GAME_PLAYER_PLACE_STRICKER && gameState <= Board::GAME_FINISH_TURN) {
            auto jcmdValue = jcmd["value"];
            auto px = jcmdValue["px"];
            auto py = jcmdValue["py"];
            intx pxv = px.get<intx>();
            intx pyv = py.get<intx>();
            this->board.GetOpponentStricker().SetStrickerPosition(vector2x(pxv, pyv));
            this->board.GetOpponentStricker().Remote_SetMoveMode(true);
        }
    } else if (jcmdID == "toaim") {
        if (gameState >= Board::GAME_PLAYER_PLACE_STRICKER && gameState <= Board::GAME_FINISH_TURN) {
            auto jcmdValue = jcmd["value"];
            auto px = jcmdValue["px"];
            auto py = jcmdValue["py"];
            intx pxv = px.get<intx>();
            intx pyv = py.get<intx>();
            this->board.GetOpponentStricker().SetStrickerPosition(vector2x(pxv, pyv));
            this->board.GetOpponentStricker().Remote_SetAimMode(true);
        }
    } else if (jcmdID == "toshoot") {
        if (gameState >= Board::GAME_PLAYER_PLACE_STRICKER && gameState <= Board::GAME_FINISH_TURN) {
            this->debugShootValReceived = jobj.dump();
            auto jcmdValue = jcmd["value"];
            auto px = jcmdValue["px"];
            auto py = jcmdValue["py"];
            auto fx = jcmdValue["fx"];
            auto fy = jcmdValue["fy"];
            intx pxv = px.get<intx>();
            intx pyv = py.get<intx>();
            intx fxv = fx.get<intx>();
            intx fyv = fy.get<intx>();
            this->board.GetOpponentStricker().SetStrickerPosition(vector2x(pxv, pyv));
            this->board.Remote_Fire(vector2x(fxv, fyv));
        }
    } else if (jcmdID == "sha") {
        auto jcmdValue = jcmd["value"];
        this->sha256Str_incoming = jcmdValue.get<std::string>();
        this->sha256Array_incoming.clear();
        if (jcmd.contains("array")) {
            auto jcmdArray = jcmd["array"];
            for (int x=0;x<jcmdArray.size();x++) {
                auto data = jcmdArray[x].get<std::string>();
                this->sha256Array_incoming.push_back(data);
            }
        }
    } else if (jcmdID == "turn") {
        if (gameState >= Board::GAME_START && gameState <= Board::GAME_FINISH_TURN) {
            this->board.TryTurnPlayer(true);
        }
    } else if (jcmdID == "turn_op") {
        if (gameState >= Board::GAME_START && gameState <= Board::GAME_FINISH_TURN) {
            this->board.TryTurnOpponent(true);
        }
    } else if (jcmdID == "score") {
        if (gameState >= Board::GAME_START && gameState <= Board::GAME_RESET) {
            auto jcmdValue = jcmd["value"];
            if (jcmdValue.contains("first") && jcmdValue.contains("second")) {
                this->player1Score = util::stringFormat("%d", jcmdValue["first"].get<int>());
                this->player2Score = util::stringFormat("%d", jcmdValue["second"].get<int>());
            }
        }
    }
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

// Stricker
void Scene::OnStricker_StateChangeTo_Grab(Stricker* stricker) {
#if ENABLE_MULTIPLAYER
    if (this->board.GetBoardState() == Board::GAME_PLAYER_FIRE) {
        return;
    }
    if (this->board.IsPlayerStricker(stricker)) {
        auto strickerPos = stricker->GetRBPosition();
        njson j;
        j["cmd"]["id"] = "tograb";
        j["cmd"]["value"]["px"] = strickerPos.x;
        j["cmd"]["value"]["py"] = strickerPos.y;
        DEBUG_PRINT("<=== json %s", j.dump().c_str());
        NetworkManager::GetInstance().SendMessage(j.dump());
    }
#endif
}

void Scene::OnStricker_StateChangeTo_Aim(Stricker* stricker) {
#if ENABLE_MULTIPLAYER
    if (this->board.GetBoardState() == Board::GAME_PLAYER_FIRE) {
        return;
    }
    if (this->board.IsPlayerStricker(stricker)) {
        auto strickerPos = stricker->GetRBPosition();
        njson j;
        j["cmd"]["id"] = "toaim";
        j["cmd"]["value"]["px"] = strickerPos.x;
        j["cmd"]["value"]["py"] = strickerPos.y;
        DEBUG_PRINT("<=== json %s", j.dump().c_str());
        NetworkManager::GetInstance().SendMessage(j.dump());
    }
#endif
}

void Scene::OnStricker_StateChangeTo_Move(Stricker* stricker) {
#if ENABLE_MULTIPLAYER
    if (this->board.GetBoardState() == Board::GAME_PLAYER_FIRE) {
        return;
    }
    if (this->board.IsPlayerStricker(stricker)) {
        auto strickerPos = stricker->GetRBPosition();
        njson j;
        j["cmd"]["id"] = "tomove";
        j["cmd"]["value"]["px"] = strickerPos.x;
        j["cmd"]["value"]["py"] = strickerPos.y;
        DEBUG_PRINT("<=== json %s", j.dump().c_str());
        NetworkManager::GetInstance().SendMessage(j.dump());
    }
#endif
}

void Scene::OnStricker_StateChangeTo_Shoot(Stricker* stricker) {
#if ENABLE_MULTIPLAYER
    if (this->board.GetBoardState() == Board::GAME_PLAYER_FIRE) {
        return;
    }
    if (this->board.IsPlayerStricker(stricker)) {
        auto strickerPos = stricker->GetRBPosition();
        auto strickerForce = stricker->GetRBForce();
        njson j;
        j["cmd"]["id"] = "toshoot";
        j["cmd"]["value"]["px"] = strickerPos.x;
        j["cmd"]["value"]["py"] = strickerPos.y;
        j["cmd"]["value"]["fx"] = strickerForce.x;
        j["cmd"]["value"]["fy"] = strickerForce.y;
        DEBUG_PRINT("<=== json %s", j.dump().c_str());
        this->debugShootValSend = j.dump();
        NetworkManager::GetInstance().SendMessage(j.dump());
    }
#endif
}

void Scene::OnStricker_StateChangeTo_PlaceStricker(Stricker* stricker) {
#if ENABLE_MULTIPLAYER
    if (this->board.GetBoardState() == Board::GAME_PLAYER_FIRE) {
        return;
    }
    
    this->SendSha();
#endif
}
    
void Scene::OnStricker_Move(Stricker* stricker) {
    
}

void Scene::OnStricker_Aim(Stricker* stricker) {
    
}

void Scene::OnFinishTurn() {    
#if ENABLE_MULTIPLAYER
    this->sha256Array.clear();
    this->sha256Source = GetCurrentSha();
    this->sha256Str = sha256(this->sha256Source);
    njson j;
    j["cmd"]["id"] = "turnend";
    auto jdump = j.dump();
    j["cmd"]["value"] = this->sha256Str;
    NetworkManager::GetInstance().SendMessage(jdump);
#endif
}

void Scene::OnPocketed(Ball& coin) {
#if ENABLE_MULTIPLAYER
    if (this->board.IsMyTurn()) {
        njson j;
        j["cmd"]["id"] = "goal";
        auto jdump = j.dump();
        j["cmd"]["value"] = coin.GetRBName();
        NetworkManager::GetInstance().SendMessage(jdump);
    }
#endif
}
    
const std::string Scene::GetCurrentSha() {
    std::stringstream s;
    for(auto c : this->board.GetCoins()) {
        auto data = c.second->ToString();
        s << data;
        //        jsonarray.push_back(data);
        //        sha256Array.push_back(data);
    }
    return s.str();
}

#if ENABLE_MULTIPLAYER
void Scene::SendSha() {
    this->sha256Array.clear();
    this->sha256Source = GetCurrentSha();
    this->sha256Str = sha256(this->sha256Source);
    auto jsonarray = njson::array();
    DEBUG_PRINT("sha256 - %s", sha256Str.c_str());
    
    //    if (this->board.IsPlayerStricker(stricker)) {
    njson j;
    j["cmd"]["id"] = "sha";
    j["cmd"]["value"] = sha256Str;
    j["cmd"]["array"] = jsonarray;
    auto jdump = j.dump();
    NetworkManager::GetInstance().SendMessage(jdump);
    //    }
}
#endif

bool Scene::CheckShas() {
    if (sha256Str_incoming != sha256Str) {
        DEBUG_PRINT("!!!!!!!!!!!!! SHA MISMATCH !!!!!!!!!!!!!");
        int arraySZ = (int)MIN(sha256Array.size(), sha256Array_incoming.size());
        int reminderSZ = 0;
        if (sha256Array.size()!=sha256Array_incoming.size()) {
            DEBUG_PRINT("ARRAY size mismatch src.sz:%d, remote.sz:%d", sha256Array.size(), sha256Array_incoming.size());
            reminderSZ = (int)ABS(sha256Array.size() - sha256Array_incoming.size());
        }
        
        for (int x=0;x<arraySZ;x++) {
            if (sha256Array[x]!=sha256Array_incoming[x]) {
                DEBUG_PRINT("SHA256 ERR LINE(%d) ===> %s", x, sha256Array_incoming[x].c_str());
            }
        }
        
        if (reminderSZ) {
            std::vector<std::string>* ptr = nullptr;
            if (sha256Array.size()>0) {
                ptr = &sha256Array;
            } else {
                ptr = &sha256Array_incoming;
            }
            
            DEBUG_PRINT("----REMINDER LINES----");
            for (int x=reminderSZ;x<ptr->size();x++) {
                DEBUG_PRINT(ptr->at(x).c_str());
            }
        }
        return false;
    }
    
    return true;
}
