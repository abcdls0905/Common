
#include "app.h"

#include "runtime/platform/platform.h"
#include "runtime/render/shader/shader.h"
#include "runtime/model/model.h"
#include "learnopengl/camera.h"
#include "runtime/util.h"

App::App()
{
    m_View = new GLView();
    m_Camera = new Camera();
}

App::~App()
{
    delete m_View;
    m_View = nullptr;
}

Shader* shader = nullptr;
unsigned int VBO, IBO, VAO;

CModel* pModel = nullptr;

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
}; 

unsigned short indice[] = 
{
    0, 1, 2, 3, 4, 5,
    6, 7, 8, 9, 10, 11,
    12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35,
};

void App::Init(int screen_width, int screen_height)
{
    m_View->init();
    m_View->createWindow(screen_width, screen_height);

    pModel = new CModel();
    pModel->m_Mesh = new Mesh();
    MeshData* meshData = new MeshData();
    meshData->CreateShader("assets/shader_1.vert", "assets/shader_1.frag");
    meshData->SetVertex(vertices, 36, 5);
    meshData->SetIndice(indice, 36);
    meshData->m_Tex = Util::LoadTexture("textures/container.jpg");
    meshData->m_Tex1 = Util::LoadTexture("textures/awesomeface.png");
    meshData->Initialize();
    pModel->m_Mesh->m_Roots.push_back(meshData);
}

void App::Update()
{
    OnFrame();
    OnRender();
    m_View->pollEvents();
}

void App::OnFrame()
{
}

bool App::OnShouldClose()
{
    return m_View->windowShouldClose();
}

void App::BeginRender()
{
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void App::EndRender()
{
    m_View->swapBuffers();
}

void App::OnRender()
{
    BeginRender();

    pModel->Render();

    EndRender();
}

void App::Shutdown()
{

}

bool App::OnMsg(unsigned int msg, size_t param1, size_t param2, int& result)
{
    return true;
}

void App::OnInput(const char* utf8)
{

}
