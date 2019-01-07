
#include "app.h"

#include "runtime/platform/platform.h"
#include "runtime/render/shader/shader.h"

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

void App::Init(int screen_width, int screen_height)
{
    m_View->init();
    m_View->createWindow(screen_width, screen_height);

    shader = new Shader("assets/shader_1.vert", "assets/shader_1.frag");
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    }; 
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    unsigned short indice[] = {0, 1, 2, 3};
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indice), indice, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

    glUseProgram(shader->ID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
    
    //clear
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);

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
