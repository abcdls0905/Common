
#pragma once
#include <vector>
#include "runtime/render/shader/shader.h"

class MeshData
{
public:
    int m_VertexSize;
    float* m_Vertex;
    int m_IndiceSize;
    unsigned short* m_Indices;
    Shader* m_Shader;
    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_IBO;
    std::vector<MeshData*> m_SubMesh;
public:
    ~MeshData();
    void SetVertex(float* v, int size);
    void SetIndice(unsigned short* i, int size);
    void CreateShader(const char* vert, const char* frag);
    void Initialize();
};

class Mesh
{
public:
    std::vector<MeshData*> m_Roots;
public:
    void Render();

    void RecursiveNode(MeshData* pMesh);

    void RenderNode(MeshData* pMesh);
};

class CModel
{
public:
    Mesh* m_Mesh;
public:
    void Render();
};