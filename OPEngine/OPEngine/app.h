
#pragma once

class View;
#include "learnopengl/camera.h"
#include "runtime/model/model.h"
#include "runtime/skybox.h"
#include "runtime/render/shader/shader.h"
#include <vector>

class App
{
public:
    View* m_View;
    Camera* m_Camera;
    std::vector<CModel*> m_Models;
    SkyBox* m_SkyBox;
    float SCR_WIDTH;
    float SCR_HEIGHT;
    Shader* simpleDepthShader;
    bool m_IsRenderDepth;
public:
    App();
    ~App();
    void Init(int screen_width, int screen_height);
    void Update();
    void OnFrame();
    bool OnShouldClose();
    void BeginRender();
    void NormalRender();
    void OnRender();
    void EndRender();
    void Shutdown();
    bool OnMsg(unsigned int msg, size_t param1, size_t param2, int& result);
    void OnInput(const char* utf8);
    bool IsRenderDepth(){return m_IsRenderDepth;}
    static App* Inst();
    static App* m_Inst;
};