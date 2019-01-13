
#pragma once
#include <vector>
#include "runtime/render/shader/shader.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class MeshData
{
public:
    int m_VertexSize;
    int m_ElemSize;
    float* m_Vertex;
    int m_IndiceSize;
    unsigned short* m_Indices;
    Shader* m_Shader;
    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_IBO;
    unsigned int m_Tex;
    unsigned int m_Tex1;
    unsigned int m_TexShadow;
    bool m_IsCube;

    glm::vec3 m_Pos;
    glm::vec3 m_Rot;
    glm::vec3 m_Scale;

    std::vector<MeshData*> m_SubMesh;
public:
    MeshData();
    ~MeshData();
    void SetVertex(float* v, int size, int eleSize);
    void SetIndice(unsigned short* i, int size);
    void CreateShader(const char* vert, const char* frag);
    void InitTexture(const char* path);
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