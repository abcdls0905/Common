#pragma once

#include "d3dUtility.h"

struct SVertex
{
    SVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v, D3DCOLOR color)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->nx = nx;
        this->ny = ny;
        this->nz = nz;
		this->u = u;
		this->v = v;
    }
	float x;
	float y;
    float z;
    float nx;
    float ny;
    float nz;
	float u;
	float v;
	//static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_DIFFUSE;
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};

class Game
{
public:
	Game();
	~Game();
	static bool GameLoop(float deltaTime);
	void Update(float deltaTime);
	void Render(float deltaTime);
	bool Initialize(IDirect3DDevice9* device);
	void InitRenderData();
    D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
	void End();
	static Game* GetInstance();
private:
	IDirect3DVertexBuffer9* m_VB;
	IDirect3DTexture9* m_Tex;
	IDirect3DIndexBuffer9* m_IB;
	IDirect3DDevice9* m_Device;
	static Game* m_GameInstance;
};
