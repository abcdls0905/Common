
#include "app.h"

#include "runtime/platform/platform.h"
#include "runtime/model/model.h"
#include "learnopengl/camera.h"
#include "runtime/util.h"
#include "vertdata.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define POST_PROCESS1
#define SHADOWMAP

const int SHADOWMAPSIZE = 512;

App* App::m_Inst = nullptr;

App* App::Inst()
{
    if (m_Inst == nullptr)
        m_Inst = new App();
    return m_Inst;
}

App::App()
{
    m_View = new GLView();
    m_Camera = new Camera(glm::vec3(0.0f, 5.0f, 6.0f));
    m_IsRenderDepth = false;
}

App::~App()
{
    delete m_View;
    m_View = nullptr;
}

Shader* shader = nullptr;
unsigned int VBO, IBO, VAO;

#ifdef POST_PROCESS

unsigned int framebuffer;
unsigned int intermediateFBO;
unsigned int quadVAO, quadVBO;
Shader* screenShader;
unsigned int screenTexture;

#endif

#ifdef SHADOWMAP

unsigned int depthMap;
unsigned int depthMap1;
unsigned int depthMapFBO;
unsigned int depthMapFBO1;
Shader* debugDepthQuad;

#endif

void App::Init(int screen_width, int screen_height)
{
    SCR_WIDTH = screen_width;
    SCR_HEIGHT = screen_height;
    m_IsFront = false;
    near_plane  = 0.1f;
    far_plane= 15.0f;
    m_View->init();
    m_View->createWindow(screen_width, screen_height);

//    glDepthRange(0.0f, 1.0f);
#ifdef POST_PROCESS
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a multisampled color attachment texture
    unsigned int textureColorBufferMultiSampled;
    glGenTextures(1, &textureColorBufferMultiSampled);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
    // create a (also multisampled) renderbuffer object for depth and stencil attachments
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        exit(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // configure second post-processing framebuffer
    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
    // create a color attachment texture
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        exit(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertData::quadVertices), VertData::quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    screenShader = new Shader("assets/post.vert", "assets/post.frag");
#endif

#ifdef SHADOWMAP
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWMAPSIZE, SHADOWMAPSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

    glGenFramebuffers(1, &depthMapFBO1);
    glGenTextures(1, &depthMap1);
    glBindTexture(GL_TEXTURE_2D, depthMap1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWMAPSIZE, SHADOWMAPSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO1);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap1, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    simpleDepthShader = new Shader("assets/dpdepth.vert", "assets/dpdepth.frag");
    debugDepthQuad = new Shader("assets/debug_depth.vert", "assets/debug_depth.frag");
#endif
    //skybox
    m_SkyBox = new SkyBox();
    m_SkyBox->Initialize();

    CModel* pModel = nullptr;
    MeshData* meshData = nullptr;

    //box
    pModel = new CModel();
    pModel->m_Mesh = new Mesh();
    meshData = new MeshData();
    meshData->CreateShader("assets/shader_1.vert", "assets/shader_1.frag");
    meshData->SetVertex(VertData::vertices, 36, 8);
    meshData->SetIndice(VertData::indice, 36);
    meshData->m_Tex = Util::LoadTexture("textures/container2.png");
    meshData->m_Tex1 = Util::LoadTexture("textures/container2_specular.png");
    meshData->m_TexShadow = depthMap;
    meshData->m_TexShadow1 = depthMap1;
    meshData->Initialize();
    meshData->m_Pos = glm::vec3(1, 0, 0);
    pModel->m_Mesh->m_Roots.push_back(meshData);
    m_Models.push_back(pModel);

    //box
    pModel = new CModel();
    pModel->m_Mesh = new Mesh();
    meshData = new MeshData();
    meshData->CreateShader("assets/shader_1.vert", "assets/shader_1.frag");
    meshData->SetVertex(VertData::vertices, 36, 8);
    meshData->SetIndice(VertData::indice, 36);
    meshData->m_Tex = Util::LoadTexture("textures/container2.png");
    meshData->m_Tex1 = Util::LoadTexture("textures/container2_specular.png");
    meshData->m_TexShadow = depthMap;
    meshData->m_TexShadow1 = depthMap1;
    meshData->Initialize();
    meshData->m_Pos = glm::vec3(5, 0, -5);
    pModel->m_Mesh->m_Roots.push_back(meshData);
    m_Models.push_back(pModel);

    //fly box
    pModel = new CModel();
    pModel->m_Mesh = new Mesh();
    meshData = new MeshData();
    meshData->CreateShader("assets/shader_1.vert", "assets/shader_1.frag");
    meshData->SetVertex(VertData::vertices, 36, 8);
    meshData->SetIndice(VertData::indice, 36);
    meshData->m_Tex = Util::LoadTexture("textures/container2.png");
    meshData->m_Tex1 = Util::LoadTexture("textures/container2_specular.png");
    meshData->m_TexShadow = depthMap;
    meshData->m_TexShadow1 = depthMap1;
    meshData->Initialize();
    meshData->m_Pos = glm::vec3(0, 2, -1.5);
    pModel->m_Mesh->m_Roots.push_back(meshData);
    m_Models.push_back(pModel);

    Shader* tempShader = new Shader("assets/shader_1.vert", "assets/shader_1.frag");
    unsigned int tempTex = Util::LoadTexture("textures/wood.png");

    for (int i = -0; i < 1; i++)
    {
        for (int j = -0; j < 1; j++)
        {
            //plane
            pModel = new CModel();
            pModel->m_Mesh = new Mesh();
            meshData = new MeshData();
            meshData->m_Shader = tempShader;
            meshData->SetVertex(VertData::planeVertices, 6, 8);
            meshData->SetIndice(VertData::planeIndice, 6);
            meshData->m_Tex = tempTex;
            meshData->m_TexShadow = depthMap;
            meshData->m_TexShadow1 = depthMap1;
            meshData->Initialize();
            meshData->m_Pos = glm::vec3(i, 0, j);
            pModel->m_Mesh->m_Roots.push_back(meshData);
            m_Models.push_back(pModel);
        }
    }

    //point light
    pModel = new CModel();
    pModel->m_Mesh = new Mesh();
    meshData = new MeshData();
    meshData->CreateShader("assets/shader_1.vert", "assets/shader_1.frag");
    meshData->SetVertex(VertData::pointLight, 6, 8);
    meshData->SetIndice(VertData::planeIndice, 6);
    meshData->m_Tex = Util::LoadTexture("textures/toy_box_normal.png");
    meshData->m_TexShadow = depthMap;
    meshData->m_TexShadow1 = depthMap1;
    meshData->Initialize();
    //glm::vec3 light_point_pos(-1.5f, 0.5f, 0);
    meshData->m_Pos = glm::vec3(0, 0.5f, 0);
    pModel->m_Mesh->m_Roots.push_back(meshData);
    //m_Models.push_back(pModel);

    tempShader = new Shader("assets/cubemap.vert", "assets/cubemap.frag");
    tempTex = Util::loadCubemap();
    unsigned int tempTex1 = Util::LoadTexture("textures/container2.png");

    for (int i = 0; i < 30; i++)
    {
        //refract
        pModel = new CModel();
        pModel->m_Mesh = new Mesh();
        meshData = new MeshData();
        meshData->m_Shader = tempShader;
        meshData->SetVertex(VertData::cubeVertices, 36, 6);
        meshData->SetIndice(VertData::indice, 36);
        meshData->m_Tex = tempTex;
        meshData->m_Tex1 = tempTex1;
        meshData->m_TexShadow = depthMap;
        meshData->m_TexShadow1 = depthMap1;
        meshData->Initialize();
        meshData->m_Pos = glm::vec3(-1, 0, 0.2*i - 3);
        meshData->m_IsCube = true;
        pModel->m_Mesh->m_Roots.push_back(meshData);
        //m_Models.push_back(pModel);
    }
    pModel = new CModel();
    pModel->m_Mesh = new Mesh();
    meshData = new MeshData();

    meshData->m_Shader = tempShader;
    meshData->SetVertex(VertData::cubeVertices, 36, 6);
    meshData->SetIndice(VertData::indice, 36);
    meshData->m_Tex = tempTex;
    meshData->m_Tex1 = tempTex1;
    meshData->m_TexShadow = depthMap;
    meshData->m_TexShadow1 = depthMap1;
    meshData->Initialize();
    meshData->m_Pos = glm::vec3(-3, 0, -3);
    meshData->m_IsCube = true;
    pModel->m_Mesh->m_Roots.push_back(meshData);
    m_Models.push_back(pModel);

    //water
    {
        const int count = 50;
        const int size = 8;
        const int trianle = 6;
        int height = 10;
        float waterVertexs[count * count * size * trianle] = {0};

        unsigned short indices[count * count * trianle] = {0};

        for (int i = 0; i < count; i++)
        {
            for (int j = 0; j < count; ++j)
            {
                int idx = (j * count + i) * trianle;
                indices[idx + 0] = idx; 
                indices[idx + 1] = idx + 1; 
                indices[idx + 2] = idx + 2; 
                indices[idx + 3] = idx + 3; 
                indices[idx + 4] = idx + 4; 
                indices[idx + 5] = idx + 5; 

                int x = 0;
                int y = 0;
                int index = 0;

                x = i;
                y = j;
                index = (j * count + i) * size * trianle;
                waterVertexs[index] = x;
                waterVertexs[index + 1] = height;
                waterVertexs[index + 2] = y;
                waterVertexs[index + 3] = 0;
                waterVertexs[index + 4] = 1;
                waterVertexs[index + 5] = 0;
                waterVertexs[index + 6] = 0;
                waterVertexs[index + 7] = 0;


                x = i + 1;
                y = j;
                waterVertexs[index + 8] = x;
                waterVertexs[index + 9] = height;
                waterVertexs[index + 10] = y;
                waterVertexs[index + 11] = 0;
                waterVertexs[index + 12] = 1;
                waterVertexs[index + 13] = 0;
                waterVertexs[index + 14] = 0;
                waterVertexs[index + 15] = 0;

                x = i + 1;
                y = j + 1;
                waterVertexs[index + 16] = x;
                waterVertexs[index + 17] = height;
                waterVertexs[index + 18] = y;
                waterVertexs[index + 19] = 0;
                waterVertexs[index + 20] = 1;
                waterVertexs[index + 21] = 0;
                waterVertexs[index + 22] = 0;
                waterVertexs[index + 23] = 0;

                x = i;
                y = j;
                waterVertexs[index + 24] = x;
                waterVertexs[index + 25] = height;
                waterVertexs[index + 26] = y;
                waterVertexs[index + 27] = 0;
                waterVertexs[index + 28] = 1;
                waterVertexs[index + 29] = 0;
                waterVertexs[index + 30] = 0;
                waterVertexs[index + 31] = 0;

                x = i + 1;
                y = j + 1;
                waterVertexs[index + 32] = x;
                waterVertexs[index + 33] = height;
                waterVertexs[index + 34] = y;
                waterVertexs[index + 35] = 0;
                waterVertexs[index + 36] = 1;
                waterVertexs[index + 37] = 0;
                waterVertexs[index + 38] = 0;
                waterVertexs[index + 39] = 0;

                x = i;
                y = j + 1;
                index = (y * count + x) * size * trianle;
                waterVertexs[index + 40] = x;
                waterVertexs[index + 41] = height;
                waterVertexs[index + 42] = y;
                waterVertexs[index + 43] = 0;
                waterVertexs[index + 44] = 1;
                waterVertexs[index + 45] = 0;
                waterVertexs[index + 46] = 0;
                waterVertexs[index + 47] = 0;
            }
        }


        pModel = new CModel();
        pModel->m_Mesh = new Mesh();
        meshData = new MeshData();
        meshData->m_Shader = tempShader;
        meshData->SetVertex(waterVertexs, 36, 6);
        meshData->SetIndice(indices, 36);
        meshData->m_Tex = tempTex;
        meshData->m_Tex1 = tempTex1;
        meshData->m_TexShadow = depthMap;
        meshData->m_TexShadow1 = depthMap1;
        meshData->Initialize();
        meshData->m_Pos = glm::vec3(-3, 0, -3);
        meshData->m_IsCube = true;
        pModel->m_Mesh->m_Roots.push_back(meshData);
        m_Models.push_back(pModel);
    }
}

void App::Update()
{
    OnFrame();
    OnRender();
    m_View->pollEvents();
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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
#ifdef POST_PROCESS
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
#endif
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

#ifdef SHADOWMAP
    glViewport(0, 0, SHADOWMAPSIZE, SHADOWMAPSIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    m_IsRenderDepth = true;
    m_IsFront = true;
    NormalRender();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO1);
    glClear(GL_DEPTH_BUFFER_BIT);
    m_IsFront = false;
    m_IsRenderDepth = true;
    NormalRender();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
#endif
}

void App::EndRender()
{
#ifdef POST_PROCESS
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
    glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    screenShader->use();
    screenShader->setInt("screenTexture", 0);
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTexture); // use the now resolved color attachment as the quad's texture
    glDrawArrays(GL_TRIANGLES, 0, 6);
#endif

#ifdef SHADOWMAP1
    debugDepthQuad->use();
    debugDepthQuad->setFloat("near_plane", near_plane);
    debugDepthQuad->setFloat("far_plane", far_plane);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    debugDepthQuad->setInt("depthMap", 0);
    renderQuad();
#endif

    m_View->swapBuffers();
}

void App::NormalRender()
{
    for (int i = 0; i < m_Models.size(); ++i)
    {
        m_Models[i]->Render();
    }
    m_SkyBox->Render();
}

void App::OnRender()
{
    BeginRender();

    m_IsRenderDepth = false;
    NormalRender();

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
