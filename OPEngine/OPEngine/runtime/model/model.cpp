
#include "model.h"
#include <stb_image.h>
#include <learnopengl/filesystem.h>

#include "runtime/util.h"
#include "app.h"

MeshData::MeshData()
{
    m_Pos = glm::vec3(0, 0, 0);
    m_Rot = glm::vec3(0, 0, 0);
    m_Scale = glm::vec3(1, 1, 1);
    m_IsCube = false;
}

MeshData::~MeshData(){
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_IBO);
}
void MeshData::SetVertex(float* v, int size, int eleSize)
{
    m_Vertex = v;
    m_VertexSize = size;
    m_ElemSize = eleSize;
}
void MeshData::SetIndice(unsigned short* i, int size)
{
    m_Indices = i;
    m_IndiceSize = size;
}
void MeshData::CreateShader(const char* vert, const char* frag)
{
    m_Shader = new Shader(vert, frag);
}

void MeshData::Initialize()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_VertexSize * m_ElemSize, m_Vertex, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_ElemSize * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, m_ElemSize * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, m_ElemSize * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &m_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * m_IndiceSize, m_Indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshData::InitTexture(const char* path)
{
    m_Tex = Util::LoadTexture(path);
}

void Mesh::Render()
{
    for (int i = 0; i < m_Roots.size(); ++i)
    {
        MeshData* pMesh = m_Roots[i];
        RecursiveNode(pMesh);
    }
}

void Mesh::RecursiveNode(MeshData* pMesh)
{
    RenderNode(pMesh);
    for (int i = 0; i < pMesh->m_SubMesh.size(); ++i)
    {
        MeshData* pChild = pMesh->m_SubMesh[i];
        RecursiveNode(pChild);
    }
} 

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 768;

void Mesh::RenderNode(MeshData* pMesh)
{
    Shader* useShader = pMesh->m_Shader;
    bool isRenderDepth = App::Inst().IsRenderDepth();
    if (isRenderDepth)
        useShader = App::Inst().simpleDepthShader;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    pMesh->m_Shader->use();
    glActiveTexture(GL_TEXTURE0);
    if (pMesh->m_IsCube)
        glBindTexture(GL_TEXTURE_CUBE_MAP, pMesh->m_Tex);
    else
        glBindTexture(GL_TEXTURE_2D, pMesh->m_Tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pMesh->m_Tex1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, pMesh->m_TexShadow);

    glBindVertexArray(pMesh->m_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_IBO);

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    model = glm::translate(model, pMesh->m_Pos);
    model = glm::rotate(model, 0.0f, glm::vec3(0.5f, 1.0f, 0.0f));
    view = App::Inst().m_Camera->GetViewMatrix();
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    useShader->setMat4("model", model);
    useShader->setMat4("view", view);
    useShader->setMat4("projection", projection);
    useShader->setInt("material.diffuse", 0);
    useShader->setInt("material.specular", 1);
    useShader->setInt("material.shadow", 2);
    useShader->setInt("skybox", 0);
    useShader->setInt("texture1", 0);

    Camera& camera = *App::Inst().m_Camera;
    useShader->setVec3("light.direction", -0.2f, -1.0f, -0.3f);
    useShader->setVec3("viewPos", camera.Position);
    useShader->setVec3("cameraPos", camera.Position);

    // light properties
    useShader->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    useShader->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    useShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    glm::vec3 lightPos(-2.0f, 8.0f, -5.0f);
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 200.f;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    useShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    // material properties
    useShader->setFloat("material.shininess", 32);

    glDrawElements(GL_TRIANGLES, pMesh->m_IndiceSize, GL_UNSIGNED_SHORT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
}  

void CModel::Render()
{
    if (!m_Mesh)
        return;
    m_Mesh->Render();
}
