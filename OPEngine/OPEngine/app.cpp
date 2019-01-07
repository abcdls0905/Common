
#include "app.h"

#include "runtime/platform/platform.h"
#include "runtime/render/shader/shader.h"
#include "runtime/model/model.h"

App::App()
{
    m_View = new GLView();
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
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f,  0.5f, 0.0f
}; 

unsigned short indice[] = {0, 1, 2};

void App::Init(int screen_width, int screen_height)
{
    m_View->init();
    m_View->createWindow(screen_width, screen_height);

    pModel = new CModel();
    pModel->m_Mesh = new Mesh();
    MeshData* meshData = new MeshData();
    meshData->CreateShader("assets/shader_1.vert", "assets/shader_1.frag");
    meshData->SetVertex(vertices, 3);
    meshData->SetIndice(indice, 3);
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
