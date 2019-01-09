
#include "model.h"
#include <stb_image.h>
#include <learnopengl/filesystem.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &m_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * m_IndiceSize, m_Indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshData::InitTexture(const char* path)
{
    glGenTextures(1, &m_Tex);
    glBindTexture(GL_TEXTURE_2D, m_Tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pMesh->m_Tex);
    glUseProgram(pMesh->m_Shader->ID);

    glBindVertexArray(pMesh->m_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_IBO);

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    model = glm::rotate(model, 0.f, glm::vec3(0.5f, 1.0f, 0.0f));
    view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    unsigned int modelLoc = glGetUniformLocation(pMesh->m_Shader->ID, "model");
    unsigned int viewLoc  = glGetUniformLocation(pMesh->m_Shader->ID, "view");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    pMesh->m_Shader->setMat4("projection", projection);

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
