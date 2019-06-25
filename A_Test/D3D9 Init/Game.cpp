#include "Game.h"

Game* Game::m_GameInstance = nullptr;

Game::Game()
{

}

Game::~Game()
{

}

bool Game::GameLoop(float deltaTime)
{
	m_GameInstance->Update();
	m_GameInstance->Render();
	return true;
}

void Game::Update()
{

}

bool Game::Initialize(IDirect3DDevice9* device)
{
	this->m_Device = device;
	InitRenderData();
	return true;
}

void Game::InitRenderData()
{
	m_Device->CreateVertexBuffer(sizeof(SVertex) * 3, D3DUSAGE_WRITEONLY, SVertex::FVF, D3DPOOL_MANAGED, &m_VB, 0);
	SVertex* vertices;
	m_VB->Lock(0, sizeof(SVertex) * 3, (void**)&vertices, D3DLOCK_READONLY);
	vertices[0] = SVertex(-1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(255, 0, 0));
	vertices[1] = SVertex(0.0f, 1.0f, 2.0f, D3DCOLOR_XRGB(0, 255, 0));
	vertices[2] = SVertex(1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(0, 0, 255));
    m_VB->Unlock();
    D3DXMATRIX proj;
    float Width = 1360;
    float Height = 768;

    D3DXMatrixPerspectiveFovLH(
        &proj,                        // result
        D3DX_PI * 0.5f,               // 90 - degrees
        (float)Width / (float)Height, // aspect ratio
        1.0f,                         // near plane
        1000.0f);                     // far plane
    m_Device->SetTransform(D3DTS_PROJECTION, &proj);
    m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    m_Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    m_Device->SetRenderState(D3DRS_LIGHTING, false);
}

void Game::Render()
{
    m_Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1, 0);
    m_Device->BeginScene();
    m_Device->SetStreamSource(0, m_VB, 0, sizeof(SVertex));
    m_Device->SetFVF(SVertex::FVF);
    m_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
    m_Device->EndScene();
    m_Device->Present(0, 0, 0, 0);
}

void Game::End()
{
	if (m_Device)
	{
		m_Device->Release();
		m_Device = nullptr;
	}
}

Game* Game::GetInstance()
{
	if (!m_GameInstance)
		m_GameInstance = new Game();
	return m_GameInstance;
}
