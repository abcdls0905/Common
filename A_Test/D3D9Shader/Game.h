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
		this->u1 = u;
		this->v1 = v;
		this->u2 = u;
		this->v2 = v;
    }
	float x;
	float y;
    float z;
    float nx;
    float ny;
	float nz;
	float u;
	float v;
	float u1;
	float v1;
	float u2;
	float v2;
	//static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_DIFFUSE;
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3;
};

class Game
{
public:
	struct ShaderHandle
	{
		D3DXHANDLE BaseTexHandle;
		D3DXHANDLE SpotLightTexHandle;
		D3DXHANDLE StringTexHandle;

		D3DXCONSTANT_DESC BaseTexDesc;
		D3DXCONSTANT_DESC SpotLightTexDesc;
		D3DXCONSTANT_DESC StringTexDesc;
	};
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

	IDirect3DVertexShader9* m_ToonShader = 0;
	ID3DXConstantTable* m_ToonConstTable = 0;

	IDirect3DPixelShader9* m_MultiTexPS = 0;
	ID3DXConstantTable* m_MultiTexCT;

	IDirect3DTexture9* BaseTex = 0;
	IDirect3DTexture9* SpotLightTex = 0;
	IDirect3DTexture9* StringTex = 0;

	ShaderHandle m_ShaderHandle;
};
