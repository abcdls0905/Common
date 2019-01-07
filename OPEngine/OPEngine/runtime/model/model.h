
#pragma once
#include <vector>
#include "runtime/render/shader/shader.h"

class Mesh
{
private:
    float* m_Vertex;
    int* m_Indices;
    Shader* m_Shader;
    unsigned int m_VBO;
    unsigned int m_IBO;
    std::vector<Mesh> m_Roots;
    std::vector<Mesh> m_SubMesh;
public:
    void SetVertex(float* v)
    {
        m_Vertex = v;
    }
    void SetIndice(int* i)
    {
        m_Indices = i;
    }
    void CreateShader(const char* vert, const char* frag)
    {
        m_Shader = new Shader(vert, frag);
    }

    void Initialize()
    {
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertex), m_Vertex, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &m_IBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_IBO);
    }

    void Render()
    {
        for (int i = 0; i < m_Roots.size(); ++i)
        {
            Mesh* pMesh = &m_Roots[i];
            RecursiveNode(pMesh);
        }
    }

    void RecursiveNode(Mesh* pMesh)
    {
        RenderNode(pMesh);
        for (int i = 0; i < pMesh->m_SubMesh.size(); ++i)
        {
            Mesh* pChild = &pMesh->m_SubMesh[i];
            RecursiveNode(pChild);
        }
    }

    void RenderNode(Mesh* pMesh)
    {

    }
};

class CModel
{
private:
    Mesh* m_Mesh;
public:
    void Render();
};