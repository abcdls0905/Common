
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
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

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
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH);
    glUseProgram(pMesh->m_Shader->ID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pMesh->m_Tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pMesh->m_Tex1);

    glBindVertexArray(pMesh->m_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_IBO);

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    model = glm::translate(model, pMesh->m_Pos);
    model = glm::rotate(model, 0.0f, glm::vec3(0.5f, 1.0f, 0.0f));
    view = App::Inst().m_Camera->GetViewMatrix();
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    pMesh->m_Shader->setMat4("model", model);
    pMesh->m_Shader->setMat4("view", view);
    pMesh->m_Shader->setMat4("projection", projection);
    pMesh->m_Shader->setInt("material.diffuse", 0);
    pMesh->m_Shader->setInt("material.specular", 1);

    glm::vec3 lightPos(-5, 5, -5);
    Camera& camera = *App::Inst().m_Camera;
    pMesh->m_Shader->setVec3("light.direction", -0.2f, -1.0f, -0.3f);
    pMesh->m_Shader->setVec3("viewPos", camera.Position);

    // light properties
    pMesh->m_Shader->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    pMesh->m_Shader->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    pMesh->m_Shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    // material properties
    pMesh->m_Shader->setFloat("material.shininess", 32);

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
