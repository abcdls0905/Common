
#include "model.h"
#include <stb_image.h>
#include <learnopengl/filesystem.h>

MeshData::~MeshData(){
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_IBO);
}
void MeshData::SetVertex(float* v, int size)
{
    m_Vertex = v;
    m_VertexSize = size;
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_VertexSize * 8, m_Vertex, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

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

void Mesh::RenderNode(MeshData* pMesh)
{
    glBindTexture(GL_TEXTURE_2D, pMesh->m_Tex);
    glUseProgram(pMesh->m_Shader->ID);

    glBindVertexArray(pMesh->m_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_IBO);

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
